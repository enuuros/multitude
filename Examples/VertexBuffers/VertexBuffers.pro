include(../Examples.pri)

SOURCES += Main.cpp

CONFIG += link_pkgconfig
PKGCONFIG += sdl GraphicsMagick++

LIBS += $$LIB_LUMINOUS 

macx:LIBS += -framework OpenGL

win32 {
	include(../../Win32/WinApps.pri)
	INCLUDEPATH += $$INC_SDL $$INC_GLEW $$INC_MAGICK
	LIBPATH += $$LNK_MULTITUDE $$LNK_MAGICK $$LNK_SDL
	LIBS += $$LIB_MAGICK $$LIB_SDL $$LIB_OPENGL $$LIB_SDL
	QMAKE_CXXFLAGS *= -wd4251	# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}