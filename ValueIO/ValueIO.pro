include(../multitude.pri)

HEADERS += Dom.hpp
HEADERS += HasValues.hpp
HEADERS += IO.hpp

SOURCES += Dom.cpp
SOURCES += HasValues.cpp
SOURCES += IO.cpp

TARGET = ValueIO

unix:LIBS += -L../lib $$LIB_RADIANT -lxerces-c

include(../libs.pri)

include(../lib_inst.pri)

win32 {
	INCLUDEPATH += $$INC_XERCES $$INC_WINPORT
	QMAKE_CXXFLAGS += -Zc:wchar_t		# treat wchar_t as a builtin type
	CONFIG += staticlib
}