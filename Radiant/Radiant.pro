include(../multitude.pri)

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
HEADERS += ImageConversion.hpp
HEADERS += IODefs.hpp
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
HEADERS += SMRingBuffer.hpp
HEADERS += StandardPaths.hpp
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

SOURCES += BinaryData.cpp
SOURCES += Color.cpp
SOURCES += ColorUtils.cpp
SOURCES += Condition.cpp
SOURCES += ConfigReader.cpp
SOURCES += DateTime.cpp
SOURCES += DirectoryCommon.cpp
# SOURCES += DirectoryBoost.cpp
SOURCES += DirectoryNonBoost.cpp
SOURCES += FileUtils.cpp
SOURCES += ImageConversion.cpp
SOURCES += Mutex.cpp
SOURCES += ResourceLocator.cpp
SOURCES += SerialPortLinux.cpp
SOURCES += Size2D.cpp
SOURCES += Sleep.cpp
SOURCES += SMRingBuffer.cpp
SOURCES += StringUtils.cpp
SOURCES += TCPServerSocket.cpp
SOURCES += TCPSocket.cpp
SOURCES += Thread.cpp
SOURCES += Timer.cpp
SOURCES += TimeStamp.cpp
SOURCES += Trace.cpp
SOURCES += Video1394.cpp
SOURCES += VideoImage.cpp
SOURCES += VideoInput.cpp
SOURCES += WatchDog.cpp

linux-*:SOURCES += PlatformUtilsLinux.cpp

macx:SOURCES += PlatformUtilsOSX.cpp

unix:LIBS += -lpthread $$LIB_RT -ldl

macx:LIBS += -framework,CoreFoundation
PKGCONFIG += libdc1394-2

include(../library.pri)

win32 {
	SOURCES -= SerialPort.cpp Video1394.cpp SMRingBuffer.cpp
    SOURCES += Video1394cmu.cpp cmu_dc1394.cpp
	INCLUDEPATH += $$INC_PTHREADS $$INC_WINPORT $$INC_LIBDC1394 $$INC_CMUCAM
	LIBPATH += $$LNK_MULTITUDE $$LNK_PTHREADS $$LNK_CMUCAM
	LIBS += $$LIB_WINPORT $$LIB_PTHREADS $$LIB_CMUCAM -lws2_32
}
