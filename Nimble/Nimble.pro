include(../multitude.pri)

HEADERS += Export.hpp
HEADERS += Grid.hpp
HEADERS += GridTmpl.hpp
HEADERS += Histogram.hpp 
HEADERS += KeyStone.hpp 
HEADERS += LensCorrection.hpp 
HEADERS += Math.hpp 
HEADERS += Matrix2.hpp 
HEADERS += Matrix3.hpp 
HEADERS += Matrix4.hpp 
HEADERS += Nimble.hpp
HEADERS += Ramp.hpp 
HEADERS += Random.hpp 
HEADERS += Rect.hpp 
HEADERS += RingBuffer.hpp 
# HEADERS += RectImpl.hpp
HEADERS += Vector2.hpp 
HEADERS += Vector3.hpp 
HEADERS += Vector4.hpp 
HEADERS += Plane.hpp

SOURCES += Grid.cpp
SOURCES += Histogram.cpp
SOURCES += KeyStone.cpp
SOURCES += LensCorrection.cpp
SOURCES += Plane.cpp
SOURCES += Random.cpp
SOURCES += Rect.cpp
SOURCES += RingBuffer.cpp
SOURCES += Vector2.cpp

win32:DEFINES += NIMBLE_EXPORT

include(../library.pri)
