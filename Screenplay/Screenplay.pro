include(../multitude.pri)

HEADERS += VideoFFMPEG.hpp

SOURCES += VideoFFMPEG.cpp

PKGCONFIG += libavutil libavformat libavcodec

LIBS += $$LIB_RADIANT

include(../libs.pri)
