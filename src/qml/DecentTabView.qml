import QtQuick 2.0

ListView {
    id: root
    orientation: Qt.Horizontal
    delegate: tabDelegate
    height: 30
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
                anchors.centerIn: parent
                Text {
                    id: tagText
                    text: tag.name
                    color: "white"
                    font.bold: true
                }
                Text {
                    text: tag.taskModel.count > 0 ? " (" + tag.taskModel.count + ")" : ""
                    color:"white"
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
                height: 5
                visible: isSelected
                color: "#43ACE8"
            }

            Rectangle {
                id: separator
                anchors.left: parent.right
                anchors.leftMargin: -3
                anchors.verticalCenter: parent.verticalCenter
                width: 1
                height: parent.height - 2*selection.height
                color: "gray" // TODO style
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
