import QtQuick 2.0
import Controller 1.0

Item {
    id: root
    height: _style.tagTabHeight

    Rectangle {
        z: flickable.z + 1
        rotation: -90
        height: parent.height
        width: height
        y: 0
        x: root.width - height
        visible: !flickable.atXEnd && flickable.visible
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: _style.queueBackgroundColor }
        }
    }

    Rectangle {
        z: flickable.z + 1
        rotation: 90
        height: parent.height
        width: height
        y: 0
        x: 0
        visible: !flickable.atXBeginning && flickable.visible
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: _style.queueBackgroundColor }
        }
    }

    ListView {
        id: flickable
        clip: true
        anchors.fill: parent
        orientation: ListView.Horizontal
        highlightMoveDuration: 50
        highlightFollowsCurrentItem: true
        model: _controller.tagsModel
        delegate:
            TagDelegate {
                height: parent.height
                isLastIndex: index === flickable.model.count - 1
                tagObj: tag
                isSelected: tag === _controller.currentTag
                onIsSelectedChanged: {
                    if (isSelected)
                        flickable.currentIndex = index
                }

                width: Math.max(Math.max(_style.tagTabWidth, root.width / flickable.model.count),
                                textItem.contentWidth + textItem2.contentWidth + textRow.spacing + 12 * _controller.dpiFactor)
            }
    }
}
