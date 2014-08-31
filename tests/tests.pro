TEMPLATE = app
QT += testlib
CONFIG += testcase

include("../src/src.pri")
include ("../global.pri")

INCLUDEPATH += $$PWD/../src/

SOURCES += signalspy.cpp \
           tests.cpp 
    
HEADERS += signalspy.h \
           tests.h
