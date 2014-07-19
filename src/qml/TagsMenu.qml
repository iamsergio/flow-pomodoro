import QtQuick 2.1
import QtQuick.Controls 1.1

Menu {
    id: tagsMenu
    visible: _controller.rightClickedTask !== null
    title: qsTr("Tags")
    Instantiator {
        id: instantiator
        model: _controller.rightClickedTask === null ? 0 : _controller.rightClickedTask.checkableTagModel
        MenuItem {
            checkable: true
            checked: instantiator.model === null ? false : checkState
            text: instantiator.model === null ? "" : tag.name
            onToggled: {
                if (checked) {
                    _controller.rightClickedTask.addTag(text)
                } else {
                    _controller.rightClickedTask.removeTag(text)
                }
            }
        }

        onObjectAdded: tagsMenu.insertItem(index, object)
        onObjectRemoved: tagsMenu.removeItem(object)
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Configure Tags...")
        onTriggered: {
            _controller.configureTabIndex = Controller.TagsTab
            _controller.currentPage = Controller.ConfigurePage
        }
    }
}
