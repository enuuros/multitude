TEMPLATE = subdirs

SUBDIRS += AudioPanning
SUBDIRS += AmbientSounds
SUBDIRS += ConfigConversion
SUBDIRS += ImageExample
SUBDIRS += PlatformExample
!win32:SUBDIRS += SamplePlayer
unix:SUBDIRS += SharedMemory
SUBDIRS += SocketExample
SUBDIRS += UDPExample
SUBDIRS += VertexBuffers
SUBDIRS += ValueTest
SUBDIRS += ValidatingXML
SUBDIRS += XMLDump
