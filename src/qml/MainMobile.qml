import QtQuick 2.2
import Controller 1.0
import com.kdab.flowpomodoro 1.0

Main {
    id: root
    topBarItem:
        MobileMenuBar {
        titleText: root.titleText
        anchors.leftMargin: _style.menuBarMargin
        anchors.rightMargin: _style.menuBarMargin
        onButtonClicked: {
            _controller.requestContextMenu(null) // reset task
            _controller.optionsContextMenuVisible = true
        }
    }

    Component {
        id: configureContextMenu
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
    }

    Loader {
        sourceComponent: (_controller.isMobile && _controller.configurePopupRequested) ? configureContextMenu
                                                                                       : null
        anchors.fill: parent
    }
}
