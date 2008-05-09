TEMPLATE = lib

HEADERS += PluginBase.hpp
HEADERS += PluginSub.hpp

SOURCES += PluginBase.cpp
SOURCES += PluginSub.cpp
SOURCES += PluginExport.cpp

TARGET = Plugin

INCLUDEPATH += ../../

DEPENDPATH += ../

CONFIG -= qt

QMAKE_LFLAGS += -rdynamic
