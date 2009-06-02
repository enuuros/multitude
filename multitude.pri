# This file is to be included by all project files of qmake.
# Use this to setup global build options & variables.
CONFIG -= qt
CONFIG += link_pkgconfig
CONFIG += thread
CONFIG += release

INCLUDEPATH += $$PWD

MULTI_FFMPEG_LIBS = -lavcodec -lavutil -lavformat

withbundles = $$(MULTI_BUNDLES)

LIB_POETIC = -lPoetic
LIB_FLUFFY = -lFluffy
LIB_LUMINOUS = -lLuminous
LIB_NIMBLE = -lNimble
LIB_OPENGL = -lGL -lGLU
LIB_GLU = -lGLU
LIB_RADIANT = -lRadiant
LIB_RESONANT = -lResonant 
LIB_SCREENPLAY = -lScreenplay
LIB_VIDEODISPLAY = -lVideoDisplay
LIB_VALUABLE = -lValuable
LIB_PATTERNS = -lPatterns

MULTI_LIB_FLAG = -L

# DEFINES += GLEW_MX

macx {
  exists(/opt/local/include/xercesc) {

    INCLUDEPATH += /opt/local/include/
    LIBS += -L/opt/local/lib/
  }

  # withbundles = $$(MULTI_BUNDLES)
  withbundles = YES

  LIB_OPENGL = -framework,OpenGL
  LIB_GLU = 

  contains(withbundles,YES) {

    MULTI_LIB_FLAG = -F

    LIB_POETIC = -framework,Poetic
    LIB_FLUFFY = -framework,Fluffy
    LIB_LUMINOUS = -framework,Luminous
    LIB_NIMBLE = -framework,Nimble
    LIB_RADIANT = -framework,Radiant
    LIB_RESONANT = -framework,Resonant -lsndfile
    LIB_SCREENPLAY = -framework,Screenplay
    LIB_VALUABLE = -framework,Valuable
    LIB_VIDEODISPLAY = -framework,VideoDisplay
	LIB_PATTERNS = -framework,Patterns
  }

}

win32 {
	INCLUDEPATH += "$$(BOOST_DIR)"
	INCLUDEPATH += $$PWD\Win32x\include
	LIBPATH += "$$(BOOST_DIR)\lib"
	LIBPATH += $$PWD\Win32x\lib
	LIB_OPENGL = -lopengl32
	LIB_GLU = -lglu32
	QMAKE_CXXFLAGS += -Zc:wchar_t -D_CRT_SECURE_NO_WARNINGS -wd4244 -wd4251 -wd4355
	DEFINES += WIN32
}

MULTI_VIDEO_LIBS = $$LIB_SCREENPLAY $$LIB_RESONANT $$LIB_VIDEODISPLAY

LIBS += $${MULTI_LIB_FLAG}$$PWD/lib

# message(QT version is $${QT_MAJOR_VERSION}.$${QT_MINOR_VERSION}.$${QT_PATCH_VERSION})

contains(QT_MAJOR_VERSION,4) {

  contains(QT_MINOR_VERSION,5) {
    HAS_QT_45=YES
    DEFINES += USE_QT45
  }

}

