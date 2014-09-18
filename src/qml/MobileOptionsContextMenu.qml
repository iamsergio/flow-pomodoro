import QtQuick 2.0
import Controller 1.0

Item {
    anchors.fill: parent
    visible: _controller.optionsContextMenuVisible
    ListModel {
        id: optionsModel
        ListElement { textRole: "Configure ..."; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true }
        ListElement { textRole: "About ..."; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true }
        ListElement { textRole: "Quit"; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true }
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
