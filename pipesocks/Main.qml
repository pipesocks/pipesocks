import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0

ApplicationWindow {
    width: 324
    height: 400
    minimumHeight: 400
    minimumWidth: 324
    maximumHeight: 400
    maximumWidth: 324
    visible: true
    title: qsTr("pipesocks GUI")
    Material.theme: Material.Dark
    Material.accent: Material.Cyan
    Material.primary: Material.BlueGrey
    font.family: qsTr("Roboto")

    header: ToolBar {
        Label {
            id: headerText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 20
            anchors.fill: parent
            text: qsTr("pipesocks")
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        Pipesocks {}
        About {}
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("pipesocks")
        }
        TabButton {
            text: qsTr("About")
        }
    }
}
