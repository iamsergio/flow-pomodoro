import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    GridLayout {
        id: grid1
        columns: 2

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: _style.marginBig
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin: 40 * _controller.dpiFactor
        columnSpacing: _style.marginBig

        Text {
            text: qsTr("Pomodoro duration")
        }

        SpinBox {
            id: spinBox
            minimumValue: 0
            maximumValue: 59
            value: _controller.defaultPomodoroDuration
            onValueChanged: {
                // The binding gets broken when user edits, not sure how to fix it.. so hack it:
                _controller.defaultPomodoroDuration = value
            }
        }
    }
}
