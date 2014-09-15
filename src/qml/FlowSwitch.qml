import QtQuick 2.0
import com.kdab.flowpomodoro 1.0

Rectangle {
    id: root
    color: "white"
    radius: 13 * _controller.dpiFactor
    height: 24 * _controller.dpiFactor
    width: 50 * _controller.dpiFactor
    property alias checked: checkboxPrivate.checked

    CheckBoxPrivate {
        id: checkboxPrivate
    }


    Rectangle {
        color: _style.menuBarBackgroundColor
        width: 18 * _controller.dpiFactor
        height: width
        radius: width
        anchors.verticalCenter: parent.verticalCenter
        x: root.checked ? parent.width - width - 2 * _controller.dpiFactor
                        : 2 * _controller.dpiFactor
        Behavior on x {
            NumberAnimation { duration: 200; easing.type: Easing.InOutExpo }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            checkboxPrivate.toggle()
        }
    }
}
