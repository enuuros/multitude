# This file is to be included by all project files of qmake.
# Use this to setup global build options & variables.

CONFIG -= qt
CONFIG += link_pkgconfig

# Release build
CONFIG += release
CONFIG -= debug

# Uncomment for debug build
#CONFIG += debug

#MULTI_CORE_LIBS = -lNimble -lDyslexic -lLuminous -lMultiTouch -lMultiStateDisplay -lRadiant -lValueIO -lxerces-c -lGLEW -lGraphicsMagick

#macx {
#  MULTI_CORE_LIBS += -Wl,-framework,Cocoa -framework,OpenGL
#  BASEPATH = /usr/local
#}
#linux {
#  MULTI_CORE_LIBS += -lGLU  -lGL
#  BASEPATH = /usr
#}

#MULTI_USUAL_LIBS = $$MULTI_CORE_LIBS -lMultiWidgets -lSDL

MULTI_FFMPEG_LIBS = -lavcodec -lavutil -lavformat
#MULTI_VIDEO_LIBS = -lResonant -lVideoDisplay $$MULTI_FFMPEG_LIBS -lsndfile


LIB_DYSLEXIC = -lDyslexic
LIB_LUMINOUS = -lLuminous
LIB_NIMBLE = -lNimble
LIB_RADIANT = -lRadiant
LIB_RESONANT = -lResonant
LIB_SCREENPLAY = -lScreenplay
LIB_VALUEIO = -lValueIO
LIB_VIDEODISPLAY = -lVideoDisplay

MULTI_LIB_FLAG = -L

macx {

  withbundles = $$(MULTI_BUNDLES)

  contains(withbundles,YES) {

    MULTI_LIB_FLAG = -F

    LIB_DYSLEXIC = -framework,Dyslexic
    LIB_LUMINOUS = -framework,Luminous
    LIB_NIMBLE = -framework,Nimble
    LIB_RADIANT = -framework,Radiant
    LIB_RESONANT = -framework,Resonant
    LIB_SCREENPLAY = -framework,Screenplay
    LIB_VALUEIO = -framework,ValueIO
    LIB_VIDEODISPLAY = -framework,VideoDisplay

  }

}

LIBS += $${MULTI_LIB_FLAG}../lib
