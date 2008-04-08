# Simple example, that builds an application, based on components
# already installed

SOURCES += Main.cpp

TARGET = BuildExample

linux-g++:LIBS += -lNimble

macx:LIBS += -framework,Nimble

