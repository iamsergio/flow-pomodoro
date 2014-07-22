import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.2
import Controller 1.0

Page {
    id: root
    page: Controller.MainPage
    property QtObject selectedTagTab: typeof tabView.getTab(tabView.currentIndex) !== "undefined" ? tabView.getTab(tabView.currentIndex).tagObj : null // Compare to "undefined" to fix bogus Qt warning at startup

    Rectangle {
        color: _style.queueBackgroundColor
        anchors.fill: parent
        radius: parent.radius
        TabView {
            id: tabView
            anchors.margins: _style.marginSmall
            anchors.fill: parent
            frameVisible: false
            style: TabViewStyle {
                tabOverlap: 6
            }

            Tab {
                property QtObject tagObj: null
                title: "All"
                sourceComponent:
                TaskListView {
                    model: _taskStorage.taskFilterModel
                    anchors.topMargin: _style.marginBig
                    anchors.fill: parent
                }
            }

            Repeater {
                model: _tagStorage.nonEmptyTagModel
                Tab {
                    property QtObject tagObj: tag
                    title: tag.name + (tag.taskModel.count == 1 ? "" : " (" + tag.taskModel.count + ")")
                    sourceComponent:
                    TaskListView {
                        model: tag.taskModel
                        anchors.topMargin: _style.marginBig
                        anchors.fill: parent
                    }
                }
            }
        }
    }
}
