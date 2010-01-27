# Install Windows 3rd party dlls to bin
win64_libs.path = /bin
win64_libs.files = $$PWD/lib64/*.dll

win64_dev_libs.path = /src/MultiTouch/multitude/Win64x/lib64
win64_dev_libs.files = $$PWD/lib64/*.dll $$PWD/lib64/*.lib

win64_extra_headers.path = /src/MultiTouch/multitude/Win64x/include
win64_extra_headers.files = $$PWD/include/*

win64_extra.path = /src/MultiTouch/multitude/Win64x
win64_extra.files = $$PWD/Win64x.pri

INSTALLS += win64_libs win64_extra_headers win64_dev_libs win64_extra

message(Including 64-bit Windows Libraries)
