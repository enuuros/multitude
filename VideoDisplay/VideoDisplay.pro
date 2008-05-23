include(../multitude.pri)

HEADERS += AudioTransfer.hpp
HEADERS += ShowGL.hpp
HEADERS += SubTitles.hpp
HEADERS += VideoIn.hpp
HEADERS += VideoInFFMPEG.hpp

SOURCES += AudioTransfer.cpp
SOURCES += ShowGL.cpp
SOURCES += SubTitles.cpp
SOURCES += VideoIn.cpp
SOURCES += VideoInFFMPEG.cpp

TARGET = VideoDisplay

# $${BASEPATH}/include/ffmpeg
#INCLUDEPATH += $${BASEPATH}/include/GraphicsMagick

PKGCONFIG += GraphicsMagick++ libavutil

DEFINES += __STDC_CONSTANT_MACROS 

LIBS += $$LIB_RESONANT $$LIB_SCREENPLAY $$LIB_LUMINOUS $$LIB_VALUEIO 
LIBS += $$LIB_RADIANT $$LIB_DYSLEXIC -lGLEW $$LIB_OPENGL $$LIB_RESONANT
macx:LIBS += -framework,OpenGL

include(../libs.pri)

include(../lib_inst.pri)

win32 {
	INCLUDEPATH += $$INC_WINPORT $$INC_GLEW $$INC_PTHREADS $$INC_FFMPEG $$INC_MAGICK $$INC_PORTAUDIO
	LIBPATH += $$LNK_MULTITUDE $$LNK_GLEW $$LNK_PTHREADS $$LNK_MULTITUDE
	LIBS += $$LIB_GLEW $$LIB_OPENGL $$LIB_WINPORT $$LIB_PTHREADS
	LIBS -= -lGL
	QMAKE_CXXFLAGS *= -wd4251		# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}