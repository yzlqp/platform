#include "ad9361widget.h"
#include "ui_ad9361widget.h"
#include <QMessageBox>
#include <QDebug>



#define   Mydebug  1

AD9361Widget::AD9361Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AD9361Widget)
{
    ui->setupUi(this);

    initAD9361();

    initNet();
//    SerialWidgt_ui = new SerialWidgt();


//    if (!SerialWidgt_ui->m_serialPort->isOpen())
//    {

//    }
//    //打开串口
//    if (!SerialWidgt_ui->m_serialPort->open(QIODevice::ReadWrite))
//    {
//        QMessageBox::warning(this, "Warning", "串口不存在或者被其他应用程序占用");
//        return;
//    }
}

AD9361Widget::~AD9361Widget()
{
    delete ui;
}


void AD9361Widget::initAD9361()
{
    //读取默认配置
    state.clear();

    state.insert("out_altvoltage0_RX_LO_frequency",ui->lineEdit_LORX->text());
    state.insert("out_altvoltage1_TX_LO_frequency",ui->lineEdit_LOTX->text());
    state.insert("in_voltage_sampling_frequency",ui->lineEdit_RX_SampleRate->text());
    state.insert("out_voltage_sampling_frequency",ui->lineEdit_TX_SampleRate->text());
    state.insert("in_voltage_rf_bandwidth",ui->lineEdit_RX_BW->text());
    state.insert("out_voltage_rf_bandwidth",ui->lineEdit_TX_BW->text());
    state.insert("in_voltage0_gain_control_mode",ui->comboBox_RX0_GainMode->currentText());
    state.insert("in_voltage1_gain_control_mode",ui->comboBox_RX1_GainMode->currentText());
    state.insert("in_voltage0_hardwaregain",ui->lineEdit_RX0_Gain->text());
    state.insert("in_voltage1_hardwaregain",ui->lineEdit_RX1_Gain->text());

    state_name.insert("out_altvoltage0_RX_LO_frequency",ui->label_LORX->text());
    state_name.insert("out_altvoltage1_TX_LO_frequency",ui->label_LOTX->text());
    state_name.insert("in_voltage_sampling_frequency",ui->label_RX_SampleRate->text());
    state_name.insert("out_voltage_sampling_frequency",ui->label_TX_SampleRate->text());
    state_name.insert("in_voltage_rf_bandwidth",ui->label_RX_BW->text());
    state_name.insert("out_voltage_rf_bandwidth",ui->label_TX_BW->text());
    state_name.insert("in_voltage0_hardwaregain",ui->label_RX0_Gain->text());
    state_name.insert("in_voltage1_hardwaregain",ui->label_RX1_Gain->text());
    state_name.insert("in_voltage0_gain_control_mode","RX0增益"+ui->label_RX0_mode->text());
    state_name.insert("in_voltage1_gain_control_mode","RX1增益"+ui->label_RX1_mode->text());

    ui->comboBoxNet->setCurrentIndex(2);
    ui->comboBox_RX0_GainMode->setCurrentIndex(1);
    ui->comboBox_RX1_GainMode->setCurrentIndex(1);

    Flag.clear(); //改变标志位
}



