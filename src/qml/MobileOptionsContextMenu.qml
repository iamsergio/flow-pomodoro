import QtQuick 2.0

Item {
    anchors.fill: parent
    visible: _controller.optionsContextMenuVisible
    ListModel {
        id: optionsModel
        ListElement { itemText: "Configure ..."; checkable: false }
        ListElement { itemText: "About ..."; checkable: false }
        ListElement { itemText: "Quit"; checkable: false }
    }

    ChoicePopup {
        anchors.fill: parent
        model: optionsModel
        onChoiceClicked: {
            _controller.optionsContextMenuVisible = false
            if (index === 0) {
                _controller.toggleConfigurePage()
            } else if (index === 1) {
                _controller.currentPage = Controller.AboutPage
            } else if (index === 2) {
                Qt.quit()
            }
        }

        onDismissPopup: {
            _controller.optionsContextMenuVisible = false
        }
    }
}
