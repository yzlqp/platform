#include "channel_simulator.h"
#include "ui_channel_simulator.h"


#define NAMELEN 16
#define VALUELEN 16



int buff[32];
/*
buff[0] = 1;      //write 1 or  read  2
buff[1] = 0;
buff[2] = 0;
buff[3] = 0;
buff[4] = 8;      // data_num

buff[5] = 1638;   //data
buff[6] = 125;
buff[7] = -1;
buff[8] = 16;

buff[9] =  1024;
buff[10] = 1025;
buff[11] = 1026;
buff[12] = 1027;
*/



int p0_gain_fix = 0;
int p1_gain_fix = 0;
int p2_gain_fix = 0;
int p3_gain_fix = 0;

int p0_delay_fix  = 0;
int p1_delay_fix  = 0;
int p2_delay_fix  = 0;
int p3_delay_fix  = 0;

int norm_coef = 0;

int a = 0,b = 0 ,c = 0;
double d;

int angle = 0;


typedef struct {
    char name[NAMELEN];
    int mode;                   // mode == 0 则为读(R)，mode == 1 则为写(W)
    int ack;					// 用于mode==1时，PS返回PC所写值，并指示ack==1
    int value;       // mode == 0 则为读到的值，mode == 1 则为写到的值
}CS_msg;



channel_simulator::channel_simulator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::channel_simulator)
{
    ui->setupUi(this);
    initNet();
    channel_simulator_ui_init();

}

channel_simulator::~channel_simulator()
{
    delete ui;
}



void channel_simulator::channel_simulator_ui_init()
{
    setWindowTitle("Channel Simulator");

    ui->lineEdit_p0_3->setDisabled(true);
    ui->lineEdit_p0_4->setDisabled(true);
    ui->lineEdit_p0_5->setDisabled(true);

    ui->lineEdit_p1_3->setDisabled(true);
    ui->lineEdit_p1_4->setDisabled(true);
    ui->lineEdit_p1_5->setDisabled(true);

    ui->lineEdit_p2_3->setDisabled(true);
    ui->lineEdit_p2_4->setDisabled(true);
    ui->lineEdit_p2_5->setDisabled(true);

    ui->lineEdit_p3_3->setDisabled(true);
    ui->lineEdit_p3_4->setDisabled(true);
    ui->lineEdit_p3_5->setDisabled(true);

    ui->comboBoxNet->setCurrentIndex(2);

    ui->checkBox_p0->setCheckState(Qt::Checked);

}


void channel_simulator::on_pushButton_clear_clicked()
{

   ui->textEdit_display->clear();
}

