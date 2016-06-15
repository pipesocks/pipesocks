#ifndef PUMP_H
#define PUMP_H

#include <QObject>
#include <QThread>
#include <QJsonDocument>
#include <QVariantMap>
#include <QDateTime>
#include <QHostInfo>
#include "securesocket.h"
#include "tcpsocket.h"
#include "udpsocket.h"

class Pump : public QObject {
    Q_OBJECT
public:
    explicit Pump(qintptr handle,const QString &Password,QObject *parent = 0);
private:
    enum Status {
        Initiated,
        TCP,
        UDP
    };
    QString Password;
    Status status;
    SecureSocket *csock;
    TcpSocket *ssock;
    UdpSocket *usock;
    QThread *cthread,*sthread,*uthread;
private slots:
    void ClientRecv(const QByteArray &Data);
    void ServerRecv(const QByteArray &Data);
    void UdpRecv(const QHostAddress &Host,unsigned short Port,const QByteArray &Data);
    void EndSession();
};

#endif // PUMP_H
