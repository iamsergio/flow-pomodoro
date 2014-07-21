import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.2
import Controller 1.0

Page {
    id: root
    page: Controller.MainPage

    Rectangle {
        color: _style.queueBackgroundColor
        anchors.fill: parent
        radius: parent.radius
        TabView {
            anchors.margins: _style.marginSmall
            anchors.fill: parent
            frameVisible: false
            style: TabViewStyle {
                tabOverlap: 6
            }

            Tab {
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
                    title: tag.name + " (" + tag.taskModel.count + ")"
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
