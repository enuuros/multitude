include(../multitude.pri)

HEADERS += Dom.hpp
HEADERS += HasValues.hpp
HEADERS += IO.hpp
HEADERS += DomStore.hpp

SOURCES += Dom.cpp
SOURCES += HasValues.cpp
SOURCES += IO.cpp

unix:LIBS += -L../lib $$LIB_RADIANT -lxerces-c

include(../library.pri)

win32 {
	INCLUDEPATH += $$INC_XERCES $$INC_WINPORT
	QMAKE_CXXFLAGS += -Zc:wchar_t		# treat wchar_t as a builtin type
	CONFIG += staticlib
}
