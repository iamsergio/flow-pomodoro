import QtQuick 2.0

import Controller 1.0

Rectangle {
    id: root
    property int page: Controller.InvalidPage

    visible: _controller.currentPage === page && _controller.expanded

    color: _style.queueBackgroundColor
    radius: _style.queueRadius
    height: _style.pageHeight
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: _style.pageMargin
    anchors.rightMargin: _style.pageMargin
}
