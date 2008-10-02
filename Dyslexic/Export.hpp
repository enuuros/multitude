/* COPYRIGHT
 *
 * This file is part of Dyslexic.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Dyslexic.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef DYSLEXIC_EXPORT_HPP
#define DYSLEXIC_EXPORT_HPP

#ifdef WIN32

// Import by default
#ifdef DYSLEXIC_EXPORT
#define DYSLEXIC_API __declspec(dllexport)
#else
#define DYSLEXIC_API __declspec(dllimport)
#endif

#else

#define DYSLEXIC_API

#endif  

#endif
