QT += quick

SOURCES += $$PWD/checkabletagmodel.cpp \
           $$PWD/circularprogressindicator.cpp \
           $$PWD/checkbox.cpp \
           $$PWD/controller.cpp  \
           $$PWD/extendedtagsmodel.cpp \
           $$PWD/jsonstorage.cpp \
           $$PWD/kernel.cpp \
           $$PWD/loadmanager.cpp \
           $$PWD/nonemptytagfilterproxy.cpp \
           $$PWD/pluginmodel.cpp \
           $$PWD/quickview.cpp \
           $$PWD/runtimeconfiguration.cpp \
           $$PWD/settings.cpp    \
           $$PWD/sortedtagsmodel.cpp \
           $$PWD/sortedtaskcontextmenumodel.cpp \
           $$PWD/storage.cpp \
           $$PWD/syncable.cpp \
           $$PWD/tag.cpp \
           $$PWD/tagref.cpp \
           $$PWD/task.cpp \
           $$PWD/taskcontextmenumodel.cpp \
           $$PWD/taskfilterproxymodel.cpp \
           $$PWD/tooltipcontroller.cpp \
           $$PWD/utils.cpp

HEADERS += $$PWD/checkabletagmodel.h \
           $$PWD/checkbox.h \
           $$PWD/circularprogressindicator.h \
           $$PWD/controller.h      \
           $$PWD/extendedtagsmodel.h \
           $$PWD/jsonstorage.h      \
           $$PWD/kernel.h \
           $$PWD/loadmanager.h \
           $$PWD/genericlistmodel.h \
           $$PWD/plugininterface.h \
           $$PWD/pluginmodel.h \
           $$PWD/quickview.h \
           $$PWD/nonemptytagfilterproxy.h \
           $$PWD/runtimeconfiguration.h \
           $$PWD/settings.h        \
           $$PWD/sortedtagsmodel.h \
           $$PWD/sortedtaskcontextmenumodel.h \
           $$PWD/storage.h \
           $$PWD/syncable.h \
           $$PWD/tag.h \
           $$PWD/tagref.h \
           $$PWD/task.h \
           $$PWD/taskcontextmenumodel.h \
           $$PWD/taskfilterproxymodel.h \
           $$PWD/tooltipcontroller.h \
           $$PWD/utils.h

RESOURCES += $$PWD/resources.qrc

!contains(DEFINES, NO_WEBDAV) {
    include("$$PWD/../3rdparty/qwebdav/qwebdav.pri")
    SOURCES += $$PWD/webdavsyncer.cpp
    HEADERS += $$PWD/webdavsyncer.h
}
