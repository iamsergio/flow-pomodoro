import QtQuick 2.0

Rectangle {
    id: root
    property bool topLineVisible: true
    property bool checked: false

    color: (itemMouseArea.pressed && !checkable) ? "#E3E3E3" : "white"
    signal clicked()
    signal toggled(bool checkState, string itemText)

    Text {
        color: "black"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 15 * _controller.dpiFactor
        text: itemText
        font.pixelSize: 17 * _controller.dpiFactor
    }

    Rectangle {
        id: topLine
        height: Math.max(1, popupRect.border.width / 2 )
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        visible: topLineVisible
        color: "gray"
    }

    MouseArea {
        id: itemMouseArea
        hoverEnabled: true // Otherwise TaskListView will get the hover and show/hide buttons
        anchors.fill: parent
        onClicked: {
            if (checkable) {
                root.toggled(!checkState, itemText)
            } else {
                root.clicked()
            }
        }
    }

    FlowCheckBox {
        id: checkbox
        visible: checkable
        anchors.right: parent.right
        anchors.rightMargin: 10 * _controller.dpiFactor
        anchors.verticalCenter: parent.verticalCenter
        checked: root.checked
        onToggled: {
            root.toggled(!checkState, itemText)
        }
    }
}
