# This project is created by yvbbrjdr

QT += core gui widgets network

TARGET = pipesocks

TEMPLATE = app

SOURCES += main.cpp \
    tcpsocket.cpp \
    securesocket.cpp \
    tcpserver.cpp

HEADERS += tcpsocket.h \
    securesocket.h \
    tcpserver.h

LIBS += -lsodium
