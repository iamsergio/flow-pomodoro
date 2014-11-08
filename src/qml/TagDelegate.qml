import QtQuick 2.0

Rectangle {
    id: root
    property string text: ""
    property alias textItem: tagText
    property alias textItem2: countText
    property alias textRow: textRow
    property bool isLastIndex:  true
    property bool isSelected: false
    property int taskCount: 0
    property QtObject tagObj: null

    height: _style.tagTabHeight
    color: "black"

    Row {
        id: textRow
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: - Math.ceil(separator.width / 2.0)
        Text {
            id: tagText
            renderType: _controller.textRenderType
            text: root.text
            color: _style.tagTabTextColor
            font.bold: true
            font.pixelSize: _style.tagTabFontSize
        }
        Text {
            id: countText
            renderType: _controller.textRenderType
            text: root.taskCount > 0 ? " (" + root.taskCount + ")" : ""
            color: _style.tagTabTextColor
            anchors.verticalCenter: tagText.verticalCenter
            anchors.verticalCenterOffset: -1
            font.pixelSize: tagText.font.pixelSize - 1
        }
    }

    Rectangle {
        id: selection
        anchors.left: parent.left
        anchors.leftMargin: -3 + separator.width - 1
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: separator.visible ? -separator.anchors.leftMargin - 1 : 0
        height: _style.tagTabSelectionHeight
        visible: isSelected
        color: "#43ACE8"
    }

    Rectangle {
        id: separator
        anchors.left: parent.right
        anchors.leftMargin: -3
        anchors.verticalCenter: parent.verticalCenter
        width: _style.tagTabSeparatorWidth
        height: parent.height - 2*selection.height
        color: _style.tagTabSeparatorColor
        visible: !root.isLastIndex // No separator for last index
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (!root.isSelected)
                _controller.setCurrentTabTag(root.tagObj)
        }
    }
}
