import QtQuick 2.4
import com.kdab.flowpomodoro 1.0



Rectangle {
    id: root

    property alias date: impl.currentDate

    border {
        width: 1
        color: "#D3D3D3"
    }

    CalendarController {
        id: impl
    }

    ListModel {
        id: weekDaysModel
        ListElement { label: "Mon" ; }
        ListElement { label: "Tue" ; }
        ListElement { label: "Wed" ; }
        ListElement { label: "Thu" ; }
        ListElement { label: "Fri" ; }
        ListElement { label: "Sat" ; }
        ListElement { label: "Sun" ; }
    }

    Rectangle {
        id: header
        height: 25 * _controller.dpiFactor
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins:1
        }
        color: "#E9E9E9"

        Text {
            color: "black"
            anchors.centerIn: parent
            font.pixelSize: 17
            font.bold: true
            text: Qt.formatDate(impl.shownDayOne, "MMM yyyy")
        }

        FontAwesomeIcon {
            id: iconLeft
            text: "\uf0a8"
            anchors {
                left: parent.left
                leftMargin: 15 * _controller.dpiFactor
            }

            height: parent.height
            color: "black"
            onClicked: {
                impl.addMonths(-1)
            }
        }

        FontAwesomeIcon {
              id: iconRight
              text: "\uf0a9"
              height: parent.height
              anchors {
                  right: parent.right
                  rightMargin: 15 * _controller.dpiFactor
              }
              color: "black"
              onClicked: {
                  impl.addMonths(1)
              }
          }
    }

    Row {
        id: weekDays
        height: 25
        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
        }

        Repeater {
            model: weekDaysModel
            Text {
                width: parent.width / 7
                text: model.label // Qt.locale().dayName(index, Locale.ShortFormat)
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 14 * _controller.dpiFactor
            }
        }
    }

    Rectangle {
        id: gridRect
        color: "#D3D3D3"
        border.color: color
        anchors {
            top: weekDays.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        Grid {
            id: grid
            columns: 7
            rows: 6
            spacing: 1

            anchors.fill: parent
            anchors.margins: 1

            Repeater {
                model: impl.dateList
                Rectangle {
                    width: (grid.width - (6 * grid.spacing)) / grid.columns
                    height: (grid.height - (5 * grid.spacing)) / grid.rows
                    readonly property bool isToday: impl.today.getTime() === modelData.getTime() // For some reason op == doesn't work
                    readonly property bool isSelected: impl.currentDate.getTime() === modelData.getTime()
                    readonly property bool isThisMonth: modelData.getMonth() === impl.shownDayOne.getMonth()
                    color: isSelected ? "#308CC6" : "white"
                    Text {
                        anchors.centerIn: parent
                        color: isSelected ? "white" : (isThisMonth ? "black" : "#515151")
                        font.pixelSize: (isToday ? 16 : (isThisMonth ? 11 : 10)) * _controller.dpiFactor
                        font.bold: isToday
                        text: isToday ? qsTr("T") : Qt.formatDate(model.modelData, (modelData.getDate() < 10 ? "d" : "dd"))
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            impl.setCurrentDate(modelData)
                        }
                    }
                }
            }
        }
    }
}
