import QtQuick 2.0
import QtQuick.Controls 1.0

Rectangle {

    signal deleteClicked()

    property QtObject taskObj: null
    property bool editMode: false
    property bool otherItemBeingEdited: false
    property bool buttonsVisible: true
    property bool hasMouseOver: mouseArea.containsMouse
    property int modelIndex: -1
    property bool selected: _controller.selectedIndex === modelIndex && !editMode && modelIndex !== -1


    id: root
    color: selected ? _style.selectedTaskBgColor : _style.taskBackgroundColor
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: _style.marginSmall
    anchors.rightMargin: _style.marginSmall
    height: 50 * _controller.dpiFactor
    border.color: _style.taskBorderColor
    border.width: selected ? _style.selectedTaskBorderWidth : 1
    radius: _style.taskBorderRadius
/*
    Row {
        anchors.left: parent.left
        anchors.leftMargin: 3
        anchors.top: parent.top
        spacing: 5
        Text {
            text: root.taskObj === null ? 0 : root.taskObj.tagModel.count
            color: "white"
        }
        Repeater {
            model: root.taskObj === null ? 0 : root.taskObj.tagModel
            Text {
                text: tag.name + ": " + tag.taskCount
                color: "white"
            }
        }
    }*/

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            _controller.indexBeingEdited = -1
            _controller.toggleSelectedIndex(modelIndex)
        }

        onPressAndHold: {
            _controller.indexBeingEdited = modelIndex
            textField.forceActiveFocus()
        }

        Text {
            id: textItem
            text: root.taskObj === null ? "" : root.taskObj.text
            elide: Text.ElideRight
            color: root.selected ? _style.selectedTaskFgColor : _style.taskFontColor
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10* _controller.dpiFactor
            anchors.right: parent.right
            anchors.rightMargin: (16*3 + 15) * _controller.dpiFactor // 3 icons, 3 margins
            font.pointSize: invisibleHelper.isOverflowing ? _style.taskFontSize - 5
                                                          : _style.taskFontSize
            visible: !textField.visible
        }

        Text {
            id: invisibleHelper
            property bool isOverflowing: contentWidth > width
            visible: false
            anchors.fill: textItem
            text: textItem.text
            font.pointSize: _style.taskFontSize
        }

        Connections {
            target: _controller
            onForceFocus: {
                if (index === modelIndex && modelIndex !== -1) {
                    textField.forceActiveFocus()
                    textField.selectAll()
                }
            }
        }

        TextField {
            id: textField
            visible: root.editMode
            text: root.taskObj === null ? "" : root.taskObj.text
            anchors.left: textItem.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: deleteImage.left
            anchors.rightMargin: _style.marginMedium
            focus: true
            onVisibleChanged: {
                if (visible) {
                    text = root.taskObj.text
                    forceActiveFocus()
                } // TODO only write when not visible
            }

            onTextChanged: {
                if (visible) {
                    root.taskObj.text = text
                }
            }
        }

        ClickableImage {
            id: deleteImage
            source: "qrc:/img/delete.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: editImage.left
            anchors.rightMargin: _style.buttonsSpacing
            visible: root.buttonsVisible
            onClicked: {
                root.deleteClicked()
            }
        }

        ClickableImage {
            id: editImage
            source: "qrc:/img/edit.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: playImage.left
            anchors.rightMargin: _style.buttonsSpacing
            visible: root.buttonsVisible
            onClicked: {

                if (modelIndex !== -1) {
                    _controller.indexBeingEdited = modelIndex
                    if (_controller.indexBeingEdited !== -1) {
                        textField.forceActiveFocus()
                    }
                }
            }
        }

        ClickableImage {
            id: playImage
            source: "qrc:/img/play.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: _style.buttonsRightMargin
            visible: root.buttonsVisible
            onClicked: {
                _controller.startPomodoro(modelIndex, _style.defaultPomodoroDuration)
            }
        }
    }
}
