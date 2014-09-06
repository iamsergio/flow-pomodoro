import QtQuick 2.0
import QtQuick.Controls 1.0

Loader {
    id: root
    signal clicked()
    property string text: ""

    sourceComponent: _controller.isMobile ? mobileButton : desktopButton
    anchors.leftMargin: _controller.isMobile ? - 8 * _controller.dpiFactor : 0

    Component {
        id: desktopButton
        Button {
            height: 40 * _controller.dpiFactor
            text: root.text
            onClicked:
                root.clicked()
        }
    }

    Component {
        id: mobileButton
        MobileButton {
            text: root.text
            onClicked:
                root.clicked()
        }
    }
}
