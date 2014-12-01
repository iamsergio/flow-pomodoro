include ("../../global.pri")
include ("../plugins.pri")

QT       += dbus
CONFIG   += plugin
TARGET = pidgin
TEMPLATE = lib

HEADERS += pidgin.h
SOURCES += pidgin.cpp

INCLUDEPATH += ../../src/
DESTDIR = ../
