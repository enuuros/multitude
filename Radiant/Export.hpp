/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
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
