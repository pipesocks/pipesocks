#ifndef MAINFORM_H
#define MAINFORM_H

#include <QObject>
#include <QVariant>
#include <QSettings>
#include "log.h"
#include "version.h"
#include "tcpserver.h"

class MainForm : public QObject {
    Q_OBJECT
public:
    explicit MainForm(QObject *rootObject,QObject *parent = 0);
private:
    QObject *window,*headerText,*pipesocks,*about,
            *pump,*pipe,*tap,*remoteHost,
            *remotePort,*localPort,*password,*start,
            *dump,*info;
    void ShowError();
    TcpServer *server;
    QSettings *settings;
private slots:
    void pumpClicked();
    void pipeClicked();
    void tapClicked();
    void startClicked();
    void dumpClicked();
    void closing();
    void fileChosen(QUrl path);
};

#endif // MAINFORM_H
