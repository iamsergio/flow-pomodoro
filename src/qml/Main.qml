import QtQuick 2.2
import Controller 1.0
import com.kdab.flowpomodoro 1.0

Rectangle {
    id: root

    property string titleText: _controller.currentTitleText

    radius: 4
    color: "transparent"
    width: 400 * _controller.dpiFactor
    height: _style.contractedHeight + (_controller.expanded ? _style.pageHeight : 0)

    function toggleConfigure()
    {
        _controller.currentPage = _controller.currentPage == Controller.ConfigurePage ? Controller.MainPage
                                                                                      : Controller.ConfigurePage
    }

    FontLoader {
        source: "/fontawesome-webfont.ttf"
    }

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
                        globalContextMenu.item.popup()
                    }
                }
            }

            MobileMenuBar {
                titleText: root.titleText
                visible: _controller.isMobile
                anchors.leftMargin: _style.menuBarMargin
                anchors.rightMargin: _style.menuBarMargin
                onButtonClicked: {
                    _controller.requestContextMenu(null) // reset task
                    _controller.optionsContextMenuVisible = true
                }
            }

            Item {
                id: header
                visible: !_controller.isMobile
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: _style.contractedHeight

                FlowSwitch {
                    id: switchItem
                    visible: _controller.expanded && _controller.currentPage === Controller.MainPage
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: _style.marginMedium
                    Binding {
                        target: _controller
                        property: "queueType"
                        value: switchItem.checked ? Controller.QueueTypeArchive
                                                  : Controller.QueueTypeToday
                    }
                }

                Text {
                    id: titleText
                    elide: _controller.currentTask.paused ? Text.ElideLeft : Text.ElideRight
                    color: _style.fontColor
                    font.pixelSize: _controller.currentTask.stopped ? _style.fontSize : _style.currentTaskFontSize
                    font.bold: true
                    anchors.left: switchItem.visible ? switchItem.right : parent.left
                    anchors.leftMargin: _style.marginMedium
                    anchors.right: buttonRow.left
                    anchors.rightMargin: 2 * _controller.dpiFactor
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: switchItem.visible ? 0 : -5 * _controller.dpiFactor
                    horizontalAlignment: switchItem.visible ? Text.AlignHCenter : Text.AlignLeft
                    text: root.titleText
                }

                Text {
                    id: focusText
                    text: qsTr("Click here to start focusing")
                    font.bold: false
                    font.pixelSize: _style.clickHereFontSize
                    color: _style.clickHereFontColor
                    visible: _controller.currentTask.stopped && !_controller.expanded
                    anchors.left: titleText.left
                    anchors.leftMargin: _style.marginSmall
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: _style.marginSmall
                }

                Row {
                    z: 2
                    id: buttonRow
                    anchors.right: parent.right
                    anchors.topMargin: 4 * _controller.dpiFactor
                    anchors.bottomMargin: 4 * _controller.dpiFactor
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: circularIndicator.mousePressed ? _style.marginMedium - 2 * _controller.dpiFactor
                                                                        :  _style.marginMedium
                    spacing: _style.buttonsSpacing

                    FontAwesomeIcon {
                        id: addIcon
                        text: "\uf055" // "\uf0fe"
                        size: 30
                        anchors.verticalCenter: parent.verticalCenter
                        toolTip: qsTr("Add new task")
                        visible: _controller.expanded
                        color: "white"
                        onClicked: {
                            _controller.addTask("New Task", /**open editor=*/true) // TODO: Pass edit mode instead
                        }
                    }

                    FlowCircularProgressIndicator {
                        id: circularIndicator
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: (mousePressed ? 3 : 5) * _controller.dpiFactor
                    }

                    FontAwesomeIcon {
                        id: configureIcon
                        anchors.verticalCenter: buttonRow.verticalCenter
                        toolTip: qsTr("Configure")
                        visible: _controller.expanded
                        text: "\uf013"
                        size: 30
                        onClicked: {
                            _controller.currentPage = _controller.currentPage == Controller.ConfigurePage ? Controller.MainPage
                                                                                                          : Controller.ConfigurePage
                        }
                    }
                }
            }

            MainPage {
                z: 2
                id: mainPage
                anchors.top: header.bottom
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
                anchors.top: header.bottom
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
                anchors.top: header.bottom
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
        id: globalContextMenuComponent
        GlobalContextMenu {
        }
    }

    Loader {
        id: globalContextMenu
        sourceComponent: _controller.isMobile ? null : globalContextMenuComponent
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
                        root.toggleConfigure()
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
        z: main.z + 1
    }
}
