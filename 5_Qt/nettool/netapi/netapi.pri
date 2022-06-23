HEADERS += $$PWD/net/netappconfig.h \
    $$PWD/net/netappdata.h \
    $$PWD/net/netquihelper.h \
    $$PWD/net/netquihelperdata.h
HEADERS += $$PWD/net/nettcpclient.h
HEADERS += $$PWD/net/nettcpserver.h

SOURCES += $$PWD/net/netappconfig.cpp \
    $$PWD/net/netappdata.cpp \
    $$PWD/net/netquihelper.cpp \
    $$PWD/net/netquihelperdata.cpp
SOURCES += $$PWD/net/nettcpclient.cpp
SOURCES += $$PWD/net/nettcpserver.cpp

contains(DEFINES, websocket) {
HEADERS += $$PWD/net/netwebclient.h
HEADERS += $$PWD/net/netwebserver.h

SOURCES += $$PWD/net/netwebclient.cpp
SOURCES += $$PWD/net/netwebserver.cpp
}
