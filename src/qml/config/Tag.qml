import QtQuick 2.0
import QtQuick.Controls 2.0
import Controller 1.0
import ".."

Item {
    id: root
    property QtObject tagObj: null
    readonly property string tagName: tagObj === null ? "" : tagObj.name
    property bool beingEdited: false
    property bool isLast: false

    signal edited(string newTagName)

    height: _style.tagHeight

    function editTag()
    {
        if (tagObj !== null) {
            _controller.editTag(root.tagName)
            textField.text = root.tagName
            textField.forceActiveFocus()
        }
    }

    MouseArea {
        z: 1
        anchors.fill: parent
        onDoubleClicked: {
            editTag()
        }

        Rectangle {
            id: topLine
            color: "#999999"
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            height: Math.floor(1 * _controller.dpiFactor)
        }

        Rectangle {
            id: bottomLine
            color: topLine.color
            visible: root.isLast
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            height: Math.floor(1 * _controller.dpiFactor)
        }

        Text {
            id: tagText
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 5 * _controller.dpiFactor
            }

            visible: !root.beingEdited
            text: tagObj === null ? "" : tagObj.name
            font {
                bold: true
                pixelSize: 20 * _controller.dpiFactor
            }
        }

        Text {
            text: tagObj === null ? "" : (" (" + tagObj.taskCount + ")")
            anchors {
                verticalCenter: parent.verticalCenter
                left: tagText.right
            }
            font {
                pixelSize: 15 * _controller.dpiFactor
            }
        }

        FontAwesomeIcon {
            id: deleteImage
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10 * _controller.dpiFactor
            }

            color: "black"
            visible: !root.beingEdited
            text: "\uf1f8"
            size: _style.deleteTagIconSize

            function reallyRemove()
            {
                _storage.removeTag(root.tagObj.name);
            }

            onClicked: {
                if (tagObj.taskCount > 0) {
                    _controller.showQuestionPopup(this, qsTr("There are tasks using this tag.\nAre you sure you want to delete it?"), "reallyRemove()")
                } else {
                    _storage.removeTag(root.tagObj.name)
                }
            }
        }

        FontAwesomeIcon {
            id: editImage
            anchors {
                verticalCenter: parent.verticalCenter
                right: deleteImage.left
                rightMargin: 15 * _controller.dpiFactor
            }

            color: "black"
            visible: !root.beingEdited
            text: "\uf040"
            size: deleteImage.size

            onClicked: {
                editTag()
            }
        }

        TextField {
            id: textField
            focus: true
            inputMethodHints: Qt.ImhNoPredictiveText
            anchors {
                left: parent.left
                right: editImage.left
                verticalCenter: parent.verticalCenter
            }

            visible: root.beingEdited
            text: root.tagName
            onAccepted: {
                root.edited(textField.text)
            }

            onActiveFocusChanged: {
                if (!activeFocus)
                    _controller.editTag("")
            }

            onVisibleChanged:
                if (!visible && _controller.tagEditStatus !== Controller.TagEditStatusNone) {
                    _controller.editTag("")
                }
        }
    }
}
