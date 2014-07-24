import QtQuick 2.1
import QtQuick.Controls 1.0

import Controller 1.0

ModalDialog {
    enabled: _controller.editMode === Controller.EditModeEditor
    dialogHeight: _style.taskEditorHeight

    content:
    Item {
        anchors.fill: parent

        Text {
            id: titleText
            font.pointSize: _style.taskEditorFontSize
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
            rowSpacing: 4
            horizontalItemAlignment: Grid.AlignRight
            columnSpacing: 4

            Text {
                text: qsTr("Summary:")
            }

            TaskTextField {
                id: textField
                objectName: "Task editor text field"
                width: descriptionTextArea.width
            }

            Text {
                text: qsTr("Description:")
            }

            TextArea {
                id: descriptionTextArea
                objectName: "Task editor text area"
                focus: false
                height: 100
                text: _controller.taskBeingEdited.description
                Binding {
                    // two way binding
                    target: _controller.taskBeingEdited
                    when: descriptionTextArea.visible && descriptionTextArea.activeFocus && _controller.editMode === Controller.EditModeEditor
                    property: "description"
                    value: descriptionTextArea.text
                }
            }

            Item {
                height: 16
                width: childrenRect.width
                Row {
                    spacing: 3
                    ClickableImage {
                        source: "qrc:/img/tag.png"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: {
                            tagsMenu.popup()
                        }

                        TagsMenu {
                            id: tagsMenu
                            task: _controller.taskBeingEdited
                            showConfigureItem: false
                        }
                    }

                    Text {
                        id: tagsLabel
                        text: qsTr("Tags:")
                    }
                }
            }

            Item {
                id: tagItem
                width: descriptionTextArea.width
                height: 70

                Flow {
                    id: flow
                    anchors.fill: parent
                    anchors.leftMargin: 3
                    Repeater {
                        model: _controller.taskBeingEdited.tagModel
                        Text {
                            property bool last: _controller.taskBeingEdited.tagModel.count === index + 1
                            text: tag.name + (last ? "" : ", ")
                            color: "black"
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
