import QtQuick 2.0
import com.kdab.flowpomodoro 1.0
import Controller 1.0

CircularProgressIndicator {
    id: root
    objectName: "circularProgressIndicator"
    property alias containsMouse: mouseArea.containsMouse
    property alias mousePressed: mouseArea.pressed
    signal clicked()
    width: height
    foregroundColor: _style.circularIndicatorForegroundColor
    backgroundColor: _style.circularIndicatorBackgroundColor
    dpiFactor: _controller.dpiFactor
    outterBorderWidth: _style.circularIndicatorBorderWidth
    innerBorderWidth: 2 * _controller.dpiFactor
    innerSpacing: 3 * _controller.dpiFactor
    value: _controller.currentTaskDuration - _controller.remainingMinutes
    minimumValue: 0
    maximumValue: _controller.currentTaskDuration
    showStopIcon: containsMouse
    fontPixelSize: 11 * _controller.dpiFactor
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked()
            if (showStopIcon) {
                _controller.stopPomodoro()
            } else if (!_controller.showPomodoroOverlay) {
                _controller.showPomodoroOverlay = true;
            }
        }
    }
}
