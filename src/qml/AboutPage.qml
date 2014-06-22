import QtQuick 2.0

import Controller 1.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Page {
    id: root
    page: Controller.AboutPage

    Item {
        z: 1
        anchors.fill: parent

        TitleText {
            id: titleText
            text: qsTr("About")
            anchors.top: parent.top
            anchors.topMargin: _style.marginSmall
            height: 50 * _controller.dpiFactor
        }


        SmallText {
            id: copyrightText
            anchors.top: titleText.bottom
            text: "<html>Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, <a href=\"mailto:info@kdab.com\">info@kdab.com</a><br><br>" +
                  qsTr("Author" + ": Sérgio Martins &lt;<a href=\"sergio.martins@kdab.com\" >sergio.martins@kdab.com</a>&gt;</html>")
            onLinkActivated: Qt.openUrlExternally(link)
        }

        RegularText {
            id: keysText
            text: qsTr("Keyboard bindings") + ":"
            anchors.top: copyrightText.bottom
            anchors.topMargin: 60 * _controller.dpiFactor
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium
            font.bold: true
        }

        GridLayout {
            anchors.top: keysText.bottom
            anchors.topMargin: _style.marginSmall
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium
            columns: 2
            columnSpacing: _style.marginMedium

            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "<space>"
            }

            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Pauses/Resumes an ongoing pomodoro")
            }

            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "S"
            }
            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Stops an ongoing pomodoro")
            }

            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "Up/Down"
            }
            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Select tasks")
            }
            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "Del"
            }
            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Deletes a selected task or an ongoing one")
            }
            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "Enter"
            }
            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Starts a selected task, or expands the window")
            }
            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: "Esc"
            }
            Text  {
                font.pointSize: _style.smallTextSize
                color: _style.smallTextColor
                text: qsTr("Collapses the window")
            }
        }

        Button {
            z: 2
            width : 100 * _controller.dpiFactor
            height: 50 * _controller.dpiFactor
            text: qsTr("OK")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.marginSmall

            onClicked: {
                _controller.currentPage = Controller.TheQueuePage
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
