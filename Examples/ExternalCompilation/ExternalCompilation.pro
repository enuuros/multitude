TARGET = ExternalCompilation

SOURCES += Main.cpp

CONFIG -= qt

linux-g++:LIBS += -lNimble -lRadiant -lVideoDisplay

macx:LIBS += -framework,Nimble -framework,Radiant -framework,VideoDisplay

CONFIG += link_pkgconfig

PKGCONFIG += GraphicsMagick++ libavutil


