import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    property alias model: repeater.model
    property int delegateHeight: 50 * _controller.dpiFactor
    signal choiceClicked(var index)

    Rectangle {
        id: background
        anchors.fill: parent
        opacity: 0.3
        color: "blue"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            _controller.optionsContextMenuVisible = false
        }

        Item {
            id: shadowContainer
            z: background.z + 1

            anchors.centerIn: parent
            width:  popupRect.width  + 16 * _controller.dpiFactor
            height: popupRect.height + 16 * _controller.dpiFactor

            Rectangle {
                id: popupRect
                width: root.width * 0.8
                height: delegateHeight * root.model.count

                anchors.centerIn: parent
                border.color: "gray"
                border.width: 1 * _controller.dpiFactor
                visible: root.model.count > 0

                Column {
                    anchors.fill: parent
                    Repeater {
                        id: repeater
                        model: testModel
                        Rectangle {
                            id: delegate
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: root.delegateHeight
                            color: itemMouseArea.pressed ? "#E3E3E3" : "white"

                            Text {
                                color: "black"
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 15 * _controller.dpiFactor
                                text: itemText
                                font.pixelSize: 19 * _controller.dpiFactor
                            }

                            Rectangle {
                                id: topLine
                                height: popupRect.border.width
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                visible: index > 0
                                color: "gray"
                            }
                            MouseArea {
                                id: itemMouseArea
                                anchors.fill: parent
                                onClicked: {
                                    root.choiceClicked(index)
                                }
                            }
                        }
                    }
                }
            }
        }

        DropShadow {
            id: shadowEffect
            anchors.fill: shadowContainer
            cached: true
            smooth: true
            horizontalOffset: 3 * _controller.dpiFactor
            verticalOffset: 3 * _controller.dpiFactor
            radius: 8.0
            samples: 16
            color: "#80000000"
            source: shadowContainer
        }
    }
}
