include ("../global.pri")

TEMPLATE = app
TARGET = flow

qtHaveModule(widgets):!android {
    QT += widgets
}

contains(QT_CONFIG, dbus) {
    QT += dbus
    SOURCES += dbus/flow.cpp
    HEADERS += dbus/flow.h
    DEFINES += FLOW_DBUS
}

include("src.pri")
SOURCES += main.cpp

OTHER_FILES += qml/AboutPage.qml      \
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
               qml/Main.qml           \
               qml/MainPage.qml       \
               qml/ModalDialog.qml    \
               qml/MobileTabView.qml  \
               qml/Page.qml           \
               qml/QuestionPopup.qml  \
               qml/RegularText.qml    \
               qml/SmallText.qml      \
               qml/TagsMenu.qml       \
               qml/Task.qml           \
               qml/TaskEditorDialog.qml \
               qml/qml/TaskListView.qml \
               qml/TaskTextField.qml \
               qml/TitleText.qml     \
               qml/ToolTip.qml


lupdate_only {
    SOURCES += $$OTHER_FILES
}

RESOURCES += resources.qrc
TRANSLATIONS += translations/flow_pt_PT.ts
CODECFORSRC  = UTF-8

linux:!android {
    QMAKE_POST_LINK += "$$PWD/runqmllint.sh $$PWD/qml"
}

DESTDIR = ../
