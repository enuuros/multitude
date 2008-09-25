include(../multitude.pri)

HEADERS += ChangeMap.hpp
HEADERS += DOMDocument.hpp
HEADERS += DOMElement.hpp
HEADERS += HasValues.hpp
HEADERS += HasValuesImpl.hpp
HEADERS += Valuable.hpp
HEADERS += ValueColor.hpp
HEADERS += ValueFloat.hpp
HEADERS += ValueFloatImpl.hpp
HEADERS += ValueInt.hpp
HEADERS += ValueIntImpl.hpp
HEADERS += ValueListener.hpp
HEADERS += ValueNumeric.hpp
HEADERS += ValueObject.hpp
HEADERS += ValueString.hpp
HEADERS += ValueStringImpl.hpp
HEADERS += ValueVector.hpp
HEADERS += ValueVectorImpl.hpp

SOURCES += ChangeMap.cpp
SOURCES += DOMDocument.cpp
SOURCES += DOMElement.cpp
SOURCES += HasValues.cpp
SOURCES += Valuable.cpp
SOURCES += ValueColor.cpp
SOURCES += ValueFloat.cpp
SOURCES += ValueInt.cpp
SOURCES += ValueListener.cpp
SOURCES += ValueObject.cpp
SOURCES += ValueString.cpp
SOURCES += ValueVector.cpp

# PKGCONFIG += xercesc

unix:LIBS += $$LIB_RADIANT -lxerces-c

include(../library.pri)

