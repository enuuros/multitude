/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */


#ifndef LUMINOUS_MULTIHEAD_HPP
#define LUMINOUS_MULTIHEAD_HPP

#include <Nimble/Rect.hpp>
#include <Nimble/Vector4.hpp>

#include <Luminous/GlKeyStone.hpp>

#include <Radiant/RefPtr.hpp>

#include <vector>

namespace Luminous {

  using Nimble::Rect;
  using Nimble::Vector2f;
  using Nimble::Vector2i;
  using Nimble::Vector4f;
  using Radiant::RefPtr;

  /// Class for managing information on multiple OpenGL vindows/viewports.
  /** This class stores information about the layout of multiple
      OpenGL windows and viewports. This information is used in
      systems that render OpenGL into several windows at
      once. MultiHead also includes keystone information, so that
      keystoning can be done by using skewed OpenGL transformation
      matrices. */
  class MultiHead : public ValueIO::HasValues
  {
  public:

    /// One OpenGL area
    /** Areas are roughly equivalent to OpenGL viewports. */
    class Area : public ValueIO::HasValues
    {
    public:
      Area();
      virtual ~Area();
      
      virtual bool readDom(DOMElement * elem);

      void setGeometry(int x, int y, int w, int h, bool copyToGraphics = true)
      {
	m_location.make(x, y);
	m_size.make(w, h);

	if(copyToGraphics)
	  setGraphicsGeometry(x, y, w, h);
      }

      void setSize(Vector2i size)
      {
	m_size = size;
      }

      void setGraphicsGeometry(int x, int y, int w, int h)
      {
	m_graphicsLocation.make(x, y);
	m_graphicsSize.make(w, h);
	updateBBox();
      }

      void setSeams(float left, float right, float bottom, float top)
      { m_seams.make(left, right, bottom, top); updateBBox(); }
      
      float maxSeam() { return m_seams.maximum(); }

      void applyGlState();
      void cleanEdges();

      GlKeyStone & keyStone() { return m_keyStone; }

      /// The pixel location of the area on the window
      const Vector2i & location() const { return m_location; }
      /// The pixel size of the area on the window
      const Vector2i & size() const { return m_size; }

      /// The offset of the graphics inside the area (virtual pixels)
      const Vector2f graphicsLocation(bool withseams = true) const
      { 
        return withseams ? 
          m_graphicsLocation - Nimble::Vector2f(m_seams[0], m_seams[3]) :
          m_graphicsLocation;
      }
      /// The size of the graphics inside this area (virtual pixels)
      const Vector2f graphicsSize(bool withseams = true) const
      {
        return withseams ? 
          m_graphicsSize + Nimble::Vector2f(m_seams[0] + m_seams[1],
                                           m_seams[2] + m_seams[3]) :
          m_graphicsSize;
      }

      /// The bounds of the graphics
      /** In principle this method only combines the information you
	  get with graphicsLocation() and graphicsSize(). In practice
	  how-ever that information is not exactly correct as */
      const Rect & graphicsBounds() const 
      { return m_graphicsBounds; }


      /** Converts a screen-space coordinate to image-space coordinate.
      @param loc The location vector to convert.
      @param windowheight height of the window
	    @param convOk Optional parameter that is set to true if the
      location is inside this area. Otherwise convOk is set to
      false.
      @return The vector in graphics coordinates.
      */
      Nimble::Vector2f windowToGraphics(Nimble::Vector2f loc, int windowheight, bool * convOk = 0);

      int active() const { return m_active; }

      void setPixelSizeCm(float sizeCm) { m_pixelSizeCm = sizeCm; }
      float pixelSizeCm() const { return m_pixelSizeCm; }
      float cmToPixels(float cm) { return cm / m_pixelSizeCm; }
    protected:

      void updateBBox();

