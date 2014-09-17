import QtQuick 2.0

Item {
    id: root
    property alias contentItem: contentItem.children
    property alias icon: iconItem.text

    signal clicked()

    onVisibleChanged: {
        if (visible)
            Qt.inputMethod.hide()
    }

    Rectangle {
        id: background
        anchors.fill: parent
        opacity: 0.5
        color: "black"
    }

    MouseArea {
        anchors.fill: parent

        Rectangle {
            id: popupRect
            width: parent ? parent.width * 0.90 : 500 * _controller.dpiFactor
            height: _style.questionDialogHeight
            anchors.centerIn: parent
            border.color: "gray"
            border.width: 1 * _controller.dpiFactor
            //radius: 7 * _controller.dpiFactor
            color: "white"
            z : borderImage.z + 1

            FontAwesomeIcon {
                id: iconItem
                size: 55
                color: "black"
                text: root.icon
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 30 * _controller.dpiFactor
                anchors.leftMargin: 30 * _controller.dpiFactor
                anchors.verticalCenter: undefined
            }

            Item {
                id: contentItem
                anchors.top: parent.top
                anchors.topMargin: 15 * _controller.dpiFactor
                anchors.left: iconItem.right
                anchors.leftMargin: 10 * _controller.dpiFactor
                anchors.right: parent.right
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
        BorderImage {
            id: borderImage
            anchors.fill: popupRect
            anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
            border { left: 10; top: 10; right: 10; bottom: 10 }
            source: "qrc:/img/shadow.png"
        }
    }
}
