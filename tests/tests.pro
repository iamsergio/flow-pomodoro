TEMPLATE = app
QT += testlib
CONFIG += testcase

include ("../global.pri")
include("../src/src.pri")

INCLUDEPATH += $$PWD/../src/

SOURCES += main.cpp \
           modelsignalspy.cpp \
           signalspy.cpp \
           testcheckabletagmodel.cpp \
           teststorage.cpp \
           testtag.cpp \
           testtask.cpp \
           testtagmodel.cpp
    
HEADERS += modelsignalspy.h \
           signalspy.h \
           testcheckabletagmodel.h \
           teststorage.h \
           testbase.h \
           testtag.h \
           testtask.h \
           testtagmodel.h
