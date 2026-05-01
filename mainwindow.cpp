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


}
