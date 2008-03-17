

SOURCES += Main.cpp

# SOURCES += Main.cpp

TARGET = Watcher

DEPENDPATH += ../../

linux-g++ {
  BASEPATH=/usr
}

macx {
  BASEPATH=/usr/local
}

INCLUDEPATH += ../../ $${BASEPATH}/include/ $${BASEPATH}/include/FTGL/ $${BASEPATH}/include/freetype2 $${BASEPATH}/include/GraphicsMagick


LIBS += -L../../lib -lRadiant  -lGraphicsMagick++ 

CONFIG -= qt
# CONFIG -= debug

QT = core gui opengl xml
