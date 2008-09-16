include(../Applications.pri)

HEADERS += CamView.hpp 
HEADERS += MainWindow.hpp 
HEADERS += ParamView.hpp

SOURCES += CamView.cpp 
SOURCES += MainWindow.cpp 
SOURCES += ParamView.cpp 
SOURCES += Main.cpp

LIBS += $$LIB_RADIANT $$LIB_LUMINOUS $$LIB_VALUABLE

CONFIG += qt

QT = core gui opengl xml

PKGCONFIG += GraphicsMagick++

win32 {
	include(../../Win32/WinApps.pri)
	INCLUDEPATH += $$INC_GLEW $$INC_WINPORT $$INC_MAGICK $$INC_PTHREADS $$INC_LIBDC1394 $$INC_LIBDC1394
	LIBPATH += $$LNK_MULTITUDE
	LIBS += $$LIB_WINPORT
	LIBS -= -framework,Cocoa
	QMAKE_CXXFLAGS *= -wd4251	# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}

include(../Applications_end.pri)
