TEMPLATE = subdirs

SUBDIRS += FireView
SUBDIRS += MoviePlayer
SUBDIRS += Watcher

linux-* {
  stuff.path = /src/MultiTouch/multitude/Applications
  stuff.files = Applications.pro Applications.pri Applications_end.pri

  INSTALLS += stuff
}
