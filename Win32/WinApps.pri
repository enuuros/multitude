include(WinPort.pri)

DEFINES += EXPORT=__declspec(dllimport)
DESTDIR = ../../Win32/app/debug
PKGCONFIG = ""
LIBS -= -framework,Cocoa
