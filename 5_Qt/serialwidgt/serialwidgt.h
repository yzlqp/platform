#ifndef SERIALWIDGT_H
#define SERIALWIDGT_H

#include <QWidget>
#include <QWidget>
#include <QSerialPort>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QSerialPortInfo>
#include <QStringList>

namespace Ui {
    class SerialWidgt;
}

class SerialWidgt : public QWidget
{
    Q_OBJECT

public:
    explicit SerialWidgt(QWidget *parent = nullptr);
    ~SerialWidgt();


    void initSerialPort();
    void sendSerialData();
    void sendData(QByteArray &sendData);

public:
    Ui::SerialWidgt *ui;
    QSerialPort* m_serialPort;
    QStringList Port_List;
    QTimer m_serialTimer;
    QByteArray temp_data;
protected:
    bool eventFilter(QObject *target, QEvent *event);
signals:
    void receiveSerialData_flag();
public slots:
    void on_pushButton_Serialopen_clicked();
    void on_pushButton_Ssend_clicked();
    void receiveSerialData();
};

#endif // SERIALWIDGT_H
