import QtQuick 2.0

Item {
    id: root
    property string text: ""
    signal clicked()
    height: 60 * _controller.dpiFactor
    width: Math.max(100 * _controller.dpiFactor, textItem.contentWidth + 30 * _controller.dpiFactor)

    Image {
        id: shadow
        anchors.fill: parent
        source: "qrc:/img/shadow2.png"

        Rectangle {
            id: buttonRect
            width: root.width
            height: root.height

            anchors.centerIn: parent

            anchors.verticalCenterOffset: mouseArea.pressed ? 0 : -3 * _controller.dpiFactor
            anchors.horizontalCenterOffset: mouseArea.pressed ? 0 : -3 * _controller.dpiFactor
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
}
