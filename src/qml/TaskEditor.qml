import QtQuick 2.2
import QtQuick.Controls 2.0
import Controller 1.0
import Task 1.0

Overlay {
    id: root
    property QtObject task: null
    readonly property string prettyDueDateString: task ? task.prettyDueDateString : ""
    objectName: "taskEditor"

    contentHeight: height - contentTopMargin - _style.pageMargin - _style.marginSmall
    centerPopup: false

    ListModel {
        id: priorityModel
        ListElement { label: "cold"; priorityValue: 10; priorityColor: "cyan" }
        ListElement { label: "neutral"; priorityValue: 0; priorityColor: "black" }
        ListElement { label: "hot"; priorityValue: 1; priorityColor: "#ff6b15" }
    }

    ListModel {
        id: recurrenceModel
        ListElement { label: "none"   ; value: Task_.PeriodTypeNone   ; }
        ListElement { label: "daily"  ; value: Task_.PeriodTypeDaily  ; }
        ListElement { label: "weekly" ; value: Task_.PeriodTypeWeekly ; }
        ListElement { label: "monthly"; value: Task_.PeriodTypeMonthly; }
        ListElement { label: "yearly" ; value: Task_.PeriodTypeYearly ; }
    }

    contentItem:
    Item {
        anchors.fill: parent

        Flickable {
            id: flik

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: buttonOk.top
                bottomMargin: 3 * _controller.dpiFactor
                leftMargin: 10 * _controller.dpiFactor
                rightMargin: 10 * _controller.dpiFactor
            }

            contentWidth: column.width
            contentHeight: column.height
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            Column {
                id: column
                spacing: 6 * _controller.dpiFactor

                width: flik.width

                Item {
                    height: 20 * _controller.dpiFactor
                    width: parent.width

                    TextInput {
                        id: textInput
                        focus: root.visible
                        inputMethodHints: Qt.ImhNoPredictiveText
                        font.pixelSize: 14 * _controller.dpiFactor
                        selectByMouse: true

                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        anchors.verticalCenter: parent.verticalCenter
                        width: 0.90 * parent.width
                        height: parent.height
                        clip: true
                        wrapMode: TextInput.WordWrap
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
                    visible: _settings.supportsPriority
                    height: 20 * _controller.dpiFactor
                    width: parent.width
                    Text {
                        id: priorityText
                        text: qsTr("Relevance") + ":"
                        anchors.left: parent.left
                        font.pixelSize: 13 * _controller.dpiFactor
                    }

                    Row {
                        objectName: "priorityRow"
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
                                    color: highlightRect.visible ? highlightRect.color : "black"
                                    font.pixelSize: 14 * _controller.dpiFactor
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                                Rectangle {
                                    id: highlightRect
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
                        readonly property bool expanded: root.prettyDueDateString !== ""
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

                    Text {
                        text: qsTr("Task will be moved to \"Today's queue\" at this date.")
                        anchors.left: parent.left
                        anchors.right: parent.right
                        wrapMode: Text.Wrap
                        font.pixelSize: 12 * _controller.dpiFactor
                        color: "#444444"
                    }

                    Calendar {
                        id: calendar
                        height: 230 * _controller.dpiFactor
                        date: root.task.dueDate
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        onDateChanged: {
                            _controller.setTaskDueDate(root.task, date)
                        }
                    }
                }

                Item {
                    visible: dueDateExpanded.expanded
                    height: Math.max(recIcon.height, recRow.implicitHeight)
                    width: parent.width

                    FontAwesomeIcon {
                        id: recIcon
                        enabled: false
                        anchors.left: parent.left
                        anchors.leftMargin: (column.width - calendar.width) / 2
                        color: "black"
                        text: "\uf021"
                        onClicked: {
                            root.task.toggleRecurrenceType(root.task.periodType)
                        }
                    }

                    Row {
                        id: recRow
                        objectName: "recRow"
                        anchors.leftMargin: 10 * _controller.dpiFactor
                        anchors.left: recIcon.right
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: -3 * _controller.dpiFactor
                        spacing: 10 * _controller.dpiFactor

                        Repeater {
                            model: recurrenceModel
                            Item {
                                height: recLabel.implicitHeight + highlight.height
                                width: recLabel.implicitWidth

                                Text {
                                    id: recLabel
                                    text: label
                                    color: "black"
                                    font.pixelSize: 14 * _controller.dpiFactor
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }

                                Rectangle {
                                    id: highlight
                                    height: 3 * _controller.dpiFactor
                                    color: "black"
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: -3 * _controller.dpiFactor
                                    anchors.leftMargin: -2 * _controller.dpiFactor
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.rightMargin: -2 * _controller.dpiFactor
                                    visible: root.task && value === root.task.periodType
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        root.task.toggleRecurrenceType(value)
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    visible: dueDateExpanded.expanded && root.task.recurs
                    height: frequency.implicitHeight
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: recIcon.width + recRow.anchors.leftMargin + recIcon.anchors.leftMargin
                    Row {
                        spacing: 10 * _controller.dpiFactor
                        Text {
                            id: frequency
                            text: qsTr("every")
                            font.pixelSize: 13 * _controller.dpiFactor
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        TextInput {
                            text: root.task ? root.task.frequency : null
                            width: 25 * _controller.dpiFactor
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 14 * _controller.dpiFactor
                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 1 * _controller.dpiFactor
                                color: "black"
                            }
                            onTextChanged: {
                                if (text)
                                    root.task.frequency = parseInt(text)
                            }
                        }

                        Text {
                            text: root.task ? root.task.frequencyWord : null
                            font.pixelSize: 13 * _controller.dpiFactor
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }

                TextArea {
                    id: descriptionArea

                    Text {
                        id: placeholderText
                        text: "enter description.."
                        enabled: false
                        color: "gray"
                        visible: descriptionArea.text.length == 0 && !descriptionArea.focus
                        anchors {
                            left: parent.left
                            leftMargin: 3 * _controller.dpiFactor
                            top: parent.top
                            topMargin: 3 * _controller.dpiFactor
                        }
                    }

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: Math.max(contentHeight, placeholderText.height * 3)
                    text: root.task.description
                }

                Item {
                    visible: _settings.supportsEffort
                    height: frequency.implicitHeight
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //anchors.leftMargin: recIcon.width + recRow.anchors.leftMargin + recIcon.anchors.leftMargin
                    Row {
                        spacing: 10 * _controller.dpiFactor
                        Text {
                            text: qsTr("Needed effort:")
                            font.pixelSize: 13 * _controller.dpiFactor
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        TextInput {
                            text: root.task ? root.task.estimatedEffort : null
                            width: 25 * _controller.dpiFactor
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 14 * _controller.dpiFactor
                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 1 * _controller.dpiFactor
                                color: "black"
                            }
                            onTextChanged: {
                                if (text)
                                    root.task.estimatedEffort = parseInt(text)
                            }
                        }

                        Text {
                            text: "hours"
                            font.pixelSize: 13 * _controller.dpiFactor
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
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
        if (task) {
            if (textInput.text) { // Mandatory summary
                task.summary = textInput.text
            }

            task.description = descriptionArea.text
        }

        _controller.editTask(null, Controller.EditModeNone)
    }
}
