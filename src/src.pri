QT += quick

SOURCES += $$PWD/archivedtasksfiltermodel.cpp \
           $$PWD/checkabletagmodel.cpp \
           $$PWD/circularprogressindicator.cpp \
           $$PWD/controller.cpp  \
           $$PWD/imageprovider.cpp \
           $$PWD/jsonstorage.cpp \
           $$PWD/pluginmodel.cpp \
           $$PWD/quickview.cpp   \
           $$PWD/settings.cpp    \
           $$PWD/sortedtagsmodel.cpp \
           $$PWD/storage.cpp \
           $$PWD/tag.cpp \
           $$PWD/tagref.cpp \
           $$PWD/task.cpp \
           $$PWD/taskfilterproxymodel.cpp \
           $$PWD/tooltipcontroller.cpp

HEADERS += $$PWD/archivedtasksfiltermodel.h \
           $$PWD/checkabletagmodel.h \
           $$PWD/circularprogressindicator.h \
           $$PWD/controller.h      \
           $$PWD/imageprovider.h \
           $$PWD/jsonstorage.h      \
           $$PWD/genericlistmodel.h \
           $$PWD/plugininterface.h \
           $$PWD/pluginmodel.h     \
           $$PWD/quickview.h       \
           $$PWD/settings.h        \
           $$PWD/sortedtagsmodel.h \
           $$PWD/storage.h \
           $$PWD/tag.h \
           $$PWD/tagref.h \
           $$PWD/task.h            \
           $$PWD/taskfilterproxymodel.h \
           $$PWD/tooltipcontroller.h

#DEFINES += NO_WEBDAV

!contains(DEFINES, NO_WEBDAV) {
    include("$$PWD/../3rdparty/qwebdav/qwebdav.pri")
    SOURCES += $$PWD/webdavsyncer.cpp
    HEADERS += $$PWD/webdavsyncer.h
}
