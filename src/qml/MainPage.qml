import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.2
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
            anchors.margins: _style.marginSmall
            height: 40

            Text {
                id: textItem
                anchors.top: parent.top
                anchors.topMargin: -5
                font.pointSize: 18
                font.bold: true
                color: _style.regularTextColor
                text: _controller.queueType === Controller.QueueTypeToday ? qsTr("Today's work queue") : qsTr("Tasks for someday")
            }

            Switch {
                id: switchItem
                checked: _controller.queueType === Controller.QueueTypeToday
                anchors.right: parent.right
                anchors.rightMargin: 2
                Binding {
                    target: _controller
                    property: "queueType"
                    value: switchItem.checked ? Controller.QueueTypeToday : Controller.QueueTypeArchive
                }
            }
        }

        TaskListView {
            id: stagedView
            model: _taskStorage.stagedTasksModel
            anchors.topMargin: _style.marginSmall
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: headerRectangle.bottom
            anchors.bottom: parent.bottom
            visible: _controller.queueType === Controller.QueueTypeToday
        }

        DecentTabView {
            id: tabView
            visible: _controller.queueType !== Controller.QueueTypeToday
            anchors.margins: _style.marginSmall
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: headerRectangle.bottom
            model: _tagStorage.nonEmptyTagModel
        }

        TaskListView {
            visible: tabView.visible
            model: _controller.currentTabTag == null ? _taskStorage.untaggedTasksModel : _controller.currentTabTag.taskModel
            anchors.topMargin: _style.marginMedium
            anchors.top: tabView.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            Text {
                anchors.centerIn: parent
                text: _controller.currentTabTag == null ? qsTr("No archived untagged tasks found")
                                                        : qsTr("No archived tasks found with tag %1", _controller.currentTabTag.name )
                visible: parent.model.count === 0
            }


        }
    }
}
