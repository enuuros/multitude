include(../multitude.pri)

HEADERS += ChangeMap.hpp
HEADERS += DOMDocument.hpp
HEADERS += DOMElement.hpp
HEADERS += Export.hpp
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
HEADERS += ValueRect.hpp
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
SOURCES += ValueRect.cpp
SOURCES += ValueString.cpp
SOURCES += ValueVector.cpp

# PKGCONFIG += xercesc

LIBS += $$LIB_RADIANT $$LIB_NIMBLE
unix: LIBS += -lxerces-c

win32 {
	DEFINES += VALUABLE_EXPORT
	LIBS += xerces-c_2.lib
	QMAKE_CXXFLAGS += -Zc:wchar_t
}

include(../library.pri)
