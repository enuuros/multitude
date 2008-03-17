include(../multitude.pri)

TEMPLATE = lib

HEADERS += KeyStone.hpp 
HEADERS += LensCorrection.hpp 

SOURCES += KeyStone.cpp
SOURCES += LensCorrection.cpp

TARGET = Nimble

DEPENDPATH += ../

INCLUDEPATH += ../

DESTDIR = ../lib
