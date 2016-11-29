import QtQuick 2.2
import Controller 1.0
import com.kdab.flowpomodoro 1.0

Rectangle {
    id: root

    property string titleText: _controller.currentTitleText ? _controller.currentTitleText : _style.slackingText
    property alias topBarItem: topBar.children

    radius: _controller.isMobile ? 0 : 4
    color: "transparent"
    width: _controller.expanded ? _style.expandedWidth : _window.contractedWidth
    height: _controller.expanded ? _style.expandedHeight : _window.contractedHeight

    Connections {
        target: _controller
        onExpandedChanged: {
            if (!_controller.expanded) {
                root.forceActiveFocus()
            }
        }
    }

    Loader {
        // Delayed loading for startup performance on mobile
        sourceComponent: _loadManager.questionPopupRequested ? Qt.createComponent("QuestionPopup.qml") : null
        anchors.fill: parent
        z: main.z + 1
        objectName: "loader1"
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
            acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
            onClicked: {
                if (mouse.button === Qt.LeftButton) {
                    _controller.expanded = !_controller.expanded
                    mouse.accepted = false
                } else if (mouse.button === Qt.RightButton) {
                    if (_controller.editMode === Controller.EditModeNone) {
                        _controller.requestContextMenu(null) // reset task
                        _window.showWidgetContextMenu(Qt.point(mouse.x, mouse.y))
                    }
                } else if (mouse.button === Qt.MiddleButton) {
                    if (!_controller.expanded) {
                        _controller.expanded = true;
                        _controller.addTask(qsTr("New Task"), true);
                    }
                }
            }

            Item {
                id: topBar
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }

                height: _style.menuBarHeight
                z: 4
            }

            MainPage {
                z: 2
                id: mainPage
                anchors {
                    top: topBar.bottom
                    bottom: parent.bottom
                    bottomMargin: _style.marginMedium
                }
            }

            Loader {
                // Loader for startup performance optimization on mobile                
                anchors {
                    top: topBar.bottom
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    bottomMargin: _style.marginMedium
                }

                sourceComponent: _loadManager.configurePageRequested ? Qt.createComponent("ConfigurePage.qml") : null
                z: 2
                objectName: "loader2"
            }

            Loader {
                z: 2
                id: aboutPage
                // Loader for startup performance optimization on mobile

                anchors {
                    top: topBar.bottom
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    bottomMargin: _style.marginMedium
                }

                sourceComponent: _loadManager.aboutPageRequested ? Qt.createComponent("AboutPage.qml") : null
                objectName: "loader3"
            }
        }

        FontAwesomeIcon {
            id: spinnerIcon
            height: contentHeight
            anchors {
                left: parent.left
                verticalCenter: undefined
                bottom: parent.bottom
                bottomMargin: _style.marginMedium + 2 * _controller.dpiFactor
                leftMargin: _style.pageMargin + 2 * _controller.dpiFactor
            }

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

    NewTagDialog { // TODO: Delay load
        anchors.fill: parent
        z: 2
    }

    TaskEditor {
        visible: _controller.editMode == Controller.EditModeEditor
        anchors.fill: parent
        task: _controller.taskBeingEdited
        z: 2
    }

    Loader {
        anchors.fill: parent
        enabled: !_controller.popupVisible
        visible: _controller.rightClickedTask !== null
        sourceComponent: _loadManager.taskContextMenuRequested ? Qt.createComponent("TaskContextMenu.qml") : null
        objectName: "loader4"
    }
}
