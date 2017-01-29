import QtQuick 2.0

import Controller 1.0
import ".."

MouseArea {
    id: root
    anchors.fill: parent
    onClicked: {
        if (_controller.tagEditStatus !== Controller.TagEditStatusNone)
            _controller.editTag("")
        mouse.accepted = false
    }

    SmallText {
        id: noTagsText
        anchors.centerIn: parent
        text: qsTr("No tags")
        visible: _storage.tagsModel.count === 0
        horizontalAlignment: Text.AlignHCenter
    }

    FontAwesomeIcon {
        id: addImage
        toolTip: qsTr("Add new tag")
        text: "\uf067"
        color: enabled ? "black" : "gray"
        size: _style.addTagIconSize
        anchors {
            top: parent.top
            topMargin: 10 * _controller.dpiFactor
            rightMargin: 10 * _controller.dpiFactor
            right: parent.right
            verticalCenter: undefined
        }

        enabled: _controller.tagEditStatus !== Controller.TagEditStatusNew
        onClicked: {
            _controller.newTagDialogVisible = true
        }
    }

    ListView {
        id: tagView
        clip: true
        anchors {
            topMargin: 10 * _controller.dpiFactor
            bottomMargin: 5 * _controller.dpiFactor
            top: addImage.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        model: _storage.tagsModel
        delegate: Tag {
            tagObj: tag
            beingEdited: tagObj !== null && tagObj.beingEdited
            isLast: index === _storage.tagsModel.count - 1
            width: parent.width
            onEdited: {
                _controller.renameTag(tag.name, newTagName)
            }
        }
    }
}
