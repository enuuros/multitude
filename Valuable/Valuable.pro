include(../multitude.pri)

HEADERS += ChangeMap.hpp
HEADERS += HasValues.hpp
HEADERS += ValueFloat.hpp
HEADERS += ValueInt.hpp
HEADERS += ValueNumeric.hpp
HEADERS += ValueObject.hpp
HEADERS += Valuable.hpp
HEADERS += ValueVector2.hpp
HEADERS += ValueVector4.hpp
HEADERS += ValueVector.hpp

SOURCES += ChangeMap.cpp
SOURCES += HasValues.cpp
SOURCES += ValueObject.cpp
SOURCES += Valuable.cpp

TARGET = Valuable

unix:LIBS += $$LIB_RADIANT -lxerces-c

macx:LIBS += -framework

include(../libs.pri)
include(../lib_inst.pri)

