import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.1

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    ListModel {
        id: windowPositionModel
        ListElement { text: "WM defined" }
        ListElement { text: "Last Position" }
        ListElement { text: "Top Center" }
        ListElement { text: "Top Left" }
        ListElement { text: "Top Right" }
    }

    ListModel {
        id: windowGeometryTypesModel
        ListElement { text: "Standard" }
        ListElement { text: "Thin" }
        //ListElement { text: "Small Square" }
        // ListElement { text: "Custom" }
    }

    Grid {
        id: grid1
        columns: 2

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: _style.marginBig
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin: 40 * _controller.dpiFactor
        columnSpacing: _style.marginBig
        rowSpacing: 10 * _controller.dpiFactor

        Text {
            text: qsTr("Pomodoro duration")
            font.pixelSize: 12 * _controller.dpiFactor
            verticalAlignment: Text.AlignVCenter;
            height: spinBox.height
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
            font.pixelSize: 12 * _controller.dpiFactor
        }

        FlowCheckBox {
            id: disablePomodoroCheckBox
            checked: _controller.pomodoroFunctionalityDisabled
            Binding {
                target: _controller
                property: "pomodoroFunctionalityDisabled"
                value: disablePomodoroCheckBox.checked
            }
        }

        Text {
            text: qsTr("Use systray")
            font.pixelSize: 12 * _controller.dpiFactor
            visible: !_controller.isMobile
        }

        FlowCheckBox {
            id: systrayCheckBox
            visible: !_controller.isMobile
            checked: _controller.useSystray
            Binding {
                target: _controller
                property: "useSystray"
                value: systrayCheckBox.checked
            }
        }

        Text {
            text: qsTr("Hide empty tags in Later screen")
            font.pixelSize: 12 * _controller.dpiFactor
            height: geometryTypesCombo.height
        }

        FlowCheckBox {
            id: hideEmptyTagsCheck
            checked: _controller.hideEmptyTags
            Binding {
                target: _controller
                property: "hideEmptyTags"
                value: hideEmptyTagsCheck.checked
            }
        }

        Text {
            visible: _controller.isAndroid
            text: qsTr("Keep screen on during pomodoro")
            font.pixelSize: 12 * _controller.dpiFactor
        }

        FlowCheckBox {
            id: keepScreenOn
            visible: _controller.isAndroid
            checked: _controller.pomodoroFunctionalityDisabled
            Binding {
                target: _controller
                property: "keepScreenOnDuringPomodoro"
                value: keepScreenOn.checked
            }
        }

        Text {
            visible: !_controller.isMobile
            text: qsTr("Initial window location")
            font.pixelSize: 12 * _controller.dpiFactor
            height: windowPositionsCombo.height
            verticalAlignment: Text.AlignVCenter;
        }

        ComboBox {
            id: windowPositionsCombo
            visible: !_controller.isMobile
            model: windowPositionModel
            currentIndex: _window.initialPosition
            width: 150 * _controller.dpiFactor
            onCurrentIndexChanged: {
                _window.initialPosition = currentIndex
            }
        }

        Text {
            visible: !_controller.isMobile
            text: qsTr("Window geometry")
            font.pixelSize: 12 * _controller.dpiFactor
            height: geometryTypesCombo.height
            verticalAlignment: Text.AlignVCenter;
        }

        ComboBox {
            id: geometryTypesCombo
            visible: !_controller.isMobile
            model: windowGeometryTypesModel
            currentIndex: _window.geometryType
            width: 150 * _controller.dpiFactor
            onCurrentIndexChanged: {
                _window.geometryType = currentIndex
            }
        }
    }
}