//配置AD9361命令行
/*
void AD9361Widget::on_pushButton_cfg_clicked()
{

    order_str.clear();
    order_str +="cd /sys/bus/iio/devices/iio:device0\r\n";
    for (QMap<int,QString>::const_iterator it = state.constBegin();it != state.constEnd();it++) {
                switch (it.key()) {

                //LO
                case 0:
                {
                   order_str +={"echo "+it.value()+" >  out_altvoltage0_RX_LO_frequency\r\n"};
                   break;
                }
                case 1:
                {
                    order_str +={"echo "+it.value()+" >  out_altvoltage1_TX_LO_frequency\r\n"};
                    break;
                }
                //sampling_frequency
                case 2:
                {
                    order_str +={"echo "+it.value()+" >  in_voltage_sampling_frequency\r\n"};
                    break;

                }
                case 3:

                {
                    order_str +={"echo "+it.value()+" >  out_voltage_sampling_frequency\r\n"};
                    break;
                }
                //bandwidth
                case 4:
                {
                    order_str +={"echo "+it.value()+" >  in_voltage_rf_bandwidth\r\n"};
                    break;
                }
                case 5:
                {
                    order_str +={"echo "+it.value()+" >  out_voltage_rf_bandwidth\r\n"};
                    break;
                }

                //gain_control_mode
                case 6:
                {
                    if(ui->comboBox_RX_GainMode->currentIndex()==0)
                    {
                        order_str +={"echo "+ui->comboBox_RX_GainMode->currentText()+" > in_voltage0_gain_control_mode\r\n"};
                        order_str +={"echo "+it.value()+" >  in_voltage0_hardwaregain\r\n"};
                    }
                    else
                    {
                        order_str +={"echo "+ui->comboBox_RX_GainMode->currentText()+" > in_voltage0_gain_control_mode\r\n"};
                    }

                    break;
                }
                case 7:
                {
                    if(ui->comboBox_TX_GainMode->currentIndex()==0)
                    {
                        order_str +={"echo "+ui->comboBox_TX_GainMode->currentText()+" > in_voltage0_gain_control_mode\r\n"};
                        order_str +={"echo "+it.value()+" >  in_voltage1_hardwaregain\r\n"};
                    }
                    else
                    {
                        order_str +={"echo "+ui->comboBox_TX_GainMode->currentText()+" > in_voltage1_gain_control_mode\r\n"};
                    }
                    break;
                }


                }
    }
    emit signal_cfg_clicked();
}
*/




//读取手动输入的配置数据
void AD9361Widget::on_lineEdit_LORX_textChanged(const QString &arg1)
{

    state.insert("out_altvoltage0_RX_LO_frequency",arg1+"000000");
    Flag.insert("out_altvoltage0_RX_LO_frequency",1);

#if Mydebug
    QMap<QString,QString>::const_iterator it1 =state.constFind("out_altvoltage0_RX_LO_frequency");
    qDebug()<<it1.key()<<" "<<it1.value();
#endif

}
void AD9361Widget::on_lineEdit_LOTX_textChanged(const QString &arg1)
{
    state.insert("out_altvoltage1_TX_LO_frequency",arg1+"000000");
    Flag.insert("out_altvoltage1_TX_LO_frequency",1);


#if Mydebug
    QMap<QString,QString>::const_iterator it =state.constFind("out_altvoltage1_TX_LO_frequency");
    qDebug()<<it.key()<<" "<<it.value();

    for(QMap<QString,int>::const_iterator it =Flag.constBegin();it != Flag.constEnd();it++)
    {
       qDebug()<<it.key()<<" "<<it.value();
    }
#endif


}
void AD9361Widget::on_lineEdit_RX_SampleRate_textChanged(const QString &arg1)
{
    state.insert("in_voltage_sampling_frequency",arg1+"000000");
    Flag.insert("in_voltage_sampling_frequency",1);

#if Mydebug
    QMap<QString,QString>::const_iterator it =state.constFind("in_voltage_sampling_frequency");
    qDebug()<<it.key()<<" "<<it.value();
#endif
}

void AD9361Widget::on_lineEdit_TX_SampleRate_textChanged(const QString &arg1)
{
    state.insert("out_voltage_sampling_frequency",arg1+"000000");
    Flag.insert("out_voltage_sampling_frequency",1);

#if Mydebug
    QMap<QString,QString>::const_iterator it =state.constFind("out_voltage_sampling_frequency");
    qDebug()<<it.key()<<" "<<it.value();
#endif

}

void AD9361Widget::on_lineEdit_RX_BW_textChanged(const QString &arg1)
{
    state.insert("in_voltage_rf_bandwidth",arg1+"000000");
    Flag.insert("in_voltage_rf_bandwidth",1);

#if Mydebug
    QMap<QString,QString>::const_iterator it =state.constFind("in_voltage_rf_bandwidth");
    qDebug()<<it.key()<<" "<<it.value();
#endif
}

