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

unix: PKGCONFIG += portaudio-2.0 sndfile

include(../library.pri)

win32 {
	DEFINES += RESONANT_EXPORT
	INCLUDEPATH += ..\Win32x\include\portaudio ..\Win32x\include\libsndfile
	LIBS += -llibsndfile-1 -lportaudio_x86 -lWin32x
	QMAKE_CXXFLAGS += -Zc:wchar_t
}
