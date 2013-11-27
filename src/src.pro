include ("../global.pri")

TEMPLATE = app
TARGET = flow

QT += quick

SOURCES += controller.cpp  \
           main.cpp        \
           pluginmodel.cpp \
           quickview.cpp   \
           settings.cpp    \
           taskmodel.cpp

HEADERS += controller.h      \
           plugininterface.h \
           pluginmodel.h     \
           quickview.h       \
           settings.h        \
           task.h            \
           taskmodel.h

OTHER_FILES += ClickableImage.qml \
               ConfigurePage.qml  \
               DefaultStyle.qml   \
               Main.qml           \
               Task.qml           \
               TheQueuePage.qml

RESOURCES += resources.qrc

DESTDIR = ../
