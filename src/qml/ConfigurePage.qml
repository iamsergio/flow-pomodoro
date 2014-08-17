import QtQuick 2.0

import Controller 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

Page {
    id: root
    page: Controller.ConfigurePage

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (_controller.tagEditStatus !== Controller.TagEditStatusNone)
                _controller.editTag("")

            // Don't collapse
            mouse.accepted = false
        }

        TitleText {
            id: titleText
            text: qsTr("Configuration")
            anchors.top: parent.top
            anchors.topMargin: _style.marginSmall
        }

        Component {
            id: mobileTabBarComponent
            Item {
                anchors.fill: parent
                MobileTabView {
                    anchors.fill: parent
                    selectedIndex: 1
                    count: model.count
                    anchors.horizontalCenter: parent.horizontalCenter
                    model: ListModel { // TODO: ListElement doesn't accept qsTr
                        ListElement { text: "General" ; source: "config/General.qml"}
                        ListElement { text: "Tags"    ; source: "config/Tags.qml" }
                        ListElement { text: "Plugins" ; source: "config/Plugins.qml" }
                        // ListElement { text: "WebDAV"  ; source: "config/WebDavSync.qml";} // TODO: hide if not supported
                        // ListElement { text: "Hacking"  ; source: "config/Hacking.qml";}
                    }
                }
            }
        }

        Component {
            id: desktopTabBarComponent
            TabView {
                frameVisible: false
                currentIndex: _controller.configureTabIndex
                anchors.fill: parent

                Tab {
                    title: qsTr("General")
                    source: "config/General.qml"
                }

                Tab {
                    title: qsTr("Tags")
                    source: "config/Tags.qml"
                }

                Tab {
                    title: qsTr("Plugins")
                    source: "config/Plugins.qml"
                }
/*
                Tab {
                    title: qsTr("WebDAV")
                    source: "config/WebDavSync.qml"
                }

                Tab {
                    title: qsTr("Hacking")
                    source: "config/Hacking.qml"
                } */
            }
        }

        Item {
            id: tabView
            anchors.top: titleText.bottom
            anchors.topMargin: _style.marginMedium
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: _style.marginSmall
            anchors.bottom: okButton.top

            Loader {
                anchors.fill: parent
                sourceComponent: _controller.isMobile ? mobileTabBarComponent
                                                      : desktopTabBarComponent
            }
        }

        Button {
            id: okButton
            width : 100 * _controller.dpiFactor
            height: 50 * _controller.dpiFactor
            text: qsTr("OK")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.marginMedium

            onClicked: {
                _controller.currentPage = Controller.MainPage
            }
        }
    }
}
