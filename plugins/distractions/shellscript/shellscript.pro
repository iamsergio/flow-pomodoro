include ("../../global.pri")
include ("../distractions.pri")

CONFIG   += plugin
TARGET = shellscript
TEMPLATE = lib

HEADERS += shellscript.h
SOURCES += shellscript.cpp

DESTDIR = ../

RESOURCES += shellscriptplugin.qrc

QT += quick

OTHER_FILES += Config.qml
