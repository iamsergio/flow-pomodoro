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
                model: _storage.stagedTasksModel
                emptyText: qsTr("No queued tasks for today.") + "\n"+ qsTr("Please create new ones or pick some from your archive.")
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
            sourceComponent: _controller.taskListRequested ? taskListComponent
                                                           : null
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
            sourceComponent:  _controller.archiveRequested ? Qt.createComponent("DecentTabView.qml")
                                                           : null
        }

        Component {
            id: archiveViewComponent
            TaskListView {
                id: archiveView
                model: _controller.currentTabTag == null ? _storage.untaggedTasksModel : _controller.currentTabTag.taskModel
                emptyText: _controller.currentTabTag == null ? qsTr("No archived untagged tasks found.")
                                                             : qsTr("No archived tasks found with tag %1").arg(_controller.currentTabTag.name)
            }
        }

        Loader {
            anchors.topMargin: _style.marginMedium
            anchors.top: tabView.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: tabView.visible
            sourceComponent: _controller.archiveRequested ? archiveViewComponent
                                                          : null
        }
    }
}
