include ("../global.pri")

TEMPLATE = app
TARGET = flow

QT += quick

qtHaveModule(widgets) {
    QT += widgets
}

contains(QT_CONFIG, dbus) {
    QT += dbus
    SOURCES += dbus/flow.cpp
    HEADERS += dbus/flow.h
    DEFINES += FLOW_DBUS
}

SOURCES += archivedtasksfiltermodel.cpp \
           checkabletagmodel.cpp \
           controller.cpp  \
           jsonstorage.cpp \
           main.cpp        \
           pluginmodel.cpp \
           quickview.cpp   \
           settings.cpp    \
           sortedtagsmodel.cpp \
           storage.cpp \
           tag.cpp \
           tagref.cpp \
           task.cpp \
           taskfilterproxymodel.cpp \
           tooltipcontroller.cpp

HEADERS += archivedtasksfiltermodel.h \
           checkabletagmodel.h \
           controller.h      \
           jsonstorage.h      \
           genericlistmodel.h \
           plugininterface.h \
           pluginmodel.h     \
           quickview.h       \
           settings.h        \
           sortedtagsmodel.h \
           storage.h \
           tag.h \
           tagref.h \
           task.h            \
           taskfilterproxymodel.h \
           tooltipcontroller.h

OTHER_FILES += qml/AboutPage.qml      \
               qml/config/General.qml \
               qml/config/Plugins.qml \
               qml/config/Tag.qml     \
               qml/config/Tags.qml    \
               qml/ClickableImage.qml \
               qml/ConfigurePage.qml  \
               qml/DecentTabView.qml  \
               qml/DefaultStyle.qml   \
               qml/Main.qml           \
               qml/MainPage.qml       \
               qml/qml/ModalDialog.qml    \
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

linux {
    QMAKE_POST_LINK += "runqmllint.sh"
}

DESTDIR = ../
