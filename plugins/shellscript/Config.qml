import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.2

Text {
    id: root
    anchors.bottom: parent ? parent.bottom : undefined
    anchors.bottomMargin: -5 * _controller.dpiFactor
    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
    font.family: "FontAwesome"
    text: "\uf115"
    font.pixelSize: 25 * _controller.dpiFactor
    scale: iconMouseArea.pressed ? 1.3 : 1
    MouseArea {
        id: iconMouseArea
        anchors.fill: parent
        onClicked: {
            _plugin.editScript()
        }
    }
}
