include(../multitude.pri)

template = app

HEADERS += Factory.hpp
HEADERS += ClassLoader.hpp

SOURCES += Test.cpp

false : {
  SOURCES += plugin/PluginBase.cpp
  SOURCES += plugin/PluginSub.cpp
}

LIBS += $$LIB_RADIANT

TARGET = Test

INCLUDEPATH += ../

CONFIG -= qt
