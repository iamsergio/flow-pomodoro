import QtQuick 2.0

Item {

    readonly property string backgroundColor : "black"
    readonly property int borderRadius       : 7
    readonly property string borderColor     : "gray"
    readonly property int borderWidth        : 1

    readonly property string progressBarBgColor       : "white"
    readonly property string progressBarFgBorderColor : "darkgray"
    readonly property int progressBarborderRadius     : 3

    readonly property color taskTitleColor  : Qt.rgba(0.9, 0.9, 0.5, 1)
    readonly property string clickHereColor : "lightsteelblue"
    readonly property int clickHereTextSize : 10
    readonly property int taskTitleSize     : 20
    readonly property int remainingTextSize : 22

    readonly property int queueRadius            : 4
    readonly property string taskBackgroundColor : "black"
    readonly property string taskBorderColor     : "gray"
    readonly property int taskBorderRadius       : 4

    readonly property string queuedTasksTitleColor : "white"
    readonly property int queuedTasksFontSize      : 15
    readonly property color queueBackgroundColor   : Qt.rgba(0.9, 0.9, 0.5, 1)

    readonly property bool deleteAnimationEnabled : true

    readonly property int pageHeight : 500

    readonly property int hoveredTaskBorderWidth    : 1
    readonly property string hoveredTaskBorderColor : "black"
    readonly property string hoveredTaskBgColor     : "lightgray"
    readonly property string hoveredTaskFgColor     : "black"

    readonly property int buttonsRightMargin : 10
    readonly property int buttonsSpacing : 5

    readonly property int pageMargin : 10
    readonly property int titleSize : 22
    readonly property string titleColor : "black"
    readonly property int regularTextSize : 13
    readonly property string regularTextColor : "black"

    readonly property int smallTextSize     : 10
    readonly property string smallTextColor : "black"
}
