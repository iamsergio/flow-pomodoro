import QtQuick 2.0

InputPopup {
    text: qsTr("Enter tag name")
    icon: "\uf02b"
    visible: _controller.newTagDialogVisible

    onClicked: {
        if (okClicked) {
            _storage.createTag(enteredText)
            if (_controller.rightClickedTask !== null)
                _controller.rightClickedTask.addTag(enteredText)
        }

        _controller.newTagDialogVisible = false
        enteredText = ""
    }
}
