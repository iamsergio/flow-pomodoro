import QtQuick 2.0



Text {
    id: root
    renderType: _controller.textRenderType
    property int size: 20
    property alias toolTip: tooltip.text
    signal clicked()
    font.pixelSize: ((mouseArea.pressed&&false ? 5 : 0 ) + size) * _controller.dpiFactor
    font.family: "FontAwesome"

    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    anchors.verticalCenter: parent.verticalCenter

    scale: mouseArea.pressed ? 1.3 : 1

    text: "\uf187"
    color: "white"
    ToolTip {
        id: tooltip
        anchors.fill: parent
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                root.clicked()
            }
        }
    }
}
