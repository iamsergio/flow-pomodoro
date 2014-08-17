QT += network xml

# Enable DEBUG output with qDebug()
DEFINES += DEBUG_WEBDAV

# Enable extended WebDAV properties (see QWebDavItem.h/cpp)
#DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

SOURCES += $$PWD/qwebdav.cpp \
           $$PWD/qwebdavitem.cpp \
           $$PWD/qwebdavdirparser.cpp \
           $$PWD/qnaturalsort.cpp

HEADERS += $$PWD/qwebdav.h \
           $$PWD/qwebdavitem.h \
           $$PWD/qwebdavdirparser.h \
           $$PWD/qnaturalsort.h \
           $$PWD/qwebdav_global.h

INCLUDEPATH += $$PWD

*gcc*|*clang* {
    QMAKE_CXXFLAGS -= -Werror
}
