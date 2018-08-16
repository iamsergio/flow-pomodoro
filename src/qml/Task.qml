import QtQuick 2.0
import Controller 1.0
import Task 1.0

Rectangle {
    id: root
    signal deleteClicked()

    readonly property string taskSummary: taskObj !== null ? taskObj.summary : ""
    readonly property bool inlineEditMode: _controller.taskBeingEdited === taskObj && _controller.editMode === Controller.EditModeInline
    readonly property bool otherItemBeingEdited: _controller.taskBeingEdited !== taskObj && _controller.editMode !== Controller.EditModeNone
    readonly property bool hasMouseOver: mouseArea.containsMouse
    readonly property bool selected: _controller.selectedTask === taskObj && !inlineEditMode && taskObj !== null
    property int modelIndex: -1
    property bool buttonsVisible: true
    property QtObject taskObj: null
    property bool taskIsHot: taskObj && taskObj.priority === Task_.PriorityHigh

    color: _style.taskBackgroundColor

    anchors {
        left: parent.left
        right: parent.right
        leftMargin: selected ? selectionTaskRectangle.width + _style.marginSmall
                             : _style.marginSmall
        rightMargin: _style.marginSmall
    }

    height: _style.taskHeight
    border.color: _style.taskBorderColor
    border.width: 1 * _controller.dpiFactor
    radius: _style.taskBorderRadius

    Gradient {
        id: grad1
        GradientStop { color: _style.taskBackgroundColor; position: 0 }
        GradientStop { color: "#202020" ; position: 0.4 }
        GradientStop { color: "#101010" ; position: 0.6 }
        GradientStop { color: _style.taskBackgroundColor ; position: 1 }
    }

    gradient: grad1

    Rectangle {
        id: selectionTaskRectangle
        width: _style.tagTabSelectionHeight
        anchors {
            top: parent.top
            bottom: parent.bottom
            topMargin: 2 * _controller.dpiFactor
            bottomMargin: 2 * _controller.dpiFactor
            left: parent.left
            leftMargin: -width - 2 * _controller.dpiFactor
        }

        color: "#43ACE8"
        visible: root.selected
    }

    Row {
        id: tagRow
        anchors {
            left: parent.left
            leftMargin: 22 * _controller.dpiFactor
            right: parent.right
            rightMargin: _style.tagRowRightMargin
            bottom: parent.bottom
            bottomMargin: _style.tagRowBottomMargin
        }

        clip: true
        visible: !root.inlineEditMode

        Repeater {
            model: root.taskObj === null ? 0 : root.taskObj.tagModel
            Text {
                property bool last: root.taskObj.tagModel.count === index + 1
                // For some reason when removing a tag QML prints a warning about tag being undefined.
                // The underlying model signals are correct, so this looks like a Qt bug.
                text: typeof tag != 'undefined' ? (tag.name + (last ? "" : ", ")) : ""
                color: root.taskIsHot ? _style.hotColor : _style.taskTagFontColor
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
        cursorShape: (task && task.isUrl) ? Qt.PointingHandCursor : Qt.ArrowCursor

        onClicked: {
            if (mouse.button === Qt.LeftButton) {
                if (_controller.editMode != Controller.EditModeNone) {
                    _controller.dismissTaskEditor()
                } else if (task.isUrl) {
                    _controller.openUrl(task.summary)
                }
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
            objectName: "taskTextItem"
            renderType: _controller.textRenderType
            text: root.taskSummary
            elide: Text.ElideRight
            color: (root.taskObj && root.taskObj.priority === Task_.PriorityHigh) ? _style.hotColor : _style.taskFontColor
            font.bold: true
            anchors {
                verticalCenter: parent.verticalCenter
                left: menuIndicator.right
                leftMargin: 5 * _controller.dpiFactor
                right: buttonRow.left
                rightMargin: 2
            }

            font.pixelSize: _style.taskFontSize
            fontSizeMode: Text.HorizontalFit
            visible: !root.inlineEditMode
        }

        Loader {
            // Startup performance improvement, since QtQuick.Controls slows this down
            id: taskTextFieldLoader
            anchors {
                left: textItem.left
                top: parent.top
                topMargin: _style.marginMedium
                right: parent.right
                rightMargin: _style.marginMedium
                bottom: parent.bottom
                bottomMargin: _style.marginMedium
            }

            sourceComponent: _loadManager.inlineEditorRequested ? taskTextFieldComponent : null
            objectName: "taskTextFieldLoader"
        }

        Component {
            id: taskTextFieldComponent
            TaskTextField {
                id: textField
                task: root.taskObj
                objectName: "inline text field"
                visible: root.inlineEditMode
                anchors.fill: parent
                taskIndex: modelIndex
            }
        }

        Rectangle {
            id: menuIndicator
            objectName: "menuIndicator"
            width: 10 * _controller.dpiFactor
            color: taskOptionsMouseArea.pressed ? "gray" : "transparent"
            height: iconItem.contentHeight + 10 * _controller.dpiFactor
            radius: 5 * _controller.dpiFactor

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 6 * _controller.dpiFactor
            }

            Text {
                id: iconItem
                renderType: _controller.textRenderType
                anchors.fill: parent
                color: root.taskIsHot ? _style.hotColor : "white"
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

            anchors {
                right: parent.right
                rightMargin: _style.buttonsRightMargin
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: (!root.taskObj || root.taskObj.priority === Task_.PriorityNone) ? 0
                                                                                                      : -3 * _controller.dpiFactor
            }

            spacing: _style.buttonsSpacing

            FontAwesomeIcon {
                id: archiveIcon
                objectName: "archiveIcon"
                toolTip: (taskObj !== null && taskObj.staged) ? qsTr("Queue for later") : qsTr("Queue for today")
                size: 29
                text: (taskObj !== null && taskObj.staged) ? "\uf187" : "\uf06a"
                visible: root.buttonsVisible
                onClicked: {
                    taskObj.staged = !taskObj.staged
                }
            }

            FontAwesomeIcon {
                id: playIcon
                objectName: "playIcon"
                size: 29
                text: "\uf01d"
                visible: root.buttonsVisible && (taskObj !== null && taskObj.staged) && !_settings.pomodoroFunctionalityDisabled
                onClicked: {
                    _controller.startPomodoro(root.taskObj, _style.defaultPomodoroDuration)
                }
            }
        }
    }
    Text {
        color: _style.taskTagFontColor

        text: root.taskObj ? root.taskObj.upperRightCornerText : ""
        visible: root.taskObj !== null
        anchors {
            top: parent.top
            right: parent.right
            rightMargin: _controller.dpiFactor * 3
            topMargin: _controller.dpiFactor * 2
        }

        font.pixelSize: _controller.dpiFactor * 11
    }

    Text {
        function colorForPriority(priority)
        {
            if (priority === Task_.PriorityLow) {
                return _style.coldColor
            } else if (priority === Task_.PriorityHigh) {
                return _style.hotColor
            }
            return "white" // or whatever color
        }

        color: root.taskObj ? colorForPriority(root.taskObj.priority) : "white"
        text: root.taskObj ? root.taskObj.priorityStr : ""
        visible: root.taskObj !== null && root.taskObj.priority !== Controller.PriorityNone
        anchors {
            bottom: parent.bottom
            right: parent.right
            rightMargin: _controller.dpiFactor * 3
            bottomMargin: _controller.dpiFactor * 2
        }

        font.pixelSize: _controller.dpiFactor * 10
    }

    Text {
        color: root.taskObj ? root.taskObj.isOverdue ? "red"
                                                     : root.taskObj.dueToday ? "yellow"
                                                                             : _style.taskTagFontColor
                            : _style.taskTagFontColor
        text: root.taskObj ? root.taskObj.prettyDueDateRecurString : ""
        visible: root.taskObj !== null && _settings.supportsDueDate && root.taskObj.prettyDueDateString && !(_controller.editMode == Controller.EditModeInline && _controller.taskBeingEdited === root.taskObj)
        anchors {
            top: parent.top
            left: parent.left
            leftMargin: _controller.dpiFactor * 20
            bottomMargin: _controller.dpiFactor * 2
        }

        font.pixelSize: _controller.dpiFactor * 11
    }
}
