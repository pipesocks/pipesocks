#ifndef TAP_H
#define TAP_H

#include <QObject>
#include <QThread>
#include <QJsonDocument>
#include <QVariantMap>
#include <QDateTime>
#include "tcpsocket.h"
#include "securesocket.h"
#include "udpsocket.h"

class Tap : public QObject {
    Q_OBJECT
public:
    explicit Tap(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,const QString &LocalHost,const QString &Password,QObject *parent = 0);
private:
    enum Status {
        Initiated,
        Handshook,
        CONNECT,
        UDPASSOCIATE
    };
    QString Password;
    Status status;
    QString LocalHost;
    TcpSocket *csock;
    SecureSocket *ssock;
    UdpSocket *usock;
    QThread *cthread,*sthread,*uthread;
    QHostAddress ClientHost;
    unsigned short ClientPort;
private slots:
    void ClientRecv(const QByteArray &Data);
    void ServerRecv(const QByteArray &Data);
    void UdpRecv(const QHostAddress &Host, unsigned short Port, const QByteArray &Data);
    void EndSession();
};

#endif // TAP_H
