/*
This file is part of pipesocks. Pipesocks is a pipe-like SOCKS5 tunnel system.
Copyright (C) 2017  yvbbrjdr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tap.h"

Tap::Tap(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,const QString &Password,GFWList *gfwlist,QObject *parent):QObject(parent),Password(Password),gfwlist(gfwlist) {
    csock=new TcpSocket(this);
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    ssock=new SecureSocket(Password,false,this);
    connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
    connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    ssock->connectToHost(RemoteHost,RemotePort);
    usock=NULL;
    CHost=csock->peerAddress();
    CPort=csock->peerPort();
    status=Initiated;
    Log::log(csock,"connection established");
}

void Tap::ClientRecv(const QByteArray &Data) {
    switch (status) {
        case Initiated: {
            if (Data[0]=='G') {
                if (Data.indexOf("gfwlist")==-1) {
                    emit csock->SendData(PAC());
                    csock->disconnectFromHost();
                    Log::log(csock,"requested global PAC");
                } else {
                    connect(gfwlist,SIGNAL(RecvGFWList(QString)),this,SLOT(RecvGFWList(QString)));
                    connect(gfwlist,SIGNAL(Fail()),this,SLOT(GFWListFail()));
                    gfwlist->RequestGFWList();
                    Log::log(csock,"requested GFWList PAC");
                }
                return;
            }
            if (Data[0]!=5) {
                csock->disconnectFromHost();
                return;
            }
            bool ok=false;
            for (int i=2;i<Data[1]+2;++i) {
                if (Data[i]==0) {
                    ok=true;
                    break;
                }
            }
            if (!ok) {
                emit csock->SendData(QByteArray::fromHex("05ff"));
                csock->disconnectFromHost();
                return;
            }
            emit csock->SendData(QByteArray::fromHex("0500"));
            status=Handshook;
            break;
        }
        case Handshook: {
            QVariantMap qvm;
            if (Data[0]!=5||Data[1]==2||Data[2]!=0) {
                emit csock->SendData(QByteArray::fromHex("05070001000000000000"));
                csock->disconnectFromHost();
                return;
            }
            QPair<QString,unsigned short>hostport=toNormal(Data.mid(3));
            qvm.insert("host",hostport.first);
            qvm.insert("port",hostport.second);
            qvm.insert("password",Password);
            qvm.insert("version",Version::GetLowestVersion());
            if (Data[1]==1) {
                qvm.insert("protocol","TCP");
                Log::log(csock,"requested TCP connection to "+hostport.first+':'+QString::number(hostport.second));
            } else if (Data[1]==3) {
                qvm.insert("protocol","UDP");
                Log::log(csock,"requested UDP association");
            }
            qvm.insert("garbage",QString(randombytes_uniform(900),'f'));
            emit ssock->SendData(QJsonDocument::fromVariant(qvm).toJson());
            break;
        }
        case CONNECT:
            emit ssock->SendData(Data);
            break;
        case UDPASSOCIATE:
            break;
    }
}

void Tap::ServerRecv(const QByteArray &Data) {
    switch (status) {
        case Initiated:
            break;
        case Handshook: {
            QVariantMap qvm(QJsonDocument::fromJson(Data).toVariant().toMap());
            if (qvm["status"]=="ok") {
                if (qvm["protocol"]=="TCP") {
                    emit csock->SendData(QByteArray::fromHex("05000001000000000000"));
                    status=CONNECT;
                } else if (qvm["protocol"]=="UDP") {
                    usock=new UdpSocket(this);
                    connect(usock,SIGNAL(RecvData(QHostAddress,unsigned short,QByteArray)),this,SLOT(UDPRecv(QHostAddress,unsigned short,QByteArray)));
                    emit csock->SendData(QByteArray::fromHex("050000")+toSOCKS5(csock->localAddress(),usock->localPort()));
                    status=UDPASSOCIATE;
                }
            } else {
                emit csock->SendData(QByteArray::fromHex("05020001000000000000"));
                csock->disconnectFromHost();
                Log::log("Connection refused by pump");
            }
            break;
        }
        case CONNECT:
            emit csock->SendData(Data);
            break;
        case UDPASSOCIATE: {
            QVariantMap qvm(QJsonDocument::fromJson(Data).toVariant().toMap());
            emit usock->SendData(UHost.toString(),UPort,QByteArray::fromHex("000000")+toSOCKS5(QHostAddress(qvm["host"].toString()),qvm["port"].toUInt())+QByteArray::fromBase64(qvm["data"].toByteArray()));
            Log::log(csock,"received a UDP package from "+qvm["host"].toString().mid(7)+':'+QString::number(qvm["port"].toUInt()));
        }
    }
}

void Tap::EndSession() {
    if (csock->state()==QAbstractSocket::ConnectedState) {
        Log::log(CHost.toString().mid(7)+':'+QString::number(CPort)+" server closed the connection");
    }
    if (ssock->state()==QAbstractSocket::ConnectedState) {
        Log::log(CHost.toString().mid(7)+':'+QString::number(CPort)+" client closed the connection");
    }
    csock->disconnectFromHost();
    ssock->disconnectFromHost();
    if (ssock->state()==QAbstractSocket::UnconnectedState&&csock->state()==QAbstractSocket::UnconnectedState) {
        if (usock)
            usock->close();
        deleteLater();
    }
}

QByteArray Tap::PAC() {
    QString pac(QString("function FindProxyForURL(url,host){return\"SOCKS5 %1:%2;SOCKS %1:%2\"}").arg(csock->localAddress().toString().mid(7)).arg(csock->localPort()));
    QString http(QString("HTTP/1.1 200 OK\r\nContent-Type: application/x-ns-proxy-autoconfig\r\nContent-Length: %1\r\n\r\n%2").arg(pac.length()).arg(pac));
    return http.toLocal8Bit();
}

void Tap::RecvGFWList(const QString &gfwlist) {
    QString pac(gfwlist.arg(csock->localAddress().toString().mid(7)).arg(csock->localPort()));
    QString http(QString("HTTP/1.1 200 OK\r\nContent-Type: application/x-ns-proxy-autoconfig\r\nContent-Length: %1\r\n\r\n%2").arg(pac.length()).arg(pac));
    emit csock->SendData(http.toLocal8Bit());
    csock->disconnectFromHost();
}

void Tap::GFWListFail() {
    emit csock->SendData("HTTP/1.1 503 Server Unavailable\r\nContent-Length: 0\r\n\r\n");
    csock->disconnectFromHost();
    Log::log(csock,"failed to get GFWList PAC");
}

void Tap::UDPRecv(const QHostAddress &Host,unsigned short Port,const QByteArray &Data) {
    UHost=Host;
    UPort=Port;
    if (Data[0]||Data[1]||Data[2]) {
        csock->disconnectFromHost();
        return;
    }
    QPair<QString,unsigned short>hostport=toNormal(Data.mid(3));
    QVariantMap qvm;
    qvm.insert("host",hostport.first);
    qvm.insert("port",hostport.second);
    if (Data[3]==1)
        qvm.insert("data",Data.mid(10).toBase64());
    else if (Data[3]==3)
        qvm.insert("data",Data.mid(Data[4]+7).toBase64());
    else if (Data[3]==4)
        qvm.insert("data",Data.mid(22).toBase64());
    emit ssock->SendData(QJsonDocument::fromVariant(qvm).toJson());
    Log::log(csock,"sent a UDP package to "+hostport.first+':'+QString::number(hostport.second));
}

QPair<QString,unsigned short>Tap::toNormal(const QByteArray &SOCKS5) {
    QString host;
    unsigned short port=0;
    if (SOCKS5[0]==1) {
        host=QString("%1.%2.%3.%4")
                .arg((unsigned char)SOCKS5[1])
                .arg((unsigned char)SOCKS5[2])
                .arg((unsigned char)SOCKS5[3])
                .arg((unsigned char)SOCKS5[4]);
        port=((unsigned short)(unsigned char)SOCKS5[5]<<8)+(unsigned char)SOCKS5[6];
    } else if (SOCKS5[0]==3) {
        host=SOCKS5.mid(2,SOCKS5[1]);
        port=((unsigned short)(unsigned char)SOCKS5[SOCKS5[1]+2]<<8)+(unsigned char)SOCKS5[SOCKS5[1]+3];
    } else if (SOCKS5[0]==4) {
        host=QString("%1%2:%3%4:%5%6:%7%8:%9%10:%11%12:%13%14:%15%16")
                .arg((unsigned char)SOCKS5[1],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[2],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[3],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[4],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[5],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[6],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[7],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[8],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[9],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[10],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[11],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[12],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[13],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[14],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[15],2,16,QLatin1Char('0'))
                .arg((unsigned char)SOCKS5[16],2,16,QLatin1Char('0'));
        port=((unsigned short)(unsigned char)SOCKS5[17]<<8)+(unsigned char)SOCKS5[18];
    }
    return QPair<QString,unsigned short>(host,port);
}

QByteArray Tap::toSOCKS5(const QHostAddress &Host,unsigned short Port) {
    QByteArray ret;
    bool is4;
    unsigned int ipv4=Host.toIPv4Address(&is4);
    if (is4) {
        ret+=char(1);
        ret+=(unsigned char)(ipv4>>24);
        ret+=(unsigned char)(ipv4>>16);
        ret+=(unsigned char)(ipv4>>8);
        ret+=(unsigned char)(ipv4);
    } else {
        Q_IPV6ADDR ipv6=Host.toIPv6Address();
        ret+=char(4);
        for (int i=0;i<16;++i)
            ret+=(unsigned char)ipv6[i];
    }
    ret+=(unsigned char)(Port>>8);
    ret+=(unsigned char)(Port);
    return ret;
}
