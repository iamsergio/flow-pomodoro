import QtQuick 2.0
import Controller 1.0

Popup {
    id: root
    visible: _controller.popupVisible
    icon: _controller.popupMessageType === Controller.PopupMessageType_Question ? "\uf059"
                                                                                : ((_controller.popupMessageType === Controller.PopupMessageType_Warning) ? "\uf071"
                                                                                                                                                          : "\uf05a")
    showCancelButton: _controller.popupMessageType === Controller.PopupMessageType_Question

    contentItem:
    Item {
        anchors.fill: parent
        Text {
            renderType: _controller.textRenderType
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium
            anchors.right: parent.right
            anchors.rightMargin: _style.marginMedium
            anchors.topMargin: _style.marginMedium
            anchors.top: parent.top
            font.pixelSize: _style.dialogFontSize
            wrapMode: Text.WordWrap
            text: _controller.popupText
            horizontalAlignment: Text.AlignHCenter
        }
    }

    onClicked: {
        _controller.onPopupButtonClicked(okClicked)
    }
}
