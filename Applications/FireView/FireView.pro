include(../Applications.pri)

HEADERS += CamView.hpp 
HEADERS += MainWindow.hpp 
HEADERS += ParamView.hpp

SOURCES += CamView.cpp 
SOURCES += MainWindow.cpp 
SOURCES += ParamView.cpp 
SOURCES += Main.cpp

TARGET = FireView

LIBS += $$LIB_RADIANT $$LIB_VALUEIO $$LIB_LUMINOUS

CONFIG += debug

CONFIG += qt

QT = core gui opengl xml

PKGCONFIG += GraphicsMagick++

target.path = $$PREFIX/bin
INSTALLS += target
