import QtQuick 2.0
import QtGraphicalEffects 1.0

ListView {
    id: root
    orientation: Qt.Horizontal
    delegate: TagDelegate {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        isLastIndex: index === model.count - 1
        width: Math.max(Math.max(_style.tagTabWidth, root.width / root.model.count),
                        textItem.contentWidth + textItem2.contentWidth + textRow.spacing + 12 * _controller.dpiFactor)
    }
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

    Component.onCompleted: {
        if (_storage.tagsModel.count > 0) {
            _controller.setCurrentTabTag(model.at(0))
        }
    }
}
