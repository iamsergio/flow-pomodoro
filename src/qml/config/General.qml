import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    GridLayout {
        id: grid1
        columns: 2

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: _style.marginBig
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin: 40 * _controller.dpiFactor
        columnSpacing: _style.marginBig

        Text {
            text: qsTr("Pomodoro duration")
        }

        SpinBox {
            id: spinBox
            enabled: !_controller.pomodoroFunctionalityDisabled
            minimumValue: 0
            maximumValue: 59
            value: _controller.defaultPomodoroDuration
            onValueChanged: {
                // The binding gets broken when user edits, not sure how to fix it.. so hack it:
                _controller.defaultPomodoroDuration = value
            }
            onVisibleChanged: {
                if (!visible && _controller.isMobile) {
                    Qt.inputMethod.hide(); // hide keyboard
                }
            }

            onEditingFinished: {
                if (_controller.isMobile)
                    Qt.inputMethod.hide(); // hide keyboard
            }

            style: _controller.isMobile ? mobileSpinBoxStyleComponent
                                        : desktopSpinBoxStyleComponent

        }

        Component {
            id: desktopSpinBoxStyleComponent
            SpinBoxStyle {}
        }

        Component {
            id: mobileSpinBoxStyleComponent
            SpinBoxStyle {
                // Set it explicitely because it's gray on Blackberry
                textColor: "black"
                incrementControl: Item {}
                decrementControl: Item {}
            }
        }

        Text {
            text: qsTr("Disable pomodoro functionality")
        }

        CheckBox {
            id: disablePomodoroCheckBox
            checked: _controller.pomodoroFunctionalityDisabled
            Binding {
                target: _controller
                property: "pomodoroFunctionalityDisabled"
                value: disablePomodoroCheckBox.checked
            }
        }
    }
}
