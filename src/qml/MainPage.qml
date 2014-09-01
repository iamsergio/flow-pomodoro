import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import Controller 1.0

Page {
    id: root
    page: Controller.MainPage

    Rectangle {
        color: _style.queueBackgroundColor
        anchors.fill: parent
        radius: parent.radius

        Item {
            id: headerRectangle
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: _style.marginMedium
            anchors.rightMargin: _style.marginSmall
            anchors.leftMargin: _style.marginMedium
            height: childrenRect.height

            Switch {
                id: switchItem
                checked: _controller.queueType === Controller.QueueTypeToday
                anchors.left: addIcon.right
                anchors.right: parent.right
                anchors.verticalCenter: addIcon.verticalCenter
                anchors.leftMargin: _style.marginMedium
                style: SwitchStyle {
                    groove:
                    Rectangle {
                            property color shadow: control.checked ? Qt.darker(highlight, 1.2): "#999"
                            property color bg: control.checked ? highlight: "#bbb"
                            property color highlight: "#bbb"

                            implicitWidth: headerRectangle.width - addIcon.width - _style.marginMedium
                            implicitHeight: addIcon.height

                            border.color: "gray"
                            color: "red"

                            radius: 2 * _controller.dpiFactor
                            Behavior on shadow { ColorAnimation{ duration: 80 } }
                            Behavior on bg { ColorAnimation{ duration: 80 } }
                            gradient: Gradient {
                                GradientStop { color: shadow; position: 0 }
                                GradientStop { color: bg ; position: 0.2 }
                                GradientStop { color: bg ; position: 1 }
                            }

                            Rectangle {
                                color: "#44ffffff"
                                height: 1
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: -1 * _controller.dpiFactor
                                width: parent.width - 2 * _controller.dpiFactor
                                x: 1
                            }

                            Text {
                                id: text1
                                anchors.left: parent.left
                                width: parent.width / 2
                                anchors.verticalCenter: parent.verticalCenter
                                visible: control.checked
                                text: qsTr("Today's tasks")
                                horizontalAlignment: Text.AlignHCenter
                                color: "#4A4A53"
                            }
                            Text {
                                anchors.right: parent.right
                                width: parent.width / 2
                                anchors.verticalCenter: parent.verticalCenter
                                visible: !control.checked
                                text: qsTr("Later tasks")
                                color: text1.color
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                    handle:
                    Rectangle {
                        opacity: control.enabled ? 1.0 : 0.5
                        implicitWidth: Math.round((parent.parent.width - padding.left - padding.right) / 2)
                        implicitHeight: control.height - padding.top - padding.bottom

                        border.color: control.activeFocus ? Qt.darker(highlight, 2) : Qt.darker(button, 2)
                        property color bg: control.activeFocus ? Qt.darker(highlight, 1.2) : button
                        property color highlight: "#43ACE8"
                        property color button: "black"
                        gradient: Gradient {
                            GradientStop { color: "gray"; position: 0}
                            GradientStop { color: bg; position: 0.5}
                            GradientStop { color: bg; position: 1}
                        }

                        Text {
                            anchors.centerIn: parent
                            color: "white"
                            text: control.checked ? qsTr("Later tasks") : qsTr("Today's tasks")
                            font.bold: true
                            font.pixelSize: 14 * _controller.dpiFactor
                        }

                        radius: 2 * _controller.dpiFactor
                    }
                }

                Binding {
                    target: _controller
                    property: "queueType"
                    value: switchItem.checked ? Controller.QueueTypeArchive : Controller.QueueTypeToday
                }
            }

            ClickableImage {
                id: addIcon
                source: "image://icons/add.png"
                anchors.top: parent.top
                anchors.left: parent.left
                toolTip: qsTr("Add new task")
                onClicked: {
                    _controller.addTask("New Task", /**open editor=*/true) // TODO: Pass edit mode instead
                }
            }
        }

        TaskListView {
            id: stagedView
            model: _storage.stagedTasksModel
            anchors.topMargin: _style.marginSmall
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: headerRectangle.bottom
            anchors.bottom: parent.bottom
            visible: _controller.queueType === Controller.QueueTypeToday
            emptyText: qsTr("No queued tasks for today.") + "\n"+ qsTr("Please create new ones or pick some from your archive.")
        }

        DecentTabView {
            id: tabView
            visible: _controller.queueType !== Controller.QueueTypeToday
            anchors.margins: _style.marginSmall
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: headerRectangle.bottom
            model: _storage.tagsModel
        }

        TaskListView {
            id: archiveView
            visible: tabView.visible
            model: _controller.currentTabTag == null ? _storage.untaggedTasksModel : _controller.currentTabTag.taskModel
            anchors.topMargin: _style.marginMedium
            anchors.top: tabView.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            emptyText: _controller.currentTabTag == null ? qsTr("No archived untagged tasks found.") + (_storage.tagsModel.count === 0 ? "" : qsTr("\nClick the tag bar above to see tagged tasks.") )
                                                         : qsTr("No archived tasks found with tag %1").arg(_controller.currentTabTag.name)
        }
    }
}
