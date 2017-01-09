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
    mainwidget.cpp \
    aboutdialog.cpp \
    version.cpp \
    gfwlist.cpp \
    udpsocket.cpp \
    log.cpp

HEADERS += tcpsocket.h \
    securesocket.h \
    tcpserver.h \
    pipe.h \
    tap.h \
    pump.h \
    mainwidget.h \
    aboutdialog.h \
    version.h \
    gfwlist.h \
    udpsocket.h \
    log.h

osx {
    LIBS += -L/usr/local/lib/
    INCLUDEPATH += /usr/local/include/
    ICON = icons/osx.icns
}

win32: RC_ICONS = icons/win.ico

LIBS += -lsodium

FORMS += mainwidget.ui \
    aboutdialog.ui
