#include "tap.h"

Tap::Tap(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,const QString &LocalHost,const QString &Password,QObject *parent):QObject(parent),Password(Password),LocalHost(LocalHost) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(csock);
    csock->moveToThread(cthread);
    cthread->start();
    ssock=new SecureSocket;
    connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
    connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    ssock->connectToHost(RemoteHost,RemotePort);
    sthread=new QThread(ssock);
    ssock->moveToThread(sthread);
    sthread->start();
    usock=NULL;
    uthread=NULL;
    status=Initiated;
}

void Tap::ClientRecv(const QByteArray &Data) {
    bool ok;
    switch (status) {
        case Initiated:
            if (Data[0]!=5) {
                emit csock->Disconnect();
                return;
            }
            ok=false;
            for (int i=2;i<Data[1]+2;++i) {
                if (Data[i]==0) {
                    ok=true;
                    break;
                }
            }
            if (!ok) {
                emit csock->SendData(QByteArray::fromHex("05ff"));
                emit csock->Disconnect();
                return;
            }
            emit csock->SendData(QByteArray::fromHex("0500"));
            status=Handshook;
            break;
        case Handshook:

            break;
        case CONNECT:

            break;
        case UDPASSOCIATE:
            break;
    }
}

void Tap::ServerRecv(const QByteArray &Data) {

}

void Tap::UdpRecv(const QHostAddress&,unsigned short,const QByteArray &Data) {

}

void Tap::EndSession() {
    emit csock->Disconnect();
    cthread->exit();
    cthread->wait();
    csock->deleteLater();
    emit ssock->Disconnect();
    sthread->exit();
    sthread->wait();
    ssock->deleteLater();
    if (usock) {
        uthread->exit();
        uthread->wait();
        usock->deleteLater();
    }
    deleteLater();
}
