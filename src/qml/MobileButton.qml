import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    property string text: ""
    signal clicked()
    height: 60 * _controller.dpiFactor
    width: Math.max(100 * _controller.dpiFactor, textItem.contentWidth + 30 * _controller.dpiFactor)

    Item {
        id: shadowContainer
        anchors.fill: parent

        Rectangle {
            id: buttonRect
            width: root.width - 16 * _controller.dpiFactor
            height: root.height - 16 * _controller.dpiFactor

            anchors.centerIn: parent

            anchors.verticalCenterOffset: mouseArea.pressed ? 3 : 0
            anchors.horizontalCenterOffset: mouseArea.pressed ? 3 : 0
            border.color: "gray"
            border.width: 1 * _controller.dpiFactor
            radius: 2 * _controller.dpiFactor
            color: mouseArea.pressed ? "#E3E3E3" : "white"

            Behavior on anchors.verticalCenterOffset {
                NumberAnimation {
                    duration: 300

                }
            }
            Behavior on anchors.horizontalCenterOffset {
                NumberAnimation {
                    duration: 300
                }
            }

            Text {
                id: textItem
                renderType: _controller.textRenderType
                color: enabled ? "black" : "gray"
                anchors.fill: parent
                text: root.text
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 17 * _controller.dpiFactor
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    root.clicked()
                }
            }
        }
    }

    DropShadow {
        id: shadowEffect
        anchors.fill: shadowContainer
        cached: true
        smooth: true
        horizontalOffset: (mouseArea.pressed ? 0 : 3) * _controller.dpiFactor
        verticalOffset: (mouseArea.pressed ? 0 : 3) * _controller.dpiFactor
        radius: 8.0
        samples: 16
        color: "#80000000"
        source: shadowContainer

        Behavior on horizontalOffset {
            NumberAnimation { duration: 300 }
        }
        Behavior on verticalOffset {
            NumberAnimation { duration: 300 }
        }

    }

}
