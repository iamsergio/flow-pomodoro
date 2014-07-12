import QtQuick 2.0
import QtQuick.Controls 1.0

Item {
    id: root
    property alias content: contentItem.children
    visible: enabled

    Rectangle {
        id: overlay
        color: _style.blockingOverlayColor
        radius: _style.borderRadius
        opacity: _style.blockingOverlayOpacity
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            onClicked: {
                mouse.accepted = true
            }
        }
    }

    Rectangle {
        id: contentItem
        z: overlay.z + 1
        height: 75
        color: "lightgray"
        border.color: "darkblue"
        border.width: 2
        radius: _style.queueRadius
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: _style.pageMargin+4
        anchors.rightMargin: _style.pageMargin+4
        anchors.bottomMargin: (!_controller.stopped ? _style.marginSmall : _style.marginMedium) + 4
    }
}
