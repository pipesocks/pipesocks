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

#include "log.h"

FILE* Log::fp=stdout;

void Log::log(const QString &message) {
    fprintf(fp,"[%s] %s\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),message.toLocal8Bit().data());
    fflush(fp);
}

void Log::log(const QAbstractSocket *socket,const QString &message) {
    log(socket->peerAddress().toString().mid(7)+':'+QString::number(socket->peerPort())+' '+message);
}

void Log::dump(const QString &path) {
    fp=fopen(path.toLocal8Bit().data(),"w");
    if (!fp)
        fp=stdout;
}

void Log::undump() {
    fflush(fp);
    fclose(fp);
    fp=stdout;
}
