import QtQuick 2.0
import com.kdab.flowpomodoro 1.0
import Controller 1.0

CircularProgressIndicator {
    id: root
    property alias containsMouse: mouseArea.containsMouse
    signal clicked()
    width: height
    foregroundColor: _style.circularIndicatorForegroundColor
    backgroundColor: mouseArea.containsMouse ? _style.circularIndicatorSelectedBackgroundColor
                                             : _style.circularIndicatorBackgroundColor
    dpiFactor: _controller.dpiFactor
    outterBorderWidth: _style.circularIndicatorBorderWidth
    visible: !_controller.currentTask.stopped
    value: _controller.currentTaskDuration - _controller.remainingMinutes
    minimumValue: 0
    maximumValue: _controller.currentTaskDuration
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked()
            _controller.stopPomodoro()
        }
    }
}
