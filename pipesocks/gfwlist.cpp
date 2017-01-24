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

#include "gfwlist.h"

const QString GFWList::GFWListAddress("https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt");

GFWList::GFWList(QObject *parent):QObject(parent) {
    retrieving=available=false;
    timer=new QTimer(this);
    nam=new QNetworkAccessManager(this);
    nam->setProxy(QNetworkProxy::NoProxy);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    connect(nam,SIGNAL(finished(QNetworkReply*)),this,SLOT(ProcessGFWList(QNetworkReply*)));
    timer->start(3600000);
    timeout();
}

void GFWList::RequestGFWList() {
    if (available)
        emit RecvGFWList(PAC);
    else if (!retrieving)
        timeout();
}

void GFWList::timeout() {
    retrieving=true;
    nam->get(QNetworkRequest(QUrl(GFWListAddress)));
}

void GFWList::ProcessGFWList(QNetworkReply *reply) {
    retrieving=false;
    if (reply->error()!=QNetworkReply::NoError) {
        Log::log("GFWList not retrieved");
        if (!available)
            emit Fail();
        return;
    }
    QStringList list(QString(QByteArray::fromBase64(reply->readAll())).split('\n'));
    PAC="function FindProxyForURL(url,host){var scheme=url.substr(0,url.indexOf(\":\"));";
    for (QStringList::iterator it=list.begin();it!=list.end();++it) {
        QString tmp=it.i->t();
        if (tmp=="")
            continue;
        if (tmp[0]=='@') {
            tmp=tmp.mid(2);
            if (tmp[0]=='|') {
                tmp.replace('.',"\\.");
                tmp.replace('/',"\\/");
                if (tmp[1]=='|') {
                    PAC+=QString("if(/(?:^|\\.)%1$/.test(host))return\"DIRECT\";").arg(tmp.mid(2));
                } else {
                    PAC+=QString("if(/^%1/.test(url))return\"DIRECT\";").arg(tmp.mid(1));
                }
            } else if (tmp[0]=='/') {
                PAC+=QString("if(%1.test(url))return\"DIRECT\";").arg(tmp);
            } else {
                PAC+=QString("if(scheme===\"http\"&&url.indexOf(\"%1\")>=0)return\"DIRECT\";").arg(tmp);
            }
        }
    }
    for (QStringList::iterator it=list.begin();it!=list.end();++it) {
        QString tmp=it.i->t();
        if (tmp=="")
            continue;
        if (tmp[0]!='!'&&tmp[0]!='@'&&tmp[0]!='[') {
            if (tmp[0]=='|') {
                tmp.replace('.',"\\.");
                tmp.replace('/',"\\/");
                if (tmp[1]=='|') {
                    PAC+=QString("if(/(?:^|\\.)%1$/.test(host))return\"SOCKS5 %2:%3;SOCKS %2:%3\";").arg(tmp.mid(2));
                } else {
                    PAC+=QString("if(/^%1/.test(url))return\"SOCKS5 %2:%3;SOCKS %2:%3\";").arg(tmp.mid(1));
                }
            } else if (tmp[0]=='/') {
                PAC+=QString("if(%1.test(url))return\"SOCKS5 %2:%3;SOCKS %2:%3\";").arg(tmp);
            } else {
                PAC+=QString("if(scheme===\"http\"&&url.indexOf(\"%1\")>=0)return\"SOCKS5 %2:%3;SOCKS %2:%3\";").arg(tmp);
            }
        }
    }
    PAC+="return\"DIRECT;SOCKS5 %2:%3;SOCKS %2:%3\";}";
    available=true;
    Log::log("GFWList retrieved");
    emit RecvGFWList(PAC);
}
