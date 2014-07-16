import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    signal clicked()
    signal pressAndHold()
    property alias containsMouse: mouseArea.containsMouse
    property alias source: icon.source
    height: icon.height
    width: icon.width

    Image {
        id: icon

        scale: mouseArea.containsMouse ? mouseArea.pressed ? 1.2
                                                           : 1.1
                                       : 1.0
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                root.clicked()
            }
            onPressAndHold: {
                root.pressAndHold()
            }
        }
    }

    Desaturate {
        visible: !root.enabled
        anchors.fill: parent
        source: icon
        desaturation: 0.8
    }
}
