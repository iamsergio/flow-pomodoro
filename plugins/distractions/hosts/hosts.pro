include ("../../global.pri")
include ("../distractions.pri")

CONFIG   += plugin
TARGET = hosts
TEMPLATE = lib
QT += qml quick

HEADERS += hosts.h
SOURCES += hosts.cpp

RESOURCES += hostsplugin.qrc