void AD9361Widget::on_lineEdit_TX_BW_textChanged(const QString &arg1)
{
    //qlonglong a;
   // a = arg1.toInt();
    state.insert("out_voltage_rf_bandwidth",arg1+"000000");
    Flag.insert("out_voltage_rf_bandwidth",1);

#if Mydebug
    QMap<QString,QString>::const_iterator it =state.constFind("out_voltage_rf_bandwidth");
    qDebug()<<it.key()<<" "<<it.value();
#endif
}
void AD9361Widget::on_lineEdit_RX0_Gain_textChanged(const QString &arg1)
{
    state.insert("in_voltage0_hardwaregain",arg1);
    Flag.insert("in_voltage0_hardwaregain",1);
#if Mydebug
    QMap<QString,QString>::const_iterator it =state.constFind("in_voltage0_hardwaregain");
    qDebug()<<it.key()<<" "<<it.value();
#endif
}

void AD9361Widget::on_lineEdit_RX1_Gain_textChanged(const QString &arg1)
{
    state.insert("in_voltage1_hardwaregain",arg1);
    Flag.insert("in_voltage1_hardwaregain",1);
#if Mydebug
    QMap<QString,QString>::const_iterator it =state.constFind("in_voltage1_hardwaregain");
    qDebug()<<it.key()<<" "<<it.value();
#endif
}
//判断增益的控制模式
void AD9361Widget::on_comboBox_RX0_GainMode_currentIndexChanged(int index)
{
    qDebug()<<index<<endl;
    if(index == 0)
    {
        ui->lineEdit_RX0_Gain->setEnabled(true);
    }
    else
    {
        ui->lineEdit_RX0_Gain->setEnabled(false);
    }
}

void AD9361Widget::on_comboBox_RX1_GainMode_currentIndexChanged(int index)
{
    qDebug()<<index<<endl;
    if(index == 0)
    {
        ui->lineEdit_RX1_Gain->setEnabled(true);
    }
    else
    {
        ui->lineEdit_RX1_Gain->setEnabled(false);
    }
}



/*             5.13         add                      */

//网口初始化

void AD9361Widget::initNet()
{
    m_tcpClientSocket = new QTcpSocket(this);
    m_udpsocket = new QUdpSocket(this);



    //显示传输协议类型
    QString s = ui->comboBoxNet->currentText();
    qDebug()<<s;

    connect(m_udpsocket,&QUdpSocket::readyRead,this,&AD9361Widget::dealMsg);
    //协议类型改变时
    connect(ui->comboBoxNet,SIGNAL(currentIndexChanged(int)),this, SLOT(updateConfig()));
    //TCP客户端连接上
    connect(m_tcpClientSocket, SIGNAL(connected()), this, SLOT(tcpClientConnected()));

    //TCP客户端接收数据
    connect(m_tcpClientSocket, SIGNAL(readyRead()), this, SLOT(receiveTcpData()));
    //TCP服务端接收到连接请求
    connect(&m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcpServerRecvConnection()));

}


