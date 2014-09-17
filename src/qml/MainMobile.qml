import QtQuick 2.2
import Controller 1.0
import com.kdab.flowpomodoro 1.0

Main {
    id: root
    anchors.fill: parent
    topBarItem:
        MobileMenuBar {
        titleText: root.titleText
        anchors.leftMargin: _style.menuBarMargin
        anchors.rightMargin: _style.menuBarMargin
        onButtonClicked: {
            _controller.requestContextMenu(null) // reset task
            _controller.optionsContextMenuVisible = true
        }
    }

    Loader {
        sourceComponent: _controller.configurePopupRequested ? Qt.createComponent("MobileOptionsContextMenu.qml")
                                                             : null
        anchors.fill: parent
    }
}
