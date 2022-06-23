#include "comappconfig.h"
#include "comquihelper.h"


QString ComAppConfig::ConfigFile = "config.ini";
QString ComAppConfig::SendFileName = "send.txt";
QString ComAppConfig::DeviceFileName = "device.txt";

QString ComAppConfig::PortName = "COM1";
int ComAppConfig::BaudRate = 9600;
int ComAppConfig::DataBit = 8;
QString ComAppConfig::Parity = QString::fromUtf8("无");
double ComAppConfig::StopBit = 1;

bool ComAppConfig::HexSend = false;
bool ComAppConfig::HexReceive = false;
bool ComAppConfig::Debug = false;
bool ComAppConfig::AutoClear = false;

bool ComAppConfig::AutoSend = false;
int ComAppConfig::SendInterval = 1000;
bool ComAppConfig::AutoSave = false;
int ComAppConfig::SaveInterval = 5000;

QString ComAppConfig::Mode = "Tcp_Client";
QString ComAppConfig::ServerIP = "127.0.0.1";
int ComAppConfig::ServerPort = 6000;
int ComAppConfig::ListenPort = 6000;
int ComAppConfig::SleepTime = 100;
bool ComAppConfig::AutoConnect = false;

void ComAppConfig::readConfig()
{
    QSettings set(ComAppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    ComAppConfig::PortName = set.value("PortName", ComAppConfig::PortName).toString();
    ComAppConfig::BaudRate = set.value("BaudRate", ComAppConfig::BaudRate).toInt();
    ComAppConfig::DataBit = set.value("DataBit", ComAppConfig::DataBit).toInt();
    ComAppConfig::Parity = set.value("Parity", ComAppConfig::Parity).toString();
    ComAppConfig::StopBit = set.value("StopBit", ComAppConfig::StopBit).toInt();

    ComAppConfig::HexSend = set.value("HexSend", ComAppConfig::HexSend).toBool();
    ComAppConfig::HexReceive = set.value("HexReceive", ComAppConfig::HexReceive).toBool();
    ComAppConfig::Debug = set.value("Debug", ComAppConfig::Debug).toBool();
    ComAppConfig::AutoClear = set.value("AutoClear", ComAppConfig::AutoClear).toBool();

    ComAppConfig::AutoSend = set.value("AutoSend", ComAppConfig::AutoSend).toBool();
    ComAppConfig::SendInterval = set.value("SendInterval", ComAppConfig::SendInterval).toInt();
    ComAppConfig::AutoSave = set.value("AutoSave", ComAppConfig::AutoSave).toBool();
    ComAppConfig::SaveInterval = set.value("SaveInterval", ComAppConfig::SaveInterval).toInt();
    set.endGroup();

    set.beginGroup("NetConfig");
    ComAppConfig::Mode = set.value("Mode", ComAppConfig::Mode).toString();
    ComAppConfig::ServerIP = set.value("ServerIP", ComAppConfig::ServerIP).toString();
    ComAppConfig::ServerPort = set.value("ServerPort", ComAppConfig::ServerPort).toInt();
    ComAppConfig::ListenPort = set.value("ListenPort", ComAppConfig::ListenPort).toInt();
    ComAppConfig::SleepTime = set.value("SleepTime", ComAppConfig::SleepTime).toInt();
    ComAppConfig::AutoConnect = set.value("AutoConnect", ComAppConfig::AutoConnect).toBool();
    set.endGroup();

    //配置文件不存在或者不全则重新生成
    if (!ComQUIHelper::checkIniFile(ComAppConfig::ConfigFile)) {
        writeConfig();
        return;
    }
}

void ComAppConfig::writeConfig()
{    
    QSettings set(ComAppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    set.setValue("PortName", ComAppConfig::PortName);
    set.setValue("BaudRate", ComAppConfig::BaudRate);
    set.setValue("DataBit", ComAppConfig::DataBit);
    set.setValue("Parity", ComAppConfig::Parity);
    set.setValue("StopBit", ComAppConfig::StopBit);

    set.setValue("HexSend", ComAppConfig::HexSend);
    set.setValue("HexReceive", ComAppConfig::HexReceive);
    set.setValue("Debug", ComAppConfig::Debug);
    set.setValue("AutoClear", ComAppConfig::AutoClear);

    set.setValue("AutoSend", ComAppConfig::AutoSend);
    set.setValue("SendInterval", ComAppConfig::SendInterval);
    set.setValue("AutoSave", ComAppConfig::AutoSave);
    set.setValue("SaveInterval", ComAppConfig::SaveInterval);
    set.endGroup();

    set.beginGroup("NetConfig");
    set.setValue("Mode", ComAppConfig::Mode);
    set.setValue("ServerIP", ComAppConfig::ServerIP);
    set.setValue("ServerPort", ComAppConfig::ServerPort);
    set.setValue("ListenPort", ComAppConfig::ListenPort);
    set.setValue("SleepTime", ComAppConfig::SleepTime);
    set.setValue("AutoConnect", ComAppConfig::AutoConnect);
    set.endGroup();
}
