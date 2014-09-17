import QtQuick 2.0

Popup {
    id: root
    centerPopup: false
    property alias text: textItem.text
    property alias enteredText: textInput.text

    contentItem:
    Item {
        anchors.fill: parent

        Text {
            id: textItem
            anchors.left: textInput.left
            anchors.right: parent.right
            anchors.leftMargin: -4 * _controller.dpiFactor
            font.pixelSize: 19 * _controller.dpiFactor
            anchors.top: parent.top
            anchors.topMargin: _controller.marginSmall
        }

        TextInput {
            id: textInput
            focus: true
            font.pixelSize: 14 * _controller.dpiFactor
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 10 * _controller.dpiFactor
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

    onVisibleChanged: {
        if (visible)
            textInput.forceActiveFocus()
    }
}
