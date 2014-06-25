import QtQuick 2.0

import Controller 1.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Page {
    id: root
    page: Controller.ConfigurePage

    TitleText {
        id: titleText
        text: qsTr("Configuration")
        anchors.top: parent.top
        anchors.topMargin: _style.marginSmall
        height: 50 * _controller.dpiFactor
    }

    TabView {
        z: 1
        anchors.top: titleText.bottom
        anchors.bottom: okButton.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: _style.marginSmall
        frameVisible: false
        Component.onCompleted: {
            addTab(qsTr("General"), Qt.createComponent("config/General.qml"))
            addTab(qsTr("Plugins"), Qt.createComponent("config/Plugins.qml"))
        }
    }

    Button {
        id: okButton
        z: 1
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
