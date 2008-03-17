include(../../multitude.pri)
include(../Applications.pri)

HEADERS += CamView.hpp MainWindow.hpp ParamView.hpp

SOURCES += CamView.cpp MainWindow.cpp ParamView.cpp Main.cpp

TARGET = FireView

LIBS += -L../../lib -lRadiant -lLuminous -lValueIO -lRadiant

CONFIG += qt

QT = core gui opengl xml

PKGCONFIG += GraphicsMagick++
