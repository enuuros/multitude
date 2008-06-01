include(../Examples.pri)

SOURCES += Main.cpp

TARGET = SamplePlayer

LIBS += $$LIB_RADIANT $$LIB_RESONANT

win32 {
	include(../../Win32/WinApps.pri)
	INCLUDEPATH += $$INC_WINPORT
	LIBPATH += $$LNK_MULTITUDE
	LIBS += ws2_32.lib
}
