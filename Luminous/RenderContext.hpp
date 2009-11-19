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

#include <Luminous/Transformer.hpp>
#include <Luminous/GLResources.hpp>
#include <Luminous/Export.hpp>

#include <Nimble/Rect.hpp>

namespace Luminous
{

  /// RenderContext contains the current rendering state. 
  class LUMINOUS_API RenderContext : public Transformer
  {
  public:
    RenderContext(Luminous::GLResources * resources);
    virtual ~RenderContext();

    Luminous::GLResources * resources() { return m_resources; }

    virtual void prepare();
    virtual void finish();

    void setRecursionLimit(size_t limit) { m_recursionLimit = limit; }
    size_t recursionLimit() const { return m_recursionLimit; }

    void setRecursionDepth(size_t rd) { m_recursionDepth = rd; }
    size_t recursionDepth() const { return m_recursionDepth; }  

    void pushClipRect(const Nimble::Rect & area);
    void popClipRect();

    bool isVisible(const Nimble::Rect & area);
    const Nimble::Rect & visibleArea() const;

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
  protected:

    Luminous::GLResources * m_resources;

    size_t m_recursionLimit;
    size_t m_recursionDepth;

    std::stack<Nimble::Rect> m_clipStack;
  };

}

#endif
