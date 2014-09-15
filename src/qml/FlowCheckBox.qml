import QtQuick 2.0
import com.kdab.flowpomodoro 1.0

Item {
    id: root
    property alias checked: checkboxPrivate.checked
    width: 22 * _controller.dpiFactor
    height: width
    signal toggled()

    CheckBoxPrivate {
        id: checkboxPrivate
    }

    Text {
        id: icon
        anchors.fill: parent
        font.pixelSize: 20 * _controller.dpiFactor
        font.family: "FontAwesome"
        anchors.verticalCenter: parent.verticalCenter
        text: root.checked ? "\uf046" : "\uf096"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            checkboxPrivate.toggle()
            root.toggled()
        }
    }
}


