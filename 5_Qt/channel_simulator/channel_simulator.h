#ifndef CHANNEL_SIMULATOR_H
#define CHANNEL_SIMULATOR_H

#include <QWidget>
#include "cmath"
#include "QtCore/qmath.h"
#include <QDebug>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QMouseEvent>
#include <QUdpSocket>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

namespace Ui {
    class channel_simulator;
}

class channel_simulator : public QWidget
{
    Q_OBJECT

public:
    explicit channel_simulator(QWidget *parent = nullptr);
    ~channel_simulator();

    void initNet();
    void sendNetData();
    void channel_simulator_ui_init();
    void display(int *data);

private slots:
    void on_pushButton_clear_clicked();
    void on_pushButton_caculator_clicked();

    void dealMsg();

    void on_Buttonconcect_clicked();

    void on_comboBoxNet_currentIndexChanged(int index);

    void on_pushButton_config_clicked();

    void on_pushButton_cat_clicked();

private:
    Ui::channel_simulator *ui;
    QUdpSocket *m_udpsocket;
    QTcpSocket *m_tcpClientSocket;
    QTcpServer  m_tcpServer;
    QTcpSocket *m_tcpServerSocket;
};

#endif // CHANNEL_SIMULATOR_H
