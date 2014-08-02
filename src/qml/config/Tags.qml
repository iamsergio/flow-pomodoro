import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    SmallText {
        id: noTagsText
        anchors.centerIn: parent
        text: qsTr("No tags")
        visible: _tagStorage.model.count === 0
        horizontalAlignment: Text.AlignHCenter
    }

    Flow {
        anchors.top: addImage.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin:_style.tagsRightMargin
        anchors.topMargin: _style.marginSmall
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: _style.tagSpacing

        move: Transition {
            NumberAnimation { properties: "x"; duration: _style.tagMoveAnimationDuration }
        }

        Item {
            id: invisible_helper
            visible: false
            Text {
                id: invisible_text
                text: "Invisible Item"
                font.pixelSize: _style.tagFontSize
                font.bold: _style.tagFontBold
                color: _style.tagFontColor
            }
            ClickableImage {
                id: invisible_image
                source: "qrc:/img/delete.png"
            }
        }

        Repeater {
            model: _tagStorage.model
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

        Tag {
            id: newTag
            visible: _controller.tagEditStatus === Controller.TagEditStatusNew
            beingEdited: true
            onEdited: {
                _controller.endAddingNewTag(newTagName)
            }
        }
    }

    ClickableImage {
        id: addImage
        toolTip: qsTr("Add new tag")
        source: "qrc:/img/add.png"
        anchors.top: parent.top
        anchors.topMargin: _style.marginSmall
        enabled: _controller.tagEditStatus !== Controller.TagEditStatusNew
        onClicked: {
            newTag.textField.text = ""
            _controller.beginAddingNewTag()
            newTag.textField.forceActiveFocus()
        }
    }
}
