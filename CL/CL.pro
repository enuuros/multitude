include(../multitude.pri)

template = app

HEADERS += Factory.hpp
HEADERS += ClassLoader.hpp

SOURCES += Test.cpp

HEADERS += plugin/PluginBase.hpp
SOURCES += plugin/PluginBase.cpp

LIBS += $$LIB_RADIANT

TARGET = Test

INCLUDEPATH += ../

CONFIG -= qt
