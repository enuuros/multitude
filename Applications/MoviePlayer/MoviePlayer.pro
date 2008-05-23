include(../Applications.pri)

HEADERS += VideoWindow.hpp

SOURCES += VideoWindow.cpp
SOURCES += Main.cpp

TARGET = MoviePlayer

LIBS += $$LIB_VIDEODISPLAY $$LIB_DYSLEXIC $$LIB_RESONANT
LIBS += $$LIB_SCREENPLAY $$LIB_RADIANT $$LIB_VALUEIO $$LIB_LUMINOUS

CONFIG += qt
QT = core gui opengl xml

PKGCONFIG += GraphicsMagick++

target.path = $$PREFIX/bin
INSTALLS += target

win32 {
	include(../../Win32/WinApps.pri)
	INCLUDEPATH += $$INC_PORTAUDIO $$INC_WINPORT $$INC_PTHREADS $$INC_GLEW $$INC_MAGICK $$INC_FFMPEG
	LIBPATH += $$LNK_MULTITUDE
	QMAKE_CXXFLAGS *= -wd4251	# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}
