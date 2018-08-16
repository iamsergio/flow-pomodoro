import QtQuick 2.0

Item {
    // Main Window:
    property string backgroundColor    : "black"
    property int borderRadius          : Math.ceil(8 * _controller.dpiFactor)
    property string borderColor        : "gray"
    property int borderWidth           : 1 * _controller.dpiFactor
    property color fontColor           : Qt.rgba(0.9, 0.9, 0.5, 1)
    property int fontSize              : 29 * _controller.dpiFactor
    property int currentTaskFontSize   : 23 * _controller.dpiFactor

    property int clickHereFontSize     : 11 * _controller.dpiFactor
    property string clickHereFontColor : "lightsteelblue"
    property int remainingFontSize     : 23 * _controller.dpiFactor
    property int taskHeight            : 50 * _controller.dpiFactor

    property int expandedWidth         : 400 * _controller.dpiFactor
    property int expandedHeight        : 550 * _controller.dpiFactor
    property int smallGeometryTitleSize: 14 *  _controller.dpiFactor

    // tags
    property string tagBackgroundColor: "black"
    property string tagFontColor: "white"
    property int tagRadius: 3 * _controller.dpiFactor
    property int tagFontSize: 11 * _controller.dpiFactor
    property int tagCountFontSize: 10 * _controller.dpiFactor
    property string tagBorderColor: "black"
    property int tagBorderWidth: 2 * _controller.dpiFactor
    property bool tagFontBold: true
    property int tagMoveAnimationDuration: 150
    property int tagTextLeftMargin: 5 * _controller.dpiFactor
    property int tagExtraWidth: 14 * _controller.dpiFactor
    property int tagSpacing: 5 * _controller.dpiFactor
    property int tagsRightMargin: 10 * _controller.dpiFactor


    // tags in task list
    property color taskTagFontColor: "lightsteelblue"
    property color selectedTaskTagFontColor: "black"
    property color tagBackground: "black"
    property int tagRowBottomMargin: 1 * _controller.dpiFactor
    property int tagRowRightMargin: 6 * _controller.dpiFactor


    // ModalDialog
    property string blockingOverlayColor: "blue"
    property real blockingOverlayOpacity: 0.6
    property string dialogColor: "lightgray"
    property string dialogBorderColor: "darkblue"
    property int dialogBorderWidth: 2 * _controller.dpiFactor
    property int dialogFontSize: 14 * _controller.dpiFactor
    property int extraMargin: 4 * _controller.dpiFactor
    property int questionDialogHeight: 135 * _controller.dpiFactor
    property int dialogButtonsMargin: 6 * _controller.dpiFactor

    // Task Editor
    property int taskEditorFontSize: 19 * _controller.dpiFactor
    property int taskEditorHeight: 250 * _controller.dpiFactor

    // Tag tabs
    property color tagTabTextColor: "white"
    property int tagTabFontSize: 11 * _controller.dpiFactor
    property int tagTabSelectionHeight: 5 * _controller.dpiFactor
    property color tagTabSeparatorColor: "gray"
    property int tagTabSeparatorWidth: 1 * _controller.dpiFactor
    property int tagTabHeight: 30 * _controller.dpiFactor
    property int tagTabWidth: 65 * _controller.dpiFactor

    property int queueRadius            : Math.ceil(5 * _controller.dpiFactor)
    property string taskBackgroundColor : "black"
    property string taskBorderColor     : "gray"
    property int taskBorderRadius       : Math.ceil(5 * _controller.dpiFactor)

    property string taskFontColor       : "white"
    property int taskFontSize           : 14 * _controller.dpiFactor
    property color queueBackgroundColor : "#FEFFC1"

    property bool deleteAnimationEnabled : true
    property int deleteAnimationDuration : 300

    property int selectedTaskBorderWidth    : Math.ceil(1 * _controller.dpiFactor)
    property string selectedTaskBorderColor : "black"

    property int buttonsRightMargin : 15 * _controller.dpiFactor
    property int buttonsSpacing : Math.ceil(10 * _controller.dpiFactor)

    property int pageMargin : Math.ceil(10 * _controller.dpiFactor)
    property int titleFontSize : 24 * _controller.dpiFactor
    property string titleColor : "black"
    property int regularTextSize : 14 * _controller.dpiFactor
    property string regularTextColor : "black"

    property int smallTextSize     : 13 * _controller.dpiFactor
    property string smallTextColor : "black"

    property int marginSmall: Math.ceil(5 * _controller.dpiFactor)
    property int marginMedium: Math.ceil(10 * _controller.dpiFactor)
    property int marginBig: Math.ceil(20 * _controller.dpiFactor)
    property int addTagIconSize: 23
    property int deleteTagIconSize: 23
    property int addTagItemHeight: 23 * _controller.dpiFactor
    property int tagHeight: 40 * _controller.dpiFactor

    // Mobile Menu Bar
    property color menuBarFontColor: fontColor
    property color menuBarBackgroundColor: "#222222"
    property color menuBarButtonPressedColor: "#4C4C4C"
    property color menuBarDotColor: "#919191"
    property int menuBarHeight: 50 * _controller.dpiFactor
    property int menuBarFontSize: 23 * _controller.dpiFactor
    property int menuBarButtonWidth: 50 * _controller.dpiFactor
    property int menuBarButtonDotWidth: 5 * _controller.dpiFactor
    property int menuBarButtonDotSpacing: 5 * _controller.dpiFactor
    property int menuBarIconMargin: 5 * _controller.dpiFactor
    property int menuBarMargin: borderWidth

    // Config tab bar
    property color configTabBarFontColor: "#999999"
    property color configTabBarSelectedFontColor: "white"
    property int configTabBarFontSize: 13 * _controller.dpiFactor
    property int configTabBarSelectedFontSize: 16 * _controller.dpiFactor
    property int configTabBarSeparatorWidth: 2 * _controller.dpiFactor
    property int configTabBarBorderWidth: 2 * _controller.dpiFactor
    property int configTabBarBorderRadius: 8 * _controller.dpiFactor
    property int configTabBarHeight: 48 * _controller.dpiFactor
    property color configTabBarBorderColor:  "black"

    property bool configTabBarEnableGradient: true
    property color configTabBarLightGradientColor: "#454C54"
    property color configTabBarDarkerGradientColor: "#222222"
    property int configTabBarWidth: 250 * _controller.dpiFactor

    // Circular Progress Indicator
    property int circularIndicatorBorderWidth: 3 * _controller.dpiFactor
    property color circularIndicatorForegroundColor: "white"
    property color circularIndicatorBackgroundColor: "#222222"
    property color circularIndicatorSelectedBackgroundColor: "#222222"

    property int choiceDelegateHeight: 35 * _controller.dpiFactor

    property string slackingText: qsTr("flow")

    property int checkBoxIconSize: 22 * _controller.dpiFactor
    property color coldColor: "cyan"
    property color hotColor: "orange"
}
