import QtQuick 2.0

Item {

    // Main Window:
    readonly property string backgroundColor    : "black"
    readonly property int borderRadius          : Math.ceil(8 * _controller.dpiFactor)
    readonly property string borderColor        : "gray"
    readonly property int borderWidth           : 1 * _controller.dpiFactor
    readonly property color fontColor           : Qt.rgba(0.9, 0.9, 0.5, 1)
    readonly property int fontSize              : 20
    readonly property int clickHereFontSize     : 10
    readonly property string clickHereFontColor : "lightsteelblue"
    readonly property int remainingFontSize     : 22

    // progress bar:
    readonly property string progressBarBgColor       : "white"
    readonly property string progressBarFgBorderColor : "darkgray"
    readonly property int progressBarBorderRadius     : Math.ceil(3 * _controller.dpiFactor)

    // tags
    readonly property string tagBackgroundColor: "blue"
    readonly property string tagFontColor: "white"
    readonly property int tagRadius: 5 * _controller.dpiFactor
    readonly property int tagFontSize: 10
    readonly property int tagCountFontSize: 8
    readonly property string tagBorderColor: "black"
    readonly property int tagBorderWidth: 2 * _controller.dpiFactor
    readonly property bool tagFontBold: true
    readonly property int tagMoveAnimationDuration: 150
    readonly property int tagTextLeftMargin: 5 * _controller.dpiFactor
    readonly property int tagExtraWidth: 30
    readonly property int tagSpacing: 5 * _controller.dpiFactor
    readonly property int tagsRightMargin: 40 * _controller.dpiFactor


    // tags in task list
    readonly property color taskTagFontColor: "lightsteelblue"
    readonly property color selectedTaskTagFontColor: "black"

    // ModalDialog
    readonly property string blockingOverlayColor: "blue"
    readonly property real blockingOverlayOpacity: 0.6
    readonly property string dialogColor: "lightgray"
    readonly property string dialogBorderColor: "darkblue"
    readonly property int dialogBorderWidth: 2 * _controller.dpiFactor
    readonly property int dialogFontSize: 11
    readonly property int extraMargin: 4 * _controller.dpiFactor
    readonly property int questionDialogHeight: 75 * _controller.dpiFactor
    readonly property int dialogButtonsMargin: 6 * _controller.dpiFactor

    // Task Editor
    readonly property int taskEditorFontSize: 14
    readonly property int taskEditorHeight: 250 * _controller.dpiFactor

    // Tag tabs
    readonly property color tagTabTextColor: "white"
    readonly property int tagTabFontSize: 9
    readonly property int tagTabSelectionHeight: 5 * _controller.dpiFactor
    readonly property color tagTabSeparatorColor: "gray"
    readonly property int tagTabSeparatorWidth: 1 * _controller.dpiFactor
    readonly property int tagTabHeight: 30 * _controller.dpiFactor
    readonly property int tagTabWidth: 65 * _controller.dpiFactor

    readonly property int queueRadius            : Math.ceil(5 * _controller.dpiFactor)
    readonly property string taskBackgroundColor : "black"
    readonly property string taskBorderColor     : "gray"
    readonly property int taskBorderRadius       : Math.ceil(5 * _controller.dpiFactor)

    readonly property string taskFontColor       : "white"
    readonly property int taskFontSize           : 15
    readonly property color queueBackgroundColor : Qt.rgba(0.9, 0.9, 0.5, 1)

    readonly property bool deleteAnimationEnabled : true
    readonly property int deleteAnimationDuration : 350

    readonly property int pageHeight : 500 * _controller.dpiFactor

    readonly property int selectedTaskBorderWidth    : Math.ceil(1 * _controller.dpiFactor)
    readonly property string selectedTaskBorderColor : "black"
    readonly property string selectedTaskBgColor     : "lightgray"
    readonly property string selectedTaskFgColor     : "black"

    readonly property int buttonsRightMargin : 10 * _controller.dpiFactor
    readonly property int buttonsSpacing : Math.ceil(5 * _controller.dpiFactor)

    readonly property int pageMargin : Math.ceil(10 * _controller.dpiFactor)
    readonly property int titleSize : 22
    readonly property string titleColor : "black"
    readonly property int regularTextSize : 13
    readonly property string regularTextColor : "black"

    readonly property int smallTextSize     : 10
    readonly property string smallTextColor : "black"

    readonly property int progressBarHeight: 10 * _controller.dpiFactor

    readonly property int marginSmall: Math.ceil(5 * _controller.dpiFactor)
    readonly property int marginMedium: Math.ceil(10 * _controller.dpiFactor)
    readonly property int marginBig: Math.ceil(20 * _controller.dpiFactor)
}
