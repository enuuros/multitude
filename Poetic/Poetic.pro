include(../multitude.pri)

HEADERS += BBox.hpp
HEADERS += Charmap.hpp
HEADERS += CPUBitmapFont.hpp
HEADERS += CPUBitmapGlyph.hpp
HEADERS += CPUFontBase.hpp
HEADERS += CPUFont.hpp
HEADERS += CPUManagedFont.hpp
HEADERS += CPUWrapperFont.hpp
HEADERS += Poetic.hpp
HEADERS += Export.hpp
HEADERS += Face.hpp
HEADERS += FontManager.hpp
HEADERS += GlyphContainer.hpp
HEADERS += Glyph.hpp
HEADERS += GPUFontBase.hpp
HEADERS += GPUFont.hpp
HEADERS += GPUManagedFont.hpp
HEADERS += GPUTextureFont.hpp
HEADERS += GPUTextureGlyph.hpp
HEADERS += GPUWrapperFont.hpp
HEADERS += Size.hpp
HEADERS += Utils.hpp

SOURCES += BBox.cpp
SOURCES += Charmap.cpp
SOURCES += CPUBitmapFont.cpp
SOURCES += CPUBitmapGlyph.cpp
SOURCES += CPUFontBase.cpp
SOURCES += CPUManagedFont.cpp
SOURCES += CPUWrapperFont.cpp
SOURCES += Poetic.cpp
SOURCES += Face.cpp
SOURCES += FontManager.cpp
SOURCES += GlyphContainer.cpp
SOURCES += Glyph.cpp
SOURCES += GPUFontBase.cpp
SOURCES += GPUFont.cpp
SOURCES += GPUManagedFont.cpp
SOURCES += GPUTextureFont.cpp
SOURCES += GPUTextureGlyph.cpp
SOURCES += GPUWrapperFont.cpp
SOURCES += Size.cpp
SOURCES += Utils.cpp

DEFINES += DYSLEXIC_FLIP_Y

PKGCONFIG += freetype2

LIBS += $$LIB_LUMINOUS $$LIB_RADIANT 
LIBS += $$LIB_OPENGL

macx:LIBS += -framework,OpenGL

include(../library.pri)

win32 {
	SOURCES -= FontManager.cpp
	INCLUDEPATH += $$INC_GLEW $$INC_FREETYPE_A $$INC_FREETYPE_B $$INC_WINPORT
	LIBPATH += $$LNK_MULTITUDE $$LNK_FREETYPE 
	LIBS += $$LIB_FREETYPE $$LIB_OPENGL
	QMAKE_CXXFLAGS *= -wd4251	# see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
}
