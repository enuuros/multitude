#ifndef RADIANT_EXPORT_HPP
#define RADIANT_EXPORT_HPP

/* This code is adapted from TnFOX. */

// Shared library support
#ifdef WIN32
  #define MTIMPORT __declspec(dllimport)
  #define MTEXPORT __declspec(dllexport)
  #define MTDLLLOCAL
  #define MTDLLPUBLIC
#else
  #define MTIMPORT
  #ifdef GCC_HASCLASSVISIBILITY
    #define MTIMPORT __attribute__ ((visibility("default")))
    #define MTEXPORT __attribute__ ((visibility("default")))
    #define MTDLLLOCAL __attribute__ ((visibility("hidden")))
    #define MTDLLPUBLIC __attribute__ ((visibility("default")))
  #else
    #define MTIMPORT
    #define MTEXPORT
    #define MTDLLLOCAL
    #define MTDLLPUBLIC
  #endif
#endif

// Define MTAPI for DLL builds
#ifdef MULTIDLL
  #ifdef MULTIDLL_EXPORTS
    #define MTAPI MTEXPORT
  #else
    #define MTAPI MTIMPORT
  #endif // MULTIDLL_EXPORTS
#else
  #define MTAPI
#endif // MULTIDLL

#endif
