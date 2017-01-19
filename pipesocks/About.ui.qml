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

import QtQuick 2.7
import QtQuick.Controls 2.0

Item {
    width: 324
    height: 286

    Image {
        id: pipesocksLogo
        x: 16
        y: 8
        width: 143
        height: 143
        sourceSize.height: 143
        sourceSize.width: 143
        source: "icons/origin.png"
    }

    Image {
        id: yvbbrjdrLogo
        x: 165
        y: 8
        width: 143
        height: 143
        sourceSize.height: 143
        sourceSize.width: 143
        source: "icons/avatar.png"
    }

    Label {
        id: info
        objectName: "info"
        x: 16
        y: 157
        width: 292
        height: 121
        font.pointSize: 14
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
}
