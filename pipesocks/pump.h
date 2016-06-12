#ifndef PUMP_H
#define PUMP_H

#include <QObject>
#include <QThread>
#include "securesocket.h"
#include "tcpsocket.h"
#include "udpsocket.h"

class Pump : public QObject {
    Q_OBJECT
public:
    explicit Pump(qintptr handle,QObject *parent = 0);
private:
    SecureSocket *csock;
    TcpSocket *ssock;
    UdpSocket *usock;
    QThread *cthread,*sthread,*uthread,*thread;
private slots:
    void ClientRecv(const QByteArray &Data);
    void ServerRecv(const QByteArray &Data);
    void UdpRecv(const QHostAddress Host,unsigned short Port,const QByteArray &Data);
    void EndSession();
};

#endif // PUMP_H
