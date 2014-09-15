import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

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

    Flow {
        anchors.top: parent.top
        anchors.topMargin: _style.marginBig
        anchors.bottom: parent.bottom
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin:_style.tagsRightMargin
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: _style.tagSpacing

        move: Transition {
            NumberAnimation { properties: "x"; duration: _style.tagMoveAnimationDuration }
        }

        FontAwesomeIcon {
            id: addImage
            toolTip: qsTr("Add new tag")
            text: "\uf067"
            anchors.verticalCenter: undefined
            color: enabled ? "black" : "gray"
            enabled: _controller.tagEditStatus !== Controller.TagEditStatusNew
            onClicked: {
                newTag.textField.text = ""
                _controller.beginAddingNewTag()
                newTag.textField.forceActiveFocus()
            }
        }

        Tag {
            id: newTag
            visible: _controller.tagEditStatus === Controller.TagEditStatusNew
            beingEdited: true
            onEdited: {
                _controller.endAddingNewTag(newTagName)
            }
        }

        Repeater {
            model: _storage.tagsModel
            delegate:
            Tag {
                tagObj: tag
                beingEdited: tagObj.beingEdited
                tagName: tagObj.name
                onEdited: {
                    _controller.renameTag(tag.name, newTagName)
                }
            }
        }
    }
}
