import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.2
import Controller 1.0

Overlay {
    id: root
    property QtObject task: null
    property string prettyDueDateString: task ? task.prettyDueDateString : ""
    objectName: "taskEditor"

    contentHeight: height - contentTopMargin - _style.pageMargin - _style.marginSmall
    centerPopup: false

    ListModel {
        id: priorityModel
        ListElement { label: "low"; priorityValue: 10; priorityColor: "green" }
        ListElement { label: "none"; priorityValue: 0; priorityColor: "black" }
        ListElement { label: "high"; priorityValue: 1; priorityColor: "red" }
    }

    contentItem:
    Item {
        anchors.fill: parent
        Column {
            id: column
            spacing: 10 * _controller.dpiFactor
            height: childrenRect.height
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10 * _controller.dpiFactor
            anchors.rightMargin: 10 * _controller.dpiFactor

            Item {
                height: 20 * _controller.dpiFactor
                width: parent.width
                Text {
                    id: summaryText
                    text: qsTr("Summary") + ":"
                    anchors.left: parent.left
                    font.pixelSize: 13 * _controller.dpiFactor
                }

                TextInput {
                    id: textInput
                    focus: true
                    font.pixelSize: 14 * _controller.dpiFactor
                    anchors.left: summaryText.right
                    anchors.leftMargin: 10 * _controller.dpiFactor
                    anchors.verticalCenter: parent.verticalCenter
                    width: 0.70 * parent.width
                    text: root.task ? root.task.summary : ""
                    onAccepted: {
                        root.accept()
                    }
                    onVisibleChanged: {
                        textInput.forceActiveFocus()
                    }
                }

                Rectangle {
                    height: 1 * _controller.dpiFactor
                    width: textInput.width
                    anchors.top: textInput.bottom
                    anchors.left: textInput.left
                    color: "black"
                }
            }
            Item {
                height: 20 * _controller.dpiFactor
                width: parent.width
                Text {
                    id: priorityText
                    text: qsTr("Priority") + ":"
                    anchors.left: parent.left
                    font.pixelSize: 13 * _controller.dpiFactor
                }


                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10 * _controller.dpiFactor
                    Repeater {
                        model: priorityModel
                        Item {
                            height: priText.height + 5 * _controller.dpiFactor
                            width: priText.width + 10 * _controller.dpiFactor
                            Text {
                                id: priText
                                text: label
                                color: "black"
                                font.pixelSize: 14 * _controller.dpiFactor
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            Rectangle {
                                height: 3 * _controller.dpiFactor
                                color: priorityColor
                                anchors.bottom: parent.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                visible: root.task && root.task.priority === priorityValue
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    root.task.priority = priorityValue
                                }
                            }
                        }
                    }
                }
            }

            Row {
                height: 20 * _controller.dpiFactor
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 5 * _controller.dpiFactor
                FontAwesomeIcon {
                    id: dueDateExpanded
                    objectName: "dueDateExpanded"
                    anchors.top: titleText.top
                    anchors.topMargin: 2 * _controller.dpiFactor
                    property bool expanded: root.prettyDueDateString !== ""
                    size: 20
                    text: expanded ? "\uf073" : "\uf133"
                    color: _style.regularTextColor
                    anchors.verticalCenter: undefined
                    onClicked: {
                        if (expanded) {
                            root.task.removeDueDate()
                            textInput.focus = true
                        } else {
                            root.task.dueDate = new Date()
                            calendar.focus = true
                        }
                    }
                }
                Text {
                    id: dueDateText

                    function dateString()
                    {
                        if (root.prettyDueDateString) {
                            return root.task.isOverdue ? root.task.dueDateString : root.prettyDueDateString
                        } else {
                            return qsTr("Not set")
                        }
                    }

                    text: qsTr("Due date") + " (" + dateString() + ")"
                    font.pixelSize: 13 * _controller.dpiFactor
                }
            }

            Column {
                visible: dueDateExpanded.expanded
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 40 * _controller.dpiFactor
                spacing: 2 * _controller.dpiFactor
                Calendar {
                    id: calendar
                    width: parent.width
                    height: 230 * _controller.dpiFactor
                    minimumDate: new Date()
                    selectedDate: (root.task && root.task.dueDateString) ? root.task.dueDate : new Date()

                    onClicked: {
                        root.task.dueDate = date
                        calendar.focus = true
                    }

                    Keys.onEnterPressed: {
                        root.task.dueDate = selectedDate
                        root.accept()
                    }
                    Keys.onReturnPressed: {
                        root.task.dueDate = selectedDate
                        root.accept()
                    }

                    style: CalendarStyle {
                        navigationBar: Rectangle {
                            height: 25 * _controller.dpiFactor

                            color: "#E9E9E9"
                            FontAwesomeIcon {
                                id: iconLeft
                                text: "\uf0a8"
                                anchors.left: parent.left
                                anchors.leftMargin: 15 * _controller.dpiFactor
                                height: parent.height
                                z: 2
                                color: "black"
                                onClicked: {
                                    control.showPreviousMonth()
                                }
                            }
                            Text {
                                height: 25 * _controller.dpiFactor
                                anchors.left: iconLeft.right
                                anchors.right: iconRight.left
                                text: styleData.title
                                font.bold: true
                                font.pixelSize: 20 * _controller.dpiFactor
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignBottom
                            }
                            FontAwesomeIcon {
                                id: iconRight
                                text: "\uf0a9"
                                anchors.right: parent.right
                                anchors.rightMargin: 15 * _controller.dpiFactor
                                height: parent.height
                                z: 2
                                color: "black"
                                onClicked: {
                                    control.showNextMonth()
                                }
                            }
                        }

                        dayOfWeekDelegate: Rectangle {
                            height: 25 * _controller.dpiFactor
                            Text {
                                text: Qt.locale().dayName(styleData.dayOfWeek, control.dayOfWeekFormat)
                                horizontalAlignment: Text.AlignHCenter
                                anchors.fill: parent
                                font.pixelSize: 14 * _controller.dpiFactor
                            }
                        }
                    }
                }

                Text {
                    text: qsTr("Task will be automatically moved to \"Today's queue\" at this date")
                    anchors.left: parent.left
                    anchors.right: parent.right
                    wrapMode: Text.Wrap
                    font.pixelSize: 12 * _controller.dpiFactor
                    color: "#444444"
                }
            }
        }

        PushButton {
            id: buttonOk
            text: qsTr("OK")
            objectName: "okTaskEditor"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10 * _controller.dpiFactor
            onClicked: {
                root.accept()
            }
        }
    }

    function accept()
    {
        if (task && textInput.text)
            task.summary = textInput.text

        _controller.editTask(null, Controller.EditModeNone)
    }
}
