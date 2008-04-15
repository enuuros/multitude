#ifndef DYSLEXIC_FONT_MANAGER_HPP
#define DYSLECIC_FONT_MANAGER_HPP

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
