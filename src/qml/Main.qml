import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.0

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

    Connections {
        target: _controller
        onExpandedChanged: {
            if (!_controller.expanded) {
                root.forceActiveFocus()
            }
        }
    }

    QuestionPopup {
        id: questionPopup
        anchors.fill: parent
        z: main.z + 1
    }

    TaskEditorDialog {
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
                        globalContextMenu.popup()
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

                Text {
                    id: titleText
                    elide: _controller.currentTask.paused ? Text.ElideLeft : Text.ElideRight
                    color: _style.fontColor
                    font.pixelSize: _controller.currentTask.stopped ? _style.fontSize : _style.currentTaskFontSize
                    font.bold: true
                    anchors.left: parent.left
                    anchors.leftMargin: _style.marginMedium
                    anchors.right: buttonRow.left
                    anchors.rightMargin: 2 * _controller.dpiFactor
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -5 * _controller.dpiFactor
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

                    FlowCircularProgressIndicator {
                        id: circularIndicator
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: (mousePressed ? 3 : 5) * _controller.dpiFactor
                    }

                    ClickableImage {
                        id: configureIcon
                        anchors.verticalCenter: buttonRow.verticalCenter
                        toolTip: qsTr("Configure")
                        visible: _controller.expanded
                        source: "image://icons/configure.png"
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

            ConfigurePage {
                z: 2
                id: configurePage
                anchors.top: header.bottom
                anchors.bottom: parent.bottom
                anchors.bottomMargin: _style.marginMedium
            }

            AboutPage {
                z: 2
                id: aboutPage
                anchors.top: header.bottom
                anchors.bottom: parent.bottom
                anchors.bottomMargin: _style.marginMedium
            }

            BusyIndicator {
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                width: 20 * _controller.dpiFactor
                anchors.bottomMargin: _style.marginMedium + 2 * _controller.dpiFactor
                anchors.leftMargin: _style.pageMargin + 2 * _controller.dpiFactor
                height: width
                running: _storage.webDAVSyncSupported && _webdavSync.syncInProgress
                z: 3
            }
        }
    }

    Connections {
        target: _controller
        onRightClickedTaskChanged: {
            if (_controller.rightClickedTask !== null)
                taskContextMenu.popup()
        }
    }

    Menu {
        id: globalContextMenu

        MenuItem {
            visible: _controller.rightClickedTask === null && !_controller.currentTask.stopped
            text: _controller.currentTask.running ? qsTr("Pause") : qsTr("Resume")
            onTriggered: {
                _controller.pausePomodoro()
            }
        }

        MenuItem {
            id: stopMenuItem
            visible: _controller.rightClickedTask === null && !_controller.currentTask.stopped
            text: qsTr("Stop")
            onTriggered: {
                _controller.stopPomodoro(true)
            }
        }

        MenuSeparator {
            visible: stopMenuItem.visible
        }

        MenuItem {
            text: qsTr("About...")
            visible: _controller.currentPage != Controller.AboutPage
            onTriggered: {
                _controller.currentPage = Controller.AboutPage
                _controller.expanded = true
            }
        }
        MenuItem {
            text: qsTr("Quit")
            onTriggered: {
                Qt.quit()
            }
        }
    }

    TaskContextMenu {
        id: taskContextMenu
        enabled: !_controller.popupVisible
        task: _controller.rightClickedTask
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

            MobileChoicePopup {
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
            }
        }
    }

    Loader {
        sourceComponent: _controller.isMobile ? configureContextMenu : null
        anchors.fill: parent
        z: main.z + 1
    }
}
