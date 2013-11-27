import QtQuick 2.0

import Controller 1.0
import "DefaultStyle.js" as Style

Rectangle {
    id: root
    property int page: Controller.InvalidPage

    visible: _controller.currentPage === page && _controller.expanded

    color: Style.queueBackgroundColor
    radius: Style.queueRadius
    height: Style.pageHeight
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: Style.pageMargin
    anchors.rightMargin: Style.pageMargin
}
