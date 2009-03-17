TEMPLATE = subdirs

win32: SUBDIRS += Win32x
SUBDIRS += Patterns
SUBDIRS += Nimble
SUBDIRS += Radiant
SUBDIRS += Valuable
SUBDIRS += Luminous
SUBDIRS += Poetic
SUBDIRS += Resonant
SUBDIRS += Screenplay
SUBDIRS += VideoDisplay

exists(Examples/Examples.pro):SUBDIRS += Examples
SUBDIRS += Applications

CONFIG += ordered

# Install some build files to the source package
stuff.path = /src/MultiTouch/multitude
stuff.files = LGPL.txt multitude.pro multitude.pri library.pri

INSTALLS += stuff
