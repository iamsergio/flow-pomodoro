import QtQuick 2.0

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
            property int numItems: 1 + _storage.tagsModel.count
            property int modelCount: _storage.tagsModel.count

            TagDelegate {
                id: untaggedTab
                text: qsTr("Untagged")
                tagObj: null
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                isLastIndex: row.modelCount === 0
                isSelected: _controller.currentTabTag === null
                taskCount: _storage.untaggedTasksModel.count
                width: Math.max(Math.max(_style.tagTabWidth, root.width / (row.numItems)),
                                textItem.contentWidth + textItem2.contentWidth + textRow.spacing + 12 * _controller.dpiFactor)
            }

            Repeater {
                id: repeater
                model: _controller.hideEmptyTags ? _storage.nonEmptyTagsModel : _storage.tagsModel
                TagDelegate {
                    anchors.top: row.top
                    anchors.bottom: row.bottom
                    isLastIndex: index === row.modelCount - 1
                    text: tag.name
                    tagObj: tag
                    taskCount: tag.taskModel.count
                    isSelected: tag == _controller.currentTabTag
                    width: Math.max(Math.max(_style.tagTabWidth, root.width / (row.numItems)),
                                    textItem.contentWidth + textItem2.contentWidth + textRow.spacing + 12 * _controller.dpiFactor)
                }
            }
        }

        Component.onCompleted: {
            if (_storage.tagsModel.count > 0) {
                _controller.setCurrentTabTag(_storage.tagsModel.at(0))
            }
        }
    }
}
