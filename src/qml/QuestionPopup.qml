import QtQuick 2.0
import QtQuick.Controls 1.0

ModalDialog {
    id: root
    enabled: _controller.popupVisible

    Rectangle {
        height: 75
        color: "lightgray"
        z: overlay.z + 1
        border.color: "darkblue"
        border.width: 2
        radius: _style.queueRadius
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: _style.pageMargin+4
        anchors.rightMargin: _style.pageMargin+4
        anchors.bottomMargin: (!_controller.stopped ? _style.marginSmall : _style.marginMedium) + 4
        Text {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 7
            anchors.topMargin: 3
            anchors.right: parent.right
            font.pointSize: 11
            wrapMode: Text.WordWrap
            text: _controller.popupText
        }
        Row {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.bottomMargin: 5
            spacing: 4

            Button {
                id: buttonOk
                text: "OK"
                onClicked: {
                    _controller.onPopupButtonClicked(true)
                }
            }

            Button {
                id: buttonCancel
                text: "Cancel"
                onClicked: {
                    _controller.onPopupButtonClicked(false)
                }
            }
        }
    }
}
