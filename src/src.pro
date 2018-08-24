include ("../global.pri")

TEMPLATE = app
TARGET = flow

qtHaveModule(widgets):!android:!ios:!qnx {
    QT += widgets
}

!android:!ios {
    QT += dbus
    SOURCES += dbus/flow.cpp
    HEADERS += dbus/flow.h
    DEFINES += FLOW_DBUS
}

static:!ios {
    DEFINES += FLOW_STATIC_BUILD
    LIBS += -lshellscript -lhosts
    LIBPATH += ../plugins/distractions/
}

include("src.pri")
SOURCES += main.cpp

OTHER_FILES += qml/AboutPage.qml      \
               qml/Choice.qml \
               qml/ChoicePopup.qml \
               qml/config/Hacking.qml \
               qml/config/General.qml \
               qml/config/Plugins.qml \
               qml/config/Tag.qml     \
               qml/config/Tags.qml    \
               qml/ConfigurePage.qml  \
               qml/DecentTabView.qml  \
               qml/DefaultStyle.qml   \
               qml/FontAwesomeIcon.qml \
               qml/FlowCircularProgressIndicator.qml \
               qml/FlowCheckBox.qml \
               qml/FlowScrollBar.qml \
               qml/FlowSwitch.qml \
               qml/InputPopup.qml \
               qml/LoadingScreen.qml \
               qml/Popup.qml \
               qml/PushButton.qml \
               qml/Main.qml \
               qml/MainDesktop.qml \
               qml/MainMobile.qml \
               qml/MainPage.qml \
               qml/MessagePopup.qml \
               qml/MobileButton.qml \
               qml/MobileMenuBar.qml \
               qml/MobileOptionsContextMenu.qml \
               qml/MobileOptionsContextMenuItem.qml \
               qml/MobileStyle.qml \
               qml/MobileTabView.qml \
               qml/NewTagDialog.qml \
               qml/Overlay.qml \
               qml/Page.qml \
               qml/PomodoroOverlay.qml \
               qml/SmallText.qml      \
               qml/TabBar.qml \
               qml/TabBarItem.qml \
               qml/TabView.qml \
               qml/TagDelegate.qml \
               qml/Task.qml           \
               qml/TaskContextMenu.qml \
               qml/TaskEditor.qml \
               qml/qml/TaskListView.qml \
               qml/TaskTextField.qml \
               qml/ToolTip.qml


lupdate_only {
    SOURCES += $$OTHER_FILES
}

TRANSLATIONS += translations/flow_pt_PT.ts
CODECFORSRC  = UTF-8

linux:!android:contains(DEFINES, DEVELOPER_MODE) {
    QMAKE_POST_LINK += "$$PWD/runqmllint.sh $$PWD/qml"
    QMAKE_POST_LINK += "; $$PWD/validate_qrc.sh $$PWD/"
}

windows {
	# App icon
	RC_FILE = flow.rc
}

osx {
    ICON = img/icon.icns
}

DESTDIR = ../

# For make INSTAL_ROOT=prefix install
target.path = /bin/
INSTALLS += target
