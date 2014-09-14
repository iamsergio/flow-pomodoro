import QtQuick 2.0
import QtGraphicalEffects 1.0

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

        Row {
            id: row
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.dialogButtonsMargin
            spacing: 4 * _controller.dpiFactor

            PushButton {
                id: buttonOk
                text: qsTr("OK")
                onClicked: {
                    _controller.onPopupButtonClicked(true)
                }
            }

            PushButton {
                id: buttonCancel
                text: qsTr("Cancel")
                onClicked: {
                    _controller.onPopupButtonClicked(false)
                }
            }
        }
    }
}
