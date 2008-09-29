TEMPLATE = subdirs

SUBDIRS += Patterns
SUBDIRS += Nimble
SUBDIRS += Radiant
SUBDIRS += Valuable
SUBDIRS += Luminous
SUBDIRS += Dyslexic
SUBDIRS += Resonant
SUBDIRS += Screenplay
SUBDIRS += VideoDisplay

exists(Examples/Examples.pro):SUBDIRS += Examples
SUBDIRS += Applications

#SUBDIRS += Documents

CONFIG += ordered

# Install some build files to the source package
linux-* {

  stuff.path = /src/MultiTouch/multitude
  stuff.files = LGPL.txt multitude.pro multitude.pri library.pri

  INSTALLS += stuff
}
