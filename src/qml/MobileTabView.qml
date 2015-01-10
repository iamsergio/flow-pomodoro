import QtQuick 2.0

Item {
    id: root
    property int count: model.count
    property variant model: null
    property int selectedIndex: -1

    function loadTab()
    {
        if (selectedIndex >= 0 && selectedIndex < count && model != null)
            contentAreaLoader.source = model.get(selectedIndex).source
    }

    onModelChanged: {
        loadTab()
    }

    onSelectedIndexChanged: {
        loadTab()
    }

    Rectangle {
        id: tabBar
        color: _style.queueBackgroundColor
        radius: _style.configTabBarBorderRadius
        border.color: _style.configTabBarBorderColor
        border.width: _style.configTabBarBorderWidth
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        height: _style.configTabBarHeight
        width: _style.configTabBarWidth

        gradient: Gradient {
            GradientStop { position: 0.0; color: _style.configTabBarDarkerGradientColor }
            GradientStop { position: 0.5; color: _style.configTabBarLightGradientColor }
            GradientStop { position: 1.0; color: _style.configTabBarDarkerGradientColor }
        }

        Rectangle {
            anchors.fill: parent;
            color: "transparent";
            radius: tabBar.radius
            border.color: tabBar.border.color
            border.width: _style.configTabBarBorderWidth
            z: 2
        }

        Row {
            id: row
            anchors.fill: parent
            spacing: 0
            Repeater {
                model: root.model
                Item {
                    anchors.top: row.top
                    anchors.bottom: row.bottom
                    width: row.width / root.count
                    Rectangle {
                        id: verticalSeparator
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: _style.configTabBarSeparatorWidth
                        color: tabBar.border.color
                        visible: index != root.count - 1
                    }
                    Text {
                        anchors.fill: parent
                        renderType: _controller.textRenderType
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: model.text
                        font.pixelSize: root.selectedIndex === index ? _style.configTabBarSelectedFontSize
                                                                     : _style.configTabBarFontSize
                        font.bold: root.selectedIndex === index
                        color: root.selectedIndex === index ? _style.configTabBarSelectedFontColor
                                                            :  _style.configTabBarFontColor
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
                            root.selectedIndex = index
                        }
                    }
                }
            }
        }
    }

    Loader {
        id: contentAreaLoader
        anchors.topMargin: _style.marginSmall
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
