TEMPLATE = app
QT += testlib
CONFIG += testcase

include("../src/src.pri")
include ("../global.pri")

INCLUDEPATH += $$PWD/../src/

SOURCES += main.cpp \
           signalspy.cpp \
           teststorage.cpp \
           #testtask.cpp
    
HEADERS += signalspy.h \
           teststorage.h \
           #testtask.h