void channel_simulator::display(int *data)
{
    ui->textEdit_display->append("多径参数:");
    QString gain = QString(" %1  %2  %3  %4").arg(data[6]).arg(data[8]).arg(data[10]).arg(data[12]);
    //qDebug()<<"gain:"<<gain<<endl;
    ui->textEdit_display->append("gain:"+gain);

    QString delay = QString(" %1  %2  %3  %4").arg(data[5]).arg(data[7]).arg(data[9]).arg(data[11]);
    //qDebug()<<"delay:"<<delay<<endl;
    ui->textEdit_display->append("delay:"+delay);

    QString norm_coef_fix = QString(" %1").arg(data[13]);
    //qDebug()<<"norm_coef:"<<norm_coef<<endl;
    ui->textEdit_display->append("norm_coef:"+norm_coef_fix);

    ui->textEdit_display->append("噪声参数:");
    QString para = QString(" a_para=%1   b_para=%2   c_para=%3").arg(data[14]).arg(data[15]).arg(data[16]);
    //qDebug()<<"para:"<<para<<endl;
    ui->textEdit_display->append("para:"+para);

    ui->textEdit_display->append("频偏参数:");
    QString angle_in = QString(" %1").arg(data[17]);

    //QByteArray angle_in1= angle_in.toUtf8();

    ui->textEdit_display->append("Foff_coef:"+angle_in);
}
void channel_simulator::on_pushButton_caculator_clicked()
{

   double p0_gain,p1_gain,p2_gain,p3_gain;
   p0_gain = ui->lineEdit_p0_gain->text().toDouble();
   p1_gain = ui->lineEdit_p1_gain->text().toDouble();
   p2_gain = ui->lineEdit_p2_gain->text().toDouble();
   p3_gain = ui->lineEdit_p3_gain->text().toDouble();


   //double p0_gain_d = pow(10,p0_gain/20)*255;
//   p0_gain_fix = qFloor(pow(10,p0_gain/20)*255);
//   p1_gain_fix = qFloor(pow(10,p1_gain/20)*255);
//   p2_gain_fix = qFloor(pow(10,p2_gain/20)*255);
//   p3_gain_fix = qFloor(pow(10,p3_gain/20)*255);


   double p0_delay,p1_delay,p2_delay,p3_delay;
   p0_delay = ui->lineEdit_p0_delay->text().toDouble();
   p1_delay = ui->lineEdit_p1_delay->text().toDouble();
   p2_delay = ui->lineEdit_p2_delay->text().toDouble();
   p3_delay = ui->lineEdit_p3_delay->text().toDouble();

   double Fs = ui->lineEdit_Fs->text().toDouble();
//   p0_delay_fix = qFloor(p0_delay*Fs);
//   p1_delay_fix = qFloor(p1_delay*Fs);
//   p2_delay_fix = qFloor(p2_delay*Fs);
//   p3_delay_fix = qFloor(p3_delay*Fs);

   if(ui->checkBox_p0->checkState() == 2)
   {
      p0_gain_fix = qFloor(pow(10,p0_gain/20)*255);
      p0_delay_fix = qFloor(p0_delay*Fs);
   }
   else
   {
      p0_gain_fix = 0;
      p0_delay_fix =0;
   }
   if(ui->checkBox_p1->checkState() == 2)
   {
      p1_gain_fix = qFloor(pow(10,p1_gain/20)*255);
      p1_delay_fix = qFloor(p1_delay*Fs);
   }
   else
   {
      p1_gain_fix = 0;
      p1_delay_fix =0;
   }
   if(ui->checkBox_p2->checkState() == 2)
   {
      p2_gain_fix = qFloor(pow(10,p2_gain/20)*255);
      p2_delay_fix = qFloor(p2_delay*Fs);
   }
   else
   {
      p2_gain_fix = 0;
      p2_delay_fix =0;
   }
   if(ui->checkBox_p3->checkState() == 2)
   {
       p3_gain_fix = qFloor(pow(10,p3_gain/20)*255);
       p3_delay_fix = qFloor(p3_delay*Fs);
   }
   else
   {
      p3_gain_fix = 0;
      p3_delay_fix =0;
   }

   norm_coef = qFloor((p0_gain_fix+p1_gain_fix+p2_gain_fix+p3_gain_fix));


   double Pn = ui->lineEdit_Pn->text().toDouble();
   double Ps = ui->lineEdit_Ps->text().toDouble();
   double snr = ui->lineEdit_SNR->text().toDouble();

   double temp1 = (p0_gain_fix*p0_gain_fix + p1_gain_fix*p1_gain_fix +p2_gain_fix*p2_gain_fix +p3_gain_fix*p3_gain_fix );
   double temp2 = norm_coef*norm_coef;
   double  temp3 = temp2/temp1;
   double ps =Ps/temp3;
   double data_max = ui->lineEdit_Data_Max->text().toDouble();

   d = data_max;

   if(snr<=-15)
       b = 1;
   else if(snr<=0)
       b = 4;
   else if(snr<=15)
       b = 32;
   else
       b = 127;

   a = qRound(b*sqrt(ps/(Pn*pow(10,(snr/10)))));

   //double x = sqrt(1+pow(0.1,(snr/10)));

//   if(snr<=10)
//       c = qRound(b*sqrt(1+pow(0.1,(snr/10)))*4*sqrt(2));
//   else if(snr<=15)
//       c = qRound(b *sqrt(1+pow(0.1,(snr/10)))*4);
//   else
//       c = qRound(b *sqrt(1+pow(0.1,(snr/10)))*2*sqrt(2));

   c = qRound((b*d + a*128)/2048);

   double fre_offset = ui->lineEdit_freoffset->text().toDouble();

   angle = qRound((360*fre_offset*16*2048)/(Fs*pow(10,6)));


   buff[0] = 0; //write 1 or  read  2
   buff[1] = 0;
   buff[2] = 0;
   buff[3] = 0;
   buff[4] = 13;

   buff[5] = p0_delay_fix  ;
   buff[6] = p0_gain_fix  ;
   buff[7] = p1_delay_fix ;
   buff[8] = p1_gain_fix  ;

   buff[9] =  p2_delay_fix;
   buff[10] = p2_gain_fix ;
   buff[11] = p3_delay_fix;
   buff[12] = p3_gain_fix ;

   buff[13] = norm_coef   ;
   buff[14] = b           ;
   buff[15] = a           ;
   buff[16] = c           ;

   buff[17] = angle;

   display(buff);
}