void AD9361Widget::dealMsg()
{
    int i=0;
    char buf[2048]={0};
    int re_count;
    int vaild_count = 0;
    QHostAddress cliAddr;
    quint16 port;
    QMap<QString,QString>::const_iterator item;
    quint64 len = m_udpsocket->readDatagram(buf,sizeof(buf),&cliAddr,&port);

    ad9361_msg* rec_msg = (ad9361_msg*)buf;

    if(len>0)
    {
        //格式化 [192.68.2.2:8888]
        QString str = QString("%3:[%1:%2]的信息")
                .arg(cliAddr.toString())
                .arg(port)
                .arg("收到来自");
        //给编辑区设置内容
        ui->textEdit_display->append(str);
    }

    re_count = rec_msg->count;
    for(i=0;i<re_count;i++)
    {
       qDebug()<<rec_msg[i].count<<rec_msg[i].name<<rec_msg[i].mode<<rec_msg[i].flag<<rec_msg[i].ack<<rec_msg[i].strval<<rec_msg[i].num<<endl;
    }

        for (i=0;i<re_count;i++) {
            switch (rec_msg->mode) {
                case 'w':
                if(rec_msg->ack == '1')
                {
                     qDebug()<<rec_msg->name<<msg[i].name<<strcmp(rec_msg->name, msg[i].name)<<strcmp(rec_msg->strval, msg[i].strval)<<endl;
                     if((!strcmp(rec_msg->name, msg[i].name)) && !strcmp(rec_msg->strval, msg[i].strval))
                     {
                        vaild_count++;
                        item = state_name.constFind(rec_msg->name);
                        QString str = QString("%1 %2").arg("成功配置："+item.value()).arg(rec_msg->strval);
                        //给编辑区设置内容
                        ui->textEdit_display->append(str);

                     }
                     else
                     {
                         item = state_name.constFind(rec_msg->name);
                         QString str = QString("%1  %2 : %3 ,%4 : %5")
                                 .arg(item.value()).arg("发送数据").arg(msg[i].strval)
                                 .arg("接收数据").arg(rec_msg->strval);
                         //给编辑区设置内容
                         ui->textEdit_display->append(str);
                     }
                }
                else {

                    QString str = QString("%1:%2").arg(rec_msg->name).arg("配置失败");
                    //给编辑区设置内容
                    ui->textEdit_display->append(str);
                }
                break;

            case 'r':
            if(rec_msg->ack == '1')
            {
//                qDebug()<<rec_msg->name<<msg[i].name<<strcmp(rec_msg->name, msg[i].name)<<strcmp(rec_msg->strval, msg[i].strval)<<endl;
//                 if((!strcmp(rec_msg->name, msg[i].name)) && !strcmp(rec_msg->strval, msg[i].strval))
//                 {
                    vaild_count++;
                    item = state_name.constFind(rec_msg->name);
                    QString str = QString("%1:%2").arg(item.value()).arg(rec_msg->strval);
                    //给编辑区设置内容
                    ui->textEdit_display->append(str);
//                 }
            }
            else {
                item = state_name.constFind(rec_msg->name);
                QString str = QString("%1:%2").arg(item.value()).arg("读取失败");
                //给编辑区设置内容
                ui->textEdit_display->append(str);
            }

            break;

            case 'c':
            if(rec_msg->ack == '1')
            {
                 if(!strcmp(rec_msg->strval, "ad9361-phy\n"))
                 {
                     vaild_count++;
                     QString str = QString("%1").arg("9361驱动匹配成功！");
                     //给编辑区设置内容
                     ui->textEdit_display->append(str);
                 }
            }
            else {

                QString str = QString("%1").arg("没有安装9361驱动！");
                //给编辑区设置内容
                ui->textEdit_display->append(str);
            }

            break;


            }
        rec_msg++;
        }

        QString str = QString("%1:%2,%3:%4").arg("发送数据个数：").arg(re_count).arg("匹配成功个数：").arg(vaild_count);
        //给编辑区设置内容
        ui->textEdit_display->append(str);


    //msgrf[20] = reinterpret_cast<ad9361_msg>(buf);
//    for (i=0;i<sizeof(buf);i++) {
//        qDebug()<< buf[i];
//    }




}




