#include "pipe.h"

Pipe::Pipe(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,QObject *parent):QObject(parent) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(csock);
    csock->moveToThread(cthread);
    cthread->start();
    ssock=new TcpSocket;
    connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
    connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    ssock->connectToHost(RemoteHost,RemotePort);
    sthread=new QThread(ssock);
    ssock->moveToThread(sthread);
    sthread->start();
    printf("[%s] New connection from %s:%d\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),csock->peerAddress().toString().toLocal8Bit().data(),csock->peerPort());
}

void Pipe::ClientRecv(const QByteArray &Data) {
    emit ssock->SendData(Data);
}

void Pipe::ServerRecv(const QByteArray &Data) {
    emit csock->SendData(Data);
}

void Pipe::EndSession() {
    emit csock->Disconnect();
    cthread->exit();
    cthread->wait();
    csock->deleteLater();
    emit ssock->Disconnect();
    sthread->exit();
    sthread->wait();
    ssock->deleteLater();
    deleteLater();
}
