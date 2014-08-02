import QtQuick 2.0

Text {
    font.pixelSize: _style.smallTextSize
    color: _style.smallTextColor
    anchors.leftMargin: _style.marginMedium
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.rightMargin: _style.marginMedium
    wrapMode: Text.WordWrap
    width: parent.width
}
