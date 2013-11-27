import QtQuick 2.0
import QtQuick.Controls 1.0

Rectangle {

    signal deleteClicked()

    property string taskText: ""
    property bool editMode: false
    property bool otherItemBeingEdited: false
    property bool buttonsVisible: true
    property bool hasMouseOver: mouseArea.containsMouse
    property int modelIndex: -1
    property bool selected: hasMouseOver && !editMode && !otherItemBeingEdited && modelIndex !== -1

    id: root
    color: selected ? _style.hoveredTaskBgColor : _style.taskBackgroundColor
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: 5
    anchors.rightMargin: 5
    height: 50
    border.color: _style.taskBorderColor
    border.width: selected ? _style.hoveredTaskBorderWidth : 1
    radius: _style.taskBorderRadius

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: (!editMode && !otherItemBeingEdited) ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: {
            _controller.indexBeingEdited = -1
            _controller.startPomodoro(modelIndex)
        }

        onPressAndHold: {
            _controller.indexBeingEdited = modelIndex
            textField.forceActiveFocus()
        }

        onDoubleClicked: {
            if (modelIndex !== -1) {
                _controller.indexBeingEdited = modelIndex
                textField.forceActiveFocus()
            }
        }

        Text {
            id: textItem
            text: taskText
            elide: Text.ElideRight
            color: root.selected ? _style.hoveredTaskFgColor : _style.taskFontColor
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 16*3 + 15 // 3 icons, 3 margins
            font.pointSize: _style.taskFontSize
            visible: !textField.visible
        }

        TextField {
            id: textField
            visible: root.editMode
            text: taskText
            anchors.left: textItem.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: deleteImage.left
            anchors.rightMargin: 10
            focus: true
            onVisibleChanged: {
                if (visible) {
                    text = taskText // The binding gets broken each time you edit, so do it here
                    forceActiveFocus()
                }
            }

            Component.onCompleted: {
                forceActiveFocus()
                selectAll()
            }

            onTextChanged: {
                if (modelIndex !== -1) {
                    _controller.updateTask(modelIndex, text)
                }
            }

            onAccepted: {
                if (modelIndex !== -1) {
                    _controller.indexBeingEdited = -1
                    _controller.updateTask(modelIndex, text)
                }
            }
        }

        ClickableImage {
            id: deleteImage
            source: "qrc:/img/delete.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: editImage.left
            anchors.rightMargin: _style.buttonsSpacing
            visible: root.buttonsVisible
            onClicked: {
                root.deleteClicked()
            }
        }

        ClickableImage {
            id: editImage
            source: "qrc:/img/edit.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: _style.buttonsRightMargin
            visible: root.buttonsVisible
            onClicked: {
                if (modelIndex !== -1) {
                    _controller.indexBeingEdited = modelIndex
                    if (_controller.indexBeingEdited !== -1) {
                        textField.forceActiveFocus()
                    }
                }
            }
        }

        /*
        ClickableImage {
            id: playImage
            source: "qrc:/img/play.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            visible: false root.buttonsVisible.
            onClicked: {
                _controller.startPomodoro(modelIndex, _style.defaultPomodoroDuration)
            }
        }*/
    }
}
