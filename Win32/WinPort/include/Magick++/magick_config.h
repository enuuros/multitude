// This may look like C code, but it is really -*- C++ -*-
//
// Copyright (C) 2003, 2004 GraphicsMagick Group
// Copyright (C) 2002 ImageMagick Studio
// Copyright 1991-1999 E. I. du Pont de Nemours and Company
//
// This program is covered by multiple licenses, which are described in
// Copyright.txt. You should have received a copy of Copyright.txt with this
// package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

#if defined(WIN32)

// GraphicsMagick build feature configuration.  Please note that
// disabling a feature via this header file may not be sufficient to
// remove any library dependencies from the build.  The VisualMagick
// project files may need to be edited to remove libraries the feature
// depends on. When building a static GraphicsMagick, coder
// registrations are made via magick\static.c so if a format is
// removed, the call to its registration function should be commented
// out in static.c.  Note that VisualMagick configure updates
// magick\static.c so re-running configure may cause local changes to
// be lost.
//
// Note that by default GraphicsMagick is configured with a
// QuantumDepth of eight, has LZW compression enabled, and looks for
// all files in the directory where the executable is located.  The
// installed configuration (i.e.  the setup.exe-style installer) is
// modifying by defining "UseInstalledMagick".  If you would like to
// install GraphicsMagick using hard-coded paths, or want to use the
// Windows registry to install GraphicsMagick, then
// UseInstalledMagick" should be defined.
//
// Enabled options are of the form:
//
//   #define option
//
// while disabled options are initially in the form
//
//   // #undef option
//
// so it is necessary to remove the comment, and change "undef" to "define"
// in order for the option to be enabled.

/////////////
//
// QuantumDepth
//
// Specify size of PixelPacket color Quantums (8, 16, or 32).
// A value of 8 uses half the memory than 16 and typically runs 30% faster,
// but provides 256 times less color resolution than a value of 16.
#define QuantumDepth 8

/////////////
//
// UseInstalledMagick
//
// Define to build a GraphicsMagick which uses registry settings or
// hard-coded paths to locate installed components.  This supports
// using the "setup.exe" style installer, or using hard-coded path
// definitions (see below).  If you want to be able to simply copy
// the built GraphicsMagick to any directory on any machine, then do
// not use this setting.
// #undef UseInstalledMagick

/////////////
//
// ProvideDllMain
//
// When building GraphicsMagick using DLLs, include a DllMain()
// function which automatically invokes InitializeMagick(NULL), and
// DestroyMagick() so that the user doesn't need to. This is enabled
// by default since no adverse effects have been found.
#define ProvideDllMain

/////////////
//
// HasLZW
//
// Define to support UNISYS patented LZW compression as used by GIF.
// The UNISYS patent on LZW expired on July 7, 2004 worldwide.
#define HasLZW 1

/////////////
//
// HasX11
//
// Define if MIT X11 is available (or stubbed)
// It is not actually necessary to use X11 or the X11 stubs library.
// The VisualMagick configure program assumes that X11 stubs is being
// used if X11 is not supported.  To achieve a slimmer GraphicsMagick,
// undefine HasX11 and remove the 'xlib' project from the
// GraphicsMagick workspace.
#define HasX11

/////////////
//
// PREFIX_MAGICK_SYMBOLS
//
// Define if the symbols in the GraphicsMagick library should all be
// prefixed with "Gm" to ensure that there are no conflicts at the
// library level. Symbol renaming is done using the C preprocessor so
// there is still risk of conflict at the source level.
#undef PREFIX_MAGICK_SYMBOLS

/////////////
//
// Optional packages
//
// All packages except autotrace are included by default in the build.

// Define to use the autotrace library (obtain sources seperately)
// #undef HasAUTOTRACE

// Define to use the bzip2 compression library
#define HasBZLIB

// Define to use the FlashPIX library (fpx module/subdirectory)
// #undef HasFPX

// Define to use the JBIG library
#define HasJBIG

// Define to use the Jasper JPEG v2 library
#define HasJP2

// Define to use the IJG JPEG v1 library
#define HasJPEG

// Define to use the "little" Color Management System (LCMS) library
#define HasLCMS

// Define to use the PNG library
#define HasPNG

// Define to use the TIFF library
#define HasTIFF

// Define to use the FreeType (TrueType & Postscript font support) library
#define HasTTF

// Define to use the Windows GDI32 library (for clipboard and emf modules)
#define HasWINGDI32

// Define to use the libwmf WMF parsing library
#define HasWMFlite

// Define to use the GNOME XML library
#define HasXML

// Define to use the zlib ZIP compression library
#define HasZLIB

/////////////
//
// Hard Coded Paths
//
// If hard-coded paths are defined via the the following define
// statements, then they will override any values from the Windows
// registry.
// It is unusual to use hard-coded paths under Windows.

// Optional: Specify where X11 application resource files are installed
// #define ApplicationDefaults "c:\\GraphicsMagick\\"

// Optional: Specify where user-specific X11 application resource files are installed
// #define PreferencesDefaults  "~\\."

// Optional: Specify where gm.exe and support executables are installed
// #define MagickBinPath       "c:\\GraphicsMagick\\"

// Optional: Specify where operating system specific files are installed
// #define MagickLibPath       "c:\\GraphicsMagick\\"

// Optional: Specify where operating system independent files are installed
// #define MagickSharePath     "c:\\GraphicsMagick\\"

// Optional: Specify where coder modules (DLLs) are installed
// #define MagickCoderModulesPath   "c:\\GraphicsMagick\\"

// Optional: Specify where filter modules (DLLs) are installed
// #define MagickFilterModulesPath   "c:\\GraphicsMagick\\"

/////////////
//
// The remaining defines should not require user modification.
//
#define HAVE_SYS_TYPES_H 1

#endif // WIN32
