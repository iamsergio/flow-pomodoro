import QtQuick 2.0

import Controller 1.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Page {
    id: root
    page: Controller.ConfigurePage

    Item {
        z: 1
        anchors.fill: parent

        TitleText {
            id: titleText
            text: qsTr("Configuration")
            anchors.top: parent.top
            anchors.topMargin: 5
            height: 50
        }

        GridLayout {
            id: grid1
            columns: 2

            anchors.top: titleText.bottom
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.rightMargin: 40
            columnSpacing: 20

            RegularText {
                text: qsTr("Pomodoro duration")
            }

            SpinBox {
                id: spinBox
                minimumValue: 0
                maximumValue: 59
                value: _controller.defaultPomodoroDuration
                onValueChanged: {
                    // The binding gets broken when user edits, not sure how to fix it.. so hack it:
                    _controller.defaultPomodoroDuration = value
                }
            }
        }

        TitleText {
            id: titleText2
            text: qsTr("Plugins")
            anchors.top: grid1.bottom
            anchors.topMargin: 40
        }

        SmallText {
            id: smallText1
            anchors.topMargin: 5
            anchors.top: titleText2.bottom
            text: qsTr("Plugins disable certain distractions when a task is running. Distractions are re-enabled when the task stops.") +
                  "\n" + qsTr("The following plugins were found:")
        }


        ListView {
            anchors.top: smallText1.bottom
            anchors.leftMargin: 10
            anchors.rightMargin: 40
            anchors.topMargin: 25
            anchors.left: parent.left
            anchors.right: parent.right
            model: _pluginModel
            height: 60
            delegate: Row {

                id: row
                height: 30
                spacing: 30

                RegularText {
                    height: row.height
                    width: 150
                    text: textRole
                }

                CheckBox {
                    anchors.verticalCenterOffset: -2
                    anchors.verticalCenter: parent.verticalCenter
                    height: row.height
                    checked: enabledRole
                    onCheckedChanged: {
                        _pluginModel.setPluginEnabled(checked, index)
                    }
                }
            }
        }

        Button {
            width : 100
            height: 50
            text: qsTr("OK")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5

            onClicked: {
                _controller.currentPage = Controller.TheQueuePage
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            // Don't collapse
            mouse.accepted = false
        }
    }
}
