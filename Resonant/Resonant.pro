include(../multitude.pri)

TEMPLATE = lib

HEADERS += Application.hpp 
HEADERS += AudioFileHandler.hpp
HEADERS += AudioLoop.hpp 
HEADERS += ControlData.hpp 
HEADERS += DSPNetwork.hpp 
HEADERS += Module.hpp 
HEADERS += ModuleFilePlay.hpp 
HEADERS += ModuleGain.hpp 
HEADERS += ModuleOutCollect.hpp 
HEADERS += ModuleSamplePlayer.hpp 

SOURCES += Application.cpp
SOURCES += AudioFileHandler.cpp
SOURCES += AudioLoop.cpp
SOURCES += ControlData.cpp
SOURCES += DSPNetwork.cpp
SOURCES += Module.cpp
SOURCES += ModuleFilePlay.cpp
SOURCES += ModuleGain.cpp
SOURCES += ModuleOutCollect.cpp
SOURCES += ModuleSamplePlayer.cpp

linux-g++ {
  HEADERS += AudioDevice.hpp 
  SOURCES += AudioDevice.cpp
}

TARGET = Resonant

DEPENDPATH += ../

INCLUDEPATH += ../ 

DESTDIR = ../lib

LIBS += -L../lib -lRadiant 

PKGCONFIG += portaudio-2.0 sndfile


