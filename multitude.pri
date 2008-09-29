# This file is to be included by all project files of qmake.
# Use this to setup global build options & variables.
CONFIG -= qt
CONFIG += link_pkgconfig

# Release build
CONFIG += release

# Uncomment for debug build
#CONFIG += debug

MULTI_FFMPEG_LIBS = -lavcodec -lavutil -lavformat

withbundles = $$(MULTI_BUNDLES)

macx {
  exists(/opt/local/include/xercesc) {

    INCLUDEPATH += /opt/local/include/
    LIBS += -L/opt/local/lib/
  }
}

LIB_DYSLEXIC = -lDyslexic
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

MULTI_LIB_FLAG = -L

macx {

  # withbundles = $$(MULTI_BUNDLES)
  withbundles = YES

  LIB_OPENGL = -framework,OpenGL
  LIB_GLU = 

  contains(withbundles,YES) {

    MULTI_LIB_FLAG = -F

    LIB_DYSLEXIC = -framework,Dyslexic
    LIB_FLUFFY = -framework,Fluffy
    LIB_LUMINOUS = -framework,Luminous
    LIB_NIMBLE = -framework,Nimble
    LIB_RADIANT = -framework,Radiant
    LIB_RESONANT = -framework,Resonant
    LIB_SCREENPLAY = -framework,Screenplay
    LIB_VALUABLE = -framework,Valuable
    LIB_VIDEODISPLAY = -framework,VideoDisplay
  }

}

MULTI_VIDEO_LIBS = $$LIB_SCREENPLAY $$LIB_RESONANT $$LIB_VIDEODISPLAY

LIBS += $${MULTI_LIB_FLAG}../lib

