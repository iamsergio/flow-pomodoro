import QtQuick 2.0

Popup {
    id: root
    visible: _controller.popupVisible

    contentItem:
    Item {
        anchors.fill: parent
        Text {
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium
            anchors.right: parent.right
            anchors.rightMargin: _style.marginMedium
            anchors.topMargin: _style.marginMedium
            anchors.top: parent.top
            font.pixelSize: _style.dialogFontSize
            wrapMode: Text.WordWrap
            text: _controller.popupText
        }
    }
}
