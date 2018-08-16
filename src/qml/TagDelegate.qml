import QtQuick 2.0

Rectangle {
    id: root
    property alias textItem: tagText
    property alias textItem2: countText
    property alias textRow: textRow
    property bool isLastIndex:  true
    property bool isSelected: false
    property QtObject tagObj: null
    property bool hasTasks: tagObj && root.tagObj.taskModel.count > 0
    property bool isHotTag: tagObj && tagObj.name === "Hot"

    height: _style.tagTabHeight
    color: "black"

    Row {
        id: textRow
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: - Math.ceil(separator.width / 2.0)
        Text {
            id: tagText
            renderType: _controller.textRenderType
            text: root.tagObj ? root.tagObj.name : ""
            color: (root.hasTasks && root.isHotTag) ? _style.hotColor : _style.tagTabTextColor
            font.bold: true
            font.pixelSize: _style.tagTabFontSize
        }
        Text {
            id: countText
            renderType: _controller.textRenderType
            text: root.tagObj ? (root.tagObj.taskModel.count > 0 ? " (" + root.tagObj.taskModel.count + ")" : "")
                              : ""
            color: (root.hasTasks && root.isHotTag) ? _style.hotColor : _style.tagTabTextColor
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
        color: root.isHotTag ? _style.hotColor : "#43ACE8"
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
            if (!root.isSelected) {
                _controller.setCurrentTag(root.tagObj)
            }
        }
    }
}