      GlKeyStone m_keyStone;
      Vector2i   m_location;
      Vector2i   m_size;
      Vector2f   m_graphicsLocation;
      Vector2f   m_graphicsSize;
      Vector4f   m_seams;
      int        m_active;
      std::string m_comment;
      Rect m_graphicsBounds;
      float      m_pixelSizeCm;
    };

    /// One OpenGL window.
    /** A window is responsible for one OpenGL context. */
    class Window : public ValueIO::HasValues
    {
    public:
      Window();
      ~Window();

      /// Set the location and size of this window
      void setGeometry(int x, int y, int w, int h)
      {
	m_location.make(x, y);
	m_size.make(w, h);
      }

      /// Resize the window, and automatically one child area
      /** This method is used when the window contains only one child
	  area, and automatially changes the size of the area to match
	  the area of the window. */

      void resizeEvent(Vector2i size);
      
      /// Number of areas that this window holds
      unsigned areaCount() const { return m_areas.size(); }
      /// Get one of the areas
      Area & area(unsigned i) { return * m_areas[i].ptr(); }

      void setSeam(float seam);

      // From ValueIO::HasValues
      virtual DOMElement * writeDom(DOMDocument * doc);
      virtual bool readDom(DOMElement * e);

      void addArea(Area * a) { m_areas.push_back(a); }

      /// Location of the window on the computer display
      const Vector2i & location() const { return m_location; }
      /// Size of the window on the computer display
      const Vector2i & size() const { return m_size; }

      /** Convert a coordinate from screen to graphics coordinates.
        This class traverses through all the areas to find an area
        that would include given location. It is entirely possible
        none of the areas contains the given coordinate, as there
        can be gaps between areas (due to keystoning, for example).
	  
        @param loc The location in screen coordinates.

        @param convOk If this value is non-null, then it is set to
        true or false depending on whether the conversion could be
        carried out.
      */
      Nimble::Vector2f windowToGraphics(Nimble::Vector2f loc, bool * convOk=0);

      /// Should the window be frameless
      bool frameless()  const { return m_frameless; }
      /// Should the window be full-screen
      bool fullscreen() const { return m_fullscreen; }
      /// Should the window be resizeable
      bool resizeable() const { return m_resizeable; }

      void setPixelSizeCm(float sizeCm);

    protected:

      // From HasValues
      virtual bool readValue(DOMElement *);

      Vector2i   m_location;
      Vector2i   m_size;
      int        m_frameless;
      int        m_fullscreen;
      int        m_resizeable;
      /// Pixel size in centimeters
      float      m_pixelSizeCm;

      std::vector<RefPtr<Area> > m_areas;
    };

    MultiHead();
    virtual ~MultiHead();
    
    /// Create 1 window with 1 area.
    void makeSingle(int x, int y, int w, int h);
    /// Create 1 window with 2 areas horizontally side by side.
    void makeDouble(int x, int y, int w, int h, float seam);    
    /// Create 1 window with 4 rotated areas
    /** This is useful for making a setup with four
	projectors/displays that are aligned in portrait mode. */
    void makeQuadSideways(int x, int y, int w, int h, float seam);
    
    /// The number of windows
    unsigned windowCount() const { return m_windows.size(); }
    /// Access one of the windows
    Window & window(unsigned i) { return * m_windows[i].ptr(); }

    /// The number of areas
    unsigned areaCount();
    /// Access on of the areas
    /** This method traverses all the windows to find the area with
	given index. */
    Area & area(unsigned i, Window ** winptr = 0);

    float seam();
    void setSeam(float seam);

    /** Total width of the display area, in graphics pixels. */
    int width();
    /** Total height of the display area, in graphics pixels. */
    int height();

    virtual DOMElement * writeDom(DOMDocument * doc);
    virtual bool readDom(DOMElement * e);

    /// Simple test function
    /** This test function creates a setup, stores into a file, reads
	the setup from file and reports how things went. */
    static void test();

    void addWindow(Window * w) { m_windows.push_back(w); }

  protected:

    std::vector<RefPtr<Window> > m_windows;
    float m_widthcm;

  };

}

#endif
