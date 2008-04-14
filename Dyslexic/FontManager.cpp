#include "FontManager.hpp"

#include "CPUManagedFont.hpp"

namespace Dyslexic
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
        Radiant::error("FontManager::getFont # failed to load '%s'", name.c_str());
        return 0;
      }
    } else mfont = it->second;

    return new CPUWrapperFont(mfont);
  }




}

