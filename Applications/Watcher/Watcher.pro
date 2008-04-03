include(../Applications.pri)

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


LIBS += $$LIB_RADIANT

PKGCONFIG += GraphicsMagick++

CONFIG -= qt
# CONFIG -= debug

QT = core gui opengl xml
