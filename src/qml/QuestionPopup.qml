import QtQuick 2.0
import QtQuick.Controls 1.0

ModalDialog {
    id: root
    enabled: _controller.popupVisible

    content:
    Item {
        anchors.fill: parent

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 7
            anchors.right: parent.right
            font.pointSize: 11
            wrapMode: Text.WordWrap
            text: _controller.popupText
        }

        Row {
            id: row
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 4
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