void AD9361Widget::on_Buttonconcect_clicked()
{
    if(ui->Buttonconcect->text() != "DisConnect")
    {
        ui->Buttonconcect->setText("DisConnect");
        switch (ui->comboBoxNet->currentIndex())
        {
        case 0:
        {
            int portNum = ui->lineEditPort->text().toInt();
            m_tcpServer.listen(QHostAddress::Any, portNum);

            break;
        }
        case 1:
        {
            //获取服务器的ip和端口
            QString ip = ui->lineEditIp->text();
            quint16 port = ui->lineEditPort->text().toInt();

            //主动和服务器连接
            m_tcpClientSocket->connectToHost(QHostAddress(ip), port);
            break;
        }
        case 2:
        {
            m_udpsocket->bind(8888);
            //格式化 [192.68.2.2:8888]aaaa
            QString str = QString("%1，%2：%3")
                    .arg("本地ip地址：192.168.1.102")
                    .arg("端口号：")
                    .arg(8888);
            //给编辑区设置内容
            ui->textEdit_display->append(str);
            break;
        }
        }
        ui->comboBoxNet->setEnabled(false);
        ui->lineEditIp->setEnabled(false);
        ui->lineEditPort->setEnabled(false);
    }
    else
    {
        ui->Buttonconcect->setText("Connect");
        switch (ui->comboBoxNet->currentIndex())
        {
        case 0:
        {
            m_tcpClientSocket->disconnectFromHost();
            m_tcpServer.disconnect();
            break;
        }
        case 1:
        {
        m_tcpClientSocket->disconnectFromHost();
        break;
        }
        }
        ui->comboBoxNet->setEnabled(true);
        ui->lineEditIp->setEnabled(true);
        ui->lineEditPort->setEnabled(true);
    }
}

void AD9361Widget::on_comboBoxNet_currentIndexChanged(int index)
{
    if(0==index)
    {
        ui->label_addr->setText("(2)本地ip地址");
        ui->label_port->setText("(3)本地端口号");

        ui->lineEditIp->setText("192.168.1.102");
        ui->lineEditPort->setText("8000");
    }
    else if(1==index)
    {
        ui->label_addr->setText("(2)服务器ip地址");
        ui->label_port->setText("(3)服务器端口号");

        ui->lineEditIp->setText("192.168.1.102");
        ui->lineEditPort->setText("8000");
    }else
    {
        ui->label_addr->setText("(2)对方ip地址");
        ui->label_port->setText("(3)对方端口号");

        ui->lineEditIp->setText("192.168.1.8");
        ui->lineEditPort->setText("3333");
    }
}

//void AD9361Widget::on_pushButton_config_clicked()
//{
//    int count;

//    buff[1] = 2;

//    //先获取对方的IP和端口
//    QString ip = ui->lineEditIp->text();
//    qint16 port = ui->lineEditPort->text().toInt();

//    //获取编辑区内容
//    QString str = ui->textEdit_display->toPlainText();
//    count = str.size();
//    //给指定的IP发送数据
//    QByteArray  data = ui->textEdit_display->toPlainText().toUtf8();
//    data = QByteArray::fromHex(data);

//    m_udpsocket->writeDatagram((char *)buff,18*4, QHostAddress(ip), port);
//    //m_udpsocket->writeDatagram(buff, QHostAddress(ip), port);
//}




//void AD9361Widget::on_pushButton_cat_clicked()
//{
//    buff[1] = 2;
//    //先获取对方的IP和端口
//    QString ip = ui->lineEditIp->text();
//    qint16 port = ui->lineEditPort->text().toInt();
//    m_udpsocket->writeDatagram((char *)buff,18*4, QHostAddress(ip), port);
//}





void AD9361Widget::on_pushButton_readpara_clicked()
{
    int count;
    int i = 0;
    QMap<QString,QString>::const_iterator item;

    if(ui->Buttonconcect->text() == "DisConnect")
    {

    count = Flag.size();

    if(count == 0)
    {
        count = state.size();
        for(QMap<QString,QString>::const_iterator it =state.constBegin();it != state.constEnd();it++)
        {

           msg[i].count = count;
           strcpy(msg[i].name,(it.key()).toLatin1().data());
           msg[i].mode = 'r';
           msg[i].flag = 's';
           msg[i].ack = '0';
           strcpy(msg[i].strval,(it.value()+'\n').toLatin1().data());
           msg[i].num = it.value().toLongLong();

           qDebug()<<msg[i].count<< it.key()<<msg[i].mode<<msg[i].flag<<msg[i].ack<<msg[i].strval<<msg[i].num<<endl;

           i++;
        }
    }
    else
    {

        for(QMap<QString,int>::const_iterator it =Flag.constBegin();it != Flag.constEnd();it++)
        {
           //qDebug()<<it.key()<<" "<<it.value();

           item = state.constFind(it.key());

           msg[i].count = count;
           strcpy(msg[i].name,(item.key()).toLatin1().data());
           msg[i].mode = 'r';
           msg[i].flag = 's';
           msg[i].ack = '0';
           strcpy(msg[i].strval,(item.value()+'\n').toLatin1().data());
           msg[i].num = item.value().toLongLong();


           qDebug()<<msg[i].count<< item.key()<<msg[i].mode<<msg[i].flag<<msg[i].ack<<msg[i].strval<<msg[i].num<<endl;
           i++;
        }

        Flag.clear();
    }


    //先获取对方的IP和端口
    QString ip = ui->lineEditIp->text();
    qint16 port = ui->lineEditPort->text().toInt();

    m_udpsocket->writeDatagram(reinterpret_cast<char *>(msg),  sizeof(msg[0])*count, QHostAddress(ip), port);
    }
    else
    {
        QMessageBox::critical(this,"错误","未连接网口，无法查询！");
    }
}

