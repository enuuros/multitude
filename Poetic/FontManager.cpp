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

#include "FontManager.hpp"

#include "CPUManagedFont.hpp"

#include <map>
#include <string>

namespace Poetic
{

  FontManager::FontManager()
  {}

  FontManager::~FontManager()
  {}

  CPUWrapperFont * FontManager::getFont(const std::string & name)
  {
	container::iterator it = m_managedFonts.find(name);

    CPUManagedFont * mfont = 0;

    if(it == m_managedFonts.end()) {
      // Need to create a new managed font
      mfont = new CPUManagedFont(0);
      m_managedFonts[name] = mfont;

      if(!mfont->load(name.c_str())) {
        Radiant::trace(Radiant::FAILURE, "FontManager::getFont # failed to load '%s'", name.c_str());
        return 0;
      }
    } else mfont = it->second;

    return new CPUWrapperFont(mfont);
  }




}
