import QtQuick 2.2
import Controller 1.0
import com.kdab.flowpomodoro 1.0

Main {
    id: root

    onRequestGlobalContextMenu: {
        globalContextMenu.popup()
    }

    topBarItem:
        Item {
        id: header
        anchors.fill: parent

        FontAwesomeIcon {
            id: addIcon
            text: "\uf055" // "\uf0fe"
            size: 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium

            toolTip: qsTr("Add new task")
            visible: _controller.expanded
            color: "white"
            onClicked: {
                _controller.addTask("New Task", /**open editor=*/true) // TODO: Pass edit mode instead
            }
        }

        Text {
            id: titleText
            renderType: _controller.textRenderType
            elide: _controller.currentTask.paused ? Text.ElideLeft : Text.ElideRight
            color: _style.fontColor
            font.pixelSize: _controller.currentTask.stopped ? _style.fontSize : _style.currentTaskFontSize
            font.bold: true
            anchors.left: addIcon.visible ? addIcon.right : parent.left
            anchors.leftMargin: _style.marginMedium
            anchors.right: buttonRow.left
            anchors.rightMargin: 2 * _controller.dpiFactor
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: switchItem.visible ? 0 : -5 * _controller.dpiFactor
            horizontalAlignment: switchItem.visible ? Text.AlignHCenter : Text.AlignLeft
            text: root.titleText
        }

        Text {
            id: focusText
            text: qsTr("Click here to start focusing")
            font.bold: false
            font.pixelSize: _style.clickHereFontSize
            color: _style.clickHereFontColor
            visible: _controller.currentTask.stopped && !_controller.expanded
            anchors.left: titleText.left
            anchors.leftMargin: _style.marginSmall
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.marginSmall
        }

        Row {
            z: 2
            id: buttonRow
            anchors.right: parent.right
            anchors.topMargin: 4 * _controller.dpiFactor
            anchors.bottomMargin: 4 * _controller.dpiFactor
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.rightMargin: circularIndicator.mousePressed ? _style.marginMedium - 2 * _controller.dpiFactor
                                                                :  _style.marginMedium
            spacing: _style.buttonsSpacing

            FlowSwitch {
                id: switchItem
                visible: _controller.expanded && _controller.currentPage === Controller.MainPage
                anchors.verticalCenter: parent.verticalCenter
                Binding {
                    target: _controller
                    property: "queueType"
                    value: switchItem.checked ? Controller.QueueTypeArchive
                                              : Controller.QueueTypeToday
                }
            }

            FlowCircularProgressIndicator {
                id: circularIndicator
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: (mousePressed ? 3 : 5) * _controller.dpiFactor
                visible: !_controller.currentTask.stopped
            }

            FontAwesomeIcon {
                id: configureIcon
                anchors.verticalCenter: buttonRow.verticalCenter
                toolTip: qsTr("Configure")
                visible: _controller.expanded
                text: "\uf013"
                size: 30
                onClicked: {
                    _controller.currentPage = _controller.currentPage == Controller.ConfigurePage ? Controller.MainPage
                                                                                                  : Controller.ConfigurePage
                }
            }
        }
    }

    GlobalContextMenu {
        id: globalContextMenu
    }
}
