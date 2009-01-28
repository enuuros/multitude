include(../multitude.pri)

HEADERS += BGThread.hpp
HEADERS += Collectable.hpp
HEADERS += CPUMipmaps.hpp
HEADERS += DynamicTexture.hpp
HEADERS += EnableStep.hpp
HEADERS += Error.hpp
HEADERS += Export.hpp
HEADERS += FramebufferObject.hpp
HEADERS += GarbageCollector.hpp
HEADERS += GLKeyStone.hpp
HEADERS += GLResource.hpp
HEADERS += GLResources.hpp
HEADERS += GLSLProgramObject.hpp
HEADERS += GLSLShaderObject.hpp
HEADERS += GPUMipmaps.hpp
HEADERS += Image.hpp
HEADERS += ImagePyramid.hpp
HEADERS += Luminous.hpp
HEADERS += MatrixStep.hpp
#HEADERS += MipmapTask.hpp
HEADERS += MultiHead.hpp
HEADERS += Path.hpp
HEADERS += PixelFormat.hpp
HEADERS += Task.hpp
HEADERS += TCBSpline.hpp
HEADERS += Texture.hpp
#HEADERS += TiledMipMapImage.hpp
HEADERS += Utils.hpp
HEADERS += VertexBuffer.hpp
HEADERS += VertexBufferImpl.hpp

HEADERS += ImageCodec.hpp
HEADERS += ImageCodecTGA.hpp
HEADERS += CodecRegistry.hpp
HEADERS += ImageCodecPNG.hpp
HEADERS += ImageCodecJPEG.hpp

SOURCES += ImageCodecJPEG.cpp
SOURCES += ImageCodecPNG.cpp
SOURCES += CodecRegistry.cpp
SOURCES += ImageCodecTGA.cpp

SOURCES += BGThread.cpp
SOURCES += Collectable.cpp
SOURCES += CPUMipmaps.cpp
SOURCES += DynamicTexture.cpp
SOURCES += Error.cpp
SOURCES += FramebufferObject.cpp
SOURCES += GarbageCollector.cpp
SOURCES += GLKeyStone.cpp
SOURCES += GLResource.cpp
SOURCES += GLResources.cpp
SOURCES += GLSLProgramObject.cpp
SOURCES += GLSLShaderObject.cpp
SOURCES += GPUMipmaps.cpp
SOURCES += Image.cpp
SOURCES += Luminous.cpp
#SOURCES += MipmapTask.cpp
SOURCES += MultiHead.cpp
SOURCES += Path.cpp
SOURCES += PixelFormat.cpp
SOURCES += Task.cpp
SOURCES += TCBSpline.cpp
SOURCES += Texture.cpp
#SOURCES += TiledMipMapImage.cpp
SOURCES += Utils.cpp
SOURCES += VertexBuffer.cpp

LIBS += $$LIB_RADIANT -lGLEW $$LIB_OPENGL
LIBS += $$LIB_VALUABLE $$LIB_GLU $$LIB_NIMBLE $$LIB_PATTERNS

unix: LIBS += -ljpeg -lpng

win32 {
	DEFINES += LUMINOUS_EXPORT
	INCLUDEPATH += ../Win32x/include/libjpeg
	LIBS += libjpeg-static-mt.lib libpng.lib -lWin32x
}

include(../library.pri)
