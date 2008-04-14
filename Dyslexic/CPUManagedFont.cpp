#include "CPUManagedFont.hpp"

#include <Radiant/Trace.hpp>

#define DEFAULT_RESOLUTION 72

#define METRIC_FONT_POINT_SIZE 64

namespace Dyslexic
{

  // Maintain a user-defined list of the font-sizes i.e. [3,4,6,8,10,16,32//
  static int g_faceSizes[] = { 8, 10, 24 };

  CPUManagedFont::CPUManagedFont(Luminous::GarbageCollector * gc)
  : Collectable(gc),
    m_metricFont(0)
  {}

  bool CPUManagedFont::load(const char * fontFilePath)
  {
    m_file = std::string(fontFilePath);

    bool ok = true;

    // Load the specified font sizes
    for(uint32_t i = 0; i < sizeof(g_faceSizes) / sizeof(g_faceSizes[0]); i++) {
      CPUBitmapFont * font = new CPUBitmapFont(Collectable::gc());

      ok = font->load(m_file.c_str());
      assert(ok);
      ok = font->setFaceSize(g_faceSizes[i], DEFAULT_RESOLUTION);
      assert(ok);
      m_fonts.push_back(font);
    }

    // Load the specified metric font
    CPUBitmapFont * bf = new CPUBitmapFont(Collectable::gc());
    ok = bf->load(m_file.c_str());
    assert(ok);
    ok = bf->setFaceSize(METRIC_FONT_POINT_SIZE, DEFAULT_RESOLUTION);
    assert(ok);

    m_metricFont = bf;

    return ok;
  }

  uint32_t CPUManagedFont::selectFont(float request) 
  {
    assert(!m_fonts.empty());

    int r = static_cast<int> (request);

    uint32_t i;
    for(i = 0; i < m_fonts.size(); i++) {
      int s = m_fonts[i]->faceSize();

      if(s >= r) break;
    }

    return i;
  }

}
