import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    SmallText {
        id: smallText1
        anchors.topMargin: _style.marginBig
        anchors.top: parent.top
        text: qsTr("Plugins disable certain distractions when a task is running. Distractions are re-enabled when the task stops.") +
              "\n" + (_pluginModel.count > 0 ? qsTr("The following plugins were found:") : "")
    }

    SmallText {
        visible: _pluginModel.count === 0
        anchors.centerIn: parent
        text: qsTr("no plugins were found")
        horizontalAlignment: Text.AlignHCenter
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

            RegularText {
                height: row.height
                width: 150 * _controller.dpiFactor
                text: textRole
            }

            CheckBox {
                anchors.verticalCenterOffset: -2
                anchors.verticalCenter: parent.verticalCenter
                height: row.height
                checked: enabledRole
                onCheckedChanged: {
                    _pluginModel.setPluginEnabled(checked, index)
                }
            }
        }
    }
}
