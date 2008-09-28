/* COPYRIGHT
 *
 * This file is part of Dyslexic.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Dyslexic.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
#ifndef DYSLEXIC_FONT_MANAGER_HPP
#define DYSLEXIC_FONT_MANAGER_HPP

#include <Dyslexic/CPUFont.hpp>
#include <Dyslexic/GPUFont.hpp>
#include <Dyslexic/CPUBitmapFont.hpp>

#include <Patterns/Singleton.hpp>

#include <Radiant/Trace.hpp>

#include <Dyslexic/CPUWrapperFont.hpp>

namespace Dyslexic
{

/** FontManager provides high level access to fonts that need to be scaled
  during runtime. It provides access to managed fonts that internally use
  glyphs rendered at different point sizes to improve rendered text quality.
*/
  class FontManager : public Patterns::Singleton<FontManager>
  {
    public:
      CPUWrapperFont * getFont(const std::string & name);

    private:
      FontManager();
      ~FontManager();
    
      // filename -> cpu font
      typedef std::map<std::string, CPUManagedFont *> container;
      container m_managedFonts;

      friend class Patterns::Singleton<FontManager>;
  };

}

#endif
