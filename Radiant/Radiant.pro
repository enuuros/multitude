include(../multitude.pri)

TEMPLATE = lib

HEADERS += BinaryData.hpp
HEADERS += Color.hpp
HEADERS += ColorUtils.hpp
HEADERS += Condition.hpp
HEADERS += ConfigReader.hpp
HEADERS += ConfigReaderTmpl.hpp
HEADERS += Directory.hpp
HEADERS += FileUtils.hpp
HEADERS += ImageConversion.hpp
HEADERS += IODefs.hpp
HEADERS += Mutex.hpp
HEADERS += Priority.hpp
HEADERS += SerialPort.hpp
HEADERS += Size2D.hpp
HEADERS += Sleep.hpp
HEADERS += StringUtils.hpp
HEADERS += TCPServerSocket.hpp
HEADERS += TCPSocket.hpp
HEADERS += ThreadData.hpp
HEADERS += Thread.hpp
HEADERS += Timer.hpp
HEADERS += Trace.hpp 
HEADERS += Video1394.hpp
HEADERS += VideoFFMPEG.hpp
HEADERS += VideoImage.hpp
HEADERS += VideoInput.hpp
HEADERS += WatchDog.hpp

SOURCES += BinaryData.cpp
SOURCES += Color.cpp
SOURCES += ColorUtils.cpp
SOURCES += ConfigReader.cpp
SOURCES += Directory.cpp
SOURCES += FileUtils.cpp
SOURCES += ImageConversion.cpp
SOURCES += Mutex.cpp
SOURCES += SerialPort.cpp
SOURCES += Size2D.cpp
SOURCES += Sleep.cpp
SOURCES += StringUtils.cpp
SOURCES += TCPServerSocket.cpp
SOURCES += TCPSocket.cpp
SOURCES += Thread.cpp
SOURCES += Timer.cpp
SOURCES += Trace.cpp
SOURCES += Video1394.cpp
SOURCES += VideoFFMPEG.cpp
SOURCES += VideoImage.cpp
SOURCES += VideoInput.cpp
SOURCES += WatchDog.cpp

TARGET = Radiant

DEPENDPATH += ../

INCLUDEPATH += ../ 

DESTDIR = ../lib

LIBS += -lpthread

PKGCONFIG += libdc1394-2 libavutil libavformat libavcodec
