import QtQuick 2.0
import QtQuick.Controls 1.0
import Controller 1.0

Menu {
    id: root

    MenuItem {
        visible: _controller.rightClickedTask === null && !_controller.currentTask.stopped
        text: _controller.currentTask.running ? qsTr("Pause") : qsTr("Resume")
        onTriggered: {
            _controller.pausePomodoro()
        }
    }

    MenuItem {
        id: stopMenuItem
        visible: _controller.rightClickedTask === null && !_controller.currentTask.stopped
        text: qsTr("Stop")
        onTriggered: {
            _controller.stopPomodoro(true)
        }
    }

    MenuSeparator {
        visible: stopMenuItem.visible
    }

    MenuItem {
        text: qsTr("About...")
        visible: _controller.currentPage != Controller.AboutPage
        onTriggered: {
            _controller.currentPage = Controller.AboutPage
            _controller.expanded = true
        }
    }
    MenuItem {
        text: qsTr("Quit")
        onTriggered: {
            Qt.quit()
        }
    }
}
