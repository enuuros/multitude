include(../multitude.pri)

HEADERS += Export.hpp
HEADERS += Grid.hpp
HEADERS += GridTmpl.hpp
HEADERS += Histogram.hpp 
HEADERS += KeyStone.hpp 
HEADERS += LineSegment2.hpp 
HEADERS += LensCorrection.hpp 
HEADERS += Math.hpp 
HEADERS += Matrix2.hpp 
HEADERS += Matrix2Impl.hpp
HEADERS += Matrix3.hpp 
HEADERS += Matrix3Impl.hpp 
HEADERS += Matrix4.hpp 
HEADERS += Matrix4Impl.hpp
HEADERS += Nimble.hpp
HEADERS += Ramp.hpp 
HEADERS += Random.hpp 
HEADERS += Rect.hpp 
HEADERS += RingBuffer.hpp 
HEADERS += RingBufferImpl.hpp 
HEADERS += Vector2.hpp 
HEADERS += Vector3.hpp 
HEADERS += Vector4.hpp 
HEADERS += Plane.hpp
HEADERS += Splines.hpp
HEADERS += SplinesImpl.hpp

SOURCES += Grid.cpp
SOURCES += Histogram.cpp
SOURCES += KeyStone.cpp
SOURCES += LineSegment2.cpp 
SOURCES += LensCorrection.cpp
SOURCES += Matrix.cpp 
SOURCES += Plane.cpp
SOURCES += Random.cpp
SOURCES += Rect.cpp
SOURCES += RingBuffer.cpp
SOURCES += Vector2.cpp
SOURCES += Splines.cpp

win32:DEFINES += NIMBLE_EXPORT

include(../library.pri)
