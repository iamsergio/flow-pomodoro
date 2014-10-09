import QtQuick 2.1
import com.kdab.flowpomodoro 1.0
import Controller 1.0

Rectangle {
    id: root
    property string titleText: ""
    property string iconSource: ""
    signal buttonClicked()

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    height: _style.menuBarHeight
    color: _style.menuBarBackgroundColor

    FontAwesomeIcon {
        id: addIcon
        text: "\uf055" // "\uf0fe"
        size: 35
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: _style.menuBarIconMargin
        anchors.left: parent.left
        toolTip: qsTr("Add new task")
        color: _style.fontColor
        onClicked: {
            _controller.addTask("New Task", /**open editor=*/true) // TODO: Pass edit mode instead
        }
    }

    Text {
        color: _style.menuBarFontColor
        text: root.titleText
        horizontalAlignment: addIcon.visible ? Text.AlignHCenter : Text.AlignLeft
        anchors.leftMargin: _style.menuBarIconMargin
        anchors.left: addIcon.visible ? addIcon.right : parent.left
        anchors.right: progressIndicator.visible ? progressIndicator.left : switchItem.left
        anchors.verticalCenter: switchItem.verticalCenter
        font.pixelSize: _style.menuBarFontSize
    }

    FlowCircularProgressIndicator {
        id: progressIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: switchItem.left
        visible: !_controller.currentTask.stopped
        showStopIcon: false
        anchors.margins: (mousePressed ? 5 : 7) * _controller.dpiFactor
    }

    FlowSwitch {
        id: switchItem
        visible: _controller.expanded && _controller.currentPage === Controller.MainPage
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: button.left
        Binding {
            target: _controller
            property: "queueType"
            value: switchItem.checked ? Controller.QueueTypeArchive
                                      : Controller.QueueTypeToday
        }
    }

    Rectangle {
        id: button
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: _style.menuBarButtonWidth
        color: mouseArea.containsMouse ? _style.menuBarButtonPressedColor : _style.menuBarBackgroundColor

        Column {
            id: column
            readonly property int numDots: 3
            height: _style.menuBarButtonDotWidth * numDots + spacing * (numDots - 1)
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: _style.menuBarButtonDotSpacing
            Repeater {
                model: column.numDots
                Rectangle {
                    color: _style.menuBarDotColor
                    anchors.horizontalCenter: column.horizontalCenter
                    width: _style.menuBarButtonDotWidth
                    height: width
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                Qt.inputMethod.hide()
                root.buttonClicked()
            }
        }
    }
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1 * _controller.dpiFactor
        anchors.top: parent.top
        color: "#393939"
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height: 2 * _controller.dpiFactor
        anchors.bottom: parent.bottom
        color: "#1B1B1B"
    }
}
