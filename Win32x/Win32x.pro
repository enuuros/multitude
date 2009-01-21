include(../multitude.pri)

SOURCES += dirent.c
SOURCES += gettimeofday.c
SOURCES += nanosleep.c
SOURCES += WinPort.cpp

INCLUDEPATH += include

LIBS += wsock32.lib

DEFINES += WINPORT_EXPORT

include(../library.pri)