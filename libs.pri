
TEMPLATE = lib

DEPENDPATH += ../

INCLUDEPATH += ../ 

DESTDIR = ../lib

macx {

  withbundles = $$(MULTI_BUNDLES)

  contains(withbundles,YES) {
    CONFIG += lib_bundle

    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $$HEADERS
#    FRAMEWORK_HEADERS.path = Headers/$$TARGET
    FRAMEWORK_HEADERS.path = Headers

    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS

    message(Creating OSX bundle)
  }

}