void AD9361Widget::on_pushButton_config_clicked()
{
    int count;
    int i = 0;
    QMap<QString,QString>::const_iterator item;

    if(ui->Buttonconcect->text() == "DisConnect")
    {

    count = Flag.size();
    qDebug()<<count<<endl;
    if(count == 0)
    {
        count = state.size();
        for(QMap<QString,QString>::const_iterator it =state.constBegin();it != state.constEnd();it++)
        {
           qDebug()<<it.key()<<" "<<it.value();

           //item = state.constFind(it.key());
           msg[i].count = count;
           strcpy(msg[i].name,(it.key()).toLatin1().data());
           msg[i].mode = 'w';
           msg[i].flag = 's';
           msg[i].ack = '0';
           strcpy(msg[i].strval,(it.value()+'\n').toLatin1().data());
           msg[i].num = it.value().toLongLong();

           qDebug()<<msg[i].count<< it.key()<<msg[i].mode<<msg[i].flag<<msg[i].ack<<msg[i].strval<<msg[i].num<<endl;

           i++;
        }
    }
    else
    {
        qDebug()<<count;

        for(QMap<QString,int>::const_iterator it =Flag.constBegin();it != Flag.constEnd();it++)
        {
           //qDebug()<<it.key()<<" "<<it.value();

           item = state.constFind(it.key());

           msg[i].count = count;
           strcpy(msg[i].name,(item.key()).toLatin1().data());
           msg[i].mode = 'w';
           msg[i].flag = 's';
           msg[i].ack = '0';
           strcpy(msg[i].strval,(item.value()+'\n').toLatin1().data());
           msg[i].num = item.value().toLongLong();


           qDebug()<<msg[i].count<< item.key()<<msg[i].mode<<msg[i].flag<<msg[i].ack<<msg[i].strval<<msg[i].num<<endl;
           i++;
        }

        Flag.clear();

    }



    //先获取对方的IP和端口
    QString ip = ui->lineEditIp->text();
    qint16 port = ui->lineEditPort->text().toInt();

//    //获取编辑区内容
//    QString str = ui->textEdit_display->toPlainText();
//    //count = str.size();
//    //给指定的IP发送数据
//    QByteArray  data = ui->textEdit_display->toPlainText().toUtf8();

    int ret=sizeof(msg[0]);

    qDebug()<<ret<<endl;

    m_udpsocket->writeDatagram(reinterpret_cast<char *>(msg),  sizeof(msg[0])*count, QHostAddress(ip), port);

    }
    else
    {
        QMessageBox::critical(this,"错误","未连接网口，无法配置！");
    }
}

void AD9361Widget::on_pushButton_check_clicked()
{
    if(ui->Buttonconcect->text() == "DisConnect")
    {
      msg[0].count = 1;
      strcpy(msg[0].name,"name");
      msg[0].mode = 'c';
      msg[0].flag = 's';
      msg[0].ack = '0';
      strcpy(msg[0].strval,"0");
      msg[0].num = 0;

      //先获取对方的IP和端口
      QString ip = ui->lineEditIp->text();
      qint16 port = ui->lineEditPort->text().toInt();

      m_udpsocket->writeDatagram(reinterpret_cast<char *>(msg),  sizeof(msg[0]), QHostAddress(ip), port);

    }
    else
    {
        QMessageBox::critical(this,"错误","未连接网口，无法检测！");
    }
}

