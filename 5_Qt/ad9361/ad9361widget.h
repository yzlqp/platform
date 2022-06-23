#ifndef AD9361WIDGET_H
#define AD9361WIDGET_H

#include <QWidget>
#include <QMap>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QMouseEvent>
#include <QUdpSocket>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <stdio.h>
#include <stdlib.h>

namespace Ui {
    class AD9361Widget;
}

class ad9361_msg {

public:
    unsigned char count;      //  count = 6;
    char name[128];           //  name = "in_voltage0_hardwaregain";
    char mode;                //  mode = 'r' 只读  'w' 写完回复 'c' 检测
    char flag;                //  flag = 's' 字符串 'n' 数字
    char ack;			      //  ack = '1' 交互成功
    char strval[32];          //  strval = "value";
    long long num;            //  num = 6000000000;
};

class AD9361Widget : public QWidget
{
    Q_OBJECT

public:
    explicit AD9361Widget(QWidget *parent = nullptr);
    ~AD9361Widget();

    void initAD9361();
    void initNet();
    void sendNetData();

public slots:
    void dealMsg();
    void on_Buttonconcect_clicked();
    void on_comboBoxNet_currentIndexChanged(int index);
signals:
    void updateSignal_clicked(); //To prompt other class objects when the interface needs to be updated
    void signal_cfg_clicked();

public:
    char    flags[10];
    QMap<QString,QString>  state;
    QMap<QString,QString>  state_name;
    QMap<QString,int>    Flag;
    QString   order_str=NULL;
    Ui::AD9361Widget *ui;

    ad9361_msg msg[20];
    ad9361_msg msgrf[20];

    QUdpSocket *m_udpsocket;
    QTcpSocket *m_tcpClientSocket;
    QTcpServer  m_tcpServer;
    QTcpSocket *m_tcpServerSocket;
private slots:
    void on_lineEdit_LORX_textChanged(const QString &arg1);
    void on_lineEdit_LOTX_textChanged(const QString &arg1);
    void on_lineEdit_RX_SampleRate_textChanged(const QString &arg1);
    void on_lineEdit_TX_SampleRate_textChanged(const QString &arg1);
    void on_lineEdit_RX_BW_textChanged(const QString &arg1);
    void on_lineEdit_TX_BW_textChanged(const QString &arg1);
    void on_comboBox_RX0_GainMode_currentIndexChanged(int index);
    void on_comboBox_RX1_GainMode_currentIndexChanged(int index);
    void on_lineEdit_RX0_Gain_textChanged(const QString &arg1);
    void on_lineEdit_RX1_Gain_textChanged(const QString &arg1);
    void on_pushButton_readpara_clicked();
    void on_pushButton_config_clicked();
    void on_pushButton_check_clicked();
    void on_pushButton_cleardisplay_clicked();
    void on_pushButton_clicked();
    void on_comboBox_RX0_GainMode_currentIndexChanged(const QString &arg1);
    void on_comboBox_RX1_GainMode_currentIndexChanged(const QString &arg1);
};

#endif // AD9361WIDGET_H
