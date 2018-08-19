import QtQuick 2.0
import Controller 1.0

Item {
    anchors.fill: parent
    visible: _controller.optionsContextMenuVisible

    property list<QtObject> optionsModel: [
        MobileOptionsContextMenuItem {
            textRole: "Configure ..."
            actionRole: "configure"
        },
        MobileOptionsContextMenuItem {
            textRole: "Download from remote"
            actionRole: "downloadRemote"
            showBusyIndicatorRole: _controller.fileDownloader.downloadInProgress
            enabledRole: !showBusyIndicatorRole && _settings.hasRemoteUrl
            dismissRole: false
        },
        MobileOptionsContextMenuItem {
            textRole: "About..."
            actionRole: "about"
        },
        MobileOptionsContextMenuItem {
            textRole: "Quit"
            actionRole: "quit"
        }
    ]

    ChoicePopup {
        objectName: "ChoicePopup"
        anchors.fill: parent
        model: optionsModel
        onChoiceClicked: {
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
            if (!_settings.hasRemoteUrl || !_controller.isReadOnly) {
                optionsModel.remove(1); // Remove "Download from remote"
            }
        }
    }
}
