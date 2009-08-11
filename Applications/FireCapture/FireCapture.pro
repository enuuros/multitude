include(../Applications.pri)

SOURCES += Main.cpp

LIBS += $$LIB_RADIANT $$LIB_NIMBLE
LIBS += $$LIB_PATTERNS $$LIB_LUMINOUS $$LIB_VALUABLE

CONFIG -= qt

win32 {
	LIBS += -lWin32x
}

include(../Applications_end.pri)
