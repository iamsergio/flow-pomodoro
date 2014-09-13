import QtQuick 2.0
import QtQuick.Controls 1.0
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

    Image {
        visible: root.checked
        source: "image://icons/checkbox.png"
        anchors.fill: parent
    }

    Image {
        visible: !root.checked
        source: "image://icons/uncheckbox.png"
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            checkboxPrivate.toggle()
            root.toggled()
        }
    }
}


