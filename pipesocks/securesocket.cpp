/*
This file is part of pipesocks. Pipesocks is a pipe-like SOCKS5 tunnel system.
Copyright (C) 2016  yvbbrjdr

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
    XORKey=0;
    for (int i=0;i<Password.length();++i)
        XORKey+=(unsigned char)Password[i].toLatin1();
}

void SecureSocket::StateChangedSlot(QAbstractSocket::SocketState state) {
    if (state==ConnectedState) {
        unsigned short l=randombytes_uniform(4000);
        QByteArray garbage(l+2,0);
        randombytes_buf(garbage.data()+2,l);
        l+=crypto_box_PUBLICKEYBYTES+2;
        garbage[0]=((unsigned char)(l>>8))^XORKey;
        garbage[1]=((unsigned char)(l))^XORKey;
        write(garbage+LocalPubKey);
    }
}

void SecureSocket::SendUnencrypted(const QByteArray &Data) {
    QByteArray encrypted(Encrypt(Data));
    unsigned short l=encrypted.length()+2;
    QByteArray len(2,0);
    len[0]=((unsigned char)(l>>8))^XORKey;
    len[1]=((unsigned char)(l))^XORKey;
    write(len+encrypted);
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
    while (RecvBuffer.length()>=2) {
        unsigned short l=((unsigned char)(RecvBuffer[0]))^XORKey;
        l=(l<<8)+(((unsigned char)(RecvBuffer[1]))^XORKey);
        if (RecvBuffer.length()<l)
            return;
        QByteArray segment(RecvBuffer.left(l));
        segment=segment.mid(2);
        RecvBuffer=RecvBuffer.mid(l);
        if (RemotePubKey.size()==0) {
            RemotePubKey=segment.right(crypto_box_PUBLICKEYBYTES);
            for (QList<QByteArray>::iterator it=SendBuffer.begin();it!=SendBuffer.end();++it)
                SendUnencrypted(it.i->t());
            SendBuffer.clear();
        } else {
            emit RecvData(Decrypt(segment));
        }
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
