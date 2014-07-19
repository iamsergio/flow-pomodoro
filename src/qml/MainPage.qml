import QtQuick 2.0
import QtQuick.Controls 1.0

import Controller 1.0

Page {
    id: root
    page: Controller.MainPage

    Rectangle {
        color: _style.queueBackgroundColor
        anchors.fill: parent
        radius: parent.radius

        TaskListView {
            model: _taskStorage.taskFilterModel
            anchors.topMargin: _style.marginSmall
            anchors.fill: parent
        }
    }
}
