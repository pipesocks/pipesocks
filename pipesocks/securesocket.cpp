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

#include "securesocket.h"

SecureSocket::SecureSocket(const QString &Password,QObject *parent):TcpSocket(parent),LocalPubKey(crypto_box_PUBLICKEYBYTES,0),LocalPriKey(crypto_box_SECRETKEYBYTES,0) {
    connect(this,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(StateChangedSlot(QAbstractSocket::SocketState)));
    if (sodium_init()==-1)
        QCoreApplication::exit(1);
    crypto_box_keypair((unsigned char*)LocalPubKey.data(),(unsigned char*)LocalPriKey.data());
    this->Password=Password.toLocal8Bit();
}

QByteArray SecureSocket::XORWithPassword(const QByteArray &Data) {
    if (Password.size()==0)
        return Data;
    QByteArray ret=Data;
    for (int i=0;i<ret.size();++i)
        ret.data()[i]^=Password[i%Password.size()];
    return ret;
}

void SecureSocket::StateChangedSlot(QAbstractSocket::SocketState state) {
    if (state==ConnectedState) {
        QByteArray prefix(2,0);
        unsigned int l=randombytes_uniform(4000);
        QByteArray garbage(l,0);
        randombytes_buf(garbage.data(),l);
        l+=crypto_box_PUBLICKEYBYTES+2;
        prefix[0]=(unsigned char)(l>>8);
        prefix[1]=(unsigned char)l;
        write(XORWithPassword(prefix)+garbage+XORWithPassword(LocalPubKey));
    }
}

void SecureSocket::SendUnencrypted(const QByteArray &Data) {
    QByteArray prefix(3,0);
    QByteArray encrypted(Encrypt(Data));
    unsigned int l=encrypted.length()+3;
    prefix[0]=(unsigned char)(l>>16);
    prefix[1]=(unsigned char)(l>>8);
    prefix[2]=(unsigned char)l;
    write(XORWithPassword(prefix)+encrypted);
}

void SecureSocket::SendDataSlot(const QByteArray &Data) {
    if (state()==QAbstractSocket::UnconnectedState)
        return;
    if (RemotePubKey.size()==0)
        SendBuffer.push_back(Data);
    else
        SendUnencrypted(Data);
}

void SecureSocket::RecvDataSlot() {
    RecvBuffer+=readAll();
    if (RemotePubKey.size()==0) {
        if (RecvBuffer.length()<2)
            return;
        QByteArray prefix(XORWithPassword(RecvBuffer.left(2)));
        unsigned int l=(unsigned char)prefix[0];
        l=(l<<8)+(unsigned char)prefix[1];
        if ((unsigned int)RecvBuffer.length()<l)
            return;
        RemotePubKey=XORWithPassword(RecvBuffer.left(l).right(crypto_box_PUBLICKEYBYTES));
        RecvBuffer=RecvBuffer.mid(l);
        for (QList<QByteArray>::iterator it=SendBuffer.begin();it!=SendBuffer.end();++it)
            SendUnencrypted(it.i->t());
        SendBuffer.clear();
    }
    while (RecvBuffer.length()>=3) {
        QByteArray prefix(XORWithPassword(RecvBuffer.left(3)));
        unsigned int l=(unsigned char)prefix[0];
        l=(l<<8)+(unsigned char)prefix[1];
        l=(l<<8)+(unsigned char)prefix[2];
        if ((unsigned int)RecvBuffer.length()<l)
            return;
        QByteArray segment(RecvBuffer.left(l).mid(3));
        RecvBuffer=RecvBuffer.mid(l);
        emit RecvData(Decrypt(segment));
    }
}

QByteArray SecureSocket::Encrypt(const QByteArray &Data) {
    QByteArray ret(crypto_box_MACBYTES+Data.length(),0),nonce(crypto_box_NONCEBYTES,0);
    randombytes_buf(nonce.data(),nonce.length());
    if (crypto_box_easy((unsigned char*)ret.data(),(unsigned char*)Data.data(),Data.length(),(unsigned char*)nonce.data(),(unsigned char*)RemotePubKey.data(),(unsigned char*)LocalPriKey.data())==0)
        return ret+nonce;
    disconnectFromHost();
    return QByteArray();
}

QByteArray SecureSocket::Decrypt(const QByteArray &Data) {
    QByteArray ret(Data.length()-crypto_box_MACBYTES-crypto_box_NONCEBYTES,0);
    if (crypto_box_open_easy((unsigned char*)ret.data(),(unsigned char*)Data.data(),Data.length()-crypto_box_NONCEBYTES,(unsigned char*)Data.right(crypto_box_NONCEBYTES).data(),(unsigned char*)RemotePubKey.data(),(unsigned char*)LocalPriKey.data())==0)
        return ret;
    disconnectFromHost();
    return QByteArray();
}
