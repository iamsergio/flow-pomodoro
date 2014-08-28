TEMPLATE = app
QT += testlib
CONFIG += testcase

include("../src/src.pri")
include ("../global.pri")

INCLUDEPATH += $$PWD/../src/

SOURCES += tests.cpp
HEADERS += tests.h
