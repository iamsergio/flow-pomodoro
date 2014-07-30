import QtQuick 2.0

ListView {
    id: root
    orientation: Qt.Horizontal
    delegate: tabDelegate
    height: _style.tagTabHeight
    clip: true

    Component {
        id: tabDelegate
        Rectangle {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: Math.max(65, root.width / root.model.count)
            color: "black"
            property bool isSelected: tag == _controller.currentTabTag
            Row {
                id: textRow
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: - Math.ceil(separator.width / 2.0)
                Text {
                    id: tagText
                    text: tag.name
                    color: _style.tagTabTextColor
                    font.bold: true
                    font.pointSize: _style.tagTabFontSize
                }
                Text {
                    text: tag.taskModel.count > 0 ? " (" + tag.taskModel.count + ")" : ""
                    color: _style.tagTabTextColor
                    anchors.verticalCenter: tagText.verticalCenter
                    anchors.verticalCenterOffset: -1
                    font.pointSize: tagText.font.pointSize - 1
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
                visible: index != root.model.count - 1 // No separator for last index
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    _controller.setCurrentTabTag(isSelected ? null : tag)
                }
            }
        }
    }
}
