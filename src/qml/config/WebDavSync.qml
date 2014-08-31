import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import Controller 1.0
import ".."

Item {
    id: root
    anchors.fill: parent

    GridLayout {
        id: grid1
        columns: 2

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: _style.marginBig
        anchors.leftMargin: _style.marginMedium
        anchors.rightMargin: 40 * _controller.dpiFactor
        columnSpacing: _style.marginBig

        Text {
            text: qsTr("Protocol")
        }

        ComboBox {
            id: httpsCombo
            currentIndex: (_controller.isHttps && _controller.openSSLSupported) ? 1 : 0
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
        }

        Text {
            text: qsTr("Host")
        }

        TextField {
            id: hostField
            text: _controller.host
            Binding {
                target: _controller
                property: "host"
                value: hostField.text
            }
        }

        Text {
            text: qsTr("Path")
        }

        TextField {
            id: pathField
            text: _controller.path
            Binding {
                target: _controller
                property: "path"
                value: pathField.text
            }
        }

        Text {
            text: qsTr("Port")
        }

        TextField {
            id: portField
            text: _controller.port
            Binding {
                target: _controller
                property: "port"
                value: portField.text
            }
        }

        Text {
            text: qsTr("User")
        }

        TextField {
            id: userField
            text: _controller.user
            Binding {
                target: _controller
                property: "user"
                value: userField.text
            }
        }

        Text {
            text: qsTr("Password")
        }

        TextField {
            id: passwordField
            echoMode: TextInput.Password
            text: _controller.password
            Binding {
                target: _controller
                property: "password"
                value: passwordField.text
            }
        }
    }

    Button {
        id: testButton
        anchors.top: grid1.bottom
        anchors.left: grid1.left
        anchors.topMargin: _style.marginBig
        text: qsTr("Test settings")
        enabled: portField.text && hostField.text && pathField.text && !_webdavSync.syncInProgress
        onClicked: {
            testSettingsText.text = ""
            _webdavSync.testSettings()
        }
    }

    Text {
        id: testSettingsText
        anchors.verticalCenter: testButton.verticalCenter
        anchors.left: testButton.right
        anchors.leftMargin: _style.marginMedium
        visible: testButton.enabled
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
        interval: 5000
        repeat: false
        onTriggered: {
            testSettingsText.text = ""
        }
    }

    Text {
        id: urlText
        anchors.top: testButton.bottom
        anchors.left: grid1.left
        anchors.topMargin: _style.marginBig
        text: qsTr("Url") + ": " + httpsCombo.currentText + "://" + hostField.text + (pathField.text.charAt(0) == "/" ? pathField.text : "/" + pathField.text) + (portField.text ? ":" + portField.text : "")
    }

    Text {
        id: passwordWarningText
        color: "red"
        anchors.top: urlText.bottom
        anchors.left: grid1.left
        anchors.topMargin: _style.marginSmall
        text: qsTr("Password will be saved in clear-text.")
        visible: passwordField.text
    }

    Text {
        color: "red"
        anchors.top: passwordWarningText.visible ? passwordWarningText.bottom : urlText.bottom
        anchors.left: grid1.left
        anchors.topMargin: _style.marginSmall
        text: qsTr("OpenSSL not supported.")
        visible: !_storage.openSSLSupported
    }
}
