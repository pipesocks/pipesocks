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

SecureSocket::SecureSocket(QObject *parent):TcpSocket(parent),LocalPubKey(crypto_box_PUBLICKEYBYTES,0),LocalPriKey(crypto_box_SECRETKEYBYTES,0) {
    connect(this,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(StateChangedSlot(QAbstractSocket::SocketState)));
    if (sodium_init()==-1)
        QCoreApplication::exit(1);
    crypto_box_keypair((unsigned char*)LocalPubKey.data(),(unsigned char*)LocalPriKey.data());
}

void SecureSocket::StateChangedSlot(QAbstractSocket::SocketState state) {
    if (state==ConnectedState)
        write(LocalPubKey.toBase64()+char(23));
}

void SecureSocket::SendDataSlot(const QByteArray &Data) {
    if (state()==UnconnectedState||state()==ClosingState)
        return;
    if (RemotePubKey.size()==0)
        SendBuffer.push_back(Data);
    else
        write(Encrypt(Data).toBase64()+char(23));
}

void SecureSocket::RecvDataSlot() {
    RecvBuffer+=readAll();
    int index;
    while ((index=RecvBuffer.indexOf(char(23)))!=-1) {
        QByteArray segment(QByteArray::fromBase64(RecvBuffer.left(index)));
        RecvBuffer=RecvBuffer.mid(index+1);
        if (RemotePubKey.size()==0) {
            RemotePubKey=segment;
            for (QList<QByteArray>::iterator it=SendBuffer.begin();it!=SendBuffer.end();++it)
                write(Encrypt(it.i->t()).toBase64()+char(23));
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
    return QByteArray();
}

QByteArray SecureSocket::Decrypt(const QByteArray &Data) {
    QByteArray ret(Data.length()-crypto_box_MACBYTES-crypto_box_NONCEBYTES,0);
    if (crypto_box_open_easy((unsigned char*)ret.data(),(unsigned char*)Data.data(),Data.length()-crypto_box_NONCEBYTES,(unsigned char*)Data.right(crypto_box_NONCEBYTES).data(),(unsigned char*)RemotePubKey.data(),(unsigned char*)LocalPriKey.data())==0)
        return ret;
    return QByteArray();
}
