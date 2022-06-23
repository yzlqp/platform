FORMS += $$PWD/netfrmmain.ui
FORMS += $$PWD/netfrmtcpclient.ui
FORMS += $$PWD/netfrmtcpserver.ui
FORMS += $$PWD/netfrmudpclient.ui
FORMS += $$PWD/netfrmudpserver.ui

HEADERS += $$PWD/netfrmmain.h
HEADERS += $$PWD/netfrmtcpclient.h
HEADERS += $$PWD/netfrmtcpserver.h
HEADERS += $$PWD/netfrmudpclient.h
HEADERS += $$PWD/netfrmudpserver.h

SOURCES += $$PWD/netfrmmain.cpp
SOURCES += $$PWD/netfrmtcpclient.cpp
SOURCES += $$PWD/netfrmtcpserver.cpp
SOURCES += $$PWD/netfrmudpclient.cpp
SOURCES += $$PWD/netfrmudpserver.cpp

contains(DEFINES, websocket) {
FORMS   += $$PWD/netfrmwebclient.ui
FORMS   += $$PWD/netfrmwebserver.ui

HEADERS += $$PWD/netfrmwebclient.h
HEADERS += $$PWD/netfrmwebserver.h

SOURCES += $$PWD/netfrmwebclient.cpp
SOURCES += $$PWD/netfrmwebserver.cpp
}
