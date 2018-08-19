import QtQuick 2.0
import Controller 1.0

Item {
    anchors.fill: parent
    visible: _controller.optionsContextMenuVisible
    ListModel {
        id: optionsModel
        ListElement { textRole: "Configure ..."; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true; actionRole: "configure" }
        ListElement { textRole: "About ..."; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true; actionRole: "about" }
        ListElement { textRole: "Quit"; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true; actionRole: "quit" }
    }

    ChoicePopup {
        anchors.fill: parent
        model: optionsModel
        onChoiceClicked: {
            _controller.optionsContextMenuVisible = false
            if (action === "quit") {
                _controller.currentPage = Controller.ConfigurePage
            } else if (action === "about") {
                _controller.currentPage = Controller.AboutPage
            } else if (action === "configure") {
                Qt.quit()
            }
        }

        onDismissPopup: {
            _controller.optionsContextMenuVisible = false
        }
    }
}
