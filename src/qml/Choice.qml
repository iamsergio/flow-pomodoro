import QtQuick 2.0

Rectangle {
    id: root
    property bool topLineVisible: true
    property bool checked: false
    property string fontAwesomeIconCode: ""

    color: (itemMouseArea.pressed && !checkableRole) ? "#E3E3E3" : "white"
    signal clicked()
    signal toggled(bool checkState, string itemText)

    Text {
        id: icon
        anchors.left: parent.left
        text: root.fontAwesomeIconCode
        font.pixelSize: 20 * _controller.dpiFactor
        font.family: "FontAwesome"
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 15 * _controller.dpiFactor
        visible: text !== ""
    }

    Text {
        color: "black"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: icon.visible ? icon.right : parent.left
        anchors.leftMargin: 15 * _controller.dpiFactor
        text: textRole
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
            if (checkableRole) {
                root.toggled(!checkedRole, textRole)
            } else {
                root.clicked()
            }
        }
    }

    FlowCheckBox {
        id: checkbox
        visible: checkableRole
        anchors.right: parent.right
        anchors.rightMargin: 5 * _controller.dpiFactor
        anchors.verticalCenter: parent.verticalCenter
        checked: root.checked
        onToggled: {
            root.toggled(!checkedRole, textRole)
        }
    }
}
