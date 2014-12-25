import QtQuick 2.1
import Controller 1.0

Item {
    id: root
    anchors.fill: parent

    ChoicePopup {
        id: choicePopup
        focus: true
        anchors.fill: parent
        model: _controller.rightClickedTask === null ? null : _controller.rightClickedTask.contextMenuModel
        title: _controller.rightClickedTask !== null ? (_controller.rightClickedTask.contextMenuModel.tagOnlyMenu ? qsTr("Select tags") : _controller.rightClickedTask.summary) : ""
        onChoiceClicked: {
            processChoice(index)
        }

        function processChoice(index)
        {
            if (index === TaskContextMenuModel.OptionTypeEdit) {
                _controller.editTask(_controller.rightClickedTask, Controller.EditModeInline)
            } else if (index ===  TaskContextMenuModel.OptionTypeDelete) {
                _controller.removeTask(_controller.rightClickedTask)
            } else if (index ===  TaskContextMenuModel.OptionTypeNewTag) {
                _controller.newTagDialogVisible = true
            } else if (index ===  TaskContextMenuModel.OptionTypeQueue) {
                _controller.rightClickedTask.staged = !_controller.rightClickedTask.staged
            }

            if (index !== TaskContextMenuModel.OptionTypeNewTag)
                _controller.setRightClickedTask(null)
        }

        onChoiceToggled: {
            _controller.rightClickedTask.toggleTag(itemText)
        }

        onDismissPopup: {
            _controller.setRightClickedTask(null)
        }

        Connections {
            target: _controller
            onEnterPressed: {
                if (_controller.rightClickedTask != null && _controller.currentMenuIndex != -1)
                    choicePopup.processChoice(_controller.currentMenuIndex)
            }
        }
    }
}
