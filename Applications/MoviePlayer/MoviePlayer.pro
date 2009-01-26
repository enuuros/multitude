include(../Applications.pri)

HEADERS += VideoWindow.hpp

SOURCES += VideoWindow.cpp
SOURCES += Main.cpp

LIBS += $$LIB_VIDEODISPLAY $$LIB_POETIC $$LIB_RESONANT
LIBS += $$LIB_SCREENPLAY $$LIB_RADIANT $$LIB_LUMINOUS
LIBS += $$LIB_VALUABLE

CONFIG += qt

QT = core gui opengl xml

win32 {
	INCLUDEPATH += ../../Win32x/include/portaudio
}

include(../Applications_end.pri)
