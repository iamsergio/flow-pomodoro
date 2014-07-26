import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.2
import Controller 1.0

Page {
    id: root
    page: Controller.MainPage
    property QtObject selectedTagTab: typeof tabView.getTab(tabView.currentIndex) !== "undefined" ? tabView.getTab(tabView.currentIndex).tagObj : null // Compare to "undefined" to fix bogus Qt warning at startup

    onSelectedTagTabChanged: {
        _controller.setCurrentTabTag(selectedTagTab)
    }

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

        TabView {
            id: tabView
            visible: _controller.queueType !== Controller.QueueTypeToday
            anchors.margins: _style.marginSmall
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: headerRectangle.bottom
            anchors.bottom: parent.bottom
            frameVisible: false
            style: TabViewStyle {
                tabOverlap: 3
            }
            Tab {
                property QtObject tagObj: null
                title: qsTr("Misc")
                sourceComponent:
                 TaskListView {
                    model: _taskStorage.untaggedTasksModel
                    anchors.topMargin: _style.marginMedium
                    anchors.fill: parent
                }
            }
            Repeater {
                model: _tagStorage.nonEmptyTagModel
                Tab {
                    property QtObject tagObj: tag
                    title: tag.name + (tag.taskModel.count <= 1 ? "" : " (" + tag.taskModel.count + ")")
                    sourceComponent:
                    TaskListView {
                        model: tag.taskModel
                        anchors.topMargin: _style.marginMedium
                        anchors.fill: parent
                    }
                }
            }
        }
    }
}
