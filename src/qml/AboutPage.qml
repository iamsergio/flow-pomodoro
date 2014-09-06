import QtQuick 2.0

import Controller 1.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Page {
    id: root
    page: Controller.AboutPage
    readonly property bool showKeyboardBindings: !_controller.isMobile

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
            text: qsTr("Flow version") + ": " + _controller.version
        }

        SmallText {
            id: copyrightText
            anchors.top: versionText.bottom
            anchors.topMargin: _style.marginMedium * 2
            text: "<html>Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, <a href=\"mailto:info@kdab.com\">info@kdab.com</a><br>" +
                  qsTr("Author") + ": Sérgio Martins &lt;<a href=\"sergio.martins@kdab.com\" >sergio.martins@kdab.com</a>&gt;<br>" +
                  "Copyright (C) 2014 Sérgio Martins &lt;<a href=\"mailto:iamsergio@gmail.com\">iamsergio@gmail.com</a>&gt;<br><br>"
            onLinkActivated: Qt.openUrlExternally(link)
        }

        RegularText {
            id: keysText
            text: qsTr("Keyboard bindings") + ":"
            anchors.top: copyrightText.bottom
            anchors.topMargin: _style.marginMedium
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium
            font.bold: true
            visible: root.showKeyboardBindings
        }

        GridLayout {
            id: keyGrid
            visible: root.showKeyboardBindings
            anchors.top: keysText.bottom
            anchors.topMargin: _style.marginSmall
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium
            columns: 2
            columnSpacing: _style.marginMedium

            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "<space>"
            }

            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Pauses/Resumes an ongoing pomodoro")
            }

            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "S"
            }
            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Stops an ongoing pomodoro")
            }

            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "Up/Down"
            }
            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Select tasks")
            }
            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "Del"
            }
            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Deletes a selected task or an ongoing one")
            }
            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "Enter"
            }
            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Starts a selected task, or expands the window")
            }
            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "Esc"
            }
            Text  {
                font.pixelSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Collapses the window")
            }
        }

        Text {
            anchors.top: keyGrid.bottom
            font.pixelSize: _style.smallTextSize
            anchors.left: parent.left
            anchors.topMargin: _style.marginMedium
            anchors.leftMargin: _style.marginMedium
            color: _style.smallTextColor
            text: "Build options: " + (_controller.isMobile ? "mobile, " : "desktop, ")
                                    + (_storage.webDAVSyncSupported ? "webdav, " : "no-webdav, ")
                                    + (_controller.openSSLSupported ? "openssl" : "no-openssl")
                                    + (_controller.hackingMenuSupported ? ", hacking" : "")
        }

        PushButton {
            z: 2
            text: qsTr("OK")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.marginSmall

            onClicked: {
                _controller.currentPage = Controller.MainPage
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                // Don't collapse
                mouse.accepted = false
            }
        }
    }
}
