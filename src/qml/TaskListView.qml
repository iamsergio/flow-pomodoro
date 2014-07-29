import QtQuick 2.0
import QtQuick.Controls 1.0

import Controller 1.0

ListView {
    id: root
    clip: true
    highlightFollowsCurrentItem: true
    currentIndex: _controller.indexBeingEdited
    spacing: 3 * _controller.dpiFactor
    visible: _controller.expanded
    onCountChanged: {
        // HACK: For some reason the first inserted element takes more than 1 event loop.
        // It doesn't go immediately into the list view after we insert it into the model in controller.cpp
        // that event loop run breaks focus, so restore it here.
        _controller.forceFocus(count-1)
    }

    delegate: Task {
        taskObj: task
        buttonsVisible: _controller.editMode === Controller.EditModeNone && hasMouseOver
        modelIndex: index

        onDeleteClicked: {
            if (_style.deleteAnimationEnabled) {
                visible = false
                animatedTask.y = y
                animatedTask.taskSummary = taskObj.summary
                animatedTask.visible = true
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
        visible: false
        NumberAnimation on y {
            id: animation
            running: false
            to: root.height
            duration: _style.deleteAnimationDuration
        }
    }

    contentItem.z: 2

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (_controller.indexBeingEdited !== -1) {
                _controller.editTask(null, Controller.EditModeNone)
            } else {
                _controller.expanded = false
            }
        }
    }
}
