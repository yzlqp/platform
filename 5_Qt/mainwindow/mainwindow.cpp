#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ad9361widget.h"
#include "customtabstyle.h"
#include "errorratepaint.h"
#include "qmylabel.h"
#include "netwidget.h"
#include "serialwidgt.h"
#include "ui_serialwidgt.h"
#include "ui_netwidget.h"
#include "ui_errorratepaint.h"
#include "ui_ad9361widget.h"
#include "netfrmmain.h"
#include "comfrmcomtool.h"
#include "ad9361main.h"
#include "channel_simulator.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->initPlatform();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initPlatform()
{
    ui->tabWidget->setTabPosition(QTabWidget::West);
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);

    ui->tabWidget->addTab(new frmMain, "Net");
    ui->tabWidget->addTab(new frmComTool, "Serial");
    ui->tabWidget->addTab(new AD9361Widget, "AD9361");
    ui->tabWidget->addTab(new channel_simulator, "Channel");

    //ui->tabWidget->addTab(new ad9361main, "new9361");
    //ui->tabWidget->addTab(new SerialWidgt, "串口助手");
    //ui->tabWidget->addTab(new NetWidget, "网口助手");
    //ui->tabWidget->addTab(new ErrorRatePaint, "ErrCheck");
}










/*MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SerialWidgt_ui = new SerialWidgt();
    NetWidget_ui   = new NetWidget();
    ErrorRatePaint_ui   = new ErrorRatePaint();
    AD9361Widget_ui =   new AD9361Widget();



    ui->tabWidget->insertTab(0,SerialWidgt_ui," 串口助手");
    ui->tabWidget->insertTab(1,NetWidget_ui,"网口助手");
    //ui->tabWidget->removeTab(2);
    //ui->tabWidget->removeTab(2);
    ui->tabWidget->insertTab(2,ErrorRatePaint_ui,"误码率曲线图");
    ui->tabWidget->insertTab(3,AD9361Widget_ui,"AD9361配置");

    ui->tabWidget->setCurrentIndex(0);


//    tabWidget->setAttribute(Qt::WA_StyledBackground);
//    tabWidget->setStyleSheet("QTabWidget#tabWidget{background-color:rgb(196,196,196);}");



//    tabWidget->insertTab(0,SerialWidgt_ui,"SerialWidgt_ui");

//    tabWidget->setGeometry(0,0,800,600);


//    QHBoxLayout *layout = new QHBoxLayout();
//    layout->addWidget(tabWidget);



//     this->setLayout(layout);


    SerialWidgt_ui->SerialWidgt::initSerialPort();
    NetWidget_ui->initNet();
    ErrorRatePaint_ui->initmychart();
    AD9361Widget_ui->initAD9361();


    connect(AD9361Widget_ui,SIGNAL(updateSignal_clicked()),this,SLOT(send_refresh_order()));

    connect(SerialWidgt_ui,SIGNAL(receiveSerialData_flag()),this,SLOT(readdata()));

    connect(AD9361Widget_ui,SIGNAL(signal_cfg_clicked()),this,SLOT(sendorder()));
}*/



/*void MainWindow::sendorder()
{
    if(ui->tabWidget->currentIndex()==3)
    {
    QByteArray data3 = AD9361Widget_ui->order_str.toUtf8();
    qDebug() << data3;
    SerialWidgt_ui->sendData(data3);
    }
   QMessageBox::critical(this, "tips", "配置成功", "确定");
}
void MainWindow::send_refresh_order()
{
    if(ui->tabWidget->currentIndex()==3)
    {
   QString data4={"cd /sys/bus/iio/devices/iio:device0\r\n"
                  "cat out_altvoltage0_RX_LO_frequency\r\ncat out_altvoltage1_TX_LO_frequency\r\n"
                  "cat in_voltage_sampling_frequency\r\ncat out_voltage_sampling_frequency\r\n"
                  "cat in_voltage_rf_bandwidth\r\ncat out_voltage_rf_bandwidth\r\n"
                  "cat in_voltage0_hardwaregain\r\ncat in_voltage1_hardwaregain\r\n"};
   QByteArray data3 = data4.toUtf8();
   SerialWidgt_ui->sendData(data3);

    }

}
void MainWindow::readdata()
{
    if(ui->tabWidget->currentIndex()==3)
    {
    int i=0;
    QString data = NULL;
    qDebug()<<SerialWidgt_ui->temp_data;
    QString temp_data1 = QString(SerialWidgt_ui->temp_data);

    if('0'<=temp_data1.at(0)&& temp_data1.at(0)<='9')
    {
                data.clear();
                while (temp_data1.at(i) != "\r" ) {
                    data +=temp_data1.at(i);
                    i++;
                }
                qDebug()<<data;

                update_data.push_back(data);

    }
    if(update_data.size()==8)
    {
         int k = 0;
         for (QVector<QString>::const_iterator it=update_data.constBegin();it !=update_data.constEnd();it++)
         {
                     switch (k)
                     {
                     case 0:
                     {
                        AD9361Widget_ui->ui->label_LORX->setText(QString("RX通道：   当前值为：%1  %2").arg(*it).arg("Hz"));
                        break;
                     }
                     case 1:
                     {
                        AD9361Widget_ui->ui->label_LOTX->setText(QString("TX通道：   当前值为：%1  %2").arg(*it).arg("Hz"));
                        break;
                     }
                     case 2:
                     {
                        AD9361Widget_ui->ui->label_RX_SampleRate->setText(QString("RX采样率:   当前值为：%1  %2").arg(*it).arg("Hz"));
                        break;
                     }
                     case 3:
                     {
                        AD9361Widget_ui->ui->label_TX_SampleRate->setText(QString("TX采样率:   当前值为：%1  %2").arg(*it).arg("Hz"));
                        break;
                     }
                     case 4:
                     {
                        AD9361Widget_ui->ui->label_RX_BW->setText(QString("RX_BW:   当前值为：%1  %2").arg(*it).arg("Hz"));
                        break;
                     }
                     case 5:
                     {
                        AD9361Widget_ui->ui->label_TX_BW->setText(QString("TX_BW:   当前值为：%1  %2").arg(*it).arg("Hz"));
                        break;
                     }
                     case 6:
                     {
                        AD9361Widget_ui->ui->label_RX_Gain->setText(QString("RX增益:   当前值为：%1  %2").arg(*it).arg("dB"));
                        break;
                     }
                     case 7:
                     {
                        AD9361Widget_ui->ui->label_TX_Gain->setText(QString("TX增益:   当前值为：%1  %2").arg(*it).arg("dB"));
                        break;
                     }

                     }
                     k++;
          }

        update_data.clear();

            QMessageBox::critical(this, "tip", "刷新配置完成", "确定");

    }

}
}*/
