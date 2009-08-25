include(../Applications.pri)

SOURCES += Main.cpp

LIBS += $$LIB_RADIANT $$LIB_NIMBLE
LIBS += $$LIB_PATTERNS $$LIB_LUMINOUS $$LIB_VALUABLE

unix: PKGCONFIG += portaudio-2.0
CONFIG -= qt

win32 {
	INCLUDEPATH += ..\Win32x\include\portaudio ..\Win32x\include\libsndfile
	LIBS += -llibsndfile-1 -lportaudio_x86 -lWin32x
	QMAKE_CXXFLAGS += -Zc:wchar_t
}

include(../Applications_end.pri)
