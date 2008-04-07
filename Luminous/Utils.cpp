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

#include <Luminous/Utils.hpp>
#include <Luminous/MatrixStep.hpp>

#include <Nimble/Vector3.hpp>


#include <cmath>
#include <cassert>

namespace Luminous {

  using namespace Nimble;

  void Utils::blendCenterSeamHorizontal(int w, int h,
					int seamWidth,
					bool withGrid)
  {
    assert(seamWidth > 1); // NVidia bug...
    /* printf("Utils::blendCenterSeamHorizontal %d\n", seamWidth);
       fflush(0); */
    
    glViewport (0, 0, w, h);
    
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glUseProgram(0);
    // glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    
    int i;
    int center = w / 2;
    int left = center - seamWidth;
    int right = center + seamWidth;
    
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);

    glRasterPos2i(center, h);
    glCopyPixels(left, 0, center, h, GL_COLOR);
     
    glBegin(GL_QUAD_STRIP);
    
    float p = 2.2f;

    for(i = 0; i < 5; i++) {
      float rel = i / (float) 5;
      float x = left + seamWidth * rel;
      glColor4f(0, 0, 0, powf(rel, p));
      glVertex2f(x, 0);
      glVertex2f(x, h);
    }

    glColor4f(0, 0, 0, 1);
    glVertex2i(center, 0);
    glVertex2i(center, h);
    
    for(i = 1; i <= 5; i++) {
      float rel = i / (float) 5;
      float x = center + seamWidth * rel;
      glColor4f(0, 0, 0, powf(1.0f - rel, p));
      glVertex2f(x, 0);
      glVertex2f(x, h);
    }

    /* glColor4f(0, 0, 0, 0);
       glVertex2i(right, 0);
       glVertex2i(right, h);
    */

    glEnd();

    // glFlush();

    /* glBlendEquation(GL_ADD);
       glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    */

    if(!withGrid)
      return;

    glLineWidth(3);

    glBegin(GL_LINES);

    int n = 10;
    float first = 1.5;
    float last = center - 1.5;
    float ampl = last - first;

    for(i = 0; i <= n; i++) {
      float x = first + ampl * i / (float) n;
      glColor3f(1, 0, 0);
      glVertex2f(x, 0);
      glVertex2f(x, h);

      x += center;
      glColor3f(0, 1, 0);
      glVertex2f(x, 0);
      glVertex2f(x, h);
    }

    last = h - 1.5;
    ampl = last - first;

    
    glColor3f(1, 0, 0);
    glVertex2f(left, 0);
    glVertex2f(left, h);

    glColor3f(0, 1, 0);
    glVertex2f(right, 0);
    glVertex2f(right, h);

    for(i = 0; i <= n; i++) {
      float y = first + ampl * i / (float) n;
      glColor3f(1, 0, 0);
      glVertex2f(0, y);
      glVertex2f(center, y);

      glColor3f(0, 1, 0);
      glVertex2f(center, y);
      glVertex2f(w, y);
    }
    
