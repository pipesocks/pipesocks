#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QUdpSocket>

class UdpSocket : public QUdpSocket {
    Q_OBJECT
public:
    explicit UdpSocket(QObject *parent = 0);
signals:
    void SendData(const QHostAddress Host,unsigned short Port,const QByteArray &Data);
    void RecvData(const QHostAddress Host,unsigned short Port,const QByteArray &Data);
private slots:
    void SendDataSlot(const QHostAddress Host,unsigned short Port,const QByteArray &Data);
    void RecvDataSlot();
};

#endif // UDPSOCKET_H
