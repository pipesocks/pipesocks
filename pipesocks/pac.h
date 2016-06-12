#ifndef PAC_H
#define PAC_H

#include <QObject>
#include <QThread>
#include "tcpsocket.h"

class PAC : public QObject {
    Q_OBJECT
public:
    explicit PAC(qintptr handle,QObject *parent = 0);
private:
    TcpSocket *csock;
    QThread *cthread;
private slots:
    void RecvData(const QByteArray &Data);
    void EndSession();
};

#endif // PAC_H
