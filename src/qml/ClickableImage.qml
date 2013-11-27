import QtQuick 2.0

Image {
    id: icon
    signal clicked()
    signal pressAndHold()
    property alias containsMouse: mouseArea.containsMouse

    scale: mouseArea.containsMouse ? mouseArea.pressed ? 1.2
                                                       : 1.1
                                   : 1.0
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            icon.clicked()
        }
        onPressAndHold: {
            icon.pressAndHold()
        }
    }
}
