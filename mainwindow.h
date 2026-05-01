#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onConnectClicked();
    void onDataRecieved();
    void onConnected();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    void buildUI();
    void parseAndDisplay(const QString& raw);

    QTcpSocket* m_socket;

    // UI Elements
    QPushButton* m_connectBtn;
    QLabel* m_tempLabel;
    QLabel* m_humLabel;
    QLabel* m_statusLabel;
    QTextEdit* m_logView;
    QByteArray m_buffer;  // accumulates partial tcp reads

};
#endif // MAINWINDOW_H
