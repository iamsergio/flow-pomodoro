import QtQuick 2.0
import "DefaultStyle.js" as Style

Text {
    font.pointSize: Style.smallTextSize
    color: Style.smallTextColor
    anchors.leftMargin: 10
    anchors.left: parent.left
    wrapMode: Text.WordWrap
    width: parent.width
}
