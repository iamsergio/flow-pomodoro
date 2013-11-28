import QtQuick 2.0

Text {
    font.pointSize: _style.smallTextSize
    color: _style.smallTextColor
    anchors.leftMargin: 10
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.rightMargin: 10
    wrapMode: Text.WordWrap
    width: parent.width
}
