include(../Examples.pri)

SOURCES += Main.cpp

LIBS += $$LIB_RADIANT $$LIB_VALUABLE $$LIB_PATTERNS

win32 {
	LIBS += -lWin32x
	CONFIG += console
}
