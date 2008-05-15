TEMPLATE = subdirs

SUBDIRS += Patterns
SUBDIRS += Nimble
SUBDIRS += Radiant
SUBDIRS += Notable
SUBDIRS += Valuable
SUBDIRS += ValueIO
SUBDIRS += ValueQT
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

SUBDIRS += Examples
SUBDIRS += Applications

unix:SUBDIRS += CL
unix:SUBDIRS += CL/plugin/plugin.pro

CONFIG += ordered

# This is here just to provide the message about the installation directory to
# the user. See multitude.pri for the actual logic.
isEmpty(PREFIX) {
  include(prefix.pri)
  message("Installation directory not set, using $$PREFIX")
} else {
  message("Installation directory set to $$PREFIX")
}

