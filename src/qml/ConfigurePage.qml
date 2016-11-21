import QtQuick 2.0

import Controller 1.0

Page {
    id: root
    page: Controller.ConfigurePage

    property list<QtObject> model: [
        TabBarItem {
            title: qsTr("General")
            source: "config/General.qml"
        },
        TabBarItem {
            title: qsTr("Tags")
            source: "config/Tags.qml"
        },
        TabBarItem {
            title: qsTr("Plugins")
            source: "config/Plugins.qml"
        }
    ]

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (_controller.tagEditStatus !== Controller.TagEditStatusNone)
                _controller.editTag("")

            // Don't collapse
            mouse.accepted = false
        }

        TabView {
            id: tabView
            model: root.model
            anchors {
                top: parent.top
                topMargin: _style.marginMedium
                left: parent.left
                right: parent.right
                margins: _style.marginSmall
                bottom: okButton.top
            }
        }

        PushButton {
            id: okButton
            objectName: "configurePageOKButton"
            text: qsTr("OK")
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: _style.marginMedium
            }

            onClicked: {
                _controller.currentPage = Controller.MainPage
            }
        }
    }
}
