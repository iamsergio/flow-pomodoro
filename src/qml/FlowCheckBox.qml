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
        renderType: _controller.textRenderType
        anchors.fill: parent
        font.pixelSize: root.width
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


