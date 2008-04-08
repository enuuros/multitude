include(../multitude.pri)
include(../libs.pri)

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
LIBS += $$LIB_RADIANT $$LIB_DYSLEXIC -lGLEW -lGL

macx:LIBS += -framework,OpenGL
