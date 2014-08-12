import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    Button {
        text: qsTr("sync")
        onClicked: {
            _storage.webDavSync()
        }
    }
}
