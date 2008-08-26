include(../Applications.pri)

SOURCES += Main.cpp

# SOURCES += Main.cpp

TARGET = Watcher

DEPENDPATH += ../../

unix {
  BASEPATH=/usr
}

macx {
  BASEPATH=/usr/local
}

INCLUDEPATH += ../../ $${BASEPATH}/include/ $${BASEPATH}/include/FTGL/ $${BASEPATH}/include/freetype2 $${BASEPATH}/include/GraphicsMagick


LIBS += $$LIB_RADIANT

PKGCONFIG += GraphicsMagick++

CONFIG -= qt
# CONFIG -= debug

QT = core gui opengl xml

win32 {
	include(../../Win32/WinApps.pri)
	INCLUDEPATH += $$INC_WINPORT $$INC_LIBDC1394 $$INC_MAGICK
	LIBPATH += $$LNK_MULTITUDE $$LNK_MAGICK
	LIBS += $$LIB_WINPORT $$LIB_MAGICK
	QMAKE_LFLAGS += /SUBSYSTEM:CONSOLE
	QMAKE_CXXFLAGS *= -wd4251	# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}
