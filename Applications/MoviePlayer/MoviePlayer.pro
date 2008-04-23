include(../Applications.pri)

HEADERS += VideoWindow.hpp

SOURCES += VideoWindow.cpp
SOURCES += Main.cpp

TARGET = MoviePlayer

LIBS += $$LIB_VIDEODISPLAY $$LIB_DYSLEXIC $$LIB_RESONANT
LIBS += $$LIB_SCREENPLAY $$LIB_RADIANT $$LIB_VALUEIO $$LIB_LUMINOUS

CONFIG += qt
QT = core gui opengl xml

PKGCONFIG += GraphicsMagick++

target.path = $$PREFIX/bin
INSTALLS += target
