import QtQuick 2.0

Item {
    id: root
    property alias model: view.model
    property alias currentItem: view.currentItem
    readonly property int delegateWidth: 90 * _controller.dpiFactor

    implicitHeight: 30 * _controller.dpiFactor
    implicitWidth: view.count * delegateWidth

    Rectangle {
        id: backgroundRect
        anchors.fill: view
        color: "black"
        radius: Math.floor(4 * _controller.dpiFactor)
        anchors {
            fill: view
            leftMargin: -10 * _controller.dpiFactor
            rightMargin: -10 * _controller.dpiFactor
        }
    }


    ListView {
        id: view
        z: 1
        width: view.count * delegateWidth

        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        orientation: ListView.Horizontal
        delegate: Rectangle {
            width: delegateWidth
            height: view.height
            readonly property string source: model.source
            readonly property bool isCurrent: index == view.currentIndex
            readonly property bool isLast: index == view.count - 1
            color: (mouseArea.containsMouse && !isCurrent) ? "#222222" : backgroundRect.color
            Text {
                text: model.title
                anchors {
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: parent.horizontalCenter
                }

                font {
                    bold: isCurrent
                    pixelSize: (isCurrent ? 18 : 15) * _controller.dpiFactor
                }

                color: isCurrent ? _style.queueBackgroundColor : "white"
            }

            MouseArea {
                id: mouseArea
                hoverEnabled: true
                anchors.fill: parent
                onClicked: {
                    view.currentIndex = index
                }
            }

            Rectangle {
                visible: !isLast
                anchors {
                    topMargin: 5 * _controller.dpiFactor
                    bottomMargin: 5 * _controller.dpiFactor
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
                width: Math.floor(1 * _controller.dpiFactor)
                color: "#999999"
            }

            Rectangle {
                id: highlight
                visible: isCurrent && false
                color: Qt.rgba(67, 172, 232, 255)
                height: Math.floor(2 * _controller.dpiFactor)
                anchors {
                    bottom: parent.bottom
                    bottomMargin: Math.floor(1 * _controller.dpiFactor)
                    left: parent.left
                    right: parent.right
                    leftMargin: 10 * _controller.dpiFactor
                    rightMargin: 10 * _controller.dpiFactor
                }
            }

        }
    }
}
