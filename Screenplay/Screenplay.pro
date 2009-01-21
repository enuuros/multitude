include(../multitude.pri)

HEADERS += Export.hpp
HEADERS += VideoFFMPEG.hpp

SOURCES += VideoFFMPEG.cpp

unix:PKGCONFIG += libavutil libavformat libavcodec

LIBS += $$LIB_RADIANT

win32 {
	DEFINES += SCREENPLAY_EXPORT
	INCLUDEPATH += ../Win32x/include/ffmpeg
	LIBS += -lavcodec -lavutil -lavformat
}

include(../library.pri)
