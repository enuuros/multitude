include(../Examples.pri)

SOURCES += Main.cpp

LIBS += $$LIB_RADIANT $$LIB_RESONANT $$LIB_VALUABLE $$LIB_PATTERNS $$LIB_NIMBLE

win32 {
	INCLUDEPATH += $$WINPORT_INCLUDE\libsndfile
	LIBS += -lWin32x -llibsndfile-1
}