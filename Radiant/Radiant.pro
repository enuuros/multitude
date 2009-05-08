include(../multitude.pri)

# CONFIG += debug

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
HEADERS += ImageConversion.hpp
HEADERS += IODefs.hpp
HEADERS += Log.hpp
HEADERS += Mutex.hpp
HEADERS += PlatformUtils.hpp
HEADERS += Priority.hpp
HEADERS += Radiant.hpp
HEADERS += RefObj.hpp
HEADERS += RefPtr.hpp
HEADERS += ResourceLocator.hpp
HEADERS += Semaphore.hpp
HEADERS += SerialPort.hpp
HEADERS += Size2D.hpp
HEADERS += Sleep.hpp
!win32: HEADERS += SHMDuplexPipe.hpp
!win32: HEADERS += SHMPipe.hpp
HEADERS += SMRingBuffer.hpp
# HEADERS += StandardPaths.hpp
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
HEADERS += Video1394.hpp
HEADERS += VideoImage.hpp
HEADERS += VideoInput.hpp
HEADERS += WatchDog.hpp

SOURCES += UDPSocket.cpp
SOURCES += BinaryData.cpp
SOURCES += Color.cpp
SOURCES += ColorUtils.cpp
SOURCES += Condition.cpp
SOURCES += ConfigReader.cpp
SOURCES += DateTime.cpp
SOURCES += DirectoryCommon.cpp
unix:SOURCES += DirectoryNonBoost.cpp
else:SOURCES += DirectoryBoost.cpp
SOURCES += FileUtils.cpp
SOURCES += FixedStr.cpp
SOURCES += ImageConversion.cpp
SOURCES += Log.cpp
SOURCES += Mutex.cpp
SOURCES += ResourceLocator.cpp
SOURCES += Size2D.cpp
SOURCES += Sleep.cpp
!win32: SOURCES += SHMDuplexPipe.cpp
!win32: SOURCES += SHMPipe.cpp
SOURCES += SMRingBuffer.cpp
SOURCES += StringUtils.cpp
SOURCES += TCPServerSocket.cpp
SOURCES += TCPSocket.cpp
SOURCES += Thread.cpp
SOURCES += Timer.cpp
SOURCES += TimeStamp.cpp
SOURCES += Trace.cpp
SOURCES += VideoImage.cpp
SOURCES += VideoInput.cpp
SOURCES += WatchDog.cpp

LIBS += $$LIB_NIMBLE $$LIB_PATTERNS

linux-* {
  SOURCES += PlatformUtilsLinux.cpp
}

macx { 
	SOURCES += PlatformUtilsOSX.cpp
	LIBS += -framework,CoreFoundation
}

unix {
	SOURCES += SerialPortPosix.cpp
	SOURCES += Video1394.cpp

	LIBS += -lpthread $$LIB_RT -ldl
	PKGCONFIG += libdc1394-2
}

win32 {
	DEFINES += RADIANT_EXPORT BOOST_ALL_DYN_LINK
    SOURCES += Video1394cmu.cpp
	SOURCES += cmu_dc1394.cpp
	SOURCES += PlatformUtilsWin32.cpp
	SOURCES += SerialPortWin32.cpp
		
	HEADERS += Video1394cmu.hpp
	HEADERS += cmu_dc1394.hpp
	
	LIBS += 1394Camera.lib win32x.lib wsock32.lib pthreadVC2.lib ShLwApi.lib shell32.lib
	QMAKE_CXXFLAGS += -Zc:wchar_t
}


include(../library.pri)
