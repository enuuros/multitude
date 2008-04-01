include(../multitude.pri)

TEMPLATE = lib


HEADERS += BGThread.hpp
HEADERS += Collectable.hpp
HEADERS += DynamicTexture.hpp
HEADERS += Error.hpp
HEADERS += FramebufferObject.hpp
HEADERS += GarbageCollector.hpp
HEADERS += GlKeyStone.hpp
HEADERS += GLResource.hpp
HEADERS += GLResources.hpp
HEADERS += GLSLProgramObject.hpp
HEADERS += GLSLShaderObject.hpp
HEADERS += Image.hpp
HEADERS += ImagePyramid.hpp
HEADERS += Luminous.hpp
HEADERS += MipmapTask.hpp
HEADERS += MultiHead.hpp
HEADERS += PixelFormat.hpp
HEADERS += Task.hpp
HEADERS += Texture.hpp
#HEADERS += TiledMipMapImage.hpp
HEADERS += Utils.hpp

SOURCES += BGThread.cpp
SOURCES += Collectable.cpp
SOURCES += DynamicTexture.cpp
SOURCES += Error.cpp
SOURCES += FramebufferObject.cpp
SOURCES += GarbageCollector.cpp
SOURCES += GlKeyStone.cpp
SOURCES += GLResource.cpp
SOURCES += GLResources.cpp
SOURCES += GLSLProgramObject.cpp
SOURCES += GLSLShaderObject.cpp
SOURCES += Image.cpp
SOURCES += JPEG.cpp
SOURCES += Luminous.cpp
SOURCES += MipmapTask.cpp
SOURCES += MultiHead.cpp
SOURCES += PixelFormat.cpp
SOURCES += PNG.cpp
SOURCES += Task.cpp
SOURCES += Texture.cpp
SOURCES += TGA.cpp
#SOURCES += TiledMipMapImage.cpp
SOURCES += Utils.cpp

TARGET = Luminous

DEPENDPATH += ../

linux-g++ {
  BASEPATH=/usr
}

macx {
  BASEPATH=/usr/local
}

INCLUDEPATH += ../ 

DESTDIR = ../lib

LIBS += -L../lib -lValueIO -lRadiant  -lGLEW -ljpeg -lpng -lxerces-c

# macx:LIBS += -lGL -lGLU
macx:LIBS += -framework,OpenGL

PKGCONFIG += GraphicsMagick++
