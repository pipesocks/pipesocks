#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>

class TcpSocket : public QTcpSocket {
    Q_OBJECT
public:
    explicit TcpSocket(QObject *parent = 0);
signals:
    void SendData(const QByteArray &Data);
    void RecvData(const QByteArray &Data);
    void Disconnect();
private slots:
    virtual void SendDataSlot(const QByteArray &Data);
    virtual void RecvDataSlot();
    void DisconnectSlot();
};

#endif // TCPSOCKET_H
