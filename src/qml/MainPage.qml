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
                anchors.topMargin: -5 * _controller.dpiFactor
                font.pixelSize: _style.titleFontSize
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

        ClickableImage {
            id: addIcon
            source: "qrc:/img/add.png"
            anchors.top: headerRectangle.bottom
            anchors.topMargin: _style.marginSmall
            anchors.right: parent.right
            anchors.rightMargin: _style.marginSmall
            toolTip: qsTr("Add new task")
            onClicked: {
                _controller.addTask("New Task", /**open editor=*/true) // TODO: Pass edit mode instead
            }
        }

        TaskListView {
            id: stagedView
            model: _storage.stagedTasksModel
            anchors.topMargin: _style.marginSmall
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: addIcon.bottom
            anchors.bottom: parent.bottom
            visible: _controller.queueType === Controller.QueueTypeToday
        }

        DecentTabView {
            id: tabView
            visible: _controller.queueType !== Controller.QueueTypeToday
            anchors.margins: _style.marginSmall
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: addIcon.bottom
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

            Text {
                horizontalAlignment: Text.AlignHCenter
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -height
                text: _controller.currentTabTag == null ? qsTr("No archived untagged tasks found.") + (_storage.tagsModel.count === 0 ? "" : qsTr("\nClick the tag bar above to see tagged tasks.") )
                                                        : qsTr("No archived tasks found with tag %1").arg(_controller.currentTabTag.name)
                visible: parent.model.count === 0
            }
        }
    }
}
