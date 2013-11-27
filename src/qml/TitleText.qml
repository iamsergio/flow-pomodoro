import QtQuick 2.0
import "DefaultStyle.js" as Style

Text {
    font.bold: true
    font.pointSize: Style.titleSize
    color: Style.titleColor
    anchors.leftMargin: 10
    anchors.left: parent.left
}
