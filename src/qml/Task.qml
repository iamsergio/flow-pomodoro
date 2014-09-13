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
    color: _style.taskBackgroundColor
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: _style.marginSmall
    anchors.rightMargin: _style.marginSmall
    height: _style.taskHeight
    border.color: _style.taskBorderColor
    border.width: selected ? _style.selectedTaskBorderWidth : 1
    radius: _style.taskBorderRadius

    Gradient {
        id: grad1
        GradientStop { color: _style.taskBackgroundColor; position: 0 }
        GradientStop { color: "#202020" ; position: 0.4 }
        GradientStop { color: "#101010" ; position: 0.6 }
        GradientStop { color: _style.taskBackgroundColor ; position: 1 }
    }

    gradient: selected ? null : grad1

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
                color: _style.taskTagFontColor
                elide: Text.ElideRight
                font.pixelSize: 12 * _controller.dpiFactor
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
            _controller.requestContextMenu(task)
        }

        Text {
            id: textItem
            text: root.taskSummary
            elide: Text.ElideRight
            color: _style.taskFontColor
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10* _controller.dpiFactor
            anchors.right: buttonRow.left
            anchors.rightMargin: 2
            font.pixelSize: _style.taskFontSize
            visible: !textField.visible
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
                toolTip: (taskObj !== null && taskObj.staged) ? qsTr("Archive task") : qsTr("Queue for today")
                source: (taskObj !== null && taskObj.staged) ? "image://icons/archive.png" : "image://icons/stage.png"
                visible: root.buttonsVisible
                onClicked: {
                    taskObj.staged = !taskObj.staged
                }
            }

            ClickableImage {
                id: deleteImage
                toolTip: qsTr("Delete task")
                source: "image://icons/delete.png"
                visible: root.buttonsVisible
                onClicked: {
                    root.deleteClicked()
                }
            }

            ClickableImage {
                id: playImage
                toolTip: qsTr("Start pomodoro on task")
                source: "image://icons/play.png"
                visible: root.buttonsVisible && (taskObj !== null && taskObj.staged) && !_controller.pomodoroFunctionalityDisabled
                onClicked: {
                    _controller.startPomodoro(root.taskObj, _style.defaultPomodoroDuration)
                }
            }
        }
    }
}
