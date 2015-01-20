include ("../../global.pri")
include ("../distractions.pri")

QT       += dbus
CONFIG   += plugin
TARGET = pidgin
TEMPLATE = lib

HEADERS += pidgin.h
SOURCES += pidgin.cpp
