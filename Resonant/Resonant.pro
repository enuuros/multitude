include(../multitude.pri)

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

LIBS += $$LIB_RADIANT 

linux-g++:LIBS += -lasound

PKGCONFIG += portaudio-2.0 sndfile

include(../libs.pri)

include(../lib_inst.pri)

win32 {
	INCLUDEPATH += $$INC_LIBSNDFILE $$INC_PORTAUDIO $$INC_WINPORT $$INC_PTHREADS $$INC_MAGICK
	LIBPATH += $$LNK_MULTITUDE $$LNK_PTHREADS $$LNK_LIBSNDFILE $$LNK_PORTAUDIO
	LIBS += $$LIB_PTHREADS $$LIB_WINPORT $$LIB_SNDFILE $$LIB_PORTAUDIO
	QMAKE_CXXFLAGS *= -wd4251		# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}