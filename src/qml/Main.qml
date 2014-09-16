import QtQuick 2.2
import Controller 1.0
import com.kdab.flowpomodoro 1.0

Rectangle {
    id: root

    property string titleText: _controller.currentTitleText
    property alias topBarItem: topBar.children
    signal requestGlobalContextMenu()

    radius: 4
    color: "transparent"
    width: 400 * _controller.dpiFactor
    height: _style.contractedHeight + (_controller.expanded ? _style.pageHeight : 0)

    Connections {
        target: _controller
        onExpandedChanged: {
            if (!_controller.expanded) {
                root.forceActiveFocus()
            }
        }
    }

    Component {
        id: questionPopupComponent
        QuestionPopup {
            anchors.fill: parent
        }
    }

    Loader {
        // Delayed loading for startup performance on mobile
        sourceComponent: _controller.questionPopupRequested ? questionPopupComponent : null
        anchors.fill: parent
        z: main.z + 1
    }

    Rectangle {
        id: main
        anchors.fill: parent
        color: _style.backgroundColor
        radius: _style.borderRadius
        border.width: _style.borderWidth
        border.color: _style.borderColor
        MouseArea {
            id: globalMouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: {
                if (mouse.button === Qt.LeftButton) {
                    _controller.expanded = !_controller.expanded
                    mouse.accepted = false
                } else if (mouse.button === Qt.RightButton) {
                    if (_controller.editMode === Controller.EditModeNone) {
                        _controller.requestContextMenu(null) // reset task
                        root.requestGlobalContextMenu()
                    }
                }
            }

            Item {
                id: topBar
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: _style.contractedHeight
                z: 4
            }

            MainPage {
                z: 2
                id: mainPage
                anchors.top: topBar.bottom
                anchors.bottom: parent.bottom
                anchors.bottomMargin: _style.marginMedium
            }

            Component {
                id: configurePageComponent
                ConfigurePage {
                    anchors.fill: parent
                }
            }

            Component {
                id: aboutPageComponent
                AboutPage {
                    anchors.fill: parent
                }
            }

            Loader {
                // Loader for startup performance optimization on mobile
                anchors.top: topBar.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottomMargin: _style.marginMedium
                sourceComponent: _controller.configurePageRequested ? configurePageComponent : null
                z: 2
            }

            Loader {
                z: 2
                id: aboutPage
                // Loader for startup performance optimization on mobile
                anchors.top: topBar.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: _style.marginMedium
                sourceComponent: _controller.aboutPageRequested ? aboutPageComponent : null
            }
        }

        FontAwesomeIcon {
            id: spinnerIcon
            anchors.left: parent.left
            anchors.verticalCenter: undefined
            height: contentHeight
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.marginMedium + 2 * _controller.dpiFactor
            anchors.leftMargin: _style.pageMargin + 2 * _controller.dpiFactor
            text: "\uf110"
            z: 3
            color: "black"
            visible: _storage.webDAVSyncSupported && _webdavSync.syncInProgress
            NumberAnimation on rotation {
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 1200
                running: spinnerIcon.visible
            }
        }
    }

    Component {
        id: taskContextMenuComponent
        TaskContextMenu {
            id: taskContextMenu
            enabled: !_controller.popupVisible
            visible: _controller.rightClickedTask !== null
        }
    }

    Loader {
        anchors.fill: parent
        sourceComponent: _controller.taskContextMenuRequested ? taskContextMenuComponent
                                                              : null
    }
}
