include(../multitude.pri)

TEMPLATE = lib

HEADERS += BBox.hpp
HEADERS += Charmap.hpp
HEADERS += CPUBitmapFont.hpp
HEADERS += CPUBitmapGlyph.hpp
HEADERS += CPUFont.hpp
HEADERS += Dyslexic.hpp
HEADERS += Face.hpp
HEADERS += GlyphContainer.hpp
HEADERS += Glyph.hpp
HEADERS += GPUFont.hpp
HEADERS += Size.hpp
HEADERS += GPUTextureFont.hpp
HEADERS += GPUTextureGlyph.hpp
HEADERS += Utils.hpp

SOURCES += BBox.cpp
SOURCES += Charmap.cpp
SOURCES += CPUBitmapFont.cpp
SOURCES += CPUBitmapGlyph.cpp
SOURCES += CPUFont.cpp
SOURCES += Dyslexic.cpp
SOURCES += Face.cpp
SOURCES += GlyphContainer.cpp
SOURCES += Glyph.cpp
SOURCES += GPUFont.cpp
SOURCES += Size.cpp
SOURCES += GPUTextureFont.cpp
SOURCES += GPUTextureGlyph.cpp
SOURCES += Utils.cpp

TARGET = Dyslexic

DEPENDPATH += ../

INCLUDEPATH += ../ 

DESTDIR = ../lib

DEFINES += DYSLEXIC_FLIP_Y

PKGCONFIG += freetype2

LIBS += -L../lib -lLuminous -lRadiant -lValueIO

macx:LIBS += -framework,OpenGL

