include(../multitude.pri)

SOURCES += dirent.c
SOURCES += gettimeofday.c
SOURCES += nanosleep.c
SOURCES += WinPort.cpp

INCLUDEPATH += include

LIBS += wsock32.lib

DEFINES += WINPORT_EXPORT

# Install windows 3rd party dlls to bin
win32_libs.path = /bin
win32_libs.files = lib/*.dll

win32_dev_libs.path = /src/MultiTouch/multitude/$$TARGET/lib
win32_dev_libs.files = lib/*.lib

win32_extra_headers.path = /src/MultiTouch/multitude/$$TARGET/include
win32_extra_headers.files = include/*

INSTALLS += win32_libs win32_extra_headers win32_dev_libs

include(../library.pri)
