import QtQuick 2.0

Item {
    id: root
    property alias contentItem: contentItem.children
    property bool centerPopup: true
    property int contentHeight: -1
    property int contentTopMargin: _style.menuBarHeight + _style.marginSmall

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
            height: root.contentHeight == -1 ? _style.questionDialogHeight : root.contentHeight
            anchors.verticalCenter: root.centerPopup ? parent.verticalCenter : undefined
            anchors.top: root.centerPopup ? undefined : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            border.color: "gray"
            border.width: 1 * _controller.dpiFactor
            anchors.topMargin: root.centerPopup ? 0 : root.contentTopMargin
            color: "white"
            z : borderImage.z + 1

            Item {
                id: contentItem
                anchors.top: parent.top
                anchors.topMargin: 15 * _controller.dpiFactor
                anchors.left: parent.left
                anchors.leftMargin: 5 * _controller.dpiFactor
                anchors.right: parent.right
                anchors.bottom: parent.bottom
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
