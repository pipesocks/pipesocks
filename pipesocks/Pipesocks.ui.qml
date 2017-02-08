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
    id: pipesocks
    width: 324
    height: 286

    RadioButton {
        id: pump
        objectName: "pump"
        x: 16
        y: 8
        width: 93
        text: qsTr("Pump")
    }

    RadioButton {
        id: pipe
        objectName: "pipe"
        x: 115
        y: 8
        width: 93
        text: qsTr("Pipe")
    }

    RadioButton {
        id: tap
        objectName: "tap"
        x: 215
        y: 8
        width: 93
        text: qsTr("Tap")
        checked: true
    }

    Label {
        id: remoteHostLabel
        x: 16
        y: 66
        width: 86
        text: qsTr("Remote Host: ")
        horizontalAlignment: Text.AlignRight
    }

    TextField {
        id: remoteHost
        objectName: "remoteHost"
        x: 108
        y: 54
        width: 200
    }

    Label {
        id: remotePortLabel
        x: 19
        y: 112
        width: 83
        text: qsTr("Remote Port: ")
        horizontalAlignment: Text.AlignRight
    }

    TextField {
        id: remotePort
        objectName: "remotePort"
        x: 108
        y: 100
        width: 200
        text: qsTr("7473")
    }

    Label {
        id: localPortLabel
        x: 33
        y: 158
        width: 69
        text: qsTr("Local Port: ")
        horizontalAlignment: Text.AlignRight
    }

    TextField {
        id: localPort
        objectName: "localPort"
        x: 108
        y: 146
        width: 200
        text: qsTr("7473")
    }

    Label {
        id: passwordLabel
        x: 36
        y: 204
        width: 66
        text: qsTr("Password: ")
        horizontalAlignment: Text.AlignRight
    }

    TextField {
        id: password
        objectName: "password"
        x: 108
        y: 192
        width: 200
        echoMode: TextInput.Password
        placeholderText: qsTr("Optional")
    }

    Button {
        id: start
        objectName: "start"
        x: 16
        y: 238
        width: 143
        text: qsTr("Start")
    }

    Button {
        id: dump
        objectName: "dump"
        x: 165
        y: 238
        width: 143
        text: qsTr("Dump")
    }
}
