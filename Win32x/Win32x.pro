include(../multitude.pri)

SOURCES += dirent.c
SOURCES += gettimeofday.c
SOURCES += nanosleep.c
SOURCES += WinPort.cpp

INCLUDEPATH += include

LIBS += wsock32.lib

DEFINES += WINPORT_EXPORT

# Install windows 3rd party dlls to bin
win32 {
	win32_libs.path = /bin
	win32_libs.files = lib/*.dll
	
	INSTALLS += win32_libs
}

include(../library.pri)