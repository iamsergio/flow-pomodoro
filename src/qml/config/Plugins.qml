import QtQuick 2.0
import QtQuick.Controls 2.0
import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    Text {
        id: smallText1
        anchors.topMargin: _style.marginMedium
        anchors.leftMargin: _style.marginMedium
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        wrapMode: Text.WordWrap
        font.pixelSize: 13 * _controller.dpiFactor
        text: qsTr("Plugins disable certain distractions when a task is running. Distractions are re-enabled when the task stops.") +
              "\n" + (_pluginModel.count > 0 ? qsTr("The following plugins were found:") : "")
    }

    Text {
        visible: _pluginModel.count === 0
        anchors.centerIn: parent
        text: qsTr("no plugins were found")
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 13 * _controller.dpiFactor
    }

    ListView {
        id: listView
        anchors.top: smallText1.bottom
        anchors.rightMargin: _style.marginMedium
        anchors.leftMargin: _style.marginMedium
        anchors.topMargin: 15 * _controller.dpiFactor
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        boundsBehavior: Flickable.StopAtBounds

        model: _pluginModel
        height: 60
        spacing: 5 * _controller.dpiFactor
        clip: true
        delegate: Item {
            width: parent.width
            height: ((icon.expanded && helpTextRole) ? (25 + helpTextRect.height) : 25) * _controller.dpiFactor

            Row {
                id: row
                width: parent.width
                height: 25 * _controller.dpiFactor
                spacing: 60 * _controller.dpiFactor
                Item {
                    id: containerItem
                    width: 150 * _controller.dpiFactor
                    height: childrenRect.height

                    FontAwesomeIcon {
                        id: icon
                        anchors.top: titleText.top
                        anchors.topMargin: 2 * _controller.dpiFactor
                        property bool expanded: false
                        size: 15
                        text: expanded ? "\uf146" : "\uf0fe"
                        color: _style.regularTextColor
                        anchors.verticalCenter: undefined

                        onClicked: {
                            expanded = !expanded
                        }
                    }

                    Text {
                        id: titleText
                        text: textRole
                        anchors.left: icon.visible ? icon.right : parent.left
                        anchors.leftMargin: 5 * _controller.dpiFactor
                        renderType: _controller.textRenderType
                        font.pixelSize: _style.regularTextSize
                        color: _style.regularTextColor
                    }
                }

                FlowCheckBox {
                    anchors.verticalCenter: undefined
                    anchors.top: containerItem.top
                    anchors.topMargin: 2 * _controller.dpiFactor
                    checked: enabledRole
                    onCheckedChanged: {
                        _pluginModel.setPluginEnabled(checked, index)
                    }
                }
            }

            Rectangle {
                id: helpTextRect
                color: Qt.lighter(_style.queueBackgroundColor, 1.3)
                radius: 5
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: row.bottom
                height: helpText.height + (helpText.anchors.topMargin * 4 * _controller.dpiFactor) + (errorText.visible ? errorText.height : 0) + configItemContainer.height
                visible: icon.expanded

                Text {
                    id: helpText
                    text: helpTextRole
                    renderType: _controller.textRenderType
                    font.pixelSize: 12 * _controller.dpiFactor
                    wrapMode: Text.WordWrap
                    color: _style.regularTextColor
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 5 * _controller.dpiFactor
                    anchors.top: parent.top
                    anchors.topMargin: 5 * _controller.dpiFactor
                    textFormat: Text.RichText
                }

                Item {
                    id: configItemContainer
                    height: childrenRect.height
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: helpText.bottom
                    anchors.topMargin: 5 * _controller.dpiFactor
                    Component.onCompleted: {
                        if (configItem) {
                            configItem.parent = configItemContainer
                        }
                    }
                }

                Text {
                    id: errorText
                    color: "red"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 5 * _controller.dpiFactor
                    anchors.top: configItemContainer.bottom
                    anchors.topMargin: 5 * _controller.dpiFactor
                    font.pixelSize: 12 * _controller.dpiFactor
                    wrapMode: Text.WordWrap
                    textFormat: Text.RichText
                    visible: objectRole.lastError
                    text: objectRole.lastError ? "<b>" + qsTr("Error") + ": </b>" + objectRole.lastError : ""
                }
            }
        }
    }

    FlowScrollBar {
        anchors.left: listView.right
        anchors.top: listView.top
        anchors.bottom: listView.bottom
        flickable: listView
    }
}
