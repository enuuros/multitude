include(../multitude.pri)

INCLUDEPATH += ../../
DEPENDPATH += ../../

LIBS += $${MULTI_LIB_FLAG}../../lib

macx:LIBS += -framework,Cocoa

win32 {
	include(../../Win32/WinApps.pri)
	INCLUDEPATH += $$INC_WINPORT
	LIBPATH += $$LNK_MULTITUDE
	LIBS += ws2_32.lib
}


