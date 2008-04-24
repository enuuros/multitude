include(../multitude.pri)

HEADERS += HasValuesQT.hpp

SOURCES += HasValuesQT.cpp

TARGET = ValueQT

CONFIG += qt

QT = core

unix:LIBS += $$LIB_VALUEIO -lxerces-c

include(../libs.pri)

include(../lib_inst.pri)
