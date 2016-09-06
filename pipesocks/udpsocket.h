#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QUdpSocket>
#include <QHostInfo>

class UdpSocket : public QUdpSocket {
    Q_OBJECT
public:
    explicit UdpSocket(QObject *parent = 0);
signals:
    void SendData(const QString &Host,unsigned short Port,const QByteArray &Data);
    void RecvData(const QHostAddress &Address,unsigned short Port,const QByteArray &Data);
public slots:
    void SendDataSlot(const QString &Host,unsigned short Port,const QByteArray &Data);
    void RecvDataSlot();
};

#endif // UDPSOCKET_H
