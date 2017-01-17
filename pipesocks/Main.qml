import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0
import QtQuick.Dialogs 1.1

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

    function showNotFilled() {
        notFilled.visible=true
    }

    function showFailedBind() {
        failedBind.visible=true
    }

    function showFileDialog() {
        fileDialog.open()
    }

    signal fileChosen(url path)

    FileDialog {
        id: fileDialog
        title: "Where to dump?"
        folder: shortcuts.home
        selectExisting: false
        nameFilters: [ "Log file (*.log)" ]
        onAccepted: fileChosen(fileDialog.fileUrl)
    }

    MessageDialog {
        id: notFilled
        title: "Error"
        text: "Blanks must be filled."
        icon: StandardIcon.Critical
    }

    MessageDialog {
        id: failedBind
        title: "Error"
        text: "Failed to bind to local port."
        icon: StandardIcon.Critical
    }

    header: ToolBar {
        Label {
            id: headerText
            objectName: "headerText"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 20
            anchors.fill: parent
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        Pipesocks {
            objectName: "pipesocks"
        }
        About {
            objectName: "about"
        }
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
