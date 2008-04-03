
TEMPLATE = lib

DEPENDPATH += ../

INCLUDEPATH += ../ 

DESTDIR = ../lib

LIBS += -L../lib

macx {

  contains($$MULTI_MUNDLES,YES) {
    CONFIG += lib_bundle
  }

}