    glEnd();
  }
  
  void Utils::fadeEdge(float w, float h, float seam,
		       float gamma, Edge e, bool withGrid)
  {
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glUseProgram(0);
    // glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);

    MatrixStep ms;
    
    if(e == RIGHT) // The default
      ;
    else if(e == LEFT) {
      glScalef(-1, 1, 1);
      glTranslatef(-w, 0, 0);
    }

    int i, n = 16;
    float left = w - seam;

    glBegin(GL_QUAD_STRIP);
    
    // glColor4f(0, 0, 0, 1);

    for(i = 0; i <= n; i++) {

      float rel = i / (float) n;
      float x = left + seam * rel;
      
      glColor4f(0, 0, 0, powf(rel, gamma));
      glVertex2f(x, 0);
      glVertex2f(x, h);
    }

    glEnd();

    if(!withGrid)
      return;

    glLineWidth(3);

    static const Vector3 colors[4] = {
      Vector3(1, 0, 0),
      Vector3(0, 1, 0),
      Vector3(0, 0, 1),
      Vector3(0.5, 0.5, 0)
    };
  
    glColor3fv(colors[(int) e].data());
    glBegin(GL_LINES);

    n = 10;
    float first = 1.5;
    float last = w - 1.5;
    float ampl = last - first;

    for(i = 0; i <= n; i++) {
      float x = first + ampl * i / (float) n;
      glVertex2f(x, 0);
      glVertex2f(x, h);
    }

    last = h - 1.5;
    ampl = last - first;
    
    glVertex2f(left, 0);
    glVertex2f(left, h);

    for(i = 0; i <= n; i++) {
      float y = first + ampl * i / (float) n;
      glVertex2f(0, y);
      glVertex2f(w, y);
    }

    glVertex2f(0, 0);
    glVertex2f(w, h);

    glVertex2f(w, 0);
    glVertex2f(0, h);

    glColor3f(0, 0, 0);
    
    glVertex2f(left + seam * 0.5f, 0);
    glVertex2f(left + seam * 0.5f, h);

    glEnd();
  }

  void Utils::glTexRect(float x1, float y1, float x2, float y2)
  {
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2f(x1, y1);

    glTexCoord2f(1, 0);
    glVertex2f(x2, y1);

    glTexCoord2f(1, 1);
    glVertex2f(x2, y2);

    glTexCoord2f(0, 1);
    glVertex2f(x1, y2);

    glEnd();
  }

  void Utils::glTexRect(Nimble::Vector2 size, const Nimble::Matrix3 & m)
  {
    const Vector3 v[4] = {
      m * Vector2(0,       0),
      m * Vector2(size.x,  0),
      m * Vector2(size.x,  size.y),
      m * Vector2(0,       size.y)
    };

    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2fv(v[0].data());

    glTexCoord2f(1, 0);
    glVertex2fv(v[1].data());
  
    glTexCoord2f(1, 1);
    glVertex2fv(v[2].data());
    
    glTexCoord2f(0, 1);
    glVertex2fv(v[3].data());

    /*

    This is wrong. The Z-value is the homogenous coordinate, and it
    should be 1 (usually).  Consider using glVertex4f if you really
    want to pass the value somewhere.

    glTexCoord2f(0, 0);
    glVertex3fv(static_cast<const GLfloat *> (&v[0].x));

    glTexCoord2f(1, 0);
    glVertex3fv(static_cast<const GLfloat *> (&v[1].x));
  
    glTexCoord2f(1, 1);
    glVertex3fv(static_cast<const GLfloat *> (&v[2].x));
    
    glTexCoord2f(0, 1);
    glVertex3fv(static_cast<const GLfloat *> (&v[3].x));
    */

    glEnd();
  }

  void Utils::glCenteredTexRect(Nimble::Vector2 size, const Nimble::Matrix3 & m)
  {
    float xh = size.x * 0.5f;
    float yh = size.y * 0.5f;
    const Vector3 v[4] = {
      m * Vector2(-xh,     -yh),
      m * Vector2(xh,   -yh),
      m * Vector2(xh,   yh),
      m * Vector2(-xh,       yh)
    };

    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2fv(v[0].data());

    glTexCoord2f(1, 0);
    glVertex2fv(v[1].data());
  
    glTexCoord2f(1, 1);
    glVertex2fv(v[2].data());
    
    glTexCoord2f(0, 1);
    glVertex2fv(v[3].data());

    glEnd();
  }

  void Utils::glLineRect(float x1, float y1, float x2, float y2)
  {
    glBegin(GL_LINE_STRIP);

    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glVertex2f(x1, y1);

    glEnd();
  }

  void Utils::glLineRect(const float * v1, const float * v2)
  {
    glLineRect(v1[0], v1[1], v2[0], v2[1]);
  }

  void Utils::glPoint(float x, float y)
  {
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
  }

  void Utils::glPoint(float * v)
  {
    glBegin(GL_POINTS);
    glVertex2fv(v);
    glEnd();
  }


  void Utils::glLine(float x1, float y1, float x2, float y2)
  {
    glBegin(GL_LINES);

    glVertex2f(x1, y1);
    glVertex2f(x2, y2);

    glEnd();
  }

  void Utils::glLine(const float * p1, const float * p2)
  {
    glBegin(GL_LINES);

    glVertex2fv(p1);
    glVertex2fv(p2);

    glEnd();

  }

  void Utils::glSoftLine(float x1, float y1, float x2, float y2, float width,
                         const float * color)
  {
    float r = color[0];
    float g = color[1];
    float b = color[2];
    float a = color[3];

    Vector2f begin(x1, y1);
    Vector2f end(x2, y2);
    Vector2f dir(end - begin);
    Vector2f up(dir.y, - dir.x);
    up.normalize(width);

    glBegin(GL_QUAD_STRIP);
    
    glColor4f(r, g, b, 0);
    glVertex2fv((begin - up).data());
    glVertex2fv((end - up).data());

    glColor4f(r, g, b, a);
    glVertex2fv(begin.data());
    glVertex2fv(end.data());

    glColor4f(r, g, b, 0);
    glVertex2fv((begin + up).data());
    glVertex2fv((end + up).data());

    glEnd();
  }

  void Utils::glSoftLine(const float * v1, const float * v2, float width,
			 const float * color)
  {
    glSoftLine(v1[0], v1[1], v2[0], v2[1], width, color);
  }

  void Utils::glFilledSoftLine(float x1, float y1, float x2, float y2,
			       float width, float edgeWidth,
			       const float * color)
  {
    float r = color[0];
    float g = color[1];
    float b = color[2];
    float a = color[3];

    Vector2f begin(x1, y1);
    Vector2f end(x2, y2);
    Vector2f dir(end - begin);
    Vector2f up(dir.y, - dir.x);
    Vector2f up2(up);
    up.normalize(width);
    up2.normalize(edgeWidth);
    Vector2f dir2(-up2.y, up2.x);

    glBegin(GL_QUADS);
    
    glColor4f(r, g, b, a);
    glVertex2fv((begin - up).data());
    glVertex2fv((end - up).data());

    glVertex2fv((end + up).data());
    glVertex2fv((begin + up).data());

    glEnd();

    glBegin(GL_QUAD_STRIP);

    
    glColor4f(r, g, b, a);
    glVertex2fv((begin - up).data());

    glColor4f(r, g, b, 0);
    glVertex2fv((begin - up - up2 - dir2).data());

    glColor4f(r, g, b, a);
    glVertex2fv((end - up).data());

    glColor4f(r, g, b, 0);
    glVertex2fv((end - up - up2 + dir2).data());

    glColor4f(r, g, b, a);
    glVertex2fv((end + up).data());

    glColor4f(r, g, b, 0);
    glVertex2fv((end + up + up2 + dir2).data());

    glColor4f(r, g, b, a);
    glVertex2fv((begin + up).data());

    glColor4f(r, g, b, 0);
    glVertex2fv((begin + up + up2 - dir2).data());

    // Repeat final points:
    glColor4f(r, g, b, a);
    glVertex2fv((begin - up).data());

    glColor4f(r, g, b, 0);
    glVertex2fv((begin - up - up2 - dir2).data());

    glEnd();
  }

  void Utils::glFilledSoftLine(const float * v1, const float * v2, float width,
			       float blendwidth, const float * color)
  {
    glFilledSoftLine(v1[0], v1[1], v2[0], v2[1], width, blendwidth, color);
  }

  void Utils::glCross(float centerx, float centery, float size, float radians)
  {
    const float   halfSize = size / 2.0f;
    Vector2f      vertices[4] =
    {
      Vector2f(-halfSize, 0.0f),
      Vector2f(halfSize, 0.0f),
      Vector2f(0.0f, -halfSize),
      Vector2f(0.0f, halfSize)
    };

    for(int i = 0; i < 4; i++)
    {
      vertices[i].rotate(radians);
      vertices[i] += Vector2f(centerx, centery);
    }

    glLine(vertices[0].data(), vertices[1].data());
    glLine(vertices[2].data(), vertices[3].data());
  }

  void Utils::glArc(float centerx, float centery, float radius,
		    float fromRadians, float toRadians, int linesegments)
  {
    float delta = (toRadians - fromRadians) / linesegments;

    glBegin(GL_LINE_STRIP);
    
    for(int i = 0; i <= linesegments; i++) {
      float angle = fromRadians + i * delta;
      glVertex2f(sinf(angle) * radius + centerx,
		 -cosf(angle) * radius + centery);
    }

    glEnd();
  }

  void Utils::glCrossf(float centerx, float centery, float size, float radians)
  {
    glCross(centerx, centery, size, radians);
  }

  void Utils::glCross(const float * loc, float size, float radians)
  {
    glCross(loc[0], loc[1], size, radians);
  }

  void Utils::glSoftArc(float centerx, float centery, float radius,
			float fromRadians, float toRadians, float width,
			int linesegments, const float * color)
  {
    float r = color[0];
    float g = color[1];
    float b = color[2];
    float a = color[3];

    float delta = (toRadians - fromRadians) / linesegments;

    glBegin(GL_QUAD_STRIP);

    float outrad = radius + width;

    for(int i = 0; i <= linesegments; i++) {
      float angle = fromRadians + i * delta;
      float sa = sinf(angle);
      float ca = cosf(angle);
      glColor4f(r, g, b, a);
      glVertex2f(sa * radius + centerx,
		 ca * radius + centery);
      glColor4f(r, g, b, 0);
      glVertex2f(sa * outrad + centerx,
		 ca * outrad + centery);
    }

    glEnd();

    glBegin(GL_QUAD_STRIP);

    float inrad = radius - width;

    for(int i = 0; i <= linesegments; i++) {
      float angle = fromRadians + i * delta;
      float sa = sinf(angle);
      float ca = cosf(angle);
      glColor4f(r, g, b, a);
      glVertex2f(sa * radius + centerx,
		 ca * radius + centery);
      glColor4f(r, g, b, 0);
      glVertex2f(sa * inrad + centerx,
		 ca * inrad + centery);
    }

    glEnd();

  }

  void Utils::glFilledSoftArc(float centerx, float centery, float radius,
			      float fromRadians, float toRadians, float width,
			      float blendwidth,
			      int linesegments, const float * color)
  {
    float r = color[0];
    float g = color[1];
    float b = color[2];
    float a = color[3];

    float delta = (toRadians - fromRadians) / linesegments;

    width *= 0.5f;

    float rs[4] = {
      radius - width - blendwidth, radius - width,
      radius + width, radius + width + blendwidth
    };

    for(int i = 0; i < linesegments; i++) {
      float a1 = fromRadians + i * delta;
      float a2 = fromRadians + (i + 1) * delta;
      float sa1 = sinf(a1);
      float ca1 = cosf(a1);
      float sa2 = sinf(a2);
      float ca2 = cosf(a2);

      glBegin(GL_QUAD_STRIP);
      
      glColor4f(r, g, b, 0);

      glVertex2f(sa1 * rs[0] + centerx, ca1 * rs[0] + centery);
      glVertex2f(sa2 * rs[0] + centerx, ca2 * rs[0] + centery);

      glColor4f(r, g, b, a);

      glVertex2f(sa1 * rs[1] + centerx, ca1 * rs[1] + centery);
      glVertex2f(sa2 * rs[1] + centerx, ca2 * rs[1] + centery);

      glVertex2f(sa1 * rs[2] + centerx, ca1 * rs[2] + centery);
      glVertex2f(sa2 * rs[2] + centerx, ca2 * rs[2] + centery);

      glColor4f(r, g, b, 0);

      glVertex2f(sa1 * rs[3] + centerx, ca1 * rs[3] + centery);
      glVertex2f(sa2 * rs[3] + centerx, ca2 * rs[3] + centery);

      glEnd();
    }    

  }

  void Utils::glCircle(float centerx, float centery, float radius,
    int linesegments)
  {
    glArc(centerx, centery, radius, 0.0f, Nimble::Math::TWO_PI, linesegments);
  }

  void Utils::glFilledCirclef(float centerx, float centery, float radius,
    int lineSegments)
  {
    assert(lineSegments >= 3);

    const float    delta = Nimble::Math::TWO_PI / lineSegments;

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(centerx, centery);
    for(int i = 0; i <= lineSegments; i++)
    {
      float   angle = i * delta;
      glVertex2f(sinf(angle) * radius + centerx, -cosf(angle) * radius + centery);
    }

    glEnd();
  }

  void Utils::glSoftCircle(float centerx, float centery, float radius,
                           float width,
                           int linesegments, const float * color)
  {
    glSoftArc(centerx, centery, radius, 0, Nimble::Math::TWO_PI,
	      width, linesegments, color);
  }

  void Utils::glFilledSoftCircle(float centerx, float centery, float radius,
				 float width, float blendwidth,
				 int linesegments, const float * color)
  {
    glFilledSoftArc(centerx, centery, radius, 0, Nimble::Math::TWO_PI,
		    width, blendwidth, linesegments, color);
  }

  void Utils::glSectorf(float centerx, float centery, float radius,
    float fromRadians, float toRadians, int lineSegments)
  {
    assert(lineSegments >= 3);

    float delta = (toRadians - fromRadians) / lineSegments;

    Vector2f  vertices[lineSegments + 1];
    for(int i = 0; i <= lineSegments; i++)
    {
      float   angle = fromRadians + i * delta;
      vertices[i].x = sinf(angle) * radius + centerx;
      vertices[i].y = -cosf(angle) * radius + centery;
    }

    glBegin(GL_LINE_STRIP);

    for(int i = 0; i <= lineSegments; i++)
    {
      glVertex2f(vertices[i].x, vertices[i].y);
    }

    glEnd();

    glLine(centerx, centery, vertices[0].x, vertices[0].y);
    glLine(centerx, centery, vertices[lineSegments].x, vertices[lineSegments].y);
  }

  void Utils::glFilledSectorf(float centerx, float centery, float radius,
    float fromRadians, float toRadians, int lineSegments)
  {
    assert(lineSegments >= 3);

    const float    delta = (toRadians - fromRadians) / lineSegments;

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(centerx, centery);
    for(int i = 0; i <= lineSegments; i++)
    {
      float    angle = fromRadians + i * delta;
      glVertex2f(sinf(angle) * radius + centerx, -cosf(angle) * radius + centery);
    }

    glEnd();
  }

  void Utils::glRoundedRectf(const float x1, const float y1, const float x2, const float y2,
    const float cornerRadius, const int cornerLineSegments)
  {
    assert(cornerLineSegments > 0);

    const Vector2  arcCenters[4] =
    {
      Vector2(x2 - cornerRadius, y1 + cornerRadius),
      Vector2(x2 - cornerRadius, y2 - cornerRadius),
      Vector2(x1 + cornerRadius, y2 - cornerRadius),
      Vector2(x1 + cornerRadius, y1 + cornerRadius)
    };

    const float   delta = Math::HALF_PI / float(cornerLineSegments);

    glBegin(GL_LINE_LOOP);

    int     i = 0, j = 0;
    float   fromRadians = 0.0f, angle = 0.0f, x = 0.0f, y = 0.0f;
    for(i = 0; i < 4; i++)
    {
      for(j = 0; j <= cornerLineSegments; j++)
      {
        angle = fromRadians + j * delta;
        x = sinf(angle) * cornerRadius + arcCenters[i].x;
        y = -cosf(angle) * cornerRadius + arcCenters[i].y;
        glVertex2f(x, y);
      }
      fromRadians += Math::HALF_PI;
    }

    glEnd();
  }

  void Utils::glRoundedRectfv(const Vector2 & low, const Vector2 & high,
    const float cornerRadius, const int cornerLineSegments)
  {
    glRoundedRectf(low.x, low.y, high.x, high.y, cornerRadius, cornerLineSegments);
  }

  void Utils::glFilledRoundedRectf(const float x1, const float y1, const float x2, const float y2,
    const float cornerRadius, const int cornerLineSegments)
  {
    assert(cornerLineSegments > 0);

    const Vector2  arcCenters[4] =
    {
      Vector2(x2 - cornerRadius, y1 + cornerRadius),
      Vector2(x2 - cornerRadius, y2 - cornerRadius),
      Vector2(x1 + cornerRadius, y2 - cornerRadius),
      Vector2(x1 + cornerRadius, y1 + cornerRadius)
    };

    const float   delta = Math::HALF_PI / float(cornerLineSegments);

    glBegin(GL_POLYGON);

    int     i = 0, j = 0;
    float   fromRadians = 0.0f, angle = 0.0f, x = 0.0f, y = 0.0f;
    for(i = 0; i < 4; i++)
    {
      for(j = 0; j <= cornerLineSegments; j++)
      {
        angle = fromRadians + j * delta;
        x = sinf(angle) * cornerRadius + arcCenters[i].x;
        y = -cosf(angle) * cornerRadius + arcCenters[i].y;
        glVertex2f(x, y);
      }
      fromRadians += Math::HALF_PI;
    }

    glEnd();
  }

  void Utils::glFilledRoundedRectfv(const Vector2 & low, const Vector2 & high,
    const float cornerRadius, const int cornerLineSegments)
  {
    glFilledRoundedRectf(low.x, low.y, high.x, high.y, cornerRadius, cornerLineSegments);
  }

  void Utils::glUsualBlend()
  {
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void Utils::glAdditiveBlend()
  {
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }

  void Utils::glGrayf(float level)
  {
    glColor3f(level, level, level);
  }

  bool Utils::glCheck(const char * msg)
  {
    GLenum e = glGetError();
    if(e) {
      printf("%s # GL ERROR %s", msg, gluErrorString(e));
      fflush(stdout);
    }

    return e == 0;
  }
}
