import QtQuick 2.1
import QtQuick.Controls 1.0

import Controller 1.0

ModalDialog {
    id: root
    enabled: _controller.editMode === Controller.EditModeEditor
    dialogHeight: _style.taskEditorHeight

    content:
    Item {
        anchors.fill: parent

        Text {
            id: titleText
            font.pixelSize: _style.taskEditorFontSize
            color: _style.regularTextColor
            anchors.top: parent.top
            anchors.topMargin: _style.marginMedium
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Edit Task")
        }

        Grid {
            anchors.top: titleText.bottom
            anchors.topMargin: _style.marginMedium

            anchors.horizontalCenter: parent.horizontalCenter

            columns: 2
            rows: 3
            rowSpacing: 4 * _controller.dpiFactor
            horizontalItemAlignment: Grid.AlignRight
            columnSpacing: 4 * _controller.dpiFactor

            Text {
                text: qsTr("Summary:")
                color: _style.regularTextColor
            }

            TaskTextField {
                id: textField
                objectName: "Task editor text field"
                width: descriptionTextArea.width
            }

            Text {
                text: qsTr("Description:")
                color: _style.regularTextColor
            }

            TextArea {
                id: descriptionTextArea
                objectName: "Task editor text area"
                focus: false
                height: 100 * _controller.dpiFactor
                text: _controller.taskBeingEdited.description
                width: root.width / 2.0
                Binding {
                    // two way binding
                    target: _controller.taskBeingEdited
                    when: descriptionTextArea.visible && descriptionTextArea.activeFocus && _controller.editMode === Controller.EditModeEditor
                    property: "description"
                    value: descriptionTextArea.text
                }
            }

            Item {
                height: 16 * _controller.dpiFactor
                width: childrenRect.width
                Row {
                    spacing: 3 * _controller.dpiFactor
                    ClickableImage {
                        source: "image://icons/tag.png"
                        anchors.verticalCenter: parent.verticalCenter
                        toolTip: qsTr("Configure tags")
                        onClicked: {
                            tagsMenu.popup()
                        }

                        TaskContextMenu {
                            id: tagsMenu
                            task: _controller.taskBeingEdited
                            showConfigureItem: false
                        }
                    }

                    Text {
                        id: tagsLabel
                        text: qsTr("Tags:")
                        color: _style.regularTextColor
                    }
                }
            }

            Item {
                id: tagItem
                width: descriptionTextArea.width
                height: 70 * _controller.dpiFactor

                Flow {
                    id: flow
                    anchors.fill: parent
                    anchors.leftMargin: 3 * _controller.dpiFactor
                    Repeater {
                        model: _controller.taskBeingEdited.tagModel
                        Text {
                            property bool last: _controller.taskBeingEdited.tagModel.count === index + 1
                            text: tag.name + (last ? "" : ", ")
                            color: _style.regularTextColor
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }

        Button {
            id: buttonCancel
            text: qsTr("OK")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.dialogButtonsMargin
            onClicked: {
                _controller.editTask(null, Controller.EditModeNone)
            }
        }
    }
}
