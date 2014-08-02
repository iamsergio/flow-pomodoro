import QtQuick 2.0
import QtQuick.Controls 1.0

import Controller 1.0

Rectangle {

    signal deleteClicked()

    property QtObject taskObj: null
    property string taskSummary: taskObj !== null ? taskObj.summary : ""
    property bool inlineEditMode: _controller.taskBeingEdited === taskObj && _controller.editMode === Controller.EditModeInline
    property bool otherItemBeingEdited: _controller.taskBeingEdited !== taskObj && _controller.editMode !== Controller.EditModeNone
    property bool buttonsVisible: true
    property bool hasMouseOver: mouseArea.containsMouse
    property int modelIndex: -1
    property bool selected: _controller.selectedTask === taskObj && !inlineEditMode && taskObj !== null

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
        anchors.rightMargin: _style.tagRowRightMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: _style.tagRowBottomMargin
        clip: true
        visible: !root.inlineEditMode

        Repeater {
            model: root.taskObj === null ? 0 : root.taskObj.tagModel
            Text {
                property bool last: root.taskObj.tagModel.count === index + 1
                // For some reason when removing a tag QML prints a warning about tag being undefined.
                // The underlying model signals are correct, so this looks like a Qt bug.
                text: typeof tag != 'undefined' ? (tag.name + (last ? "" : ", ")) : ""
                color: root.selected ?  _style.selectedTaskTagFontColor : _style.taskTagFontColor
                elide: Text.ElideRight
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.LeftButton) {
                _controller.editTask(null, Controller.EditModeNone)
                _controller.toggleSelectedTask(task)
            } else {
                _controller.requestContextMenu(task)
            }
        }

        onDoubleClicked: {
            _controller.editTask(task, Controller.EditModeInline)
        }

        onPressAndHold: {
            _controller.editTask(task, Controller.EditModeInline)
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
            font.bold: textItem.font.bold
        }

        TaskTextField {
            id: textField
            objectName: "inline text field"
            visible: root.inlineEditMode
            anchors.left: textItem.left
            anchors.top: parent.top
            anchors.topMargin:  _style.marginMedium
            anchors.right: parent.right
            anchors.rightMargin: _style.marginMedium
            taskIndex: modelIndex
        }

        Row {
            id: buttonRow
            anchors.right: parent.right
            anchors.rightMargin: _style.buttonsRightMargin
            anchors.verticalCenter: parent.verticalCenter
            spacing: _style.buttonsSpacing

            ClickableImage {
                id: archiveImage
                toolTip: qsTr("Archive task")
                source: (taskObj !== null && taskObj.staged) ? "qrc:/img/archive.png" : "qrc:/img/stage.png"
                visible: root.buttonsVisible
                onClicked: {
                    taskObj.staged = !taskObj.staged
                }
            }

            ClickableImage {
                id: deleteImage
                toolTip: qsTr("Delete task")
                source: "qrc:/img/delete.png"
                visible: root.buttonsVisible
                onClicked: {
                    root.deleteClicked()
                }
            }

            ClickableImage {
                id: editImage
                toolTip: qsTr("Edit task")
                source: "qrc:/img/edit.png"
                visible: root.buttonsVisible && false // Disabled for now, not enough space
                onClicked: {
                    if (modelIndex !== -1) {
                        _controller.editTask(root.taskObj, Controller.EditModeEditor)
                        if (_controller.editMode === Controller.EditModeInline) {
                            textField.forceActiveFocus()
                        }
                    }
                }
            }

            ClickableImage {
                id: playImage
                toolTip: qsTr("Start pomodoro on task")
                source: "qrc:/img/play.png"
                visible: root.buttonsVisible && (taskObj !== null && taskObj.staged)
                onClicked: {
                    _controller.startPomodoro(root.taskObj, _style.defaultPomodoroDuration)
                }
            }
        }
    }
}
