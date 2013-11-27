import QtQuick 2.0
import QtQuick.Controls 1.0

import Controller 1.0
import "DefaultStyle.js" as Style

Page {
    id: root
    page: Controller.TheQueuePage

    Rectangle {
        color: Style.queueBackgroundColor
        anchors.fill: parent
        radius: parent.radius
        opacity: 1

        ListView {
            id: listView
            anchors.topMargin: 5
            anchors.fill: parent
            model: _taskModel
            clip: true

            spacing: 3
            delegate: Task {
                taskText: name
                editMode: index === _controller.indexBeingEdited
                otherItemBeingEdited: index !==_controller.indexBeingEdited && _controller.indexBeingEdited !== -1
                buttonsVisible: !otherItemBeingEdited && (editMode || hasMouseOver)
                modelIndex: index

                onDeleteClicked: {
                    _controller.removeTask(index)

                    if (Style.deleteAnimationEnabled) {
                        visible = false
                        animatedRectangle.y = y
                        animatedRectangle.taskText = taskText
                        animatedRectangle.visible = true
                        animation.running = true
                    }
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
