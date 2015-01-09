import QtQuick 2.0

import Controller 1.0

Page {
    id: root
    page: Controller.AboutPage
    MouseArea {
        anchors.fill: parent
        onClicked: {
            // Don't collapse
            mouse.accepted = false
        }

        Item {
            z: 1
            anchors.fill: parent

            SmallText {
                id: versionText
                anchors.top: parent.top
                anchors.topMargin: _style.marginMedium
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignRight
                text: "Flow " + _controller.version + (_controller.gitDate ? " (" + _controller.gitDate + ")" : "") + "; Qt-" + _controller.qtVersion
            }

            Column {
                id: mainColumn
                spacing: 5 * _controller.dpiFactor
                anchors.top: versionText.bottom
                anchors.topMargin: 10 * _controller.dpiFactor
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 10 * _controller.dpiFactor
                anchors.rightMargin: 10 * _controller.dpiFactor
                anchors.bottom: pushButton.top
                anchors.bottomMargin: 5 * _controller.dpiFactor

                Rectangle {
                    id: copyrectRect
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: copyrightText.height + 5 * _controller.dpiFactor

                    SmallText {
                        id: copyrightText
                        anchors.verticalCenter: parent.verticalCenter
                        text: "<html>Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, <a href=\"mailto:info@kdab.com\">info@kdab.com</a><br>" +
                              "Copyright (C) 2014 Sérgio Martins &lt;<a href=\"mailto:iamsergio@gmail.com\">iamsergio@gmail.com</a>&gt;"
                        onLinkActivated: {
                            Qt.openUrlExternally(link)
                        }
                    }

                    radius: 5 * _controller.dpiFactor
                    color: Qt.lighter(_style.queueBackgroundColor, 1.3)
                }

                Rectangle {
                    id: bindingsRect
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: childrenRect.height + 5
                    visible: !_controller.isMobile
                    Text {
                        id: keysText
                        text: qsTr("Keyboard bindings") + ":"

                        anchors.left: parent.left
                        anchors.leftMargin: _style.marginMedium
                        font.bold: true
                        renderType: _controller.textRenderType
                        font.pixelSize: _style.regularTextSize
                        color: _style.regularTextColor
                    }

                    Grid {
                        id: keyGrid
                        anchors.top: keysText.bottom
                        anchors.topMargin: _style.marginSmall
                        anchors.left: parent.left
                        anchors.leftMargin: _style.marginMedium
                        columns: 2
                        columnSpacing: _style.marginMedium

                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: "<space>"
                        }

                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            text: qsTr("Pauses/Resumes an ongoing pomodoro")
                            renderType: _controller.textRenderType
                        }

                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: "S"
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            text: qsTr("Stops an ongoing pomodoro")
                            renderType: _controller.textRenderType
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: "T"
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            text: qsTr("Toggles today/later lists")
                            renderType: _controller.textRenderType
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: "C"
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            text: qsTr("Opens configuration dialog")
                            renderType: _controller.textRenderType
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: "Up/Down"
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: qsTr("Select tasks")
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: "Del"
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            text: qsTr("Deletes a selected task or an ongoing one")
                            renderType: _controller.textRenderType
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: "Enter"
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            text: qsTr("Starts a selected task, or expands the window")
                            renderType: _controller.textRenderType
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: "Esc"
                        }
                        Text  {
                            font.pixelSize: _style.smallTextSize
                            color: _style.smallTextColor
                            renderType: _controller.textRenderType
                            text: qsTr("Collapses the window")
                        }
                    }
                    color: Qt.lighter(_style.queueBackgroundColor, 1.3)
                    radius: 5
                }

                Rectangle {
                    id: githubRect
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: column.height
                    Column {
                        id: column
                        spacing: 7 * _controller.dpiFactor
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        height: childrenRect.height + 10
                        Text {
                            id: optionsText
                            renderType: _controller.textRenderType
                            font.pixelSize: _style.smallTextSize
                            anchors.left: parent.left
                            anchors.leftMargin: _style.marginMedium
                            color: _style.smallTextColor
                            text: _controller.buildOptionsText
                        }

                        Text {
                            id: dataPathText
                            renderType: _controller.textRenderType
                            font.pixelSize: _style.smallTextSize
                            anchors.left: parent.left
                            anchors.leftMargin: _style.marginMedium
                            anchors.right: parent.right
                            anchors.rightMargin: _style.marginMedium
                            color: _style.smallTextColor
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            text: qsTr("Data file") + ": " + _storage.dataFile();
                        }

                        SmallText {
                            id: githubText
                            text: "Source: <html><a href=\"https://github.com/iamsergio/flow-pomodoro\">https://github.com/iamsergio/flow-pomodoro</a>"
                            onLinkActivated: {
                                Qt.openUrlExternally(link)
                            }
                        }
                        SmallText {
                            id: licenseText
                            text: "License: <html><a href=\"https://github.com/iamsergio/flow-pomodoro/blob/master/License.txt\">https://github.com/iamsergio/flow-pomodoro/blob/master/License.txt</a>"
                            onLinkActivated: {
                                Qt.openUrlExternally(link)
                            }
                        }
                    }
                    color: Qt.lighter(_style.queueBackgroundColor, 1.3)
                    radius: 5
                }
                SmallText {
                    id: countText
                    anchors.left: parent.left
                    text: qsTr("Task count: %1").arg(_storage.taskCount)
                }
            }

            PushButton {
                z: 2
                id: pushButton
                text: qsTr("OK")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: _style.marginSmall

                onClicked: {
                    _controller.currentPage = Controller.MainPage
                }
            }
        }
    }
}
