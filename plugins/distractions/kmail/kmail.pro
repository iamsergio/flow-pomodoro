include ("../../global.pri")
include ("../distractions.pri")

QT       += qml quick dbus
CONFIG   += plugin
TARGET = kmail
TEMPLATE = lib

HEADERS += kmail.h
SOURCES += kmail.cpp
