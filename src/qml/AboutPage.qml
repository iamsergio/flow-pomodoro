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
            anchors.topMargin: 5
            height: 50
        }


        SmallText {
            anchors.top: titleText.bottom
            text: "<html>Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, <a href=\"mailto:info@kdab.com\">info@kdab.com</a><br><br>" +
                  qsTr("Author" + ": Sérgio Martins &lt;<a href=\"sergio.martins@kdab.com\" >sergio.martins@kdab.com</a>&gt;</html>")
            onLinkActivated: Qt.openUrlExternally(link)

        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                // Don't collapse
                //mouse.accepted = false
            }
        }
    }
}
