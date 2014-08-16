import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.0

import Controller 1.0
import com.kdab.flowpomodoro 1.0


Rectangle {
    id: root

    property string titleText: _controller.currentTask.stopped ? qsTr("You're slacking") : _controller.currentTask.summary

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
                        contextMenu.popup()
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
                    contextMenu.popup()
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
                    anchors.right: parent.right
                    anchors.rightMargin: (16*2 + 15) * _controller.dpiFactor // ( two icons, 3 margins)
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
                    anchors.rightMargin: _style.marginMedium
                    spacing: _style.buttonsSpacing

                    FlowCircularProgressIndicator {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: 5 * _controller.dpiFactor
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
        }
    }

    Connections {
        target: _controller
        onRightClickedTaskChanged: {
            if (_controller.rightClickedTask !== null)
                contextMenu.popup()
        }
    }

    Menu {
        id: contextMenu
        enabled: !_controller.popupVisible

        MenuItem {
            visible: _controller.rightClickedTask === null && !_controller.currentTask.stopped
            text: _controller.currentTask.running ? qsTr("Pause") : qsTr("Resume")
            onTriggered: {
                _controller.pausePomodoro()
            }
        }

        MenuItem {
            visible: _controller.rightClickedTask === null && !_controller.currentTask.stopped
            text: qsTr("Stop")
            onTriggered: {
                _controller.stopPomodoro(true)
            }
        }

        MenuItem {
            visible: _controller.rightClickedTask === null && !_controller.currentTask.stopped
            text: qsTr("Delete")
            onTriggered: {
                _controller.stopPomodoro(false)
            }
        }

        MenuItem {
            visible: _controller.rightClickedTask !== null
            text: qsTr("Edit...")
            onTriggered: {
                _controller.editTask(_controller.rightClickedTask, Controller.EditModeEditor)
            }
        }

        TagsMenu {
            task: _controller.rightClickedTask
        }

        MenuSeparator { }

        MenuItem {
            text: qsTr("Configure...")
            visible: _controller.currentPage != Controller.ConfigurePage
            onTriggered: {
                _controller.currentPage = _controller.currentPage == Controller.ConfigurePage ? Controller.MainPage
                                                                                              : Controller.ConfigurePage
            }
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
                _controller.stopPomodoro(true)
                Qt.quit()
            }
        }
    }
}
