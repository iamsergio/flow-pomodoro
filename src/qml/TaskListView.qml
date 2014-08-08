import QtQuick 2.0
import QtQuick.Controls 1.0

import Controller 1.0

ListView {
    id: root
    property alias emptyText: emptyTextLabel.text
    clip: true
    highlightFollowsCurrentItem: true
    highlightMoveVelocity: 20000
    spacing: 3 * _controller.dpiFactor
    visible: _controller.expanded
    onCountChanged: {
        // HACK: For some reason the first inserted element takes more than 1 event loop.
        // It doesn't go immediately into the list view after we insert it into the model in controller.cpp
        // that event loop run breaks focus, so restore it here.

        // Make the newly inserted task visible
        if (_controller.editMode !== Controller.EditModeNone)
           currentIndex = root.count - 1

        _controller.forceFocus(count-1)
    }

    delegate: Task {
        taskObj: task
        buttonsVisible: _controller.editMode === Controller.EditModeNone && (hasMouseOver || _controller.isMobile)
        modelIndex: index

        onDeleteClicked: {
            if (_style.deleteAnimationEnabled) {
                visible = false
                animatedTask.y = y
                animatedTask.taskSummary = taskObj.summary
                animation.running = true
            }
            _controller.removeTask(taskObj)
        }
    }

    displaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 200 }
    }

    Task { // Task for the dropping animation
        y: 200
        id: animatedTask
        width : parent.width
        anchors.left: parent.left
        visible: animation.running
        NumberAnimation on y {
            id: animation
            running: false
            to: root.height
            duration: _style.deleteAnimationDuration
        }
    }

    Text {
        id: emptyTextLabel
        horizontalAlignment: Text.AlignHCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -height
        text: _controller.currentTabTag == null ? qsTr("No archived untagged tasks found.") + (_storage.tagsModel.count === 0 ? "" : qsTr("\nClick the tag bar above to see tagged tasks.") )
                                                : qsTr("No archived tasks found with tag %1").arg(_controller.currentTabTag.name)
        visible: parent.model.count === 0
        wrapMode: Text.WordWrap
        anchors.leftMargin: _style.marginSmall
        anchors.rightMargin: _style.marginSmall
    }

    contentItem.z: 2

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (_controller.taskBeingEdited !== null) {
                _controller.editTask(null, Controller.EditModeNone)
            } else {
                _controller.expanded = false
            }
        }
    }
}