//网口初始化

void channel_simulator::initNet()
{
    m_tcpClientSocket = new QTcpSocket(this);
    m_udpsocket = new QUdpSocket(this);



    //显示传输协议类型
    QString s = ui->comboBoxNet->currentText();
    qDebug()<<s;

    connect(m_udpsocket,&QUdpSocket::readyRead,this,&channel_simulator::dealMsg);
    //协议类型改变时
    connect(ui->comboBoxNet,SIGNAL(currentIndexChanged(int)),this, SLOT(updateConfig()));
    //TCP客户端连接上
    connect(m_tcpClientSocket, SIGNAL(connected()), this, SLOT(tcpClientConnected()));

    //TCP客户端接收数据
    connect(m_tcpClientSocket, SIGNAL(readyRead()), this, SLOT(receiveTcpData()));
    //TCP服务端接收到连接请求
    connect(&m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcpServerRecvConnection()));

}


void channel_simulator::dealMsg()
{
    int buf[1024]={0},i=0;
    QHostAddress cliAddr;
    quint16 port;
    quint64 len = m_udpsocket->readDatagram((char *)buf,sizeof(buf),&cliAddr,&port);

    if(len>0)
    {
        //格式化 [192.68.2.2:8888]aaaa
        QString str = QString("[%1:%2]%3")
                .arg(cliAddr.toString())
                .arg(port)
                .arg("查询结果如下：");
        //给编辑区设置内容
        ui->textEdit_display->append(str);
    }

    for(i = 0 ; i<(int)len/4 ;i++)
    {
        qDebug()<<buf[i];

     }

     display(buf);

}




void channel_simulator::on_Buttonconcect_clicked()
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

void channel_simulator::on_comboBoxNet_currentIndexChanged(int index)
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

void channel_simulator::on_pushButton_config_clicked()
{
    int count;

    if(ui->Buttonconcect->text() == "DisConnect")
    {
    buff[0] = 2;

    //先获取对方的IP和端口
    QString ip = ui->lineEditIp->text();
    qint16 port = ui->lineEditPort->text().toInt();

    //获取编辑区内容
    QString str = ui->textEdit_display->toPlainText();
    count = str.size();
    //给指定的IP发送数据
    QByteArray  data = ui->textEdit_display->toPlainText().toUtf8();
    data = QByteArray::fromHex(data);

    m_udpsocket->writeDatagram((char *)buff,18*4, QHostAddress(ip), port);
    //m_udpsocket->writeDatagram(buff, QHostAddress(ip), port);
    }
    else
    {
     QMessageBox::critical(this,"错误","未连接网口，无法配置！");
    }
}




void channel_simulator::on_pushButton_cat_clicked()
{
    if(ui->Buttonconcect->text() == "DisConnect")
    {
    buff[0] = 1;
    //先获取对方的IP和端口
    QString ip = ui->lineEditIp->text();
    qint16 port = ui->lineEditPort->text().toInt();
    m_udpsocket->writeDatagram((char *)buff,18*4, QHostAddress(ip), port);
    }
    else
    {
     QMessageBox::critical(this,"错误","未连接网口，无法查询！");
    }
}
