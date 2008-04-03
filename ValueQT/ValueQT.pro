include(../multitude.pri)
include(../libs.pri)

HEADERS += HasValuesQT.hpp

SOURCES += HasValuesQT.cpp

TARGET = ValueQT

CONFIG += qt

QT = core

unix:LIBS += $$LIB_VALUEIO -lxerces-c
