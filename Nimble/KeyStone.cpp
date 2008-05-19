/* COPYRIGHT
 *
 * This file is part of ConfigReader.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "ConfigReader.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "KeyStone.hpp"

#include <Nimble/Rect.hpp>
#include <Nimble/Matrix4.hpp>

namespace Nimble {

  KeyStone::KeyStone()
    : m_width(640),
      m_height(480),
      m_dpyWidth(2048),
      m_dpyHeight(768),
      m_dpyX(0),
      m_dpyY(0),
      m_extra(0, 0, 0, 0),
      m_containedPixelCount(m_width * m_height),
      m_version(0)
  { 
    m_matrix.identity();
    m_matrixOut.identity();
    m_matrixExtension.identity();

    setVertices("0 0  640 0  640 480  0 480", 640, 480, 1920, 1080, 0, 0);
    
    //calculateMatrix();
    //updateLimits();
  }

  void KeyStone::setVertices(const char * str,
			     int w, int h,
			     int dpyw, int dpyh,
			     int dpyx, int dpyy)
  {
    // printf("KeyStone::setVertices # (%s) \n%d %d\n", str, dpyw, dpyh);

    char * ptr = (char *) str;

    Vector2 vertices[4];
    
    float * corners = vertices[0].data();
    int i;
    
    for(i = 0; i < 8; i++)
      corners[i] = strtod(ptr, & ptr);
    
    setVertices(vertices, w, h, dpyw, dpyh, dpyx, dpyy);
  }

  void KeyStone::setVertices(const Nimble::Vector2 * vertices, 
			     int w, int h,
			     int dpyw, int dpyh,
			     int dpyx, int dpyy)
  {
    int i;

    m_lensCorrection.setCameraResolution(w, h);

    for(i = 0; i < 4; i++) {

      Vector2 tmp = vertices[i];

      tmp.x = Math::Min(tmp.x, (float) (w - 1));
      tmp.y = Math::Min(tmp.y, (float) (h - 1));

      m_originals[i] = tmp;
    }
    m_width = w;
    m_height = h;

    setOutputGeometry(dpyw, dpyh, dpyx, dpyy);

    /*
    for(i = 0; i < 4; i++) {

      Vector2 vtmp1(vertices[i]);
      Vector2 vtmp2 = project(vtmp1);
      
      printf("[%.6f %.6f] => [%.6f %.6f]\n", 
	     vtmp1.x, vtmp1.y, vtmp2.x, vtmp2.y);
    }
    */
    // printf("KeyStone::setVertices # DONE");

  }
    
  void KeyStone::setOutputGeometry(unsigned w, unsigned h, int x, int y)
  {
    m_dpyWidth  = w;
    m_dpyHeight = h;

    m_dpyX = x;
    m_dpyY = y;
    
    calculateMatrix();
    updateLimits();
  }


  void KeyStone::calculateMatrix()
  {
    // Formula from page 20.

    for(int i = 0; i < 4; i++) {
      Vector2 tmp = m_lensCorrection.correct(m_originals[i]);
      m_vertices[i].make(tmp.x / m_width, tmp.y / m_height);
    }

    m_matrix = projectionMatrix(m_vertices);
    
    bool ok = true;
    m_matrix = m_matrix.inverse(&ok);

    Matrix3 tmp;
    tmp.identity();
    tmp[0][0] = 1.0f / m_width;
    tmp[1][1] = 1.0f / m_height;
    
    m_matrix = m_matrix * tmp;

    tmp.identity();

    tmp[0][0] = m_dpyWidth;
    tmp[1][1] = m_dpyHeight;

    tmp[0][2] = m_dpyX;
    tmp[1][2] = m_dpyY;
    
    m_matrixOut = tmp * m_matrixExtension * m_matrix;

    updated();
  }

  Nimble::Vector2 KeyStone::project(const Nimble::Vector2 & v) const
  {
    // "p.z" is really "p.w", we need to do the "perspective" correction here.
    Vector3 p = m_matrixOut * m_lensCorrection.correct(v);
    return Vector2(p.x / p.z, p.y / p.z);
  }

  Nimble::Vector2 KeyStone::project01(const Nimble::Vector2 & v) const
  {
    // "p.z" is really "p.w", we need to do the "perspective" correction here.
    Vector3 p = m_matrix * m_lensCorrection.correct(v);
    return Vector2(p.x / p.z, p.y / p.z);
  }

  Nimble::Vector2 KeyStone::projectInverse(const Nimble::Vector2 & v) const
  {
    Matrix3 m = m_matrixOut.inverse();
    return project(m, v);
  }
  
  void  KeyStone::moveCorner(Nimble::Vector2 loc)
  {
    int index = closestCorner(loc);
    m_originals[index] = loc;

    calculateMatrix();
    updateLimits();    
  }

  int KeyStone::closestCorner(Nimble::Vector2 loc) const
  {
    float best = (m_originals[0] - loc).length();
    int index = 0;

    for(int i = 1; i < 4; i++) {
      float d2 = (m_originals[i] - loc).length();
      if(best > d2) {
	best = d2;
	index = i;
      }
    }

    return index;
  }


  void KeyStone::flipHorizontal()
  {
    Nimble::Vector2 tmp = m_originals[1];
    m_originals[1] = m_originals[0];
    m_originals[0] = tmp;

    tmp = m_originals[3];
    m_originals[3] = m_originals[2];
    m_originals[2] = tmp;
    
    calculateMatrix();
    updateLimits();
  }

  void KeyStone::flipVertical()
  {
    Nimble::Vector2 tmp = m_originals[1];
    m_originals[1] = m_originals[2];
    m_originals[2] = tmp;

    tmp = m_originals[3];
    m_originals[3] = m_originals[0];
    m_originals[0] = tmp;
    
    calculateMatrix();
    updateLimits();
  }

  void KeyStone::rotate(int turns)
  {
    for(int t = 0; t < turns; t++) {
      Nimble::Vector2 v = m_originals[0];
    
      for(int i = 0; i < 3; i++)
	m_originals[i] = m_originals[i + 1];

      m_originals[3] = v;

      calculateMatrix();
    }
  }

  void KeyStone::addExtra(int index, float v)
  {
    m_extra[index] += v;

    if(m_extra[index] < 0.0f)
      m_extra[index] = 0.0f;

    if(m_extra[index] >= 100.0f)
      m_extra[index] = 100.0f;
  }  

  Nimble::Rect KeyStone::outputBounds()
  {
    return Nimble::Rect(m_dpyX, m_dpyY,
				  m_dpyX + m_dpyWidth, m_dpyY + m_dpyHeight);
  }

  void KeyStone::setLensParam(int i, float v)
  {
    m_lensCorrection.setParam(i, v);
    calculateMatrix();
    updateLimits();
  }

  void KeyStone::calibrateOutput(const Nimble::Vector2 * targets,
				 const Nimble::Vector2 * real)
  {
    int i;

    Nimble::Vector2 tnorm[4]; // Target points in [0-1] space
    Nimble::Vector2 rnorm[4]; // Real points in [0-1] space

    Matrix3 tmp;
    tmp.identity();

    tmp[0][0] = m_dpyWidth;
    tmp[1][1] = m_dpyHeight;

    tmp[0][2] = m_dpyX;
    tmp[1][2] = m_dpyY;
    
    Nimble::Matrix3 backToNorm = (tmp * m_matrixExtension).inverse();
    
    for(i = 0; i < 4; i++) {
      tnorm[i] = project(backToNorm, targets[i]);
      rnorm[i] = project(backToNorm, real[i]);
      /*
      printf("KeyStone::calibrateOutput # target = [%f %f] vs "
             "real = [%f %f]\n",
	     tnorm[i].x, tnorm[i].y, rnorm[i].x, rnorm[i].y);
      */
    }
    
    Nimble::Matrix3 realToNormalized = projectionMatrix(rnorm).inverse();
    Nimble::Matrix3 normalizedToTarget = projectionMatrix(tnorm);

    m_matrixExtension =
      m_matrixExtension * normalizedToTarget * realToNormalized;

    calculateMatrix();

  }

  void KeyStone::setOutputExtension(const Nimble::Matrix3 & m)
  {
    m_matrixExtension = m;
    calculateMatrix();
  }

  void KeyStone::updateLimits()
  {
    updateLimits(m_limits);

    Vector2 tl = topLeft();
    Vector2 tr = topRight();

    Vector2 bl = bottomLeft();
    Vector2 br = bottomRight();

    float ksw = (tr.x - tl.x + br.x - bl.x) * 0.5f;
    float ksh = (bl.y - tl.y + br.y - tr.y) * 0.5f;

    /*
    std::cout << tl << " # " << tr << " # " << bl << " # " << br << std::endl;
    
    printf("tl = [%.1f %.1f] ksw = %f, ksh = %f\n", tl.x, tl.y, ksw, ksh);
    */
    Vector4 scaledExtra(m_extra[0] / ksw, m_extra[1] / ksw,
                        m_extra[2] / ksh, m_extra[3] / ksh);

    updateLimits(m_extraLimits, & scaledExtra);
  }

  void KeyStone::updateLimits(std::vector<Nimble::Vector2i> & limits, 
                              const Vector4 * offsets)
  {
    limits.resize(m_height);

    Rect bounds(0, 0, 1, 1);

    if(offsets) {
      /*
      printf("KeyStone::updateLimits # offsets: %f %f %f %f\n",
             (*offsets)[0], (*offsets)[1], (*offsets)[2], (*offsets)[3]);
      */
      bounds.low().x -= (*offsets)[0];
      bounds.low().y -= (*offsets)[2];
      bounds.high().x += (*offsets)[1];
      bounds.high().y += (*offsets)[3];
    }

    int count = 0;

    for(int y = 0; y < m_height; y++) {
      
      int first = 0;
      int last = 0;

      bool inside = false;

      for(int x = 0; x < m_width; x++) {
        
        Vector2 v1 = m_lensCorrection.correct(Vector2(x, y));
        v1 = project(m_matrix, v1);
        
        bool in = bounds.contains(v1);

	if(!in) {
	  if(inside) {
	    last = x;
	    inside = false;
	    break;
	  }
	}
	else {
	  count++;
	  if(!inside) {
	    first = x;
	    inside = true;
	  }
	}
      }

      if(inside)
	last = m_width - 1;

      int wid = last - first;
      /* v = project(Vector2(640, y));
	 printf("Projecion limits[%d] = %d - %d (%d %.3f %.3f)\n", y, 
	 first, last, wid, v.x, v.y);
      */
      limits[y].make(first, wid);
    }
    
    if(!offsets)
      m_containedPixelCount = count;

    updated();
  }

  /** Calculates the projection matrix. See Paul Heckbert's master's
   * thesis, pages 19-21. */

  Matrix3 KeyStone::projectionMatrix(const Vector2 * vertices)
  {
    float dx1 = vertices[1].x - vertices[2].x;
    float dx2 = vertices[3].x - vertices[2].x;
    float dy1 = vertices[1].y - vertices[2].y;
    float dy2 = vertices[3].y - vertices[2].y;

    float sx = vertices[0].x - vertices[1].x + 
      vertices[2].x - vertices[3].x;

    float sy = vertices[0].y - vertices[1].y + 
      vertices[2].y - vertices[3].y;

    float del = Math::Det(dx1, dx2, dy1, dy2);

    float g = Math::Det(sx, dx2, sy, dy2) / del;
    float h = Math::Det(dx1, sx, dy1, sy) / del;

    float a = vertices[1].x - vertices[0].x + g * vertices[1].x;
    float b = vertices[3].x - vertices[0].x + h * vertices[3].x;
    float c = vertices[0].x;

    float d = vertices[1].y - vertices[0].y + g * vertices[1].y;
    float e = vertices[3].y - vertices[0].y + h * vertices[3].y;
    float f = vertices[0].y;

    return Matrix3(a, b, c,
		   d, e, f,
		   g, h, 1);
  }
}
