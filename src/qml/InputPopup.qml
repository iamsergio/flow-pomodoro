import QtQuick 2.0

Popup {
    id: root
    visible: _controller.popupVisible
    property alias text: textItem.text

    contentItem:
    Item {
        anchors.fill: parent

        Text {
            id: textItem
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 15 * _controller.dpiFactor
            anchors.top: parent.top
            anchors.topMargin: _controller.marginSmall
        }

        TextInput {
            id: textInput
            focus: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -10 * _controller.dpiFactor
            width: 0.70 * parent.width
            text: qsTr("Teste")
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
