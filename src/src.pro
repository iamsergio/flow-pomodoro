include ("../global.pri")

TEMPLATE = app
TARGET = flow

QT += quick

SOURCES += controller.cpp  \
           main.cpp        \
           pluginmodel.cpp \
           quickview.cpp   \
           settings.cpp    \
           sortmodel.cpp   \
           tag.cpp \
           tagref.cpp \
           tagstorage.cpp \
           tagstorageqsettings.cpp \
           taskstorage.cpp \
           taskstorageqsettings.cpp \
           task.cpp \
           taskfilterproxymodel.cpp

HEADERS += controller.h      \
           genericlistmodel.h \
           plugininterface.h \
           pluginmodel.h     \
           quickview.h       \
           settings.h        \
           sortmodel.h       \
           tag.h \
           tagref.h \
           tagstorage.h \
           tagstorageqsettings.h \
           task.h            \
           taskfilterproxymodel.h \
           taskstorage.h \
           taskstorageqsettings.h

contains(QT_CONFIG, dbus) {
    QT += dbus
    SOURCES += dbus/flow.cpp
    HEADERS += dbus/flow.h
    DEFINES += FLOW_DBUS
}

OTHER_FILES += AboutPage.qml      \
               config/General.qml \
               config/Plugins.qml \
               config/Tag.qml     \
               config/Tags.qml    \
               ClickableImage.qml \
               ConfigurePage.qml  \
               DefaultStyle.qml   \
               Main.qml           \
               ModalDialog.qml    \
               Page.qml           \
               QuestionPopup.qml  \
               RegularText.qml    \
               SmallText.qml      \
               Task.qml           \
               TaskEditorDialog.qml \
               TaskTextField.qml \
               TitleText.qml      \
               TheQueuePage.qml


RESOURCES += resources.qrc

linux {
    QMAKE_POST_LINK += "runqmllint.sh"
}

DESTDIR = ../
