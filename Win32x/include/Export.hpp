/* COPYRIGHT
 *
 * This file is part of WinPort.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "WinPort.h" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef WINPORT_EXPORT_HPP
#define WINPORT_EXPORT_HPP

#ifdef WIN32

// Import by default
#ifdef WINPORT_EXPORT
#define WINPORT_API __declspec(dllexport)
#else
#define WINPORT_API __declspec(dllimport)
#endif

#endif  

#endif
