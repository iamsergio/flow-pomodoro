import QtQuick 2.0
import QtGraphicalEffects 1.0

ListView {
    id: root
    orientation: Qt.Horizontal
    delegate: tabDelegate
    height: _style.tagTabHeight
    clip: true

    LinearGradient {
        id: rightScrollIndicator
        anchors.fill: parent
        visible: !root.atXEnd
        start: Qt.point(root.width - _style.tagScrollIndicatorFadeWidth, 0)
        end: Qt.point(root.width, 0)
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: _style.queueBackgroundColor }
        }
    }

    LinearGradient {
        id: leftScrollIndicator
        visible: !root.atXBeginning
        anchors.fill: parent
        start: Qt.point(0, 0)
        end: Qt.point(_style.tagScrollIndicatorFadeWidth, 0)
        gradient: Gradient {
            GradientStop { position: 0.0; color: _style.queueBackgroundColor }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }

    Component {
        id: tabDelegate
        Rectangle {
            id: background
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: Math.max(Math.max(_style.tagTabWidth, root.width / root.model.count),
                            tagText.contentWidth + countText.contentWidth + textRow.spacing + 12 * _controller.dpiFactor)
            color: _style.tagBackground

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
                    font.pixelSize: _style.tagTabFontSize
                }
                Text {
                    id: countText
                    text: tag.taskModel.count > 0 ? " (" + tag.taskModel.count + ")" : ""
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

    Component.onCompleted: {
        if (_storage.tagsModel.count > 0) {
            _controller.setCurrentTabTag(model.at(0))
        }
    }
}
