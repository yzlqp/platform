#include "netwidget.h"
#include "ui_netwidget.h"
#include <QMessageBox>
#include <QTime>

NetWidget::NetWidget(QWidget *parent) : QWidget(parent), ui(new Ui::NetWidget)
{
    ui->setupUi(this);
}

NetWidget::~NetWidget()
{
    delete ui;
}


//网口初始化
void NetWidget::initNet()
{
    m_tcpClientSocket = new QTcpSocket(this);
    m_udpsocket = new QUdpSocket(this);

    //网络收发数据统计
    m_nNetSendCount = 0;
    m_nNetRecvCount = 0;

    //显示传输协议类型
    QString s = ui->comboBoxNet->currentText();
    qDebug()<<s;

    connect(m_udpsocket,&QUdpSocket::readyRead,this,&NetWidget::dealMsg);
    //协议类型改变时
    connect(ui->comboBoxNet,SIGNAL(currentIndexChanged(int)),this, SLOT(updateConfig()));
    //TCP客户端连接上
    connect(m_tcpClientSocket, SIGNAL(connected()), this, SLOT(tcpClientConnected()));

    //TCP客户端接收数据
    connect(m_tcpClientSocket, SIGNAL(readyRead()), this, SLOT(receiveTcpData()));
    //TCP服务端接收到连接请求
    connect(&m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcpServerRecvConnection()));
    //清楚网口接收
    connect(ui->label_netrec, &QMyLabel::sg_clicked,this,[=]()
                    {
                        ui->textEdit_Netrec->clear();
                    });
    //清除网口输入
    connect(ui->label_netsend, &QMyLabel::sg_clicked,this,[=]()
                    {
                        ui->textEdit_Netsend->clear();
                    });
    //网络循环发送定时器
    connect(&m_netTimer, &QTimer::timeout, this, [=]
        {
            sendNetData();
        });
}


void NetWidget::dealMsg()
{
    char buf[1024]={0};
    QHostAddress cliAddr;
    quint16 port;
    quint64 len = m_udpsocket->readDatagram(buf,sizeof(buf),&cliAddr,&port);
    if(len>0)
    {
        //格式化 [192.68.2.2:8888]aaaa
        QString str = QString("[%1:%2] %3")
                .arg(cliAddr.toString())
                .arg(port)
                .arg(buf);
        //给编辑区设置内容
        ui->textEdit_Netrec->append(str);
    }

}


void NetWidget::updateConfig()
{
    int index = ui->comboBoxNet->currentIndex();
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

        ui->lineEditIp->setText("192.168.1.102");
        ui->lineEditPort->setText("7");
    }
}


//TCP客户端连接上
void NetWidget::tcpClientConnected()
{
     ui->textEdit_Netrec->setText("成功和服务器建立连接");
}


//TCP客户端/服务端接收数据
void NetWidget::receiveTcpData()
{
    QByteArray data;
    QByteArray temp2 = "";
    if(ui->comboBoxNet->currentIndex()==0)
    {
        data = m_tcpServerSocket->readAll();
    }
    else
    {
        data = m_tcpClientSocket->readAll();
    }
    //接收计数
    m_nNetRecvCount += data.count();
    ui->label_netRecvCount->setText(QString("接收：%1").arg(QString::number(m_nNetRecvCount)));

    //十六进制显示
        if (ui->checkBox_netShowHex->isChecked())
        {
            data = data.toHex();


            QByteArray temp = "";

            //testdata.clear();
            for (int i = 1; i <= data.length(); i++)
            {
                temp.append(data.at(i - 1));
                temp2.append(data.at(i - 1));
                if(ui->checkBox_datavector->isChecked())
                {

                    //testdata.push_back(data.at(i - 1));
                    //qDebug()<<data.at(i - 1);

                }

                if (i % 2 == 0)
                {
                    temp.append(" ");
                }
            }
            data = temp;
        }

    ui->textEdit_Netrec->moveCursor(QTextCursor::End);

    //ui->textEdit_Netrec->insertPlainText(data);

//    QString str = QString("[%1:%2] 成功连接").arg(ip).arg(portNum);
//    ui->textEdit_Netrec->setText(str); //显示到编辑区

    QTime ct = QTime::currentTime(); // 获取当前时间
    QString str1=data;
    QString str = QString("%1:\r\n%2").arg(ct.toString()).arg(str1);
    ui->textEdit_Netrec->append(str.toUtf8());
    if(!ui->checkBox_datavector->isChecked())
    {
        //Calculaterrornum(temp2);
    }
}


