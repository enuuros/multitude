#ifndef DYSLEXIC_CPU_MANAGED_FONT_HPP
#define DYSLEXIC_CPU_MANAGED_FONT_HPP

#include <Dyslexic/CPUBitmapFont.hpp>

#include <Luminous/Collectable.hpp>

#include <vector>

namespace Dyslexic
{
  
  /// A managed font on the CPU that uses multiple fonts internally to provide
  /// better matches at various different scales.
  class CPUManagedFont : public Luminous::Collectable
  {
    public:
      CPUManagedFont(Luminous::GarbageCollector * gc = 0);

      bool load(const char * fontFilePath);

      uint32_t selectFont(float pointSize);

      int fontCount() const { return static_cast<int> (m_fonts.size()); }
      CPUFont * getFont(int i);

      CPUFont * getMetricFont() { return m_metricFont; }

    private:
      typedef std::vector<CPUFont *> container;
  
      std::string m_file;
      container m_fonts;

      CPUFont * m_metricFont;      
 };


}

#endif
