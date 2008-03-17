TEMPLATE = subdirs

SUBDIRS += Nimble
SUBDIRS += Radiant
SUBDIRS += ValueIO
SUBDIRS += ValueQT
SUBDIRS += Luminous
SUBDIRS += Dyslexic

# Get these from environment variables
ffmpeg = $$(WITH_FFMPEG)
portaudio = $$(WITH_PORTAUDIO19)

contains(ffmpeg, YES) {
  message(Compiling with FFMPEG support)
}
else {
  message(No FFMPEG support - enable FFMPEG support by setting WITH_FFMPEG to
YES)
  ffmpeg = NO
}

contains(portaudio, YES) {
  message(Compiling with PortAudio v19 support)
}
else {
  message(No PortAudio19 support - enable PortAudio19 support by setting
WITH_PORTAUDIO19 to YES)
  portaudio = NO
}

contains(portaudio, YES) {
  SUBDIRS += Resonant

  contains(ffmpeg, YES) {
    SUBDIRS += VideoDisplay
  }
}

#SUBDIRS += Tests
SUBDIRS += Applications

CONFIG += ordered
