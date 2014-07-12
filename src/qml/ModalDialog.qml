import QtQuick 2.0
import QtQuick.Controls 1.0

Item {
    id: root
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
}
