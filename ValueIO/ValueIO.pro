include(../multitude.pri)

HEADERS += Dom.hpp
HEADERS += HasValues.hpp
HEADERS += IO.hpp

SOURCES += Dom.cpp
SOURCES += HasValues.cpp
SOURCES += IO.cpp

TARGET = ValueIO

unix:LIBS += -L../lib $$LIB_RADIANT -lxerces-c

include(../libs.pri)
