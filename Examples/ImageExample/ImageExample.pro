include(../Examples.pri)

SOURCES += Main.cpp

unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += sdl
}

LIBS += $$LIB_RADIANT $$LIB_LUMINOUS $$LIB_VALUABLE

win32 {
	LIBS += -lSDL -lSDLmain $$LIB_OPENGL
}
