import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.0

import Controller 1.0


Rectangle {
    id: root
    radius: 4
    color: "transparent"
    width: 400 * _controller.dpiFactor
    height: _style.contractedHeight + (_controller.expanded ? _style.pageHeight + 10* _controller.dpiFactor : 0)

    function mouseOver()
    {
        return globalMouseArea.containsMouse || stopIcon.containsMouse || pauseIcon.containsMouse
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
                        contextMenu.popup()
                    }
                }
            }

            MobileMenuBar {
                visible: _controller.isMobile
                anchors.leftMargin: 1 * _controller.dpiFactor
                anchors.rightMargin: 1 * _controller.dpiFactor
                onButtonClicked: {
                    _controller.currentPage = _controller.currentPage == Controller.ConfigurePage ? Controller.MainPage
                                                                                                  : Controller.ConfigurePage
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
                    anchors.top: parent.top
                    text: _controller.currentTask.stopped ? qsTr("You're slacking") : _controller.currentTask.summary
                    visible: !remainingText.visible
                }

                Text {
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

                Text {
                    id: remainingText
                    color: _style.fontColor
                    visible: (mouseOver() || _controller.firstSecondsAfterAdding) && _controller.remainingMinutes > 0 && !_controller.currentTask.stopped && !_controller.expanded
                    font.pixelSize: _style.remainingFontSize
                    font.bold: true
                    anchors.left: parent.left
                    anchors.leftMargin: _style.marginMedium
                    anchors.top: parent.top
                    text: _controller.currentTask.paused ? qsTr("Paused (%1m)").arg(_controller.remainingMinutes) : (_controller.remainingMinutes + "m " + qsTr("remaining ..."))
                }

                Row {
                    z: 2
                    id: buttonRow
                    anchors.right: parent.right
                    anchors.bottomMargin: 5
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: progressBar.anchors.rightMargin + 2
                    spacing: _style.buttonsSpacing
                    layoutDirection: "RightToLeft" // Right to left so configure always sticks to right margin, otherwise will stick to left margin when row grows. Because row doesn't shrink back when pause/stop become invisible.

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

                    ClickableImage {
                        id: stopIcon
                        anchors.verticalCenter: parent.verticalCenter
                        toolTip: qsTr("Stop current task")
                        visible: !_controller.currentTask.stopped && (_controller.expanded || mouseOver() || _controller.currentTask.paused)
                        source: "image://icons/stop.png"
                        onClicked: {
                            _controller.stopPomodoro(true)
                        }

                        onPressAndHold: {
                            _controller.stopPomodoro(false)
                        }
                    }

                    ClickableImage {
                        id: pauseIcon
                        toolTip: qsTr("Pause current task")
                        anchors.verticalCenter: parent.verticalCenter
                        visible: !_controller.currentTask.stopped && (_controller.expanded || mouseOver() || _controller.currentTask.paused)
                        source: _controller.currentTask.paused ? "image://icons/play.png" : "image://icons/pause.png"
                        onClicked: {
                            _controller.pausePomodoro()
                        }
                    }
                }
            }

            MainPage {
                z: 2
                id: mainPage
                anchors.top: header.bottom
                anchors.bottom:  progressBar.visible ? progressBar.top : parent.bottom
                anchors.bottomMargin: progressBar.visible ? _style.marginSmall : _style.marginMedium
            }

            ConfigurePage {
                z: 2
                id: configurePage
                anchors.top: header.bottom
                anchors.bottom:  progressBar.visible ? progressBar.top : parent.bottom
                anchors.bottomMargin: progressBar.visible ? _style.marginSmall : _style.marginMedium
            }

            AboutPage {
                z: 2
                id: aboutPage
                anchors.top: header.bottom
                anchors.bottom:  progressBar.visible ? progressBar.top : parent.bottom
                anchors.bottomMargin: progressBar.visible ? _style.marginSmall : _style.marginMedium
            }

            ProgressBar
            {
                id: progressBar
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: _style.marginSmall
                anchors.leftMargin: _style.marginMedium
                anchors.rightMargin: _style.marginMedium

                height: _style.progressBarHeight
                visible: !_controller.currentTask.stopped

                minimumValue: 0
                maximumValue: _controller.currentTaskDuration
                value: _controller.currentTaskDuration - _controller.remainingMinutes
                style: ProgressBarStyle {
                    background: Rectangle {
                        radius: _style.progressBarBorderRadius
                        color: _style.progressBarBgColor
                        border.color: _style.borderColor
                        border.width: _style.borderWidth
                    }
                }
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
            enabled: !_controller.currentTask.stopped
            visible: _controller.rightClickedTask === null
            text: _controller.currentTask.running ? qsTr("Pause") : qsTr("Resume")
            onTriggered: {
                _controller.pausePomodoro()
            }
        }

        MenuItem {
            enabled: !_controller.currentTask.stopped
            visible: _controller.rightClickedTask === null
            text: qsTr("Stop")
            onTriggered: {
                _controller.stopPomodoro(true)
            }
        }

        MenuItem {
            enabled: !_controller.currentTask.stopped
            visible: _controller.rightClickedTask === null
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
            text: qsTr("About...")
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
