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
            height: _style.checkBoxIconSize
            width: 55 * _controller.dpiFactor
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
                if (_controller.isMobile) {
                    focus = false
                    Qt.inputMethod.hide(); // hide keyboard
                }
            }

            style: _controller.isMobile ? (_controller.isAndroid ? dummySpinBox.style
                                                                : mobileSpinBoxStyleComponent)
                                        : desktopSpinBoxStyleComponent
        }

        SpinBox {
            id: dummySpinBox
            visible: false
        }

        Component {
            id: desktopSpinBoxStyleComponent
            SpinBoxStyle {
                // Weird text clipping on windows with native rendering
                renderType: Text.QtRendering
            }
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
            verticalAlignment: Text.AlignVCenter
            height: Math.max(disablePomodoroCheckBox.height, contentHeight)
        }

        FlowCheckBox {
            id: disablePomodoroCheckBox
            width: _style.checkBoxIconSize
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
            verticalAlignment: Text.AlignVCenter
            height: Math.max(systrayCheckBox.height, contentHeight)
        }

        FlowCheckBox {
            id: systrayCheckBox
            width: _style.checkBoxIconSize
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
            verticalAlignment: Text.AlignVCenter
            height: Math.max(hideEmptyTagsCheck.height, contentHeight)
        }

        FlowCheckBox {
            id: hideEmptyTagsCheck
            width: _style.checkBoxIconSize
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
            verticalAlignment: Text.AlignVCenter
            height: Math.max(keepScreenOn.height, contentHeight)
        }

        FlowCheckBox {
            id: keepScreenOn
            width: _style.checkBoxIconSize
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
            verticalAlignment: Text.AlignVCenter
            height: Math.max(windowPositionsCombo.height, contentHeight)
        }

        ComboBox {
            id: windowPositionsCombo
            visible: !_controller.isMobile
            model: windowPositionModel
            currentIndex: _window.initialPosition
            width: 150 * _controller.dpiFactor
            enabled: _controller.stickyWindow
            onCurrentIndexChanged: {
                _window.initialPosition = currentIndex
            }
        }

        Text {
            visible: !_controller.isMobile
            text: qsTr("Window geometry")
            font.pixelSize: 12 * _controller.dpiFactor
            verticalAlignment: Text.AlignVCenter
            height: Math.max(geometryTypesCombo.height, contentHeight)
        }

        ComboBox {
            id: geometryTypesCombo
            visible: !_controller.isMobile
            enabled: _controller.stickyWindow
            model: windowGeometryTypesModel
            currentIndex: _window.geometryType
            width: 150 * _controller.dpiFactor
            onCurrentIndexChanged: {
                _window.geometryType = currentIndex
            }
        }

        Text {
            visible: stickyWindowBox.visible
            text: qsTr("Sticky Window (requires restart)")
            font.pixelSize: 12 * _controller.dpiFactor
            verticalAlignment: Text.AlignVCenter
            height: Math.max(stickyWindowBox.height, contentHeight)
        }

        FlowCheckBox {
            id: stickyWindowBox
            visible: !_controller.isMobile && !_controller.isOSX
            checked: _controller.stickyWindow
            width: _style.checkBoxIconSize
            Binding {
                target: _controller
                property: "stickyWindow"
                value: stickyWindowBox.checked
            }
        }

        Text {
            visible: showTaskAgeBox.visible
            text: qsTr("Show age (in days) on each task")
            font.pixelSize: 12 * _controller.dpiFactor
            verticalAlignment: Text.AlignVCenter
            height: Math.max(showTaskAgeBox.height, contentHeight)
        }

        FlowCheckBox {
            id: showTaskAgeBox
            visible: _controller.expertMode
            checked: _controller.showTaskAge
            width: _style.checkBoxIconSize
            Binding {
                target: _controller
                property: "showTaskAge"
                value: showTaskAgeBox.checked
            }
        }

        Text {
            visible: showAllTasksView.visible
            text: qsTr("Show \"all tasks\" view")
            font.pixelSize: 12 * _controller.dpiFactor
            verticalAlignment: Text.AlignVCenter
            height: Math.max(showAllTasksView.height, contentHeight)
        }

        FlowCheckBox {
            id: showAllTasksView
            visible: _controller.expertMode
            checked: _controller.showAllTasksView
            width: _style.checkBoxIconSize
            Binding {
                target: _controller
                property: "showAllTasksView"
                value: showAllTasksView.checked
            }
        }
    }
}
