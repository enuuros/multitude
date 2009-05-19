include(../Examples.pri)

SOURCES += Main.cpp

LIBS += $$LIB_RADIANT 

win32 {
	LIBS += -lWin32x
	CONFIG += console
}