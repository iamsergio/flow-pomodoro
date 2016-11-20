import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
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
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 5 * _controller.dpiFactor
            }

            visible: !root.beingEdited
            text: tagObj === null ? "" : (tagObj.name/*  + " (" + root.tagObj.taskCount + ")"*/)
            font {
                bold: true
                pixelSize: 20 * _controller.dpiFactor
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

        TextField { // TODO: Replace with something in QtQuick.Controls2
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
            style: TextFieldStyle {
                textColor: "black"
                font.pixelSize: 12 * _controller.dpiFactor
                background: Rectangle {
                    radius: 2 * _controller.dpiFactor
                    implicitWidth: 100 * _controller.dpiFactor
                    implicitHeight: 24 * _controller.dpiFactor
                    border.color: "#333"
                    border.width: 1 * _controller.dpiFactor
                }
            }

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
