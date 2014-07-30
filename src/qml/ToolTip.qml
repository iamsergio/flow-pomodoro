import QtQuick 2.0

MouseArea {
    id: root
    property int delay: 2000
    property string text: ""

    hoverEnabled: true
    onEntered: {
        timer.start()
    }

    onExited: {
        timer.stop()
        _toolTipController.hideToolTip()
    }

    Timer {
        id: timer
        interval: 2000
        repeat: false
        onTriggered: {
            _toolTipController.showToolTip(root, text)
        }
    }
}
