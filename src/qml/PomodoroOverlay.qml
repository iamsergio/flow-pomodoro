import QtQuick 2.0

Rectangle {
    id: root
    color: "black"
    anchors.fill: parent
    visible: _controller.showPomodoroOverlay
    opacity: visible ? 1 : 0
    MouseArea {
        anchors.fill: parent
        FlowCircularProgressIndicator {
            id: circularIndicator
            anchors.centerIn: parent
            width: root.width * 0.40
            height: width
            outterBorderWidth: width * 0.1
            innerBorderWidth:  outterBorderWidth / 2
            innerSpacing: 2 * _controller.dpiFactor + outterBorderWidth
            fontPixelSize: (width - outterBorderWidth*2) * 0.4
        }

        onClicked: {
            _controller.showPomodoroOverlay = false
        }

        Text {
            color: "white"
            font.pixelSize: circularIndicator.fontPixelSize / 3
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10 * _controller.dpiFactor
            anchors.rightMargin: 10 * _controller.dpiFactor
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            anchors.bottom: circularIndicator.top
            anchors.bottomMargin: 20 * _controller.dpiFactor
            text: _controller.currentTask !== null ? _controller.currentTask.summary : ""
        }
    }

    Behavior on opacity {
        NumberAnimation { duration: 2000 }
    }
}
