include ("../../global.pri")
include ("../plugins.pri")

CONFIG   += plugin
TARGET = hosts
TEMPLATE = lib
QT += qml quick

HEADERS += hosts.h
SOURCES += hosts.cpp

RESOURCES += hostsplugin.qrc

INCLUDEPATH += ../../src/
DESTDIR = ../
