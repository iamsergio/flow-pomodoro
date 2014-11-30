import QtQuick 2.0
import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    Text {
        id: smallText1
        anchors.topMargin: _style.marginBig
        anchors.leftMargin: _style.marginMedium
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        wrapMode: Text.WordWrap
        font.pixelSize: 13 * _controller.dpiFactor
        text: qsTr("Plugins disable certain distractions when a task is running. Distractions are re-enabled when the task stops.") +
              "\n" + (_pluginModel.count > 0 ? qsTr("The following plugins were found:") : "")
    }

    Text {
        visible: _pluginModel.count === 0
        anchors.centerIn: parent
        text: qsTr("no plugins were found")
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 13 * _controller.dpiFactor
    }

    ListView {
        anchors.top: smallText1.bottom
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin: 40 * _controller.dpiFactor
        anchors.topMargin: 25 * _controller.dpiFactor
        anchors.left: parent.left
        anchors.right: parent.right
        model: _pluginModel
        height: 60
        delegate: Row {
            id: row
            height: 30 * _controller.dpiFactor
            spacing: 30 * _controller.dpiFactor

            Text {
                height: row.height
                width: 150 * _controller.dpiFactor
                text: textRole
                renderType: _controller.textRenderType
                font.pixelSize: _style.regularTextSize
                color: _style.regularTextColor
            }

            FlowCheckBox {
                anchors.verticalCenterOffset: -2
                anchors.verticalCenter: parent.verticalCenter
                checked: enabledRole
                onCheckedChanged: {
                    _pluginModel.setPluginEnabled(checked, index)
                }
            }
        }
    }
}
