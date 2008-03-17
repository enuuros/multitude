include(../multitude.pri)

TEMPLATE = lib

HEADERS += HasValuesQT.hpp

SOURCES += HasValuesQT.cpp

TARGET = ValueQT

DEPENDPATH += ../

INCLUDEPATH += ../

DESTDIR = ../lib

CONFIG += qt

QT = core

unix:LIBS += -L../lib -lValueIO -lRadiant -lxerces-c
