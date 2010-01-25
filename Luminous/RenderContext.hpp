/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in
 * file "LGPL.txt" that is distributed with this source package or obtained
 * from the GNU organization (www.gnu.org).
 *
 */

#ifndef LUMINOUS_RENDERCONTEXT_HPP
#define LUMINOUS_RENDERCONTEXT_HPP

#include <Luminous/Luminous.hpp>
#include <Luminous/Transformer.hpp>
#include <Luminous/GLResource.hpp>
#include <Luminous/GLResources.hpp>
#include <Luminous/GLSLProgramObject.hpp>
#include <Luminous/Export.hpp>
#include <Luminous/VertexBuffer.hpp>

#include <Nimble/Rect.hpp>
#include <Nimble/Vector2.hpp>

namespace Luminous
{
  class Texture2D;

  /// RenderContext contains the current rendering state.
  class LUMINOUS_API RenderContext : public Transformer
  {
  public:

    enum BlendFunc {
      BLEND_USUAL,
      BLEND_NONE,
      BLEND_ADDITIVE,
      BLEND_SUBTRACTIVE
    };

    class FBOPackage;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /** Experimental support for getting temporary FBOs for this context.
        */
    class FBOHolder
    {
      friend class RenderContext;
    public:

      LUMINOUS_API FBOHolder();
      LUMINOUS_API FBOHolder(RenderContext * context, FBOPackage * package);
      LUMINOUS_API FBOHolder(const FBOHolder & that);

      LUMINOUS_API ~FBOHolder();

      /** Copies the data pointers from the argument object. */
      LUMINOUS_API FBOHolder & operator = (const FBOHolder & that);

      LUMINOUS_API Luminous::Texture2D * finish();
      /** The relative texture coordinates for this useful texture area. */
      inline const Nimble::Vector2 & texUV() const { return m_texUV; }

    private:

      void release();

      RenderContext * m_context;
      FBOPackage    * m_package;
      Nimble::Vector2 m_texUV;
    };

    enum {
      FBO_EXACT_SIZE = 0x1,
      VBO_VERBUF_SIZE = 10 * 2 * sizeof(GL_FLOAT),
      VBO_INDBUF_SIZE = 10
    };

#endif

    RenderContext(Luminous::GLResources * resources);
    virtual ~RenderContext();

    Luminous::GLResources * resources() { return m_resources; }

    virtual void prepare();
    virtual void finish();

    void setRecursionLimit(size_t limit) ;
    size_t recursionLimit() const;

    void setRecursionDepth(size_t rd);
    size_t recursionDepth() const;

    void pushClipRect(const Nimble::Rect & area);
    void popClipRect();

    bool isVisible(const Nimble::Rect & area);
    const Nimble::Rect & visibleArea() const;

    ///@internal
    FBOHolder getTemporaryFBO(Nimble::Vector2 basicsize,
                              float scaling, uint32_t flags = 0);

    // Render functions:

    /** Draw a line rectangle, with given thickness and color. */
    void drawLineRect(const Nimble::Rectf & rect, float thickness, const float * rgba);
    /** Draws a solid rectangle, with given thickness and color. */
    void drawRect(const Nimble::Rectf & rect, const float * rgba);

    /** Draws a solid circle. */
    void drawCircle(Nimble::Vector2f center, float radius,
                    const float * rgba, int segments = -1);

    /** Draws a line that contains multiple segments.

        @arg vertices Pointer to the line vertices

        @arg n Number of vertices

        @arg width Width of the line

        @arg rgba The line color in RGBA format
     */
    void drawPolyLine(const Nimble::Vector2f * vertices, int n,
                      float width, const float * rgba);
    /** Draw a textured rectangle with given color.

        @arg size The size of the rectangle to be drawn.

        @arg rgba The color in RGBA format. If the argument is null,
        then it will be ignored.
    */
    void drawTexRect(Nimble::Vector2 size, const float * rgba);
    void drawTexRect(Nimble::Vector2 size, const float * rgba,
                     const Nimble::Rect & texUV);
    void drawTexRect(Nimble::Vector2 size, const float * rgba,
                     Nimble::Vector2 texUV);

    // render function implemented with VBO. Work in progress!!!
    /// @todo more primitives?
    void renderVBO();
    void updateVBO();
    void drawLineRectVBO(const Nimble::Rectf & rect, float thickness, const float * rgba);
    void drawRectVBO(const Nimble::Rectf & rect, const float * rgba);
    void drawCircleVBO(Nimble::Vector2f center, float radius,
                    const float * rgba, int segments = -1);
    void drawPolyLineVBO(const Nimble::Vector2f * vertices, int n,
                      float width, const float * rgba);
    void drawTexRectVBO(Nimble::Vector2 size, const float * rgba);
    void drawTexRectVBO(Nimble::Vector2 size, const float * rgba,
                     const Nimble::Rect & texUV);
    void drawTexRectVBO(Nimble::Vector2 size, const float * rgba,
                     Nimble::Vector2 texUV);

    /// Sets the current blend function, and enables blending
    /** If the function is BLEND_NONE, then blending is disabled. */
    void setBlendFunc(BlendFunc f);

    static const char ** blendFuncNames();

  private:

    void clearTemporaryFBO(FBOPackage * fbo);

    Luminous::GLResources * m_resources;

    Luminous::VertexBuffer m_vb;
    Luminous::IndexBuffer  m_ib;
    Luminous::GLSLProgramObject* m_transformer;

    //float*                 m_pVB;
    //GLuint*                m_pIB;
    //GLuint                 m_vCounter;
    //GLuint                 m_iCounter;

    class Internal;
    Internal * m_data;

  };

}

#endif
