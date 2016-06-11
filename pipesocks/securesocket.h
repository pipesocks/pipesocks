#ifndef SECURESOCKET_H
#define SECURESOCKET_H

#include <QCoreApplication>
#include <sodium.h>
#include "tcpsocket.h"

class SecureSocket : public TcpSocket {
    Q_OBJECT
public:
    explicit SecureSocket(QObject *parent = 0);
private:
    QByteArray LocalPubKey,LocalPriKey,RemotePubKey;
    QByteArrayList SendBuffer;
    QByteArray RecvBuffer;
    QByteArray Encrypt(const QByteArray &Data);
    QByteArray Decrypt(const QByteArray &Data);
private slots:
    void StateChangedSlot(QAbstractSocket::SocketState state);
    void SendDataSlot(const QByteArray &Data);
    void RecvDataSlot();
};

#endif // SECURESOCKET_H
