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
#ifndef DYSLEXIC_CPU_FONT_BASE_HPP
#define DYSLEXIC_CPU_FONT_BASE_HPP

#include <Dyslexic/CPUFont.hpp>
#include <Dyslexic/Export.hpp>

#include <Luminous/Collectable.hpp>

namespace Dyslexic
{
  class GPUFontBase;
  class GPUFont;

  /// A base class for all CPU fonts.
  class DYSLEXIC_API CPUFontBase : public CPUFont, public Luminous::Collectable
  {
    public:
      CPUFontBase(Luminous::GarbageCollector * gc = 0);
      virtual ~CPUFontBase();

      Face * face();
      const Size & size() const;

      int error() const;

      float advance(const char * str);
      float advance(const wchar_t * str);

      int faceSize() const;
      virtual bool setFaceSize(int size, int resolution);

      float ascender() const;
      float descender() const;      
      float lineHeight() const;

      void bbox(const char * str, BBox & bbox);
      void bbox(const wchar_t * wstr, BBox & bbox);

    ////
      void detach(GPUFontBase * gpuFont);

      const Glyph * getGlyph(unsigned int charCode);

      virtual bool load(const char * fontFilePath);

      virtual GPUFontBase * createGPUFont() = 0;
      GPUFont * getGPUFont(Luminous::GLResources * resources);

    protected:
      virtual Glyph * makeGlyph(unsigned int g) = 0;

      Face * m_face;
      Size m_size;
      int m_error;

    private:
      inline bool checkGlyph(unsigned int g);

      GlyphContainer * m_glyphList;
      Nimble::Vector2 m_pen;

      typedef std::vector<GPUFontBase *> container;  
      container m_gpuFonts;

      friend class GPUFont;

  };

}

#endif
