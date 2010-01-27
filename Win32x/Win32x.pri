# Install Windows 3rd party dlls to bin
win32_libs.path = /bin
win32_libs.files = $$PWD/lib32/*.dll

win32_dev_libs.path = /src/MultiTouch/multitude/Win32x/lib32
win32_dev_libs.files = $$PWD/lib32/*.dll $$PWD/lib32/*.lib

win32_extra_headers.path = /src/MultiTouch/multitude/Win32x/include
win32_extra_headers.files = $$PWD/include/*

win32_extra.path = /src/MultiTouch/multitude/Win32x
win32_extra.files = $$PWD/Win32x.pri

INSTALLS += win32_libs win32_extra_headers win32_dev_libs win32_extra

message(Including 32-bit Windows Libraries)