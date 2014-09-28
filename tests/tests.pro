TEMPLATE = app
QT += testlib
CONFIG += testcase debug

include ("../global.pri")
include("../src/src.pri")

INCLUDEPATH += $$PWD/../src/

DEFINES += UNIT_TEST_RUN

SOURCES += assertingproxymodel.cpp \
           main.cpp \
           modelsignalspy.cpp \
           signalspy.cpp \
           testarchivedtasksmodel.cpp \
           testcheckabletagmodel.cpp \
           teststagedtasksmodel.cpp \
           teststorage.cpp \
           testtaskfiltermodel.cpp \
           testtag.cpp \
           testtask.cpp \
           testtagmodel.cpp \
           testwebdav.cpp

HEADERS += assertingproxymodel.h \
           modelsignalspy.h \
           signalspy.h \
           testarchivedtasksmodel.h \
           testcheckabletagmodel.h \
           testtaskfiltermodel.h \
           teststorage.h \
           testbase.h \
           teststagedtasksmodel.h \
           testtag.h \
           testtask.h \
           testtagmodel.h \
           testwebdav.h
