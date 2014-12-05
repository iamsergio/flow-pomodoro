import QtQuick 2.0

Item {
    id: root
    property QtObject flickable: null
    property real delta: flickable.contentItem.height - flickable.height
    property real percentage: Math.round(100 * Math.max(Math.min(visible ? flickable.contentY / delta : 0, 1), 0), 0)
    visible: delta > 0

    width: 15 * _controller.dpiFactor
    MouseArea {
        anchors.fill: parent

        onWheel: {
            if (wheel.angleDelta.y < 0 && !root.flickable.atYEnd) {
                root.flickable.contentY += root.delta / 10
            } else if (wheel.angleDelta.y > 0 && !root.flickable.atYBeginning) {
                root.flickable.contentY -= root.delta / 10
            }
        }

        onClicked: {
            if (mouse.y < handle.y) {
                root.flickable.contentY -= Math.max(0, 20 * _controller.dpiFactor)
            } else if (mouse.y > handle.y + handle.height) {
                root.flickable.contentY += Math.min(20 * _controller.dpiFactor, root.delta)
            }
        }
        Item {
            id: handle
            width: parent.width
            height: Math.max(root.flickable.height * root.flickable.height / root.flickable.contentItem.height, 40 * _controller.dpiFactor)

            Rectangle {
                id: handleBackground
                width: 4 * _controller.dpiFactor
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                color: "black"
                radius: 5 * _controller.dpiFactor
            }
            onYChanged: {
                if (mouseArea.drag.active) {
                    root.flickable.contentY = (y / (root.height - handle.height)) * root.delta
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.YAxis
                drag.minimumY: 0
                drag.maximumY: root.height - handle.height
            }
        }
    }

    Connections {
        // I could just use a binding y: foo, but it prints a binding loop
        target: root.flickable
        onContentYChanged: {
            if (!mouseArea.drag.active) {
                handle.y = root.percentage * (root.height - handle.height) / 100
            }
        }
    }
}
