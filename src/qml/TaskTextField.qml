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

    Binding {
        // two way binding
        target: _controller.taskBeingEdited
        when: textField.visible && _controller.editMode !== Controller.EditModeNone
        property: "summary"
        value: textField.text
    }

    Connections {
        target: _controller
        onForceFocus: {
            if (textField.visible && index === taskIndex && taskIndex !== -1) {
                textField.forceActiveFocus()
                textField.selectAll()
            }
        }
    }

    style: TextFieldStyle {
        // Set text color explicitely, for some reason it defaults to gray on blackberry
        textColor: "black"
    }

    onAccepted: {
        _controller.editTask(null, Controller.EditModeNone)
    }
}
