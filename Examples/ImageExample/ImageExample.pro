include(../Examples.pri)

SOURCES += Main.cpp

TARGET = ImageExample

CONFIG += link_pkgconfig
PKGCONFIG += GraphicsMagick++ sdl

LIBS += -lValueIO -lRadiant
