import QtQuick 2.0

Popup {
    id: root
    property alias text: textItem.text
    property alias enteredText: textInput.text

    contentItem:
    Item {
        anchors.fill: parent

        Text {
            id: textItem
            anchors.left: textInput.left
            anchors.right: parent.right
            anchors.leftMargin: -2 * _controller.dpiFactor
            font.pixelSize: 18 * _controller.dpiFactor
            anchors.top: parent.top
            anchors.topMargin: _controller.marginSmall
        }

        TextInput {
            id: textInput
            focus: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: 0.70 * parent.width
        }

        Rectangle {
            height: 1 * _controller.dpiFactor
            width: textInput.width
            anchors.top: textInput.bottom
            anchors.left: textInput.left
            color: "black"
        }
    }
}
