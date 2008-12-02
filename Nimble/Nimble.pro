include(../multitude.pri)

HEADERS += Export.hpp
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
# HEADERS += RectImpl.hpp
HEADERS += Vector2.hpp 
HEADERS += Vector3.hpp 
HEADERS += Vector4.hpp 

SOURCES += KeyStone.cpp
SOURCES += LensCorrection.cpp
SOURCES += Random.cpp
SOURCES += Rect.cpp

include(../library.pri)

win32 {
	CONFIG += staticlib
}
