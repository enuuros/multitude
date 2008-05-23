rem === LIBS =============================
cd ..\Nimble
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vclib ..\Nimble.pro
cd ..\..\ValueIO
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vclib ..\ValueIO.pro
cd ..\..\Radiant
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vclib ..\Radiant.pro
cd ..\..\Luminous
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vclib ..\Luminous.pro
cd ..\..\Dyslexic
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vclib ..\Dyslexic.pro
cd ..\..\Resonant
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vclib ..\Resonant.pro
cd ..\..\ScreenPlay
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vclib ..\ScreenPlay.pro
cd ..\..\VideoDisplay
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vclib ..\VideoDisplay.pro

rem === APPS =============================
cd ..\..\Applications\MoviePlayer
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vcapp ..\MoviePlayer.pro
cd ..\..\FireView
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vcapp ..\FireView.pro
cd ..\..\Watcher
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vcapp ..\Watcher.pro
rem -- set Properties/Linker/System/Subsystem = /SUBSYSTEM:CONSOLE from IDE (for Watcher)

rem === EXAMPLES =============================
cd ..\..\..\Examples\BuildExample
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vcapp ..\BuildExample.pro
cd ..\..\ImageExample
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vcapp ..\ImageExample.pro
cd ..\..\SocketExample
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vcapp ..\SocketExample.pro
rem -- set Properties/Linker/System/Subsystem = /SUBSYSTEM:CONSOLE from IDE (for SocketExample)
cd ..\..\ValueExample
mkdir win
cd win
qmake -win32 -spec ..\..\..\ext\Qt\mkspecs\win32-msvc2005 -t vcapp ..\ValueExample.pro
rem -- set Properties/Linker/System/Subsystem = /SUBSYSTEM:CONSOLE from IDE (for ValueExample)


cd C:\IPCity\CityWall\develop\multitudeNew\Win32