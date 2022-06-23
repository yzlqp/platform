#ifndef NETWIDGET_H
#define NETWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QMouseEvent>
#include <QUdpSocket>

namespace Ui {
    class NetWidget;
}

class NetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetWidget(QWidget *parent = nullptr);
    ~NetWidget();

    void initNet();
    void sendNetData();

public slots:

    void updateConfig();
    void tcpClientConnected();
    void receiveTcpData();
    void tcpServerRecvConnection();

public:
    Ui::NetWidget *ui;
    QTcpSocket *m_tcpClientSocket;
    QTcpServer  m_tcpServer;
    QTcpSocket *m_tcpServerSocket;
    QUdpSocket *m_udpsocket;
    QTimer m_netTimer;
    int m_nSerialSendCount;
    int m_nSerialRecvCount;
    int m_nNetSendCount;
    int m_nNetRecvCount;

private slots:
    void on_Buttonconcect_clicked();
    void on_pushButton_Netsend_clicked();
    void on_checkBox_netSendHex_stateChanged(int arg1);
    void on_pushButton_clicked();
    void dealMsg();
    //void on_comboBoxNet_currentIndexChanged(const QString &arg1);
};

#endif // NETWIDGET_H
