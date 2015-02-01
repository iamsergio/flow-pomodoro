TEMPLATE = app
QT += testlib widgets
CONFIG += testcase debug
DEFINES += UNIT_TEST_RUN

include ("../global.pri")
include("../src/src.pri")

DEFINES-=DEVELOPER_MODE

INCLUDEPATH += $$PWD/../src/

SOURCES += assertingproxymodel.cpp \
           main.cpp \
           modelsignalspy.cpp \
           quick/testui.cpp \
           signalspy.cpp \
           testarchivedtasksmodel.cpp \
           testbase.cpp \
           testcheckabletagmodel.cpp \
           teststagedtasksmodel.cpp \
           teststorage.cpp \
           testtaskfiltermodel.cpp \
           testtag.cpp \
           testtask.cpp \
           testtagmodel.cpp

!contains(DEFINES, NO_WEBDAV) {
    SOURCES += testwebdav.cpp
}

!contains(DEFINES, NO_WEBDAV) {
    HEADERS += testwebdav.h
}

HEADERS += assertingproxymodel.h \
           modelsignalspy.h \
           quick/testui.h \
           signalspy.h \
           testarchivedtasksmodel.h \
           testcheckabletagmodel.h \
           testtaskfiltermodel.h \
           teststorage.h \
           testbase.h \
           teststagedtasksmodel.h \
           testtag.h \
           testtask.h \
           testtagmodel.h
