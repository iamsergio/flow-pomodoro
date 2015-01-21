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

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: row.width
        clip: true

        Row {
            id: row
            height: parent.height
            width: childrenRect.width

            Repeater {
                id: repeater
                model: _controller.tagsModel
                TagDelegate {
                    anchors.top: row.top
                    anchors.bottom: row.bottom
                    isLastIndex: index === repeater.model.count - 1
                    tagObj: tag
                    isSelected: tag === _controller.currentTag
                    width: Math.max(Math.max(_style.tagTabWidth, root.width / repeater.model.count),
                                    textItem.contentWidth + textItem2.contentWidth + textRow.spacing + 12 * _controller.dpiFactor)
                }
            }
        }
    }
}
