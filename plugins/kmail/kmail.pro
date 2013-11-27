include ("../../global.pri")

QT       += qml quick dbus
CONFIG   += plugin
TARGET = kmail
TEMPLATE = lib

HEADERS += kmail.h
SOURCES += kmail.cpp

INCLUDEPATH += ../../src
DESTDIR = ../
