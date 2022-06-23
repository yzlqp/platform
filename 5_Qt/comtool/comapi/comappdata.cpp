#include "comappdata.h"
#include "comquihelper.h"



QStringList ComAppData::Intervals = QStringList();
QStringList ComAppData::Datas = QStringList();
QStringList ComAppData::Keys = QStringList();
QStringList ComAppData::Values = QStringList();

QString ComAppData::SendFileName = "send.txt";
void ComAppData::readSendData()
{
    //读取发送数据列表
    ComAppData::Datas.clear();
    QString fileName = QString("%1/%2").arg(ComQUIHelper::appPath()).arg(ComAppData::SendFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                ComAppData::Datas.append(line);
            }
        }

        file.close();
    }

    //没有的时候主动添加点免得太空
    if (ComAppData::Datas.count() == 0) {
        ComAppData::Datas << "16 FF 01 01 E0 E1" << "16 FF 01 01 E1 E2";
    }
}

QString ComAppData::DeviceFileName = "device.txt";
void ComAppData::readDeviceData()
{
    //读取转发数据列表
    ComAppData::Keys.clear();
    ComAppData::Values.clear();
    QString fileName = QString("%1/%2").arg(ComQUIHelper::appPath()).arg(ComAppData::DeviceFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                QStringList list = line.split(";");
                QString key = list.at(0);
                QString value;
                for (int i = 1; i < list.count(); i++) {
                    value += QString("%1;").arg(list.at(i));
                }

                //去掉末尾分号
                value = value.mid(0, value.length() - 1);
                ComAppData::Keys.append(key);
                ComAppData::Values.append(value);
            }
        }

        file.close();
    }
}

void ComAppData::saveData(const QString &data)
{
    if (data.length() <= 0) {
        return;
    }

    QString fileName = QString("%1/%2.txt").arg(ComQUIHelper::appPath()).arg(STRDATETIME);
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        file.write(data.toUtf8());
        file.close();
    }
}

void ComAppData::loadIP(QComboBox *cbox)
{
    //获取本机所有IP
    static QStringList ips;
    if (ips.count() == 0) {
#ifdef emsdk
        ips << "127.0.0.1";
#else
        QList<QNetworkInterface> netInterfaces = QNetworkInterface::allInterfaces();
        foreach (const QNetworkInterface  &netInterface, netInterfaces) {
            //移除虚拟机和抓包工具的虚拟网卡
            QString humanReadableName = netInterface.humanReadableName().toLower();
            if (humanReadableName.startsWith("vmware network adapter") || humanReadableName.startsWith("npcap loopback adapter")) {
                continue;
            }

            //过滤当前网络接口
            bool flag = (netInterface.flags() == (QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanBroadcast | QNetworkInterface::CanMulticast));
            if (flag) {
                QList<QNetworkAddressEntry> addrs = netInterface.addressEntries();
                foreach (QNetworkAddressEntry addr, addrs) {
                    //只取出IPV4的地址
                    if (addr.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                        QString ip4 = addr.ip().toString();
                        if (ip4 != "127.0.0.1") {
                            ips << ip4;
                        }
                    }
                }
            }
        }
#endif
    }

    cbox->clear();
    cbox->addItems(ips);
    if (!ips.contains("127.0.0.1")) {
        cbox->addItem("127.0.0.1");
    }
}
