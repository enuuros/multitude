include(../../multitude.pri)
include(../Applications.pri)

HEADERS += VideoWindow.hpp

SOURCES += VideoWindow.cpp
SOURCES += Main.cpp

TARGET = MoviePlayer

LIBS += -lVideoDisplay

CONFIG += qt
QT = core gui opengl xml

PKGCONFIG += GraphicsMagick++
