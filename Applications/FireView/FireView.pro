include(../Applications.pri)

CONFIG += debug

HEADERS += CamView.hpp 
HEADERS += MainWindow.hpp 
HEADERS += ParamView.hpp

SOURCES += CamView.cpp 
SOURCES += MainWindow.cpp 
SOURCES += ParamView.cpp 
SOURCES += Main.cpp

LIBS += $$LIB_RADIANT $$LIB_LUMINOUS $$LIB_VALUABLE $$LIB_NIMBLE
LIBS += $$LIB_PATTERNS

CONFIG += qt

QT = core gui opengl xml

win32 {
	LIBS += -lWin32x
}

include(../Applications_end.pri)
