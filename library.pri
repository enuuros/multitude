# Common rules to build libraries
TEMPLATE = lib

DEPENDPATH += ../

INCLUDEPATH += ../ 

DESTDIR = ../lib

linux-* {

  target.path = /lib
 
  includes.path = /include/$$TARGET
  includes.files = $$HEADERS

  INSTALLS += target includes
}

macx {

    CONFIG += lib_bundle

    TAKE_HEADERS = $$HEADERS
    
    !isEmpty($$EXPORT_HEADERS) {

      TAKE_HEADERS = $$EXPORT_HEADERS
    }

    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $$TAKE_HEADERS
    FRAMEWORK_HEADERS.path = Headers

    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS

    message(Creating OSX bundle)
}

#DEFINES += EXPORT=""

VERSION=1.0

win32 {
	DEFINES += EXPORT=__declspec(dllexport)
	include(./Win32/WinPort.pri)
}

