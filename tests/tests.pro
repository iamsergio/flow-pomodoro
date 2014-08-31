TEMPLATE = app
QT += testlib
CONFIG += testcase

include("../src/src.pri")
include ("../global.pri")

INCLUDEPATH += $$PWD/../src/

SOURCES += main.cpp \
           signalspy.cpp \
           teststorage.cpp \
           testtag.cpp \
           testtask.cpp
    
HEADERS += signalspy.h \
           teststorage.h \
           testbase.h \
           testtag.h \
           testtask.h
