import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: pipesocks
    width: 324
    height: 286

    RadioButton {
        id: pump
        x: 16
        y: 8
        width: 93
        height: 40
        text: qsTr("Pump")
    }

    RadioButton {
        id: pipe
        x: 115
        y: 8
        width: 93
        height: 40
        text: qsTr("Pipe")
    }

    RadioButton {
        id: tap
        x: 215
        y: 8
        width: 93
        height: 40
        text: qsTr("Tap")
        checked: true
    }

    Label {
        id: remoteHostLabel
        x: 16
        y: 66
        width: 86
        height: 16
        text: qsTr("Remote Host: ")
        horizontalAlignment: Text.AlignRight
    }

    TextField {
        id: remoteHost
        x: 108
        y: 54
        width: 200
        height: 40
    }

    Label {
        id: remotePortLabel
        x: 19
        y: 112
        width: 83
        height: 16
        text: qsTr("Remote Port: ")
        horizontalAlignment: Text.AlignRight
    }

    TextField {
        id: remotePort
        x: 108
        y: 100
        width: 200
        height: 40
        text: qsTr("1080")
    }

    Label {
        id: localPortLabel
        x: 33
        y: 158
        width: 69
        height: 16
        text: qsTr("Local Port: ")
        horizontalAlignment: Text.AlignRight
    }

    TextField {
        id: localPort
        x: 108
        y: 146
        width: 200
        height: 40
        text: qsTr("1080")
    }

    Label {
        id: passwordLabel
        x: 36
        y: 204
        width: 66
        height: 16
        text: qsTr("Password: ")
        horizontalAlignment: Text.AlignRight
    }

    TextField {
        id: password
        x: 108
        y: 192
        width: 200
        height: 40
        echoMode: TextInput.Password
        placeholderText: qsTr("Optional")
    }

    Button {
        id: start
        x: 16
        y: 238
        width: 143
        height: 40
        text: qsTr("Start")
    }

    Button {
        id: dump
        x: 165
        y: 238
        width: 143
        height: 40
        text: qsTr("Dump")
    }
}
