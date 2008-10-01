rem === LIBS =============================
cd ..\Dyslexic
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\Dyslexic.pro
cd ..\..\Luminous
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\Luminous.pro
cd ..\..\Nimble
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\Nimble.pro
cd ..\..\Patterns
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\Patterns.pro
cd ..\..\Radiant
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\Radiant.pro
cd ..\..\Resonant
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\Resonant.pro
cd ..\..\ScreenPlay
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\ScreenPlay.pro
cd ..\..\Valuable
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\Valuable.pro
cd ..\..\ValueIO
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\ValueIO.pro
cd ..\..\VideoDisplay
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vclib ..\VideoDisplay.pro

rem === APPS =============================
cd ..\..\Applications\FireView
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\FireView.pro
cd ..\..\MoviePlayer
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\MoviePlayer.pro
cd ..\..\Watcher
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\Watcher.pro
rem -- set Properties/Linker/System/Subsystem = /SUBSYSTEM:CONSOLE from IDE (for Watcher)

rem === EXAMPLES =============================
cd ..\..\..\Examples\BuildExample
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\BuildExample.pro
cd ..\..\ExternalCompilation
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\ExternalCompilation.pro
cd ..\..\ImageExample
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\ImageExample.pro
cd ..\..\PlatformExample
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\PlatformExample.pro
cd ..\..\SamplePlayer
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\SamplePlayer.pro
cd ..\..\SocketExample
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\SocketExample.pro
rem -- set Properties/Linker/System/Subsystem = /SUBSYSTEM:CONSOLE from IDE (for SocketExample)
cd ..\..\VertexBuffers
mkdir win
cd win
qmake -win32 -spec ..\..\..\..\..\..\ext\Qt\qt-win-opensource-src-4.4.1\mkspecs\win32-msvc2005 -t vcapp ..\VertexBuffers.pro


cd C:\develop\MultiTouch\trunk\multitude\Win32