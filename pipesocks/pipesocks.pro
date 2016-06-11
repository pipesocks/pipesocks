# This project is created by yvbbrjdr

QT += core gui widgets network

TARGET = pipesocks

TEMPLATE = app

SOURCES += main.cpp \
    tcpsocket.cpp \
    securesocket.cpp \
    tcpserver.cpp \
    udpsocket.cpp

HEADERS += tcpsocket.h \
    securesocket.h \
    tcpserver.h \
    udpsocket.h

LIBS += -lsodium
