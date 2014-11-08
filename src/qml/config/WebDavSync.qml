import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.2

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    Grid {
        id: grid1
        columns: 2

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: _style.marginBig
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin: 40 * _controller.dpiFactor
        columnSpacing: _style.marginBig
        rowSpacing: 5 * _controller.dpiFactor

        Text {
            text: qsTr("Protocol")
            font.pixelSize: 12 * _controller.dpiFactor
        }

        ComboBox {
            id: httpsCombo
            currentIndex: (_controller.isHttps && _controller.openSSLSupported) ? 1 : 0
            enabled: !_webdavSync.syncInProgress
            model: ListModel {

                ListElement { text: "http" }
                ListElement { text: "https" }

                Component.onCompleted: {
                    if (!_controller.openSSLSupported) {
                        remove(1)
                    }
                }
            }
            Binding {
                target: _controller
                property: "isHttps"
                value: httpsCombo.currentIndex == 1
            }

            style: ComboBoxStyle {
                label: // Explicitely define a label style, since on blackberry it's gray for some reason
                    Text {
                        id: textitem
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 4 * _controller.dpiFactor
                        anchors.rightMargin: 10 * _controller.dpiFactor
                        anchors.verticalCenter: parent.verticalCenter
                        text: control.currentText
                        color: "black"
                        elide: Text.ElideLeft
                        renderType: _controller.textRenderType
                    }
            }
        }

        Text {
            text: qsTr("Host")
            font.pixelSize: 12 * _controller.dpiFactor
        }

        TextField {
            id: hostField
            text: _controller.host
            enabled: !_webdavSync.syncInProgress
            Binding {
                target: _controller
                property: "host"
                value: hostField.text
            }
            style:
            TextFieldStyle {
                    // Set text color explicitely, for some reason it defaults to gray on blackberry
                    textColor: "black"
            }
        }

        Text {
            text: qsTr("Path")
            font.pixelSize: 12 * _controller.dpiFactor
        }

        TextField {
            id: pathField
            text: _controller.path
            enabled: !_webdavSync.syncInProgress
            Binding {
                target: _controller
                property: "path"
                value: pathField.text
            }
            style:
            TextFieldStyle {
                    // Set text color explicitely, for some reason it defaults to gray on blackberry
                    textColor: "black"
            }
        }

        Text {
            text: qsTr("Port")
            font.pixelSize: 12 * _controller.dpiFactor
        }

        TextField {
            id: portField
            text: _controller.port
            enabled: !_webdavSync.syncInProgress
            Binding {
                target: _controller
                property: "port"
                value: portField.text
            }
            style:
            TextFieldStyle {
                    // Set text color explicitely, for some reason it defaults to gray on blackberry
                    textColor: "black"
            }
        }

        Text {
            text: qsTr("User")
            font.pixelSize: 12 * _controller.dpiFactor
        }

        TextField {
            id: userField
            text: _controller.user
            enabled: !_webdavSync.syncInProgress
            Binding {
                target: _controller
                property: "user"
                value: userField.text
            }
            style:
            TextFieldStyle {
                // Set text color explicitely, for some reason it defaults to gray on blackberry
                textColor: "black"
            }
        }

        Text {
            text: qsTr("Password")
            font.pixelSize: 12 * _controller.dpiFactor
        }

        TextField {
            id: passwordField
            echoMode: TextInput.Password
            text: _controller.password
            enabled: !_webdavSync.syncInProgress
            Binding {
                target: _controller
                property: "password"
                value: passwordField.text
            }
            style:
            TextFieldStyle {
                    // Set text color explicitely, for some reason it defaults to gray on blackberry
                    textColor: "black"
            }
        }

        Text {
            text: qsTr("Sync at startup")
            font.pixelSize: 12 * _controller.dpiFactor
        }

        FlowCheckBox {
            id: syncAtStartupCheckBox
            checked: _controller.syncAtStartup
            Binding {
                target: _controller
                property: "syncAtStartup"
                value: syncAtStartupCheckBox.checked
            }
        }
    }

    PushButton {
        id: testButton
        anchors.top: grid1.bottom
        anchors.left: grid1.left
        anchors.topMargin: _style.marginMedium
        text: qsTr("Test settings")
        enabled: portField.text && hostField.text && pathField.text && !_webdavSync.syncInProgress
        onClicked: {
            testSettingsText.text = ""
            _webdavSync.testSettings()
        }
    }

    PushButton {
        anchors.top: testButton.top
        anchors.left: testButton.right
        anchors.leftMargin: _style.marginSmall
        text: qsTr("Sync")
        enabled: testButton.enabled
        onClicked: {
            testSettingsText.text = ""
            _webdavSync.sync()
        }
    }

    Text {
        id: testSettingsText
        anchors.left: testButton.left
        anchors.leftMargin: _style.marginSmall
        anchors.right: parent.right
        anchors.rightMargin: _style.marginMedium
        anchors.top: testButton.bottom
        anchors.topMargin: _style.marginSmall
        visible: testButton.enabled
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.pixelSize: 12 * _controller.dpiFactor
    }

    Connections {
        target: _webdavSync
        onTestSettingsFinished: {
            testSettingsText.color = success ? "black" : "red"
            testSettingsText.text = success ? qsTr("Success!") : errorMessage
            timer.start()
        }
    }

    Timer {
        id: timer
        interval: 10000
        repeat: false
        onTriggered: {
            testSettingsText.text = ""
        }
    }

    Text {
        id: urlText
        anchors.top: testSettingsText.bottom
        anchors.left: grid1.left
        anchors.right: parent.right
        anchors.rightMargin: _style.marginSmall
        anchors.topMargin: _style.marginSmall
        text: qsTr("Url") + ": " + httpsCombo.currentText + "://" + hostField.text + (pathField.text.charAt(0) == "/" ? pathField.text : "/" + pathField.text) + (portField.text ? ":" + portField.text : "")
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.pixelSize: 12 * _controller.dpiFactor
    }

    Text {
        id: passwordWarningText
        color: "red"
        anchors.top: urlText.bottom
        anchors.left: grid1.left
        anchors.topMargin: _style.marginSmall
        text: qsTr("Password will be saved in clear-text.")
        visible: passwordField.text
        font.pixelSize: 12 * _controller.dpiFactor
    }

    Text {
        color: "red"
        anchors.top: passwordWarningText.visible ? passwordWarningText.bottom : urlText.bottom
        anchors.left: grid1.left
        anchors.topMargin: _style.marginSmall
        text: qsTr("OpenSSL not supported.")
        visible: !_controller.openSSLSupported
        font.pixelSize: 12 * _controller.dpiFactor
    }
}
