linux-* {

  target.path = $$PREFIX/lib
 
  !isEmpty(MULTITOUCH_SDK) {
    includes.path = $$PREFIX/include/$$TARGET
    includes.files = $$HEADERS
    INSTALLS += includes
  }

  INSTALLS += target
}

