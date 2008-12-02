/* COPYRIGHT
 *
 * This file is part of Poetic.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Poetic.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
#ifndef POETIC_FREETYPE_HPP
#define POETIC_FREETYPE_HPP

//#include <ft2build.h>
//#include FT_FREETYPE_H

struct FT_LibraryRec_;

/** Poetic is an OpenGL font rendering library.
 *
 * Poetic is a C++ OpenGL font rendering library based on FTGL
 * 2.1.2. It was built to be used in multithreaded rendering.
 *
 * @author Esa Nuuros
 */
namespace Poetic
{

  bool initialize();
  
  FT_LibraryRec_ ** freetype();

  int error();
}

#endif
