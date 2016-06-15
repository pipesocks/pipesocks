#include "pump.h"

Pump::Pump(qintptr handle,const QString &Password,QObject *parent):QObject(parent),Password(Password) {
    csock=new SecureSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(csock);
    csock->moveToThread(cthread);
    cthread->start();
    ssock=NULL;
    sthread=NULL;
    usock=NULL;
    uthread=NULL;
    status=Initiated;
    printf("[%s] New connection from %s:%d\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),csock->peerAddress().toString().toLocal8Bit().data(),csock->peerPort());
}

void Pump::ClientRecv(const QByteArray &Data) {
    QVariantMap qvm(QJsonDocument::fromJson(Data).toVariant().toMap());
    switch (status) {
        case Initiated:
            if (qvm["password"]!=Password) {
                emit csock->Disconnect();
            }
            if (qvm["protocol"]=="tcp") {
                ssock=new TcpSocket;
                connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
                connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
                ssock->connectToHost(qvm["host"].toString(),qvm["port"].toUInt());
                sthread=new QThread(ssock);
                ssock->moveToThread(sthread);
                sthread->start();
                status=TCP;
            } else if (qvm["protocol"]=="udp") {
                usock=new UdpSocket;
                connect(usock,SIGNAL(RecvData(QHostAddress,unsigned short,QByteArray)),this,SLOT(UdpRecv(QHostAddress,unsigned short,QByteArray)));
                uthread=new QThread(usock);
                usock->moveToThread(uthread);
                uthread->start();
                status=UDP;
            }
            break;
        case TCP:
            emit ssock->SendData(Data);
            break;
        case UDP:
            if (!qvm["host"].toString()[0].isNumber()) {
                QHostInfo host(QHostInfo::fromName(qvm["host"].toString()));
                if (host.addresses().empty()) {
                    emit csock->Disconnect();
                    return;
                }
                emit usock->SendData(host.addresses().front(),qvm["port"].toUInt(),qvm["Data"].toByteArray());
            }
            emit usock->SendData(QHostAddress(qvm["host"].toString()),qvm["port"].toUInt(),qvm["Data"].toByteArray());
            break;
    }
}

void Pump::ServerRecv(const QByteArray &Data) {
    emit csock->SendData(Data);
}

void Pump::UdpRecv(const QHostAddress &Host,unsigned short Port,const QByteArray &Data) {
    QVariantMap qvm;
    qvm.insert("host",Host.toString());
    qvm.insert("port",Port);
    qvm.insert("data",Data);
    emit csock->SendData(QJsonDocument::fromVariant(qvm).toJson());
}

void Pump::EndSession() {
    emit csock->Disconnect();
    cthread->exit();
    cthread->wait();
    csock->deleteLater();
    if (ssock) {
        emit ssock->Disconnect();
        sthread->exit();
        sthread->wait();
        ssock->deleteLater();
    }
    if (usock) {
        uthread->exit();
        uthread->wait();
        usock->deleteLater();
    }
    deleteLater();
}
