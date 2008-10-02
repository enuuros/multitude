include(../multitude.pri)

HEADERS += Export.hpp
HEADERS += VideoFFMPEG.hpp

SOURCES += VideoFFMPEG.cpp

PKGCONFIG += libavutil libavformat libavcodec

LIBS += $$LIB_RADIANT

include(../library.pri)

win32 {
	INCLUDEPATH += $$INC_WINPORT $$INC_FFMPEG
	LIBPATH += $$LNK_MULTITUDE $$LNK_FFMPEG
	LIBS += $$LIB_WINPORT $$LIB_FFMPEG 
	QMAKE_CXXFLAGS *= -wd4251		# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}
