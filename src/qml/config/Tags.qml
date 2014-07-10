import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    SmallText {
        id: noTagsText
        anchors.centerIn: parent
        text: qsTr("No tags")
        visible: _tagStorage.model.count === 0
        horizontalAlignment: Text.AlignHCenter
    }


    Flow {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin: 40 * _controller.dpiFactor
        anchors.topMargin: _style.marginBig
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 5
        Repeater {
            model: _tagStorage.model
            delegate: Row {
                id: row
                height: 30 * _controller.dpiFactor
                spacing: 30 * _controller.dpiFactor

                Rectangle {
                    height: label.contentHeight
                    width: label.contentWidth + 30
                    border.width: 1
                    border.color: "black"
                    color: "transparent"
                    radius: 10
                    RegularText {
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 4
                        anchors.verticalCenterOffset: 2
                        anchors.verticalCenter: parent.verticalCenter
                        height: row.height
                        width: 150 * _controller.dpiFactor
                        text: tag.name + "; " + tag.taskCount

                    }

                    ClickableImage {
                        id: deleteImage
                        source: "qrc:/img/delete.png"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: _style.buttonsSpacing
                        visible: true

                        function reallyRemove()
                        {
                            _tagStorage.removeTag(tag.name);
                        }

                        onClicked: {
                            if (tag.taskCount > 0 || true) {
                                _controller.showQuestionPopup(this, qsTr("There are tasks using this tag. Are you sure you want to delete it?"), "reallyRemove()")
                            } else {
                                _tagStorage.removeTag(tag.name)
                            }
                        }
                    }
                }
            }
        }
    }

    ClickableImage {
        // Just for testing purposes
        id: playImage
        source: "qrc:/img/add.png"
        anchors.bottom: parent.bottomight
        visible: true
        onClicked: {
            _tagStorage.createTag("Test" + _tagStorage.model.count)
        }
    }
}
