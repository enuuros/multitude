# Common rules to build multitude libraries
TEMPLATE = lib

DESTDIR = $$PWD/lib

target.path = /lib
 
includes.path = /include/$$TARGET
includes.files = $$HEADERS

src_code.path = /src/MultiTouch/multitude/$$TARGET
src_code.files = $$HEADERS $$SOURCES

PROJECT_FILE = $$join(TARGET, "", "", ".pro")
src_code.files += $$PROJECT_FILE

INSTALLS += target includes src_code

# On Windows, put DLLs into /bin with the exes
win32 {
	tt = $$join(TARGET, "", "$$DESTDIR/", ".dll")
	dlls.path = /apps/bin
	dlls.files += $$tt
	dlls.CONFIG += no_check_exist
	
	INSTALLS += dlls
}

  macx {

    CONFIG += lib_bundle

    TAKE_HEADERS = $$HEADERS
    
    !isEmpty($$EXPORT_HEADERS) {

      TAKE_HEADERS = $$EXPORT_HEADERS
    }

    !isEmpty($$EXTRA_HEADERS) {

      TAKE_HEADERS += $$EXTRA_HEADERS
    }

    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $$TAKE_HEADERS
    FRAMEWORK_HEADERS.path = Headers

    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS

    message(Creating OSX bundle)
}