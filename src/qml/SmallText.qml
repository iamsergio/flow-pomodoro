import QtQuick 2.0

Text {
    renderType: _controller.textRenderType
    font.pixelSize: _style.smallTextSize - 1
    color: _style.smallTextColor
    anchors.leftMargin: _style.marginMedium
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.rightMargin: _style.marginMedium
    wrapMode: Text.WordWrap
}
