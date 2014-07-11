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
           taskstorage.cpp \
           taskstorageqsettings.cpp \
           task.cpp \
           tagstorage.cpp \
           tagstorageqsettings.cpp \
           taskfilterproxymodel.cpp

HEADERS += controller.h      \
           plugininterface.h \
           pluginmodel.h     \
           quickview.h       \
           settings.h        \
           sortmodel.h       \
           task.h            \
           tag.h \
           tagref.h \
           taskstorage.h \
           taskstorageqsettings.h \
           tagstorage.h \
           tagstorageqsettings.h \
           taskfilterproxymodel.h \
           genericlistmodel.h

contains(QT_CONFIG, dbus) {
    QT += dbus
    SOURCES += dbus/flow.cpp
    HEADERS += dbus/flow.h
    DEFINES += FLOW_DBUS
}

OTHER_FILES += AboutPage.qml \
               ClickableImage.qml \
               ConfigurePage.qml  \
               DefaultStyle.qml   \
               Main.qml           \
               Task.qml           \
               TheQueuePage.qml

RESOURCES += resources.qrc

DESTDIR = ../
