import QtQuick 2.0
import QtQuick.Controls 2.0

import Controller 1.0

TextField {
    id: textField
    property int taskIndex: -1
    property QtObject task: null
    text: _controller.taskBeingEdited.summary
    focus: true
    inputMethodHints: Qt.ImhNoPredictiveText
    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else if (_controller.isMobile) {
            Qt.inputMethod.hide()
        }
    }

    Connections {
        target: _controller
        onForceFocus: {
            if (textField.visible && task === root.taskObj) {
                textField.forceActiveFocus()
                textField.selectAll()
            }
        }

        onAboutToAddTask: {
            // Save the current text when user presses the add icon again:
            if (textField.visible && _controller.taskBeingEdited) {
                _controller.taskBeingEdited.summary = textField.text
            }
        }
    }

    color: "white"
    font.pixelSize: 13 * _controller.dpiFactor

    function save()
    {
        _controller.taskBeingEdited.summary = textField.text
        _controller.editTask(null, Controller.EditModeNone)
    }

    onAccepted: {
        textField.save()
    }

    Connections {
        target: textField.visible ? _controller : null
        ignoreUnknownSignals: true
        onTaskEditorDismissed: {
            textField.save()
        }
    }
}
