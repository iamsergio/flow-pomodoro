import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Controller 1.0
import ".."

Item {
    id: root
    height: invisible_helper.childrenRect.height + 2 *  _controller.dpiFactor
    width: label.contentWidth + taskCountLabel.contentWidth + _style.tagExtraWidth
    property QtObject tagObj: null
    property bool beingEdited: false
    property string tagName: ""
    property var textField: textField

    signal edited(string newTagName)

    MouseArea {
        anchors.fill: parent

        function editTag()
        {
            if (tagObj !== null) {
                _controller.editTag(root.tagName)
                textField.text = root.tagName
                textField.forceActiveFocus()
            }
        }

        onDoubleClicked: {
            editTag()
        }
        onPressAndHold: {
            editTag()
        }

        Rectangle {
            anchors.fill: parent
            border.width: _style.tagBorderWidth
            border.color: _style.tagBorderColor
            color: _style.tagBackgroundColor
            radius: _style.tagRadius

            TextField {
                id: textField
                focus: true
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                visible: root.beingEdited
                width: parent.width + 2
                text: root.tagName
                style: TextFieldStyle {
                     textColor: "black"
                     background: Rectangle {
                         radius: 6
                         implicitWidth: 100
                         implicitHeight: 24
                         border.color: "#333"
                         border.width: 1
                     }
                }
                onAccepted: {
                    root.edited(textField.text)
                }
                onActiveFocusChanged: {
                    if (!activeFocus) {
                        _controller.editTag("")
                    }
                }
                onVisibleChanged: {
                    if (!visible && _controller.tagEditStatus !== Controller.TagEditStatusNone) {
                        _controller.editTag("")
                    }
                }
            }

            Text {
                id: label
                font.pixelSize: invisible_text.font.pixelSize
                color: _style.tagFontColor
                font.bold: true
                anchors.left: parent.left
                anchors.leftMargin: _style.tagTextLeftMargin
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                height: parent.height
                text: root.beingEdited ? textField.text : root.tagName
                visible: !root.beingEdited
            }

            Text {
                id: taskCountLabel
                anchors.left: label.right
                text: (tagObj === null || !tagObj.taskCount) ? "" : " (" + tagObj.taskCount + ")"
                font.pixelSize: _style.tagCountFontSize
                anchors.verticalCenter: label.verticalCenter
                verticalAlignment: Text.AlignVCenter
                color: _style.tagFontColor
                visible: !root.beingEdited
            }

            ClickableImage {
                id: deleteImage
                source: "qrc:/img/delete.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: _style.buttonsSpacing
                visible: !root.beingEdited

                function reallyRemove()
                {
                    _storage.removeTag(tagObj.name);
                }

                onClicked: {
                    if (tagObj.taskCount > 0) {
                        _controller.showQuestionPopup(this, qsTr("There are tasks using this tag. Are you sure you want to delete it?"), "reallyRemove()")
                    } else {
                        _storage.removeTag(root.tagName)
                    }
                }
            }
        }
    }
}
