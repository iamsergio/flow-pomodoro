include ("../../global.pri")

CONFIG   += plugin
TARGET = shellscript
TEMPLATE = lib

HEADERS += shellscript.h
SOURCES += shellscript.cpp

INCLUDEPATH += ../../src/
DESTDIR = ../
