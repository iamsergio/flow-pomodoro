include ("../../global.pri")

QT       += dbus
CONFIG   += plugin
TARGET = shellscript
TEMPLATE = lib

HEADERS += shellscript.h
SOURCES += shellscript.cpp

INCLUDEPATH += ../../src/
DESTDIR = ../
