include(../../multitude.pri)
include(../Applications.pri)

HEADERS += CamView.hpp 
HEADERS += MainWindow.hpp 
HEADERS += ParamView.hpp

SOURCES += CamView.cpp 
SOURCES += MainWindow.cpp 
SOURCES += ParamView.cpp 
SOURCES += Main.cpp

TARGET = FireView

LIBS += -L../../lib -lRadiant -lLuminous -lValueIO -lRadiant

CONFIG += qt

QT = core gui opengl xml

PKGCONFIG += GraphicsMagick++
