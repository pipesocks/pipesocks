#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDateTime>
#include <QTcpServer>
#include "pump.h"
#include "pipe.h"
#include "tap.h"
#include "pac.h"

class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    enum Mode {
        PumpServer,
        PipeServer,
        TapClient,
        PACServer
    };
    explicit TcpServer(Mode mode,const QString &RemoteHost,unsigned short RemotePort,const QString &LocalHost,const QString &Password,QObject *parent = 0);
private:
    Mode mode;
    QString RemoteHost;
    unsigned short RemotePort;
    QString LocalHost;
    QString Password;
protected:
    void incomingConnection(qintptr handle);
};

#endif // TCPSERVER_H
