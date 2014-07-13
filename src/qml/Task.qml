import QtQuick 2.0
import QtQuick.Controls 1.0

import Controller 1.0

Rectangle {

    signal deleteClicked()

    property QtObject taskObj: null
    property string taskSummary: taskObj !== null ? taskObj.summary : ""
    property bool inlineEditMode: _controller.taskBeingEdited === taskObj && _controller.editMode === Controller.EditModeInline
    property bool otherItemBeingEdited: _controller.taskBeingEdited !== taskObj && _controller.taskBeingEdited !== null
    property bool buttonsVisible: true
    property bool hasMouseOver: mouseArea.containsMouse
    property int modelIndex: -1
    property bool selected: _controller.selectedIndex === modelIndex && !inlineEditMode && modelIndex !== -1

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

    Row {
        id: tagRow
        anchors.left: parent.left
        anchors.leftMargin: textItem.anchors.leftMargin
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 1
        clip: true
        visible: !root.inlineEditMode

        Repeater {
            model: root.taskObj === null ? 0 : root.taskObj.tagModel
            Text {
                property bool last: root.taskObj.tagModel.count === index + 1
                text: tag.name + (last ? "" : ", ")
                color: root.selected ?  _style.selectedTaskTagFontColor : _style.taskTagFontColor
                elide: Text.ElideRight
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            _controller.editTask(-1, Controller.EditModeNone)
            _controller.toggleSelectedIndex(modelIndex)
        }

        onPressAndHold: {
            _controller.editTask(modelIndex, Controller.EditModeInline)
            textField.forceActiveFocus()
        }

        Text {
            id: textItem
            text: root.taskSummary
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
            visible: root.inlineEditMode
            text: root.taskSummary
            anchors.left: textItem.left
            anchors.top: parent.top
            anchors.topMargin:  _style.marginMedium
            anchors.right: parent.right
            anchors.rightMargin: _style.marginMedium
            focus: true
            onVisibleChanged: {
                if (visible) {
                    text = root.taskObj.summary
                    forceActiveFocus()
                } // TODO only write when not visible
            }

            Binding {
                // two way binding
                target: _controller.taskBeingEdited
                when: textField.visible && _controller.taskBeingEdited !== null
                property: "summary"
                value: textField.text
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
                    _controller.editTask(modelIndex, Controller.EditModeEditor)
                    if (_controller.taskBeingEdited !== null) {
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
