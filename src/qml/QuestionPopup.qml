import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    visible: _controller.popupVisible
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
        Item {
            id: shadowContainer
            z: background.z + 1
            height: _style.questionDialogHeight
            width: parent ? parent.width * 0.80 : 500 * _controller.dpiFactor
            anchors.centerIn: parent

            Rectangle {
                id: popupRect
                width: shadowContainer.width - 16 * _controller.dpiFactor
                height: shadowContainer.height - 16 * _controller.dpiFactor
                anchors.centerIn: parent
                border.color: "gray"
                border.width: 1 * _controller.dpiFactor
                radius: 7 * _controller.dpiFactor
                color: "white"

                Item {
                    anchors.fill: parent

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: _style.marginMedium
                        anchors.right: parent.right
                        anchors.rightMargin: _style.marginMedium
                        anchors.topMargin: _style.marginSmall
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
                        spacing: 4

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
        }

        DropShadow {
            id: shadowEffect
            anchors.fill: shadowContainer
            cached: true
            smooth: true
            horizontalOffset: 3 * _controller.dpiFactor
            verticalOffset: 3 * _controller.dpiFactor
            radius: 8.0
            samples: 16
            color: "#80000000"
            source: shadowContainer

            Behavior on horizontalOffset {
                NumberAnimation { duration: 300 }
            }
            Behavior on verticalOffset {
                NumberAnimation { duration: 300 }
            }
        }
    }

}
