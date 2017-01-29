import QtQuick 2.0

Item {
    id: root
    property alias model: bar.model
    TabBar {
        id: bar
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }

    Loader {
        id: contentsLoader
        anchors {
            left: parent.left
            right: parent.right
            top: bar.bottom
            bottom: parent.bottom
        }
        source: bar.currentItem ? bar.currentItem.source : ""
    }
}
