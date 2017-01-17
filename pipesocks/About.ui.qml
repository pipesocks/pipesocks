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
        source: "icons/origin.png"
    }

    Image {
        id: yvbbrjdrLogo
        x: 165
        y: 8
        width: 143
        height: 143
        source: "icons/avatar.png"
    }

    Label {
        id: information
        x: 16
        y: 157
        width: 292
        height: 121
        text: qsTr("pipesocks\nPowered by yvbbrjdr\nIcon depicted by Rena\nLicensed by GPL v3")
        font.pointSize: 20
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
}
