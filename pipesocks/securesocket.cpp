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

SecureSocket::SecureSocket(const QString &Password,bool passive,QObject *parent):TcpSocket(parent),passive(passive),LocalPubKey(crypto_box_PUBLICKEYBYTES,0),LocalPriKey(crypto_box_SECRETKEYBYTES,0) {
    connect(this,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(StateChangedSlot(QAbstractSocket::SocketState)));
    if (sodium_init()==-1)
        QCoreApplication::exit(1);
    crypto_box_keypair((unsigned char*)LocalPubKey.data(),(unsigned char*)LocalPriKey.data());
    if ((unsigned int)Password.size()>=crypto_secretbox_KEYBYTES)
        SecretKey=Password.toLocal8Bit().left(crypto_secretbox_KEYBYTES);
    else
        SecretKey=Password.toLocal8Bit()+QByteArray(crypto_secretbox_KEYBYTES-Password.toLocal8Bit().size(),(char)0x98);
}

void SecureSocket::SendEncrypted(const QByteArray &Data) {
    QByteArray prefix(4,0);
    unsigned int l=crypto_secretbox_MACBYTES+4+crypto_secretbox_NONCEBYTES+Data.size();
    prefix[0]=(unsigned char)(l>>24);
    prefix[1]=(unsigned char)(l>>16);
    prefix[2]=(unsigned char)(l>>8);
    prefix[3]=(unsigned char)l;
    write(SecretEncrypt(prefix)+Data);
}

void SecureSocket::SendPubKey() {
    unsigned int l=randombytes_uniform(900);
    QByteArray garbage(l,0);
    randombytes_buf(garbage.data(),l);
    SendEncrypted(SecretEncrypt(garbage+LocalPubKey));
}

void SecureSocket::StateChangedSlot(QAbstractSocket::SocketState state) {
    if (state==ConnectedState&&!passive) {
        SendPubKey();
    }
}

void SecureSocket::SendUnencrypted(const QByteArray &Data) {
    SendEncrypted(PublicEncrypt(Data));
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
    while ((unsigned int)RecvBuffer.length()>=crypto_secretbox_MACBYTES+4+crypto_secretbox_NONCEBYTES) {
        QByteArray prefix(SecretDecrypt(RecvBuffer.left(crypto_secretbox_MACBYTES+4+crypto_secretbox_NONCEBYTES)));
        if (prefix=="")
            return;
        unsigned int l=(unsigned char)prefix[0];
        l=(l<<8)+(unsigned char)prefix[1];
        l=(l<<8)+(unsigned char)prefix[2];
        l=(l<<8)+(unsigned char)prefix[3];
        if ((unsigned int)RecvBuffer.length()<l)
            return;
        QByteArray segment(RecvBuffer.left(l).mid(crypto_secretbox_MACBYTES+4+crypto_secretbox_NONCEBYTES));
        RecvBuffer=RecvBuffer.mid(l);
        if (RemotePubKey.size()==0) {
            RemotePubKey=SecretDecrypt(segment).right(crypto_box_PUBLICKEYBYTES);
            if (passive) {
                SendPubKey();
            }
            for (QList<QByteArray>::iterator it=SendBuffer.begin();it!=SendBuffer.end();++it)
                SendUnencrypted(it.i->t());
            SendBuffer.clear();
        } else {
            emit RecvData(PublicDecrypt(segment));
        }
    }
}

QByteArray SecureSocket::PublicEncrypt(const QByteArray &Data) {
    QByteArray ret(crypto_box_MACBYTES+Data.length(),0),nonce(crypto_box_NONCEBYTES,0);
    randombytes_buf(nonce.data(),nonce.length());
    if (crypto_box_easy((unsigned char*)ret.data(),(unsigned char*)Data.data(),Data.length(),(unsigned char*)nonce.data(),(unsigned char*)RemotePubKey.data(),(unsigned char*)LocalPriKey.data())==0)
        return ret+nonce;
    disconnectFromHost();
    return QByteArray();
}

QByteArray SecureSocket::PublicDecrypt(const QByteArray &Data) {
    QByteArray ret(Data.length()-crypto_box_MACBYTES-crypto_box_NONCEBYTES,0);
    if (crypto_box_open_easy((unsigned char*)ret.data(),(unsigned char*)Data.data(),Data.length()-crypto_box_NONCEBYTES,(unsigned char*)Data.right(crypto_box_NONCEBYTES).data(),(unsigned char*)RemotePubKey.data(),(unsigned char*)LocalPriKey.data())==0)
        return ret;
    disconnectFromHost();
    return QByteArray();
}

QByteArray SecureSocket::SecretEncrypt(const QByteArray &Data) {
    QByteArray ret(crypto_secretbox_MACBYTES+Data.length(),0),nonce(crypto_secretbox_NONCEBYTES,0);
    randombytes_buf(nonce.data(),nonce.length());
    if (crypto_secretbox_easy((unsigned char*)ret.data(),(unsigned char*)Data.data(),Data.length(),(unsigned char*)nonce.data(),(unsigned char*)SecretKey.data())==0)
        return ret+nonce;
    disconnectFromHost();
    return QByteArray();
}

QByteArray SecureSocket::SecretDecrypt(const QByteArray &Data) {
    QByteArray ret(Data.length()-crypto_secretbox_MACBYTES-crypto_secretbox_NONCEBYTES,0);
    if (crypto_secretbox_open_easy((unsigned char*)ret.data(),(unsigned char*)Data.data(),Data.length()-crypto_secretbox_NONCEBYTES,(unsigned char*)Data.right(crypto_secretbox_NONCEBYTES).data(),(unsigned char*)SecretKey.data())==0)
        return ret;
    disconnectFromHost();
    return QByteArray();
}
