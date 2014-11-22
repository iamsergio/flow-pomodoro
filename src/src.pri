QT += quick

SOURCES += $$PWD/archivedtasksfiltermodel.cpp \
           $$PWD/checkabletagmodel.cpp \
           $$PWD/circularprogressindicator.cpp \
           $$PWD/checkbox.cpp \
           $$PWD/controller.cpp  \
           $$PWD/jsonstorage.cpp \
           $$PWD/kernel.cpp \
           $$PWD/loadmanager.cpp \
           $$PWD/pluginmodel.cpp \
           $$PWD/runtimeconfiguration.cpp \
           $$PWD/settings.cpp    \
           $$PWD/sortedtagsmodel.cpp \
           $$PWD/storage.cpp \
           $$PWD/syncable.cpp \
           $$PWD/tag.cpp \
           $$PWD/tagref.cpp \
           $$PWD/task.cpp \
           $$PWD/taskcontextmenumodel.cpp \
           $$PWD/taskfilterproxymodel.cpp \
           $$PWD/tooltipcontroller.cpp \
           $$PWD/utils.cpp

HEADERS += $$PWD/archivedtasksfiltermodel.h \
           $$PWD/checkabletagmodel.h \
           $$PWD/checkbox.h \
           $$PWD/circularprogressindicator.h \
           $$PWD/controller.h      \
           $$PWD/jsonstorage.h      \
           $$PWD/kernel.h \
           $$PWD/loadmanager.h \
           $$PWD/genericlistmodel.h \
           $$PWD/plugininterface.h \
           $$PWD/pluginmodel.h     \
           $$PWD/runtimeconfiguration.h \
           $$PWD/settings.h        \
           $$PWD/sortedtagsmodel.h \
           $$PWD/storage.h \
           $$PWD/syncable.h \
           $$PWD/tag.h \
           $$PWD/tagref.h \
           $$PWD/task.h \
           $$PWD/taskcontextmenumodel.h \
           $$PWD/taskfilterproxymodel.h \
           $$PWD/tooltipcontroller.h \
           $$PWD/utils.h

!contains(DEFINES, NO_WEBDAV) {
    include("$$PWD/../3rdparty/qwebdav/qwebdav.pri")
    SOURCES += $$PWD/webdavsyncer.cpp
    HEADERS += $$PWD/webdavsyncer.h
}

version.target = version.h
version.commands = $$PWD/generate_version.sh $$PWD
version.depends = $$PWD/../.git

QMAKE_EXTRA_TARGETS += version
PRE_TARGETDEPS += version.h
