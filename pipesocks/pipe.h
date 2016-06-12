#ifndef PIPE_H
#define PIPE_H

#include <QObject>
#include <QThread>
#include "tcpsocket.h"

class Pipe : public QObject {
    Q_OBJECT
public:
    explicit Pipe(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,QObject *parent = 0);
private:
    TcpSocket *csock,*ssock;
    QThread *cthread,*sthread;
private slots:
    void ClientRecv(const QByteArray &Data);
    void ServerRecv(const QByteArray &Data);
    void EndSession();
};

#endif // PIPE_H
