import QtQuick 2.0

InputPopup {
    objectName: "newTagDialog"
    text: qsTr("Enter tag name")
    icon: "\uf02b"
    visible: _controller.newTagDialogVisible

    function buttonClicked(okClicked)
    {
        Qt.inputMethod.commit()
        if (okClicked) {
            _storage.createTag(enteredText)
            if (_controller.rightClickedTask !== null)
                _controller.rightClickedTask.addTag(enteredText)
        }

        _controller.newTagDialogVisible = false
        enteredText = ""
        Qt.inputMethod.hide()
    }

    onClicked: {
        buttonClicked(okClicked)
    }

    onAccepted: {
        buttonClicked(true)
    }

    onVisibleChanged: {
        if (visible) {
            enteredText = ""
        }
    }
}
