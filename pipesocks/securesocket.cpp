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
            for (QByteArrayList::iterator it=SendBuffer.begin();it!=SendBuffer.end();++it)
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
