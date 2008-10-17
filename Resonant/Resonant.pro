include(../multitude.pri)

HEADERS += Application.hpp 
HEADERS += AudioFileHandler.hpp
HEADERS += AudioLoop.hpp 
HEADERS += ControlData.hpp 
HEADERS += DSPNetwork.hpp 
HEADERS += Export.hpp
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

LIBS += $$LIB_RADIANT 

PKGCONFIG += portaudio-2.0 sndfile

include(../library.pri)

win32 {
	INCLUDEPATH += $$INC_LIBSNDFILE $$INC_PORTAUDIO $$INC_WINPORT $$INC_PTHREADS 
	LIBPATH += $$LNK_MULTITUDE $$LNK_PTHREADS $$LNK_LIBSNDFILE $$LNK_PORTAUDIO
	LIBS += $$LIB_PTHREADS $$LIB_WINPORT $$LIB_SNDFILE $$LIB_PORTAUDIO
	QMAKE_CXXFLAGS *= -wd4251		# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}