void AD9361Widget::on_pushButton_cleardisplay_clicked()
{
    ui->textEdit_display->clear();
}

void AD9361Widget::on_pushButton_clicked()
{
    QMessageBox::information(this,"Information","本地振荡器控制 LO\n"
                             "AD9361 收发器包含两个相同的 RFPLL 合成器，用于生成所需的 LO 信号。一个是为 RX 通道编程的，另一个是为 TX 通道编程的。该驱动器支持的调谐范围为 70MHz 至 6GHz（AD9363：325-3800 MHz），调谐粒度为 2Hz。\n"
                             "RX 信号路径:\n 此 IIO 设备属性允许用户在 521KSPS 到 61.44MSPS 的范围内控制以 Hz 为单位的基带 (BB) 采样率\n"
                             "TX 信号路径:\n 此 IIO 设备属性允许用户在 218KSPS 到 61.44MSPS 的范围内控制以 Hz 为单位的基带 (BB) 采样率\n"
                             "RX Gain Control:默认ADI提供的优化增益表在全表模式下提供 77 个条目，在拆分增益表模式下提供 41 个。每个索引通常导致 1 dB单调增益步长。共有 3 个不同的表格可用于不同的 RX LO 频率范围。驱动程序会根据 RX LO 设置自动换入和换出表格。\n"
                             "标准表：\n"
                             "RX LO 范围	       最小增益	最大增益\n"
                             "70 … 1300兆赫	    -1分贝	 73分贝\n"
                             "1300 … 4000兆赫	-3分贝	 71分贝\n"
                             "4000 … 6000兆赫	-10分贝	 62分贝\n"
                             "模式	        描述\n"
                             "manual	    在 MGC 模式下，BBP 控制增益索引指针。在最简单的形式中，BBP 评估 I/O 端口的数字信号电平，然后适当地调整增益。BBP 可以通过两种方式之一控制手动增益。默认方法使用SPI写入（写入 in_voltage[0,1]_hardwaregain）以dB 为单位的总增益。这会根据加载的增益表产生不同的增益指数。或者，BBP 可以脉冲控制输入引脚以移动增益指数。\n"
                             "slow_attack	慢速攻击模式适用于缓慢变化的信号，例如在 WCDMA 和 FDD LTE 等一些 FDD 应用中发现的信号。慢速攻击 AGC 使用带有滞后的二阶控制环路，它改变增益以将平均信号功率保持在可编程窗口内\n"
                             "hybrid        混合模式与慢速 AGC 模式相同，只是不使用增益更新计数器。相反，当 BBP 将 CTRL_IN2 信号拉高时会发生增益更新。“混合”术语的出现是因为 BBP 已经从 AD9361 中夺走了对算法的一些控制，因此增益控制不再是完全自动的。\n"
                             "fast_attack	快速攻击模式适用于“突发”开和关的波形，例如 TDD 应用或 GSM/EDGE FDD 应用中的波形。AGC 在突发开始时对过载做出非常快速的响应，因此 AGC 可以在信号的数据部分到达时稳定到最佳增益指数。\n"
                             );
}


void AD9361Widget::on_comboBox_RX0_GainMode_currentIndexChanged(const QString &arg1)
{
    qDebug()<<arg1<<endl;
    state.insert("in_voltage0_gain_control_mode",arg1);
    Flag.insert("in_voltage0_gain_control_mode",1);

}

void AD9361Widget::on_comboBox_RX1_GainMode_currentIndexChanged(const QString &arg1)
{
    qDebug()<<arg1<<endl;
    state.insert("in_voltage1_gain_control_mode",arg1);
    Flag.insert("in_voltage1_gain_control_mode",1);
}