void NetWidget::sendNetData()
{
    //获取输入框内容
    QByteArray data = ui->textEdit_Netsend->toPlainText().toUtf8();
    //按十六进制发送
    if (ui->checkBox_netSendHex->isChecked())
    {
        data = QByteArray::fromHex(data);
    }
    int count;
    if (ui->comboBoxNet->currentIndex()==1)
    {
        count = m_tcpClientSocket->write(data);
    }
    else if(ui->comboBoxNet->currentIndex()==0)
    {
        count = m_tcpServerSocket->write(data);
    }
    else
    {
        if(NULL == m_udpsocket)
        {
            return;
        }
        //先获取对方的IP和端口
        QString ip = ui->lineEditIp->text();
        qint16 port = ui->lineEditPort->text().toInt();

        //获取编辑区内容
        QString str = ui->textEdit_Netsend->toPlainText();
        count = str.size();
        //给指定的IP发送数据

        QByteArray  data = ui->textEdit_Netsend->toPlainText().toUtf8().toHex();
        qDebug()<<data;
        m_udpsocket->writeDatagram(data, QHostAddress(ip), port);
    }
    //发送计数
    m_nNetSendCount += count;

    ui->label_netSendCount->setText(QString("发送：%1").arg(QString::number(m_nNetSendCount)));
}


//TCP服务端接收到连接请求
void  NetWidget::tcpServerRecvConnection()
{
    m_tcpServerSocket = m_tcpServer.nextPendingConnection();
    QString ip = m_tcpServerSocket->peerAddress().toString();
    int portNum = m_tcpServerSocket->peerPort();

    QString str = QString("[%1:%2] 成功连接").arg(ip).arg(portNum);
    ui->textEdit_Netrec->setText(str); //显示到编辑区

    connect(m_tcpServerSocket, SIGNAL(readyRead()), this, SLOT(receiveTcpData()));
}


void NetWidget::on_Buttonconcect_clicked()
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
            ui->textEdit_Netrec->append(str);
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


void NetWidget::on_pushButton_Netsend_clicked()
{
    if (ui->Buttonconcect->text() != "DisConnect")
    {
        QMessageBox::critical(this, "Warning", "网络尚未连接\n发送失败", "确定");
        return;
    }
    if(ui->checkBox_NetSendCycle->isChecked()&&ui->pushButton_Netsend->text()=="发送")
        {
        m_netTimer.setInterval(ui->lineEdit_interalNet->text().toInt());
        m_netTimer.start();

        ui->pushButton_Netsend->setText("停止发送");
    }
    else
    {
       m_netTimer.stop();
       ui->pushButton_Netsend->setText("发送");
    }
    sendNetData();
}


void NetWidget::on_checkBox_netSendHex_stateChanged(int state)
{
    switch (state)
        {
        case Qt::Unchecked:		//取消勾选
        {

                QByteArray str = ui->textEdit_Netsend->toPlainText().toLatin1();
                QString text = QByteArray::fromHex(str);
                ui->textEdit_Netsend->clear();

                ui->textEdit_Netsend->setText(text);
            break;

        }
        case Qt::Checked:
        {

                QByteArray str = ui->textEdit_Netsend->toPlainText().toLatin1();
                QByteArray text = str.toHex();
                ui->textEdit_Netsend->clear();

                QByteArray temp = "";
                for (int i = 1; i <= text.length(); i++)
                {
                    temp.append(text.at(i - 1));
                    if (i % 2 == 0)
                    {
                        temp.append(" ");
                    }
                }
                ui->textEdit_Netsend->setText(temp);
            break;
        }
        }
}


void NetWidget::on_pushButton_clicked()
{
    m_nNetSendCount = 0;
    m_nNetRecvCount = 0;
    ui->label_netSendCount->setText("发送：0");
    ui->label_netRecvCount->setText("接收：0");
}
