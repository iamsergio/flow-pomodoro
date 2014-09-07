import QtQuick 2.0
import QtQuick 2.0

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    Column {
        anchors.topMargin: _style.marginSmall
        anchors.fill: parent
        spacing: 2 * _controller.dpiFactor
        PushButton {
            text: qsTr("Remove duplicate tasks")
            onClicked: {
                _storage.removeDuplicateData()
            }
        }
        PushButton {
            visible: _storage.webDAVSyncSupported
            text: qsTr("WebDAV Sync")
            onClicked: {
                _controller.webDavSync()
            }
        }
        PushButton {
            text: qsTr("Dump debug info")
            onClicked: {
                _storage.dumpDebugInfo()
            }
        }
    }
}
