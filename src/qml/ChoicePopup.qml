import QtQuick 2.0

Item {
    id: root
    property alias model: repeater.model
    property alias secondaryModel: secondaryRepeater.model // Was too lazy to create a model that agregates CheckableTagModel with a couple of custom items (Edit/Delete)
    property int delegateHeight: _style.choiceDelegateHeight
    property string title: ""
    signal choiceClicked(var index)
    signal choiceToggled(bool checkState, string itemText)
    signal dismissPopup()

    function modelCount()
    {
        var count = 0
        if (typeof model !== "undefined")
            count += model.count

        if (typeof secondaryModel !== "undefined")
            count += secondaryModel.count

        return count
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

        function choiceClicked(index)
        {
            root.choiceClicked(index)
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
                visible: root.model.count > 0
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
                            text: root.title
                            anchors.fill: parent
                            elide: Text.ElideRight
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 20 * _controller.dpiFactor
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
                            fontAwesomeIconCode: iconCode
                            onClicked: {
                                if (checkable) {
                                    root.choiceToggled(index)
                                } else {
                                    mouseArea.choiceClicked(index)
                                }
                            }
                        }
                    }

                    Repeater {
                        id: secondaryRepeater
                        model: root.secondaryModel
                        Choice {
                            anchors.left: column.left
                            anchors.right: column.right
                            height: root.delegateHeight
                            topLineVisible: true
                            checked: checkState
                            onClicked: {
                                mouseArea.choiceClicked(index + root.model.count)
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
