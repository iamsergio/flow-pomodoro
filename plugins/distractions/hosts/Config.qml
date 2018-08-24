import QtQuick 2.0
import QtQuick.Controls 2.0

Item {
    id: root
    anchors.left: parent ? parent.left : undefined
    anchors.right: parent ? parent.right : undefined
    height: column.height + 5 * _controller.dpiFactor

    Column {
        id: column
        width: parent.width - 10
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 3 * _controller.dpiFactor

        Text {
            color: "black"
            text: qsTr("Enter one host per line:")
            x: textArea.x
            font.pixelSize: 12 * _controller.dpiFactor
        }

        TextArea {
            id: textArea
            height: 120 * _controller.dpiFactor
            anchors.left: parent.left
            anchors.right: parent.right
            font.pixelSize: 12 * _controller.dpiFactor
            text: _plugin.hosts
            Text {
                font.family: "FontAwesome"
                text: "\uf0c7"
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: 3 * _controller.dpiFactor
                anchors.bottomMargin: 5 * _controller.dpiFactor
                color: enabled ? "black" : "gray"
                font.pixelSize: 25 * _controller.dpiFactor
                scale: iconMouseArea.pressed ? 1.3 : 1
                enabled: _plugin.hosts !== textArea.text
                MouseArea {
                    id: iconMouseArea
                    anchors.fill: parent
                    onClicked: {
                        _plugin.hosts = textArea.text
                    }
                }
            }
        }
    }
}
