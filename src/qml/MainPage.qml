import QtQuick 2.0
import Controller 1.0

Page {
    id: root
    page: Controller.MainPage

    Rectangle {
        color: _style.queueBackgroundColor
        anchors.fill: parent
        radius: parent.radius

        Component {
            id: taskListComponent
            TaskListView {
                id: stagedView
                objectName: "stagedView"
                model: _storage.stagedTasksModel
                emptyText: qsTr("No tasks queued for today.") + "\n"+ qsTr("Create new ones or pick some from your archive.")
            }
        }

        Loader {
            anchors.topMargin: _style.marginSmall
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            asynchronous: true
            visible: _controller.queueType === Controller.QueueTypeToday
            sourceComponent: _loadManager.taskListRequested ? taskListComponent : null
            objectName: "taskListLoader"
        }

        Loader {
            id: tabView
            // Delayed loading for startup performance improvement
            visible: _controller.queueType !== Controller.QueueTypeToday
            anchors.margins: _style.marginSmall
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: _style.tagTabHeight
            sourceComponent:  _loadManager.archiveRequested ? Qt.createComponent("DecentTabView.qml") : null
            objectName: "tabViewLoader"
        }

        Component {
            id: archiveViewComponent
            TaskListView {
                id: archiveView
                objectName: "archiveView"

                function emptyTextForViewType()
                {
                    if (!_controller.currentTag)
                        return ""

                    if (_controller.currentTag === _controller.allTasksTag) {
                        return qsTr("No archived tasks found.")
                    } else if (_controller.currentTag === _controller.hotTasksTag) {
                        return qsTr("No hot tasks found.")
                    } else if (_controller.currentTag === _controller.untaggedTasksTag) {
                        return qsTr("No archived untagged tasks found.")
                    } else if (_controller.currentTag === _controller.dueDateTasksTag) {
                        return qsTr("No tasks with due date found.")
                    } else {
                        return qsTr("No archived tasks found with tag %1").arg(_controller.currentTag.name)
                    }
                }

                model: _controller.currentTag ? _controller.currentTag.taskModel : null
                emptyText: emptyTextForViewType()
            }
        }

        Loader {
            anchors.topMargin: _style.marginMedium
            anchors.top: tabView.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: tabView.visible
            sourceComponent: _loadManager.archiveRequested ? archiveViewComponent : null
            objectName: "archiveViewLoader"
        }
    }
}
