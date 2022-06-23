QT       += core gui
QT       += serialport
QT       += network
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=                          \
    CustomTabStyle/customtabstyle.cpp \
    ad9361/ad9361widget.cpp         \
    ad9361config/ad9361main.cpp     \
    errcheck/errorratepaint.cpp     \
    main.cpp                        \
    mainwindow/mainwindow.cpp       \
    netwidget/netwidget.cpp         \
    serialwidgt/serialwidgt.cpp     \
    label/qmylabel.cpp              \
    channel_simulator/channel_simulator.cpp

HEADERS +=                          \
    CustomTabStyle/customtabstyle.h \
    ad9361/ad9361widget.h           \
    ad9361config/ad9361main.h       \
    errcheck/errorratepaint.h       \
    mainwindow/mainwindow.h         \
    netwidget/netwidget.h           \
    serialwidgt/serialwidgt.h       \
    label/qmylabel.h                \
    channel_simulator/channel_simulator.h

FORMS +=                            \
    ad9361/ad9361widget.ui          \
    ad9361config/ad9361main.ui      \
    errcheck/errorratepaint.ui      \
    mainwindow/mainwindow.ui        \
    netwidget/netwidget.ui          \
    serialwidgt/serialwidgt.ui      \
    channel_simulator/channel_simulator.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
###############################################################
###############################################################
DESTDIR     = $$PWD/bin
TARGET      = demoo
TEMPLATE    = app
RC_FILE     = qrc/main.rc

RCONFIG      += warn_off
###RC_FILE     = comtool/qrc/main.rc
###############################################################
###############################################################
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/ad9361
INCLUDEPATH += $$PWD/ad9361config
INCLUDEPATH += $$PWD/errcheck
INCLUDEPATH += $$PWD/label
INCLUDEPATH += $$PWD/mainwindow
INCLUDEPATH += $$PWD/netwidget
INCLUDEPATH += $$PWD/serialwidgt
INCLUDEPATH += $$PWD/CustomTabStyle
INCLUDEPATH += $$PWD/nettool/netapi
INCLUDEPATH += $$PWD/nettool/netform
INCLUDEPATH += $$PWD/comtool/comapi
INCLUDEPATH += $$PWD/comtool/comform
INCLUDEPATH += $$PWD/comtool/3rd_qextserialport
INCLUDEPATH += $$PWD/channel_simulator

###INCLUDEPATH += $$PWD/3rd_qextserialport
###include ($$PWD/3rd_qextserialport/3rd_qextserialport.pri)
RESOURCES   += $$PWD/qrc/main.qrc
###RESOURCES   += $$PWD/comtool/qrc/main.qrc
###############################################################
###############################################################
###############################################################
########################## nettool ############################
greaterThan(QT_MAJOR_VERSION, 4) {
        #判断是否有websocket模块
        qtHaveModule(websockets) {
        QT += websockets
        DEFINES += websocket }
}

HEADERS += $$PWD/nettool/netapi/netappconfig.h \
    $$PWD/nettool/netapi/netappdata.h \
    $$PWD/nettool/netapi/netquihelper.h \
    $$PWD/nettool/netapi/netquihelperdata.h
HEADERS += $$PWD/nettool/netapi/nettcpclient.h
HEADERS += $$PWD/nettool/netapi/nettcpserver.h
HEADERS += $$PWD/channel_simulator/channel_simulator.h

SOURCES += $$PWD/nettool/netapi/netappconfig.cpp \
    $$PWD/nettool/netapi/netappdata.cpp \
    $$PWD/nettool/netapi/netquihelper.cpp \
    $$PWD/nettool/netapi/netquihelperdata.cpp
SOURCES += $$PWD/nettool/netapi/nettcpclient.cpp
SOURCES += $$PWD/nettool/netapi/nettcpserver.cpp

contains(DEFINES, websocket) {
HEADERS += $$PWD/nettool/netapi/netwebclient.h
HEADERS += $$PWD/nettool/netapi/netwebserver.h

SOURCES += $$PWD/nettool/netapi/netwebclient.cpp
SOURCES += $$PWD/nettool/netapi/netwebserver.cpp
}
HEADERS     +=  $$PWD/nettool/nethead.h

FORMS += $$PWD/nettool/netform/netfrmmain.ui
FORMS += $$PWD/nettool/netform/netfrmtcpclient.ui
FORMS += $$PWD/nettool/netform/netfrmtcpserver.ui
FORMS += $$PWD/nettool/netform/netfrmudpclient.ui
FORMS += $$PWD/nettool/netform/netfrmudpserver.ui

HEADERS += $$PWD/nettool/netform/netfrmmain.h
HEADERS += $$PWD/nettool/netform/netfrmtcpclient.h
HEADERS += $$PWD/nettool/netform/netfrmtcpserver.h
HEADERS += $$PWD/nettool/netform/netfrmudpclient.h
HEADERS += $$PWD/nettool/netform/netfrmudpserver.h

SOURCES += $$PWD/nettool/netform/netfrmmain.cpp
SOURCES += $$PWD/nettool/netform/netfrmtcpclient.cpp
SOURCES += $$PWD/nettool/netform/netfrmtcpserver.cpp
SOURCES += $$PWD/nettool/netform/netfrmudpclient.cpp
SOURCES += $$PWD/nettool/netform/netfrmudpserver.cpp

contains(DEFINES, websocket) {
FORMS   += $$PWD/nettool/netform/netfrmwebclient.ui
FORMS   += $$PWD/nettool/netform/netfrmwebserver.ui

HEADERS += $$PWD/nettool/netform/netfrmwebclient.h
HEADERS += $$PWD/nettool/netform/netfrmwebserver.h

SOURCES += $$PWD/nettool/netform/netfrmwebclient.cpp
SOURCES += $$PWD/nettool/netform/netfrmwebserver.cpp
}
###include ($$PWD/nettool/netapi/netapi.pri)
###include ($$PWD/nettool/netform/netform.pri)
###############################################################
###############################################################
###############################################################
########################## comtool ############################
HEADERS += \
    $$PWD/comtool/comapi/comappconfig.h \
    $$PWD/comtool/comapi/comappdata.h \
    $$PWD/comtool/comapi/comquihelper.h \
    $$PWD/comtool/comapi/comquihelperdata.h

SOURCES += \
    $$PWD/comtool/comapi/comappconfig.cpp \
    $$PWD/comtool/comapi/comappdata.cpp \
    $$PWD/comtool/comapi/comquihelper.cpp \
    $$PWD/comtool/comapi/comquihelperdata.cpp

FORMS += \
    $$PWD/comtool/comform/comfrmcomtool.ui

HEADERS += \
    $$PWD/comtool/comform/comfrmcomtool.h

SOURCES += \
    $$PWD/comtool/comform/comfrmcomtool.cpp

HEADERS     += $$PWD/comtool/comhead.h

HEADERS += \
    $$PWD/comtool/3rd_qextserialport/qextserialport.h \
    $$PWD/comtool/3rd_qextserialport/qextserialport_global.h \
    $$PWD/comtool/3rd_qextserialport/qextserialport_p.h

SOURCES += $$PWD/comtool/3rd_qextserialport/qextserialport.cpp

win32:SOURCES += $$PWD/comtool/3rd_qextserialport/qextserialport_win.cpp
unix:SOURCES += $$PWD/comtool/3rd_qextserialport/qextserialport_unix.cpp

###include ($$PWD/comtool/comapi/comapi.pri)
###include ($$PWD/comtool/comform/comform.pri)
###include ($$PWD/comtool/3rd_qextserialport/3rd_qextserialport.pri)
###############################################################
###############################################################
