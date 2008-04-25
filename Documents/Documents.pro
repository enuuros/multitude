include(../multitude.pri)

TEMPLATE = subdirs

docs.path = $$PREFIX/doc/multitude
docs.files = 
docs.extra = doxygen multitude.dox; install -m 644 -p html/* $$PREFIX/doc/multitude

INSTALLS += docs
