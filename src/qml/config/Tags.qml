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
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin: 40 * _controller.dpiFactor
        anchors.topMargin: _style.marginBig
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 5

        move: Transition {
            NumberAnimation { properties: "x"; duration: _style.tagMoveAnimationDuration }
        }

        Text {
            id: invisible_helper
            text: "Invisible Item"
            visible: false
            font.pointSize: _style.tagFontSize
            font.bold: _style.tagFontBold
            color: _style.tagFontColor
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
            visible: _controller.addingNewTag
            beingEdited: true
            onEdited: {
                _controller.endAddingNewTag(newTagName)
            }
        }
    }

    ClickableImage {
        id: addImage
        source: "qrc:/img/add.png"
        anchors.bottom: parent.bottomight
        enabled: !_controller.addingNewTag
        onClicked: {
            newTag.textField.text = ""
            _controller.beginAddingNewTag()
            newTag.textField.forceActiveFocus()
        }
    }
}
