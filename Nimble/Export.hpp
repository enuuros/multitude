/* COPYRIGHT
 *
 * This file is part of ConfigReader.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "ConfigReader.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef NIMBLE_EXPORT_HPP
#define NIMBLE_EXPORT_HPP

#ifdef WIN32

// Import by default
#ifdef NIMBLE_EXPORT
#define NIMBLE_API __declspec(dllexport)
#else
#define NIMBLE_API __declspec(dllimport)
#endif

#else
#define NIMBLE_API
#endif  

#endif
