import QtQuick 2.0
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
        anchors.leftMargin: 22 * _controller.dpiFactor
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
                renderType: _controller.textRenderType
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
            _controller.editTask(task, Controller.EditModeInline)
        }

        Text {
            id: textItem
            renderType: _controller.textRenderType
            text: root.taskSummary
            elide: Text.ElideRight
            color: _style.taskFontColor
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: menuIndicator.right
            anchors.leftMargin: 5 * _controller.dpiFactor
            anchors.right: buttonRow.left
            anchors.rightMargin: 2
            font.pixelSize: _style.taskFontSize
            fontSizeMode: Text.HorizontalFit
            visible: !root.inlineEditMode
        }

        Loader {
            // Startup performance improvement, since QtQuick.Controlls slows this down
            id: taskTextFieldLoader
            anchors.left: textItem.left
            anchors.top: parent.top
            anchors.topMargin: _style.marginMedium
            anchors.right: parent.right
            anchors.rightMargin: _style.marginMedium
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.marginMedium
            sourceComponent: _loadManager.inlineEditorRequested ? taskTextFieldComponent : null
        }

        Component {
            id: taskTextFieldComponent
            TaskTextField {
                id: textField
                objectName: "inline text field"
                visible: root.inlineEditMode
                anchors.fill: parent
                taskIndex: modelIndex
            }
        }

        Rectangle {
            id: menuIndicator
            anchors.left: parent.left
            anchors.leftMargin: 6 * _controller.dpiFactor
            width: 10 * _controller.dpiFactor
            color: taskOptionsMouseArea.pressed ? "gray" : "transparent"
            height: iconItem.contentHeight + 10 * _controller.dpiFactor
            anchors.verticalCenter: parent.verticalCenter
            radius: 5 * _controller.dpiFactor

            Text {
                id: iconItem
                renderType: _controller.textRenderType
                anchors.fill: parent
                color: "white"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 23 * _controller.dpiFactor
                font.family: "FontAwesome"
                text: "\uf142"
            }

            MouseArea {
                id: taskOptionsMouseArea
                anchors.fill: parent
                // Bigger hit area, for touch
                anchors.margins: -10 * _controller.dpiFactor
                onClicked: {
                    _controller.requestContextMenu(task)
                }
            }
        }

        Row {
            id: buttonRow
            anchors.right: parent.right
            anchors.rightMargin: _style.buttonsRightMargin
            anchors.verticalCenter: parent.verticalCenter
            spacing: _style.buttonsSpacing

            /*FontAwesomeIcon {
                id: deleteIcon
                toolTip: qsTr("Delete task")
                size: 30
                text: "\uf014"
                visible: root.buttonsVisible
                onClicked: {
                    _controller.removeTask(taskObj)
                }
            }*/

            FontAwesomeIcon {
                id: archiveIcon
                toolTip: (taskObj !== null && taskObj.staged) ? qsTr("Archive task") : qsTr("Queue for today")
                size: 30
                text: (taskObj !== null && taskObj.staged) ? "\uf187" : "\uf06a"
                visible: root.buttonsVisible
                onClicked: {
                    taskObj.staged = !taskObj.staged
                }
            }

            FontAwesomeIcon {
                id: playIcon
                size: 30
                text: "\uf01d"
                visible: root.buttonsVisible && (taskObj !== null && taskObj.staged) && !_controller.pomodoroFunctionalityDisabled
                onClicked: {
                    _controller.startPomodoro(root.taskObj, _style.defaultPomodoroDuration)
                }
            }
        }
    }
}
