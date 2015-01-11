import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.1

import Controller 1.0

TextField {
    id: textField
    property int taskIndex: -1
    text: _controller.taskBeingEdited.summary
    focus: true
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
            if (textField.visible && index === taskIndex && taskIndex !== -1) {
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

    font.pixelSize: 13 * _controller.dpiFactor
    textColor: _controller.isAndroid ? "white" : "black"

    onAccepted: {
        _controller.taskBeingEdited.summary = textField.text
        _controller.editTask(null, Controller.EditModeNone)
    }
}
