include(../Examples.pri)

SOURCES += Main.cpp

LIBS += $$LIB_RADIANT $$LIB_RESONANT $$LIB_VALUABLE

win32 {
	include(../../Win32/WinApps.pri)
	INCLUDEPATH += $$INC_WINPORT
	LIBPATH += $$LNK_MULTITUDE
	LIBS += ws2_32.lib Win32x.lib
}
