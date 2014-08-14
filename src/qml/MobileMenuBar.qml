import QtQuick 2.1


Rectangle {
    id: root
    property string titleText: ""
    property string iconSource: ""
    signal buttonClicked()

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    height: _style.menuBarHeight
    color: _style.menuBarBackgroundColor

    Image {
        id: image
        height: parent.height - 2 * _style.menuBarIconMargin
        visible: source !== ""
        fillMode: Image.PreserveAspectFit
        anchors.leftMargin: _style.menuBarIconMargin
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        source: root.iconSource
        smooth: true
    }

    Text {
        color: _style.menuBarFontColor
        text: root.titleText
        anchors.leftMargin: _style.menuBarIconMargin
        anchors.left: image.right
        anchors.verticalCenter: image.verticalCenter
        font.pixelSize: _style.menuBarFontSize
    }

    Rectangle {
        id: button
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: _style.menuBarButtonWidth
        color: mouseArea.containsMouse ? _style.menuBarButtonPressedColor : _style.menuBarBackgroundColor

        Column {
            id: column
            readonly property int numDots: 3
            height: _style.menuBarButtonDotWidth * numDots + spacing * (numDots - 1)
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: _style.menuBarButtonDotSpacing
            Repeater {
                model: column.numDots
                Rectangle {
                    color: _style.menuBarDotColor
                    anchors.horizontalCenter: column.horizontalCenter
                    width: _style.menuBarButtonDotWidth
                    height: width
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                root.buttonClicked()
            }
        }
    }
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1 * _controller.dpiFactor
        anchors.top: parent.top
        color: "#393939"
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height: 2 * _controller.dpiFactor
        anchors.bottom: parent.bottom
        color: "#1B1B1B"
    }
}
