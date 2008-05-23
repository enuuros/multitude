EXTDIR = "C:\IPCity\CityWall\develop\ext"
DESTDIR = ../../Win32/lib/debug
#
INC_CMUCAM = $$join(EXTDIR,,,"\cmucam\1394camera")
INC_FFMPEG = $$join(EXTDIR,,,"\ffmpeg\include")
INC_FREETYPE_A = $$join(EXTDIR,,,"\freetype\include")
INC_FREETYPE_B = $$join(EXTDIR,,,"\freetype\include\freetype2")
INC_GLEW = $$join(EXTDIR,,,"\glew\include")
INC_LIBDC1394 = $$join(EXTDIR,,,"\libdc1394")
INC_LIBJPEG = $$join(EXTDIR,,,"\libjpeg\include")
INC_LIBPNG = $$join(EXTDIR,,,"\libpng\include")
INC_LIBSNDFILE = $$join(EXTDIR,,,"\libsndfile")
INC_MAGICK = $$join(EXTDIR,,,"\GraphicsMagick-1.1.11\Magick++\lib")
INC_MAGICK += $$join(EXTDIR,,,"\GraphicsMagick-1.1.11")
INC_PORTAUDIO = $$join(EXTDIR,,,"\portaudio\include")
INC_PTHREADS = $$join(EXTDIR,,,"\pthreads\Pre-built.2\include")
INC_SDL = $$join(EXTDIR,,,"\SDL\include")
INC_XERCES = $$join(EXTDIR,,,"\xerces\include")
INC_ZLIB = $$join(EXTDIR,,,"\zlib")
INC_WINPORT = ../../Win32/WinPort/include
INCLUDEPATH += ".."
# 
LNK_CMUCAM = $$join(EXTDIR,,,"\cmucam\1394camera\Debug")
LNK_FFMPEG = $$join(EXTDIR,,,"\ffmpeg\lib")
LNK_FREETYPE = $$join(EXTDIR,,,"\freetype\lib")
LNK_GLEW = $$join(EXTDIR,,,"\glew\lib")
LNK_JPEG = $$join(EXTDIR,,,"\libjpeg\lib")
LNK_LIBSNDFILE = $$join(EXTDIR,,,"\libsndfile")
LNK_MAGICK = $$join(EXTDIR,,,"\GraphicsMagick-1.1.11\VisualMagick\lib")
LNK_PNG = $$join(EXTDIR,,,"\libpng\lib")
LNK_PORTAUDIO = $$join(EXTDIR,,,"\portaudio\build\msvc\Win32\Debug")
LNK_PTHREADS = $$join(EXTDIR,,,"\pthreads\Pre-built.2\lib")
LNK_SDL = $$join(EXTDIR,,,"\SDL\lib")
LNK_XERCES = $$join(EXTDIR,,,"\xerces\lib")
LNK_MULTITUDE = C:\IPCity\CityWall\develop\multitudeNew\Win32\lib\debug
#
LIB_CMUCAM = 1394camerad.lib
LIB_FFMPEG = avcodec.lib avformat.lib avutil.lib
LIB_FREETYPE = freetype.lib
LIB_GLEW = Glew32.lib
LIB_GLU = Glu32.lib
LIB_JPEG = libjpeg-static-mt-debug.lib
LIB_MAGICK = CORE_DB_Magick++_.lib
LIB_OPENGL = OpenGL32.lib
LIB_SNDFILE = libsndfile-1.lib
LIB_PNG = libpng.lib
LIB_PORTAUDIO = portaudio_x86.lib
LIB_PTHREADS = pthreadVC2.lib
LIB_SDL = SDL.lib SDLmain.lib
LIB_XERCES = xerces-c_2D.lib
LIB_WINPORT = WinPort.lib
#
DEFINES *= _CRT_SECURE_NO_DEPRECATE
QMAKE_CXXFLAGS_DEBUG += -Od
QMAKE_CXXFLAGS *= -wd4244	# conversion from xx to yy, possible loss of data
QMAKE_CXXFLAGS *= -wd4305	# truncation from xx to yy
QMAKE_CXXFLAGS *= -wd4800	# forcing value to bool 'true' or 'false' (performance warning)
QMAKE_LFLAGS += /INCREMENTAL:NO
PKGCONFIG = ""