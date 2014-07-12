import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import Controller 1.0


Rectangle {
    id: root
    radius: 4
    color: "transparent"
    width: 400 * _controller.dpiFactor
    height: 50 * _controller.dpiFactor + (_controller.expanded ? _style.pageHeight + 10* _controller.dpiFactor : 0)

    function mouseOver()
    {
        return globalMouseArea.containsMouse || addIcon.containsMouse || stopIcon.containsMouse || pauseIcon.containsMouse
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

    Rectangle {
        id: main
        anchors.fill: parent
        color: _style.backgroundColor
        radius: _style.borderRadius
        border.width: _style.borderWidth
        border.color: _style.borderColor

        Text {
            id: titleText
            elide: _controller.paused ? Text.ElideLeft : Text.ElideRight
            color: _style.fontColor
            font.pointSize: _style.fontSize
            font.bold: true
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium
            anchors.right: parent.right
            anchors.rightMargin: (16*2 + 15) * _controller.dpiFactor // ( two icons, 3 margins)
            anchors.top: parent.top
            text: _controller.stopped ? qsTr("You're slacking") : _controller.currentTask.summary
            visible: !remainingText.visible
        }

        Text {
            text: qsTr("Click here to start focusing")
            font.bold: false
            font.pointSize: _style.clickHereFontSize
            color: _style.clickHereFontColor
            visible: _controller.stopped && !_controller.expanded
            anchors.left: titleText.left
            anchors.leftMargin: _style.marginSmall
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.marginSmall
        }

        Text {
            id: remainingText
            color: _style.fontColor
            visible: (mouseOver() || _controller.firstSecondsAfterAdding) && _controller.remainingMinutes > 0 && !_controller.stopped && !_controller.expanded
            font.pointSize: _style.remainingFontSize
            font.bold: true
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium
            anchors.top: parent.top
            text: _controller.paused ? qsTr("Paused (%1m)").arg(_controller.remainingMinutes) : (_controller.remainingMinutes + "m " + qsTr("remaining ..."))
        }

        Row {
            z: 2
            anchors.right: parent.right
            anchors.bottomMargin: 5
            anchors.bottom: titleText.bottom
            anchors.rightMargin: progressBar.anchors.rightMargin + 2
            spacing: _style.buttonsSpacing

            ClickableImage {
                id: pauseIcon
                visible: !_controller.stopped && (_controller.expanded || mouseOver() || _controller.paused)
                source: _controller.paused ? "qrc:/img/play.png" : "qrc:/img/pause.png"
                onClicked: {
                    _controller.pausePomodoro()
                }
            }

            ClickableImage {
                id: stopIcon
                visible: !_controller.stopped && (_controller.expanded || mouseOver() || _controller.paused)
                source: "qrc:/img/stop.png"
                onClicked: {
                    _controller.stopPomodoro(true)
                }

                onPressAndHold: {
                    _controller.stopPomodoro(false)
                }
            }

            ClickableImage {
                id: addIcon
                visible: _controller.expanded || !_controller.running || mouseOver()
                source: "qrc:/img/add.png"
                onClicked: {
                    _controller.addTask("New Task", /**open editor=*/true)
                }
            }
        }

        TheQueuePage {
            z: 2
            id: theQueuePage
            anchors.bottom:  progressBar.visible ? progressBar.top : parent.bottom
            anchors.bottomMargin: progressBar.visible ? _style.marginSmall : _style.marginMedium
        }

        ConfigurePage {
            z: 2
            id: configurePage
            anchors.bottom:  progressBar.visible ? progressBar.top : parent.bottom
            anchors.bottomMargin: progressBar.visible ? _style.marginSmall : _style.marginMedium
        }

        AboutPage {
            z: 2
            id: aboutPage
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
            visible: !_controller.stopped

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

        MouseArea {
            z: _controller.expanded ? 0 : 1
            id: globalMouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: {
                if (mouse.button === Qt.LeftButton) {
                    _controller.expanded = !_controller.expanded
                    mouse.accepted = false
                } else if (mouse.button === Qt.RightButton) {
                    if (_controller.taskBeingEdited === null && !_controller.popupVisible) {
                        contextMenu.popup()
                    }
                }
            }
        }
    }

    Menu {
        id: contextMenu
        title: qsTr("Edit")
        enabled: !_controller.popupVisible

        MenuItem {
            enabled: !_controller.stopped
            text: _controller.running ? qsTr("Pause") : qsTr("Resume")
            onTriggered: {
                _controller.pausePomodoro()
            }
        }

        MenuItem {
            enabled: !_controller.stopped
            text: qsTr("Stop")
            onTriggered: {
                _controller.stopPomodoro(true)
            }
        }

        MenuItem {
            visible: !_controller.stopped
            text: qsTr("Delete")
            onTriggered: {
                _controller.stopPomodoro(false)
            }
        }

        MenuSeparator { }

        MenuItem {
            text: qsTr("Configure")
            onTriggered: {
                _controller.currentPage = Controller.ConfigurePage
                _controller.expanded = true
            }
        }

        MenuItem {
            text: qsTr("About")
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
