include(../multitude.pri)

HEADERS += CameraDriver.hpp
HEADERS += UDPSocket.hpp
HEADERS += BinaryData.hpp
HEADERS += BinaryStream.hpp
HEADERS += Color.hpp
HEADERS += ColorUtils.hpp
HEADERS += Condition.hpp
HEADERS += Config.hpp
HEADERS += ConfigReader.hpp
HEADERS += ConfigReaderTmpl.hpp
HEADERS += cycle.h
HEADERS += CycleRecord.hpp
HEADERS += DateTime.hpp
HEADERS += Directory.hpp
HEADERS += Endian.hpp
HEADERS += Export.hpp
HEADERS += FileUtils.hpp
HEADERS += FixedStr.hpp
HEADERS += FixedStrImpl.hpp
HEADERS += Grid.hpp
HEADERS += GridTmpl.hpp
HEADERS += ImageConversion.hpp
HEADERS += IODefs.hpp
HEADERS += Log.hpp
HEADERS += Mutex.hpp
HEADERS += PlatformUtils.hpp
HEADERS += Priority.hpp
HEADERS += Radiant.hpp
HEADERS += RGBA.hpp
HEADERS += RefObj.hpp
HEADERS += RefPtr.hpp
HEADERS += ResourceLocator.hpp
HEADERS += Semaphore.hpp
HEADERS += SerialPort.hpp
HEADERS += Size2D.hpp
HEADERS += Sleep.hpp
!win32:HEADERS += SHMDuplexPipe.hpp
!win32:HEADERS += SHMPipe.hpp
HEADERS += SMRingBuffer.hpp
HEADERS += StringUtils.hpp
HEADERS += TCPServerSocket.hpp
HEADERS += TCPSocket.hpp
HEADERS += ThreadData.hpp
HEADERS += Thread.hpp
HEADERS += Timer.hpp
HEADERS += TimeStamp.hpp
HEADERS += Trace.hpp
HEADERS += Types.hpp
HEADERS += VectorStorage.hpp
HEADERS += VideoImage.hpp
HEADERS += VideoInput.hpp
HEADERS += WatchDog.hpp
HEADERS += ClonablePtr.hpp
HEADERS += VideoCamera.hpp

SOURCES += CameraDriver.cpp
SOURCES += BinaryData.cpp
SOURCES += VideoCamera.cpp
SOURCES += Color.cpp
SOURCES += ColorUtils.cpp
SOURCES += Condition.cpp
SOURCES += ConfigReader.cpp
SOURCES += DateTime.cpp
SOURCES += DirectoryCommon.cpp
SOURCES += FileUtils.cpp
SOURCES += FixedStr.cpp
SOURCES += Grid.cpp
SOURCES += ImageConversion.cpp
SOURCES += Log.cpp
SOURCES += Mutex.cpp
SOURCES += ResourceLocator.cpp
SOURCES += Size2D.cpp
SOURCES += Sleep.cpp
!win32:SOURCES += SHMDuplexPipe.cpp
!win32:SOURCES += SHMPipe.cpp
SOURCES += SMRingBuffer.cpp
SOURCES += StringUtils.cpp
SOURCES += TCPSocket.cpp
SOURCES += Thread.cpp
SOURCES += Timer.cpp
SOURCES += TimeStamp.cpp
SOURCES += Trace.cpp
SOURCES += VideoImage.cpp
SOURCES += VideoInput.cpp
SOURCES += WatchDog.cpp
LIBS += $$LIB_NIMBLE \
    $$LIB_PATTERNS
linux-*:SOURCES += PlatformUtilsLinux.cpp
macx {
    SOURCES += PlatformUtilsOSX.cpp
    LIBS += -framework,CoreFoundation
}
unix {
    HEADERS += VideoCamera1394.hpp
    SOURCES += DirectoryPosix.cpp
    SOURCES += SerialPortPosix.cpp
    SOURCES += TCPServerSocketPosix.cpp
    SOURCES += TCPSocketPosix.cpp
    SOURCES += UDPSocketPosix.cpp
    SOURCES += VideoCamera1394.cpp
    LIBS += -lpthread \
        $$LIB_RT \
        -ldl
    PKGCONFIG += libdc1394-2
}
win32 {
    DEFINES += RADIANT_EXPORT

    HEADERS += VideoCameraCMU.hpp

    SOURCES += VideoCameraCMU.cpp
    SOURCES += PlatformUtilsWin32.cpp
    SOURCES += SerialPortWin32.cpp
    SOURCES += DirectoryQt.cpp
    SOURCES += TCPServerSocketQt.cpp
    SOURCES += TCPSocketQt.cpp
    SOURCES += UDPSocketQt.cpp

    LIBS += win32x.lib wsock32.lib pthreadVC2.lib ShLwApi.lib shell32.lib 1394camera.lib

    CONFIG += qt
    QT = core network
	
	PTGREY_PATH="C:\Program Files\Point Grey Research\FlyCapture2"

	exists($$PTGREY_PATH\include) {
		HEADERS += VideoCameraPTGrey.hpp
		SOURCES += VideoCameraPTGrey.cpp
		INCLUDEPATH += $$PTGREY_PATH\include
		
		# 64bit libs have a different path
		exists($$PTGREY_PATH\lib64):LIBPATH += $$PTGREY_PATH\lib64
		exists($$PTGREY_PATH\lib):LIBPATH += $$PTGREY_PATH\lib
		LIBS += FlyCapture2.lib
	}

}

include(../library.pri)
