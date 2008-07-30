
include(../multitude.pri)

HEADERS += ChangeMap.hpp
HEADERS += HasValues.hpp
HEADERS += HasValuesImpl.hpp
HEADERS += Valuable.hpp
HEADERS += ValueColor.hpp
HEADERS += ValueFloat.hpp
HEADERS += ValueInt.hpp
HEADERS += ValueListener.hpp
HEADERS += ValueNumeric.hpp
HEADERS += ValueObject.hpp
HEADERS += ValueString.hpp
HEADERS += ValueStringImpl.hpp
HEADERS += ValueVector.hpp
HEADERS += ValueVectorImpl.hpp

SOURCES += ChangeMap.cpp
SOURCES += HasValues.cpp
SOURCES += Valuable.cpp
SOURCES += ValueColor.cpp
SOURCES += ValueListener.cpp
SOURCES += ValueObject.cpp
SOURCES += ValueString.cpp
SOURCES += ValueVector.cpp

TARGET = Valuable

# PKGCONFIG += xercesc

unix:LIBS += $$LIB_RADIANT -lxerces-c

include(../libs.pri)
include(../lib_inst.pri)

