include(../Examples.pri)

SOURCES += Main.cpp

TARGET = ValueExample

LIBS += $$LIB_RADIANT $$LIB_VALUEIO

win32 {
	include(../../Win32/WinApps.pri)
	INCLUDEPATH += $$INC_XERCES $$INC_WINPORT
	LIBPATH += $$LNK_MULTITUDE $$LNK_XERCES
	LIBS += $$LIB_XERCES
	#QMAKE_CXXFLAGS *= -wd4251	# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}