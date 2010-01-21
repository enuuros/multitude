# Install Windows 3rd party dlls to bin
win64_libs.path = /bin
win64_libs.files = $$PWD/lib64/*.dll

win64_dev_libs.path = /src/MultiTouch/multitude/Win64x/lib64
win64_dev_libs.files = $$PWD/lib64/*.dll $$PWD/lib64/*.lib

win64_extra_headers.path = /src/MultiTouch/multitude/Win64x/include
win64_extra_headers.files = $$PWD/include/*

INSTALLS += win64_libs win64_extra_headers win64_dev_libs

message(Including 64-bit Windows Libraries)
