include(../multitude.pri)
include(../libs.pri)

HEADERS += VideoFFMPEG.hpp

SOURCES += VideoFFMPEG.cpp

PKGCONFIG += libavutil libavformat libavcodec

LIBS += $$LIB_RADIANT
