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
    printf("[%s] New connection from %s:%d\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),csock->peerAddress().toString().toLocal8Bit().data(),csock->peerPort());
}

void Tap::ClientRecv(const QByteArray &Data) {
    bool ok;
    QVariantMap qvm;
    QString host;
    unsigned short port;
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
            if (Data[0]!=5||Data[1]==2||Data[2]!=0||Data[3]==4) {
                emit csock->SendData(QByteArray::fromHex("05070001000000000000"));
                emit csock->Disconnect();
                return;
            }
            if (Data[1]==1) {
                qvm.insert("protocol","tcp");
            } else if (Data[1]==3) {
                qvm.insert("protocol","udp");
            }
            if (Data[3]==1) {
                host=QString("%1.%2.%3.%4")
                        .arg((unsigned char)Data[4])
                        .arg((unsigned char)Data[5])
                        .arg((unsigned char)Data[6])
                        .arg((unsigned char)Data[7]);
            } else if (Data[3]==3) {
                host=Data.mid(5,Data[4]);
            }
            qvm.insert("host",host);
            port=((unsigned char)Data[Data.length()-2]<<8)+(unsigned char)Data[Data.length()-1];
            qvm.insert("port",port);
            qvm.insert("password",Password);
            emit ssock->SendData(QJsonDocument::fromVariant(qvm).toJson());
            if (Data[1]==1) {
                emit csock->SendData(QByteArray::fromHex("05000001000000000000"));
                status=CONNECT;
            } else if (Data[1]==3) {
                usock=new UdpSocket;
                connect(usock,SIGNAL(RecvData(QHostAddress,unsigned short,QByteArray)),this,SLOT(UdpRecv(QHostAddress,unsigned short,QByteArray)));
                usock->bind();
                uthread=new QThread(usock);
                usock->moveToThread(uthread);
                uthread->start();
                emit csock->SendData(QByteArray::fromHex("05000001"));//
                status=UDPASSOCIATE;
            }
            break;
        case CONNECT:
            emit ssock->SendData(Data);
            break;
        case UDPASSOCIATE:
            break;
    }
}

void Tap::ServerRecv(const QByteArray &Data) {
    if (status==CONNECT) {
        emit csock->SendData(Data);
    } else if (status==UDPASSOCIATE) {
        QVariantMap qvm(QJsonDocument::fromJson(Data).toVariant().toMap());
        //
    }
}

void Tap::UdpRecv(const QHostAddress &Host,unsigned short Port,const QByteArray &Data) {
    ClientHost=Host;
    ClientPort=Port;
    //
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
