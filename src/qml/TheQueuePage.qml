import QtQuick 2.0
import QtQuick.Controls 1.0

import Controller 1.0

Page {
    id: root
    page: Controller.TheQueuePage

    Rectangle {
        color: _style.queueBackgroundColor
        anchors.fill: parent
        radius: parent.radius
        opacity: 1

        ListView {
            id: listView
            anchors.topMargin: _style.marginSmall
            anchors.fill: parent
            model: _taskStorage.taskFilterModel
            clip: true
            highlightFollowsCurrentItem: true
            currentIndex: _controller.indexBeingEdited

            spacing: 3 * _controller.dpiFactor

            onCountChanged: {
                // HACK: For some reason the first inserted element takes more than 1 event loop.
                // It doesn't go immediately into the list view after we insert it into the model in controller.cpp
                // that event loop run breaks focus, so restore it here.
                _controller.forceFocus(count-1)
            }

            delegate: Task {
                taskObj: task
                editMode: index === _controller.indexBeingEdited && index !== -1
                otherItemBeingEdited: index !==_controller.indexBeingEdited && _controller.indexBeingEdited !== -1
                buttonsVisible: !otherItemBeingEdited && (editMode || hasMouseOver)
                modelIndex: index

                onDeleteClicked: {
                    if (_style.deleteAnimationEnabled) {
                        visible = false
                        animatedRectangle.y = y
                        animatedRectangle.taskSummary = taskObj.summary
                        animatedRectangle.visible = true
                        animation.running = true
                    }
                    _controller.removeTask(index)
                }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 200 }
            }

            Task { // Task for the dropping animation
                y: 200
                id: animatedRectangle
                width : parent.width
                anchors.left: parent.left
                visible: false
                NumberAnimation on y {
                    id: animation
                    running: false
                    to: root.height
                    duration: 500
                }
            }

            contentItem.z: 2

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (_controller.indexBeingEdited !== -1) {
                        _controller.indexBeingEdited = -1
                    } else {
                        _controller.expanded = false
                    }
                }
            }
        }
    }
}
