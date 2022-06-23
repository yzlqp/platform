#include "serialwidgt.h"
#include "ui_serialwidgt.h"
#include <QMessageBox>
#include<QKeyEvent>
#include "ad9361widget.h"

SerialWidgt::SerialWidgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialWidgt)
{
    ui->setupUi(this);

    //setStyleSheet("background-color:rgb(185,185,185);");

}
//串口初始化
void SerialWidgt::initSerialPort()
{
    m_serialPort = new QSerialPort();


    //设置校验位
    ui->comboBox_Checkoutbit->addItem("None",Qt::UserRole);
    ui->comboBox_Checkoutbit->addItem("Odd",Qt::UserRole);
    ui->comboBox_Checkoutbit->addItem("Even",Qt::UserRole);
    //ui->comboBox_Checkoutbit->setCurrentText(0);

    //设置停止位
    ui->comboBox_Stopbit->addItem("1");
    ui->comboBox_Stopbit->addItem("1.5");
    ui->comboBox_Stopbit->addItem("2");

    //设置数据位
    ui->comboBox_Databit->addItem("8");
    ui->comboBox_Databit->addItem("7");
    ui->comboBox_Databit->addItem("6");
    ui->comboBox_Databit->addItem("5");

    //设置波特率
    ui->comboBox_Baudrate->addItem("115200");
    ui->comboBox_Baudrate->addItem("9600");

    //默认设置
    ui->comboBox_Checkoutbit->setCurrentText(0);
    ui->comboBox_Stopbit->setCurrentText(0);
    ui->comboBox_Stopbit->setCurrentText(0);
    ui->comboBox_Baudrate->setCurrentText(0);

    //设置发送编辑框相应事件
    ui->textEdit_Ssend->setFocus();
    ui->textEdit_Ssend->installEventFilter(this);

    QStringList serialStrList;
    // 读取串口信息
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        m_serialPort->setPort(info);
        // 判断端口能否以读写方式打开
        qDebug()<<endl<<"Name"<<endl;
        if(m_serialPort->open(QIODevice::ReadWrite))
        {

            serialStrList.append(m_serialPort->portName());
            m_serialPort->close(); // 然后自动关闭等待人为开启（通过"打开串口按钮"）
        }
    }

    QStringList  Port_List = serialStrList;
    qDebug()<<"port list:"<<Port_List;

    for (int i = 0;i<Port_List.size();i++)
    {
        ui->comboBox_Port->addItem(Port_List[i]);

    }

    //串口接收数据
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(receiveSerialData()));
    //串口循环发送定时器
    connect(&m_serialTimer, &QTimer::timeout, this, [=]
        {
            sendSerialData();
        });

    //清楚串口接收
    connect(ui->label_Sclear, &QMyLabel::sg_clicked,this,[=]()
                    {
                        ui->textEdit_Srec->clear();
                    });
    //清除串口输入
    connect(ui->label_Sclearsend, &QMyLabel::sg_clicked,this,[=]()
                    {
                        ui->textEdit_Ssend->clear();
                    });
    //串口信息改变时更新当前配置
    connect(ui->comboBox_Baudrate,SIGNAL(currentIndexChanged(int)),this,SLOT(updateConfig));


}
//串口接收数据
void SerialWidgt::receiveSerialData()
{
    //取出原始数据
    QByteArray data = m_serialPort->readAll();

    ui->textEdit_Srec->moveCursor(QTextCursor::End);
    ui->textEdit_Srec->insertPlainText(data);
    if (ui->checkBox_serialShowNewline->isChecked())
    {
        ui->textEdit_Srec->insertPlainText("\r\n");
    }
    temp_data = data;
    emit receiveSerialData_flag();
}
SerialWidgt::~SerialWidgt()
{
    delete ui;
}

