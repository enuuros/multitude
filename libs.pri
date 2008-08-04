TEMPLATE = lib

DEPENDPATH += ../

INCLUDEPATH += ../ 

DESTDIR = ../lib

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

DEFINES += EXPORT=""

win32 {
	DEFINES += EXPORT=__declspec(dllexport)
	include(./Win32/WinPort.pri)
}