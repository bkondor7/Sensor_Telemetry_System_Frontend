#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)

{
    buildUI();

    m_socket = new QTcpSocket(this);

    // Wire up socket signals
    connect(m_socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::onDataRecieved);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &MainWindow::onSocketError);

}


// ---------- UI Layout -----------

void MainWindow::buildUI() {

    setWindowTitle("Sensor Dash");
    resize(420, 380);

    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);

    // ----- Connection Row -----
    auto* connRow = new QHBoxLayout();
    m_connectBtn = new QPushButton("Connect", this);
    m_statusLabel = new QLabel("Disconnected", this);
    m_statusLabel->setStyleSheet("color: gray;");
    connRow->addWidget(m_connectBtn);
    connRow->addWidget(m_statusLabel);
    connRow->addStretch();
    mainLayout->addLayout(connRow);

    // ---- LiVE Readings ----
    auto* readingsBox = new QGroupBox("Live Readings", this);
    auto* readLayout = new QVBoxLayout(readingsBox);

    m_tempLabel = new QLabel("Temperature: - C", this);
    m_humLabel = new QLabel("Humidity: - %", this);

    QFont bigFont = m_tempLabel->font();
    bigFont.setPointSize(18);
    bigFont.setBold(true);
    m_tempLabel->setFont(bigFont);
    m_humLabel->setFont(bigFont);

    readLayout->addWidget(m_tempLabel);
    readLayout->addWidget(m_humLabel);
    mainLayout->addWidget(readingsBox);

    // ---- Raw Log ----
    mainLayout->addWidget(new QLabel("Raw Data Log:", this));
    m_logView = new QTextEdit(this);
    m_logView->setReadOnly(true);
    m_logView->setMaximumHeight(160);
    m_logView->setFontFamily("Courier New");
    mainLayout->addWidget(m_logView);

    // Wire up the connect button
    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
}

   // ----- Slots -----
void MainWindow::onConnectClicked() {

    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    } else {
        m_statusLabel->setText("Connecting...");
        m_statusLabel->setStyleSheet("color: orange;");
        m_socket->connectToHost("127.0.0.1", 9000);
    }
}

void MainWindow::onConnected() {

    m_statusLabel->setText("Connected");
    m_statusLabel->setStyleSheet("color: green;");
    m_connectBtn->setText("Disconnect");
    m_logView->append("[" + QDateTime::currentDateTime().toString("hh:mm:ss") + "] Connected to server.");
}

void MainWindow::onDisconnected() {

    m_statusLabel->setText("Disconnected");
    m_statusLabel->setStyleSheet("color: gray;");
    m_connectBtn->setText("Connect");
    m_logView->append("[" + QDateTime::currentDateTime().toString("hh:mm:ss") + "] Disconnected.");
}

void MainWindow::onSocketError(QAbstractSocket::SocketError) { // error

    m_statusLabel->setText("Error: " + m_socket->errorString());
    m_statusLabel->setStyleSheet("color: red;");

}

void MainWindow::onDataRecieved() {

    // accumulate bytes in buffer until fll newline-terminated frame
    // guarding against TCP fragmentation partial reads
    m_buffer += m_socket->readAll();

    while (m_buffer.contains('\n')) {
        int idx = m_buffer.indexOf('\n');
        QByteArray line = m_buffer.left(idx).trimmed();
        m_buffer.remove(0, idx + 1);

        if (!line.isEmpty()) {
            parseAndDisplay(QString::fromUtf8(line));

        }
    }
}

// ----- Parser -----
// Expected format: "TEMP:22.413,HUM:56.200
void MainWindow::parseAndDisplay(const QString& raw) {

    double temp = 0.0, hum = 0.0;
    bool ok1 = false, ok2 = false;

    const QStringList parts = raw.split(',');
    for (const QString part : parts) {
        if (part.startsWith("TEMP:")) {
            temp = part.mid(5).toDouble(&ok1);
        } else if (part.startsWith("HUM:")) {
            hum = part.mid(4).toDouble(&ok2);
        }
    }

    if (ok1 && ok2) {
        m_tempLabel->setText(QString("Temperature: %1 C").arg(temp, 0, 'f', 2));
        m_tempLabel->setText(QString("Humidity: %1 %").arg(hum, 0, 'f', 2));

        // append abbreviated entry to the log view
        m_logView->append(
            QString("[1%] T=%2C H=%3%")
                .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                .arg(temp, 0, 'f', 2)
                .arg(hum, 0, 'f', 2)
        );

        // Keep log from growing unbounded
        if (m_logView->document()->blockCount() > 200)
            m_logView->clear();
    }
}