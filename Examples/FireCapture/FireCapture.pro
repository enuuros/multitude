include(../Examples.pri)

SOURCES += Main.cpp

LIBS += $$LIB_RADIANT $$LIB_NIMBLE
LIBS += $$LIB_PATTERNS

CONFIG -= qt

win32 {
	LIBS += -lWin32x
}

include(../Examples_end.pri)
