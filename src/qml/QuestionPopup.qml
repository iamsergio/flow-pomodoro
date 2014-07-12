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
            font.pointSize: _style.dialogFontSize
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
                text: qsTr("OK")
                onClicked: {
                    _controller.onPopupButtonClicked(true)
                }
            }

            Button {
                id: buttonCancel
                text: qsTr("Cancel")
                onClicked: {
                    _controller.onPopupButtonClicked(false)
                }
            }
        }
    }
}
