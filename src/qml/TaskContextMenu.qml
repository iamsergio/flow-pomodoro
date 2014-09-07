import QtQuick 2.1
import QtQuick.Controls 1.1
import Controller 1.0

Menu {
    id: root
    property QtObject task: null
    property bool showConfigureItem: true

    visible: task !== null
    title: qsTr("Tags")

    MenuItem {
        text: qsTr("Edit...")
        onTriggered: {
            _controller.editTask(_controller.rightClickedTask, Controller.EditModeEditor)
        }
    }

    MenuItem {
        text: qsTr("Delete")
        onTriggered: {
            _controller.removeTask(_controller.rightClickedTask)
        }
    }

    MenuSeparator {}

    Instantiator {
        id: instantiator
        model: task === null ? 0 : task.checkableTagModel
        MenuItem {
            checkable: true
            checked: instantiator.model === null ? false : checkState
            text: instantiator.model === null ? "" : tag.name
            onToggled: {
                if (task !== null) { // For some reason this is triggered when popup closes, so check for null
                    if (checked) {
                        task.addTag(text)
                    } else {
                        task.removeTag(text)
                    }
                }
            }
        }

        onObjectAdded: root.insertItem(index, object)
        onObjectRemoved: root.removeItem(object)
    }
    MenuSeparator {
        visible: showConfigureItem
    }
    MenuItem {
        visible: showConfigureItem
        text: qsTr("Configure Tags...")
        onTriggered: {
            _controller.configureTabIndex = Controller.TagsTab
            _controller.currentPage = Controller.ConfigurePage
        }
    }
}
