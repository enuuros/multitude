include(../Examples.pri)

SOURCES += Main.cpp

TARGET = SocketExample

INCLUDEPATH += ../../
LIBS += $$LIB_RADIANT 

win32 {
	include(../../Win32/WinApps.pri)
	INCLUDEPATH += $$INC_WINPORT
	LIBPATH += $$LNK_MULTITUDE
	LIBS += ws2_32.lib
}
