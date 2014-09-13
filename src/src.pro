include ("../global.pri")

TEMPLATE = app
TARGET = flow

qtHaveModule(widgets):!android:!blackberry:!ios {
    QT += widgets
}

contains(QT_CONFIG, dbus) {
    QT += dbus
    SOURCES += dbus/flow.cpp
    HEADERS += dbus/flow.h
    DEFINES += FLOW_DBUS
}

include("src.pri")
SOURCES += quickview.cpp \
           main.cpp

HEADERS += quickview.h

OTHER_FILES += qml/AboutPage.qml      \
               qml/Choice.qml \
               qml/config/Hacking.qml \
               qml/config/General.qml \
               qml/config/Plugins.qml \
               qml/config/Tag.qml     \
               qml/config/Tags.qml    \
               qml/config/WebDavSync.qml \
               qml/ClickableImage.qml \
               qml/ConfigurePage.qml  \
               qml/DecentTabView.qml  \
               qml/DefaultStyle.qml   \
               qml/FlowCircularProgressIndicator.qml \
               qml/PushButton.qml \
               qml/Main.qml           \
               qml/MainPage.qml       \
               qml/ModalDialog.qml    \
               qml/MobileButton.qml \
               qml/MobileChoicePopup.qml \
               qml/MobileMenuBar.qml \
               qml/MobileStyle.qml \
               qml/MobileTabView.qml  \
               qml/Page.qml           \
               qml/QuestionPopup.qml  \
               qml/RegularText.qml    \
               qml/SmallText.qml      \
               qml/TagDelegate.qml \
               qml/Task.qml           \
               qml/TaskContextMenu.qml \
               qml/qml/TaskListView.qml \
               qml/TaskTextField.qml \
               qml/ToolTip.qml


lupdate_only {
    SOURCES += $$OTHER_FILES
}

RESOURCES += resources.qrc
TRANSLATIONS += translations/flow_pt_PT.ts
CODECFORSRC  = UTF-8

linux:!android {
    QMAKE_POST_LINK += "$$PWD/runqmllint.sh $$PWD/qml"
    QMAKE_POST_LINK += "; $$PWD/validate_qrc.sh"
}

DESTDIR = ../
