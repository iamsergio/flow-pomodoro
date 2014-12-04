import QtQuick 2.0

Item {
    id: root
    property alias model: repeater.model
    property int delegateHeight: _style.choiceDelegateHeight
    property string title: ""
    signal choiceClicked(var index)
    signal choiceToggled(bool checkState, string itemText)
    signal dismissPopup()

    function modelCount()
    {
        if (model !== null && typeof model !== "undefined")
            return model.count

        return 0
    }

    Rectangle {
        id: background
        anchors.fill: parent
        opacity: 0.5
        color: "black"
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: false
        onClicked: {
            root.dismissPopup()
        }

        function choiceClicked(index, dismiss)
        {
            root.choiceClicked(index)
            if (dismiss)
                root.dismissPopup()
        }

        BorderImage {
            id: borderImage
            anchors.fill: flickable
            anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
            border { left: 10; top: 10; right: 10; bottom: 10 }
            source: "qrc:/img/shadow.png"
        }

        Flickable {
            id: flickable
            z: background.z + 1
            height: Math.min(popupRect.height, parent.height - 100 * _controller.dpiFactor)
            width:  popupRect.width
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            contentHeight: popupRect.height
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            Rectangle {
                id: popupRect
                width: root.width * 0.8
                height: delegateHeight * root.modelCount() + (header.visible ? header.height : 0)

                anchors.centerIn: parent
                border.color: "gray"
                border.width: 1 * _controller.dpiFactor
                radius: 2 * _controller.dpiFactor

                Column {
                    id: column
                    anchors.fill: parent

                    Rectangle {
                        id: header
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: root.delegateHeight
                        visible: root.title
                        Text {
                            renderType: _controller.textRenderType
                            text: root.title
                            anchors.fill: parent
                            elide: Text.ElideRight
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 20 * _controller.dpiFactor
                            fontSizeMode: Text.Fit
                            minimumPixelSize: 16 * _controller.dpiFactor
                        }

                        Rectangle {
                            color: "lightblue"
                            height: 5 * _controller.dpiFactor
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                root.dismissPopup()
                            }
                        }
                    }

                    Repeater {
                        id: repeater
                        model: root.model
                        Choice {
                            anchors.left: column.left
                            anchors.right: column.right
                            topLineVisible: index > 0
                            height: root.delegateHeight
                            fontAwesomeIconCode: iconRole
                            checked: checkedRole
                            onClicked: {
                                if (checkableRole) {
                                    root.choiceToggled(index)
                                } else {
                                    mouseArea.choiceClicked(index, dismiss)
                                }
                            }
                            onToggled: {
                                root.choiceToggled(checkState, itemText)
                            }
                        }
                    }
                }
            }
        }
    }
}
