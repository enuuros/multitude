include(../multitude.pri)

TEMPLATE = lib

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

DEPENDPATH += ../

BASEPATH = /usr

macx {
  BASEPATH = /usr/local
}

INCLUDEPATH += ../ $${BASEPATH}/include/ffmpeg
INCLUDEPATH += $${BASEPATH}/include/GraphicsMagick

DEFINES += __STDC_CONSTANT_MACROS 

LIBS += -L../lib -lResonant -lDyslexic -lLuminous -lValueIO  -lRadiant -lGLEW 

DESTDIR = ../lib

macx:LIBS += -framework,OpenGL
