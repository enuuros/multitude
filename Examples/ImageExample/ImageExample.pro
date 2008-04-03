include(../Examples.pri)

SOURCES += Main.cpp

TARGET = ImageExample

CONFIG += link_pkgconfig
PKGCONFIG += GraphicsMagick++ sdl

LIBS += $$LIB_RADIANT $$LIB_VALUEIO $$LIB_LUMINOUS

macx:LIBS += -framework OpenGL
