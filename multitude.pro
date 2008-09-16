TEMPLATE = subdirs

SUBDIRS += Patterns
SUBDIRS += Nimble
SUBDIRS += Radiant
SUBDIRS += Valuable
SUBDIRS += ValueIO
SUBDIRS += Luminous
SUBDIRS += Dyslexic
#SUBDIRS += Documents


# Get these from environment variables
ffmpeg = $$(WITH_FFMPEG)
portaudio = $$(WITH_PORTAUDIO19)

contains(ffmpeg, YES) {
  message(Compiling with FFMPEG support)
}
else {
  message(No FFMPEG support - enable FFMPEG support by setting WITH_FFMPEG to YES)
  ffmpeg = NO
}

contains(portaudio, YES) {
  message(Compiling with PortAudio v19 support)
}
else {
  message(No PortAudio19 support - enable PortAudio19 support by setting WITH_PORTAUDIO19 to YES)
  portaudio = NO
}

contains(portaudio, YES) {
  SUBDIRS += Resonant

  contains(ffmpeg, YES) {
    SUBDIRS += Screenplay
    SUBDIRS += VideoDisplay
  }
}

exists(Examples/Examples.pro):SUBDIRS += Examples
SUBDIRS += Applications

CONFIG += ordered

# Install some build files to the source package
linux-* {

  stuff.path = /src/MultiTouch/multitude
  stuff.files = LGPL.txt multitude.pro multitude.pri library.pri

  INSTALLS += stuff
}
