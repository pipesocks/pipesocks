# This project is created by yvbbrjdr

QT += core gui widgets network

TARGET = pipesocks

TEMPLATE = app

SOURCES += main.cpp \
    tcpsocket.cpp \
    securesocket.cpp \
    tcpserver.cpp \
    pipe.cpp \
    tap.cpp \
    pump.cpp \
    pac.cpp \
    mainwidget.cpp

HEADERS += tcpsocket.h \
    securesocket.h \
    tcpserver.h \
    pipe.h \
    tap.h \
    pump.h \
    pac.h \
    mainwidget.h

osx {
    LIBS += -L/usr/local/lib/
    INCLUDEPATH += /usr/local/include/
}

LIBS += -lsodium

DISTFILES += proxy.pac

FORMS += mainwidget.ui