void SerialWidgt::on_pushButton_Serialopen_clicked()
{
    //判断是打开还是关闭
    if (!m_serialPort->isOpen())
    {
        //设置串口名
        m_serialPort->setPortName(ui->comboBox_Port->currentText());

        //设置波特率
        m_serialPort->setBaudRate(ui->comboBox_Baudrate->currentText().toInt());

        //设置校验位
        m_serialPort->setParity(QSerialPort::Parity(ui->comboBox_Checkoutbit->currentData(Qt::UserRole).toInt()));

        //设置数据位
        m_serialPort->setDataBits(QSerialPort::DataBits(ui->comboBox_Databit->currentText().mid(0, 1).toInt()));

        //设置停止位
        m_serialPort->setStopBits(QSerialPort::StopBits(ui->comboBox_Stopbit->currentData(Qt::UserRole).toInt()));

        int ret = m_serialPort->open(QIODevice::ReadWrite);
        //打开串口
        if (!ret)
        {
            QMessageBox::warning(this, "Warning", "串口不存在或者被其他应用程序占用");
            return;
        }else
        {
            ui->comboBox_Databit->setEnabled(false);
            ui->comboBox_Port->setEnabled(false);
            ui->comboBox_Stopbit->setEnabled(false);
            ui->comboBox_Baudrate->setEnabled(false);
            ui->comboBox_Checkoutbit->setEnabled(false);

            ui->pushButton_Serialopen->setFixedSize(154,28);
            ui->pushButton_Serialopen->setStyleSheet(
                            "QPushButton{"
                            "color:rgba(255,0,0);"//红色
                            "border: 2px solid rgb(178, 34, 34);"
                            "}"
                           "QPushButton:hover{"
                           "border: 2px solid rgb(255, 165, 0);"
                           "}");
            ui->pushButton_Serialopen->setText("关闭串口");
        }
    }
    else
    {
        m_serialPort->clear();
        m_serialPort->close();

        ui->comboBox_Databit->setEnabled(true);
        ui->comboBox_Port->setEnabled(true);
        ui->comboBox_Stopbit->setEnabled(true);
        ui->comboBox_Baudrate->setEnabled(true);
        ui->comboBox_Checkoutbit->setEnabled(true);

        ui->pushButton_Serialopen->setFixedSize(154,28);
        ui->pushButton_Serialopen->setStyleSheet(
                    "QPushButton{"
                         "color:rgba(0,0,0);" //黑色
                         "border: 2px solid rgb(54, 100, 139);"
                         "}"
                         "QPushButton:hover{"
                         "border: 2px solid rgb(0, 150, 136);"//橘红色
                         "}"
                   );
         ui->pushButton_Serialopen->setText("打开串口");
    }
}
//发送串口数据
void SerialWidgt::sendSerialData()
{
    //获取输入框内容
    QString data = ui->textEdit_Ssend->toPlainText();
    data +='\n';
    QByteArray data2 = data.toUtf8();
    m_serialPort->write(data2);
}
void SerialWidgt::on_pushButton_Ssend_clicked()
{
    if (!m_serialPort->isOpen())
        {
            QMessageBox::critical(this, "Warning", "串口尚未连接\n发送失败", "确定");
            return;
        }
    //自动循环发送
    if (ui->checkBox_serialSendCycle->isChecked() && (ui->pushButton_Ssend->text() == "发送"))
    {
        m_serialTimer.setInterval(ui->lineEdit_interal->text().toInt());
        m_serialTimer.start();

        ui->pushButton_Ssend->setText("停止发送");
    }else
    {
        m_serialTimer.stop();
        ui->pushButton_Ssend->setText("发送");
    }

    //发送串口数据
    sendSerialData();
}
void SerialWidgt::sendData(QByteArray &sendData)
{
    // 发送数据帧
    m_serialPort->write(sendData);
}
//事件过滤器
bool SerialWidgt::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->textEdit_Ssend)
    {
        if(event->type() ==  QEvent::KeyPress)
        {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
                        if(k->key() == Qt::Key_Return)
                        {
//                            on_pushButton_Ssend_clicked();
                            sendSerialData();
                            ui->textEdit_Ssend->clear();
                            return true;
                        }
//                        if(k->key() == Qt::Key_Escape)
//                        {
//                            ui->textEdit_usetest->moveCursor(QTextCursor::End);
//                            for(QVector<char>::iterator it=testdata.begin();it != testdata.end();it++)
//                            {
//                                QString temp = (QString)*it;
//                                //ui->textEdit_usetest->append(temp);
//                                ui->textEdit_usetest->insertPlainText(temp);
//                            }

//                            return true;
//                        }

        }
    }
     return QWidget::eventFilter(target,event);
}
