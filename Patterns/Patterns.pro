include(../multitude.pri)

HEADERS += Factory.hpp
HEADERS += NotCopyable.hpp
HEADERS += Singleton.hpp

SOURCES += Dummy.cpp

TARGET = Patterns

# Must be at end (Headers for OSX Bundles)
include(../libs.pri)

target.path = $$PREFIX/lib
INSTALLS = target
