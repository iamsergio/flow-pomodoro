import QtQuick 2.0
import Controller 1.0

Item {
    anchors.fill: parent
    visible: _controller.optionsContextMenuVisible
    ListModel {
        id: optionsModel
        ListElement { textRole: "Configure ..."; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true; actionRole: "configure" }
        ListElement { textRole: "Download from remote"; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true; actionRole: "downloadRemote" }
        ListElement { textRole: "About ..."; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true; actionRole: "about" }
        ListElement { textRole: "Quit"; checkableRole: false; iconRole: ""; checkedRole: false; dismissRole: true; actionRole: "quit" }
    }

    ChoicePopup {
        anchors.fill: parent
        model: optionsModel
        onChoiceClicked: {
            _controller.optionsContextMenuVisible = false
            if (action === "configure") {
                _controller.currentPage = Controller.ConfigurePage
            } else if (action === "about") {
                _controller.currentPage = Controller.AboutPage
            } else if (action === "quit") {
                Qt.quit()
            } else if (action === "downloadRemote") {
                _controller.downloadFromRemote()
            }
        }

        onDismissPopup: {
            _controller.optionsContextMenuVisible = false
        }

        Component.onCompleted: {
            if (!_settings.hasRemoteUrl) {
                optionsModel.remove(1); // Remove "Download from remote"
            }
        }
    }
}
