SOURCES += Main.cpp

INCLUDEPATH += ../../

TARGET = ImageExample

LIBS += -L../../lib -lLuminous

CONFIG += link_pkgconfig
PKGCONFIG += GraphicsMagick++ sdl
