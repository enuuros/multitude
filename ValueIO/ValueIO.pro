include(../multitude.pri)

TEMPLATE = lib

HEADERS += Dom.hpp
HEADERS += HasValues.hpp
HEADERS += IO.hpp

SOURCES += Dom.cpp
SOURCES += HasValues.cpp
SOURCES += IO.cpp

TARGET = ValueIO

DEPENDPATH += ../

INCLUDEPATH += ../

DESTDIR = ../lib

unix:LIBS += -L../lib -lRadiant -lxerces-c

