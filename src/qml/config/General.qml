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
            enabled: !_settings.pomodoroFunctionalityDisabled
            minimumValue: 0
            maximumValue: 59
            height: _style.checkBoxIconSize
            width: 55 * _controller.dpiFactor
            value: _settings.defaultPomodoroDuration
            onValueChanged: {
                // The binding gets broken when user edits, not sure how to fix it.. so hack it:
                _settings.defaultPomodoroDuration = value
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
            checked: _settings.pomodoroFunctionalityDisabled
            Binding {
                target: _settings
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
            checked: _settings.useSystray
            Binding {
                target: _settings
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
            checked: _settings.hideEmptyTags
            Binding {
                target: _settings
                property: "hideEmptyTags"
                value: hideEmptyTagsCheck.checked
            }
        }

        Text {
            text: qsTr("Show tag menu after new task")
            font.pixelSize: 12 * _controller.dpiFactor
            verticalAlignment: Text.AlignVCenter
            height: Math.max(showContextMenuAfterAddCheckbox.height, contentHeight)
        }

        FlowCheckBox {
            id: showContextMenuAfterAddCheckbox
            objectName: "showContextMenuAfterAddCheckbox"
            width: _style.checkBoxIconSize
            checked: _settings.showContextMenuAfterAdd
            Binding {
                target: _settings
                property: "showContextMenuAfterAdd"
                value: showContextMenuAfterAddCheckbox.checked
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
            checked: _settings.keepScreenOnDuringPomodoro
            Binding {
                target: _settings
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
            currentIndex: _settings.initialPosition
            width: 150 * _controller.dpiFactor
            enabled: _settings.stickyWindow
            onCurrentIndexChanged: {
                _settings.initialPosition = currentIndex
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
            enabled: _settings.stickyWindow
            model: windowGeometryTypesModel
            currentIndex: _settings.geometryType
            width: 150 * _controller.dpiFactor
            onCurrentIndexChanged: {
                _settings.geometryType = currentIndex
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
            checked: _settings.stickyWindow
            width: _style.checkBoxIconSize
            Binding {
                target: _settings
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
            checked: _settings.showTaskAge
            width: _style.checkBoxIconSize
            Binding {
                target: _settings
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
            checked: _settings.showAllTasksView
            width: _style.checkBoxIconSize
            Binding {
                target: _settings
                property: "showAllTasksView"
                value: showAllTasksView.checked
            }
        }

        Text {
            visible: showSupportsDueDate.visible
            text: qsTr("Due date support")
            font.pixelSize: 12 * _controller.dpiFactor
        }

        FlowCheckBox {
            id: showSupportsDueDate
            checked: _settings.supportsDueDate
            Binding {
                target: _settings
                property: "supportsDueDate"
                value: showSupportsDueDate.checked
            }
        }
    }
}
