#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include "securesocket.h"

class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    enum Mode {
        Pump,
        Pipe,
        Tap,
        PAC
    };
    explicit TcpServer(Mode mode,const QString &RemoteHost,unsigned short RemotePort,const QString &LocalHost,const QString &Password,QObject *parent = 0);
private:
    Mode mode;
    QString RemoteHost;
    unsigned short RemotePort;
    QString LocalHost;
    QString Password;
};

#endif // TCPSERVER_H
