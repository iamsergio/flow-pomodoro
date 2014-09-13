import QtQuick 2.1
import Controller 1.0

Item {
    id: root
    anchors.fill: parent
    visible: _controller.rightClickedTask !== null
    ListModel {
        id: menuModel
        ListElement { itemText: "Edit ..."; checkable: false }
        ListElement { itemText: "Delete"; checkable: false }
        ListElement { itemText: "ConfigureTags ..."; checkable: false }
    }

    MobileChoicePopup {
        anchors.fill: parent
        model: menuModel
        secondaryModel: visible ? _controller.rightClickedTask.checkableTagModel : null
        title: _controller.rightClickedTask !== null ? _controller.rightClickedTask.summary : ""
        onChoiceClicked: {
            if (index === 0) {
                // Edit
                _controller.editTask(_controller.rightClickedTask, Controller.EditModeInline)
            } else if (index === 1) {
                // Delete
                _controller.removeTask(_controller.rightClickedTask)
            } else if (index === 2) {
                _controller.configureTabIndex = Controller.TagsTab
                _controller.currentPage = Controller.ConfigurePage
            } else {
                console.warn("Unknown index " + index)
            }
            _controller.setRightClickedTask(null)
        }

        onChoiceToggled: {
            if (checkState) {
                _controller.rightClickedTask.addTag(itemText)
            } else {
                _controller.rightClickedTask.removeTag(itemText)
            }
        }

        onDismissPopup: {
            _controller.setRightClickedTask(null)
        }
    }
}
