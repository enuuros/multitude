include(../multitude.pri)

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

LIBS += $$LIB_VALUEIO $$LIB_RADIANT -lGLEW -ljpeg -lpng -lxerces-c $$LIB_OPENGL

linux-g++:LIBS += -lGL -lGLU

PKGCONFIG += GraphicsMagick++

include(../libs.pri)

include(../lib_inst.pri)

win32 {
	INCLUDEPATH += $$INC_WINPORT $$INC_GLEW $$INC_MAGICK $$INC_LIBPNG $$INC_ZLIB $$INC_LIBJPEG  
	INCLUDEPATH += $$INC_XERCES $$INC_PTHREADS
	LIBPATH += $$LNK_MULTITUDE $$LNK_PTHREADS $$LNK_GLEW $$LNK_JPEG $$LNK_MAGICK $$LNK_XERCES
	LIBPATH += $$LNK_PNG 
	LIBS += $$LIB_WINPORT $$LIB_VALUEIO 
	LIBS += $$LIB_PTHREADS $$LIB_OPENGL $$LIB_GLU $$LIB_GLEW $$LIB_JPEG $$LIB_MAGICK $$LIB_XERCES
	LIBS += $$LIB_PNG
	LIBS -= -lGLEW -ljpeg -lpng -lxerces-c -lGL
	QMAKE_CXXFLAGS += -Zc:wchar_t	# treat wchar_t as a builtin type
	QMAKE_CXXFLAGS *= -wd4251		# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}