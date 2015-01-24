import QtQuick 2.2
import Controller 1.0
import com.kdab.flowpomodoro 1.0

Main {
    id: root
    property bool smallMode: !(_controller.expanded || _window.geometryType === QuickView.GeometryStandard)

    topBarItem:
        Item {
        id: header
        objectName: "header"
        anchors.fill: parent

        FontAwesomeIcon {
            id: addIcon
            text: "\uf055" // "\uf0fe"
            size: 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: _style.marginMedium

            toolTip: qsTr("Add new task")
            visible: _controller.expanded && _controller.currentPage == Controller.MainPage
            color: "white"
            onClicked: {
                if (_controller.rightClickedTask === null) // Ignore if we have the context menu open
                    _controller.addTask("New Task", /**open editor=*/true) // TODO: Pass edit mode instead
            }
        }

        Text {
            function fontPixelSize()
            {
                if (root.smallMode) {
                    return _style.smallGeometryTitleSize
                } else {
                    return _controller.currentTask.stopped ? _style.fontSize
                                                           : _style.currentTaskFontSize
                }
            }

            id: titleText
            renderType: _controller.textRenderType
            elide: _controller.currentTask.paused ? Text.ElideLeft : Text.ElideRight
            color: _style.fontColor
            font.pixelSize: fontPixelSize()
            font.bold: true
            anchors.left: addIcon.visible ? addIcon.right : parent.left
            anchors.leftMargin: _style.marginMedium
            anchors.right: buttonRow.left
            anchors.rightMargin: 2 * _controller.dpiFactor
            anchors.fill: root.smallMode ? parent : undefined
            verticalAlignment: root.smallMode ? Text.AlignTop : Text.AlignVCenter
            anchors.verticalCenter: root.smallMode ? undefined : parent.verticalCenter
            anchors.verticalCenterOffset: _controller.expanded ? 0 : -5 * _controller.dpiFactor
            horizontalAlignment: switchItem.visible ? Text.AlignHCenter : Text.AlignLeft
            text: root.titleText
        }

        Text {
            id: focusText
            text: qsTr("Click here to start focusing")
            font.bold: false
            font.pixelSize: _style.clickHereFontSize
            color: _style.clickHereFontColor
            visible: _controller.currentTask.stopped && !_controller.expanded && _window.geometryType === QuickView.GeometryStandard
            anchors.left: titleText.left
            anchors.leftMargin: _style.marginSmall
            anchors.bottom: parent.bottom
            anchors.bottomMargin: _style.marginSmall
        }

        Row {
            z: 2
            id: buttonRow
            anchors.right: parent.right
            anchors.topMargin: root.smallMode ? 0 : 4 * _controller.dpiFactor
            anchors.bottomMargin: 4 * _controller.dpiFactor
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.rightMargin: circularIndicator.mousePressed ? _style.marginMedium - 2 * _controller.dpiFactor
                                                                :  _style.marginMedium
            spacing: _style.buttonsSpacing

            FlowSwitch {
                id: switchItem
                objectName: "switchItem"
                visible: _controller.expanded && _controller.currentPage === Controller.MainPage
                anchors.verticalCenter: parent.verticalCenter
                checked: _controller.queueType == Controller.QueueTypeArchive
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
                visible: !_controller.currentTask.stopped && !root.smallMode
            }

            Text {
                id: progressText
                anchors.top: parent.top
                height: root.height
                visible: !_controller.currentTask.stopped && root.smallMode
                text: _controller.remainingMinutes + qsTr("m")
                color: _style.fontColor
                font.pixelSize: titleText.font.pixelSize
                font.bold: true
            }

            FontAwesomeIcon {
                id: configureIcon
                objectName: "configureIcon"
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
}
