import QtQuick 2.0

Item {
    id: root
    property alias model: repeater.model
    property int delegateHeight: _style.choiceDelegateHeight
    property string title: ""
    signal choiceClicked(var action)
    signal choiceToggled(bool checkState, string itemText)
    signal dismissPopup()

    function modelCount()
    {
        if (model !== null && typeof model !== "undefined")
            return model.count

        return 0
    }

    function ensureVisible(index)
    {
        if (index === 0)
            index = -1; // If we request for index 0 to be visible, lets also show the header

        if (!isVisible(index)) {
            // +1 because of header, and another +1 because numbering starts at 0
            var newContentY = (index + 2) * delegateHeight - flickable.height
            if (newContentY >= 0) {
                flickable.contentY = newContentY
            } else {
                flickable.contentY = (index + 1 + (index === 0 ? 0 : 0)) * delegateHeight
            }
        }
    }

    function isVisible(index)
    {
        return ((index + 2) * delegateHeight <= flickable.height + flickable.contentY) &&
                ((index + 1) * delegateHeight >= flickable.contentY)
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

        function choiceClicked(index, dismiss, action)
        {
            root.choiceClicked(action)
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
                    id: column1
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
                            anchors.leftMargin: 8 * _controller.dpiFactor
                            anchors.rightMargin: 8 * _controller.dpiFactor
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
                            objectName: "taskMenuChoice"
                            anchors.left: column1.left
                            anchors.right: column1.right
                            topLineVisible: index > 0
                            height: root.delegateHeight
                            fontAwesomeIconCode: iconRole
                            checked: checkedRole
                            current: index === _controller.currentMenuIndex
                            onClicked: {
                                if (checkableRole) {
                                    root.choiceToggled(index)
                                } else {
                                    mouseArea.choiceClicked(index, dismiss, actionRole)
                                }
                            }
                            onToggled: {
                                root.choiceToggled(checkState, itemText)
                            }
                            onCurrentChanged: {
                                if (current)
                                    root.ensureVisible(index);
                            }
                        }
                    }
                }
            }
        }
    }
}
