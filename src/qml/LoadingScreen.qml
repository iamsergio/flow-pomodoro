import QtQuick 2.0

Rectangle {
    id: root
    color: "yellow"

    Loader {
        id: loader
        anchors.fill: parent
        sourceComponent: _controller.startupFinished ? Qt.createComponent("MainMobile.qml")
                                                     : null // Delayed loading
    }
}
