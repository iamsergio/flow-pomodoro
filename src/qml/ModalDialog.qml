import QtQuick 2.0
import QtQuick.Controls 1.0

Item {
    id: root
    property alias content: contentItem.children
    property int dialogHeight: 75
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
        height: root.dialogHeight
        color: _style.dialogColor
        border.color: _style.dialogBorderColor
        border.width:  _style.dialogBorderWidth
        radius: _style.queueRadius
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: _style.pageMargin + _style.extraMargin
        anchors.rightMargin: anchors.leftMargin
        anchors.bottomMargin: (!_controller.currentTask.stopped ? _style.marginSmall : _style.marginMedium) + _style.extraMargin
    }
}
