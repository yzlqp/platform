#include "mainwindow.h"
#include <QApplication>
#include "netquihelper.h"
#include "comquihelper.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));

    //Net设置编码+字体+中文翻译文件
    QUIHelper::initAll();
    //读取配置文件
    AppConfig::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    AppConfig::readConfig();
    AppData::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    AppData::readSendData();
    AppData::readDeviceData();

    //Com编码+字体+中文翻译文件
    ComQUIHelper::initAll();
    //读取配置文件
    ComAppConfig::ConfigFile = QString("%1/%2.ini").arg(ComQUIHelper::appPath()).arg(ComQUIHelper::appName());
    ComAppConfig::readConfig();
    ComAppData::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    ComAppData::readSendData();
    ComAppData::readDeviceData();

    MainWindow w;
    w.setWindowTitle("Debug Tools V1");
    w.resize(800, 700);
    QUIHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}


