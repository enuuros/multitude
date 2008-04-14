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

/*

class MyWidget
{
public:

  MyWidget() 
  {
    m_cpuFont = FontManager::instance().getFont("verdana");
    m_cpuFont->setFaceSize(72);
  }

  void render(GLResources * glrc)
  {
    GPUManagedFont * fg = glrc->getResource(m_cpuFont);
    if(!fg) {
      fg = new GPUManagedFont(m_cpuFont, glrc);
    }

    fg->render("foo", matrix);
  }

private:
  CPUWrapperFont * m_cpuFont;
};

*/


  /// @todo separate interfaces (CPUFont, GPUFont). Make them plain interfaces
  /// so managed fonts can implement them

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
