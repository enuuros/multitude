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

#include "RenderContext.hpp"
#include "Texture.hpp"
#include "FramebufferObject.hpp"

#include "Utils.hpp"
#include "GLSLProgramObject.hpp"

#include <strings.h>

#define DEFAULT_RECURSION_LIMIT 4

namespace Luminous
{

  using namespace Nimble;
  using namespace Radiant;
  using namespace Luminous;

  class RenderContext::FBOPackage : public GLResource
  {
  public:
    friend class FBOHolder;


    FBOPackage() : m_users(0) {}
    virtual ~FBOPackage() {}

    void setSize(Nimble::Vector2i size)
    {
      m_tex.bind();
      m_tex.setWidth(size.x);
      m_tex.setHeight(size.y);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, 0);

      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // <- essential on Nvidia
    }

    void activate()
    {
      m_fbo.attachTexture2D(&m_tex, Luminous::COLOR0, 0);
      m_fbo.check();
    }

    int userCount() const { return m_users; }

    Luminous::Framebuffer   m_fbo;
    Luminous::Renderbuffer  m_rbo;
    Luminous::Texture2D     m_tex;
    int m_users;
  };

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////

  RenderContext::FBOHolder::FBOHolder()
      : m_context(0),
      m_package(0),
      m_texUV(1,1)
  {
  }

  RenderContext::FBOHolder::FBOHolder(RenderContext * context, FBOPackage * package)
      : m_context(context), m_package(package),
      m_texUV(1,1)
  {
    m_package->m_users++;
  }

  RenderContext::FBOHolder::FBOHolder(const FBOHolder & that)
  {
    FBOHolder * that2 = (FBOHolder *) & that;
    m_context = that2->m_context;
    m_package = that2->m_package;
    m_texUV   = that2->m_texUV;
    m_package->m_users++;
  }

  RenderContext::FBOHolder::~FBOHolder()
  {
    release();
  }

  /** Copies the data pointers from the argument object. */
  RenderContext::FBOHolder & RenderContext::FBOHolder::operator =
      (const RenderContext::FBOHolder & that)
  {
    release();
    FBOHolder * that2 = (FBOHolder *) & that;
    m_context = that2->m_context;
    m_package = that2->m_package;
    m_texUV   = that2->m_texUV;
    m_package->m_users++;
    return *this;
  }


  Luminous::Texture2D * RenderContext::FBOHolder::finish()
  {
    if(!m_package)
      return 0;

    Luminous::Texture2D * tex = & m_package->m_tex;

    release();

    return tex;
  }

  void RenderContext::FBOHolder::release()
  {
    if(m_package) {
      m_package->m_users--;

      if(!m_package->m_users) {
        m_context->clearTemporaryFBO(m_package);
      }

      m_package = 0;
      m_context = 0;
    }
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////

  class RenderContext::Internal
  {
  public:
    enum { FBO_STACK_SIZE = 100 };

    Internal()
        : m_recursionLimit(DEFAULT_RECURSION_LIMIT),
        m_recursionDepth(0),
        m_fboStackIndex(-1)
    {
      bzero(m_fboStack, sizeof(m_fboStack));
    }

    void pushFBO(FBOPackage * fbo)
    {
      m_fboStackIndex++;
      assert(m_fboStackIndex < FBO_STACK_SIZE);
      m_fboStack[m_fboStackIndex] = fbo;
    }

    FBOPackage * popFBO(FBOPackage * fbo)
    {
      assert(fbo == m_fboStack[m_fboStackIndex]);
      m_fboStackIndex--;

      if(m_fboStackIndex >= 0)
        return m_fboStack[m_fboStackIndex];

      return 0;
    }

    size_t m_recursionLimit;
    size_t m_recursionDepth;

    std::stack<Nimble::Rectangle> m_clipStack;

    typedef std::list<Radiant::RefPtr<FBOPackage> > FBOPackages;

    FBOPackages m_fbos;


    FBOPackage * m_fboStack[FBO_STACK_SIZE];
    int m_fboStackIndex;
    // temporarilly having screen size to make it work for lod and AA.
    Vector2i m_screenSize;
  };

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////

  RenderContext::RenderContext(Luminous::GLResources * resources)
      : Transformer(),
      m_resources(resources),
      m_data(new Internal)
  {
    resetTransform();
    m_data->m_recursionDepth = 0;

    // Make sure the clip stack is empty
    while(!m_data->m_clipStack.empty())
      m_data->m_clipStack.pop();
  }

  RenderContext::~RenderContext()
  {
    delete m_data;
  }

  void RenderContext::prepare()
  {
    resetTransform();
    m_data->m_recursionDepth = 0;

    // Make sure the clip stack is empty
    while(!m_data->m_clipStack.empty())
      m_data->m_clipStack.pop();


    static bool once = true;
    if(once) {
      m_vb.allocate(VBO_VERBUF_SIZE, VertexBuffer::STATIC_DRAW);
      m_ib.allocate(VBO_INDBUF_SIZE, IndexBuffer::STATIC_DRAW);

      float * pVB = static_cast<float *> (m_vb.map(VertexBuffer::WRITE_ONLY));
      GLuint * pIB = static_cast<GLuint *> (m_ib.map(IndexBuffer::WRITE_ONLY));

      // rectangle
      const Vector4d vr[4] = {
        Vector4d(0, 0, 0, 1),
        Vector4d(1, 0, 0, 1),
        Vector4d(1, 1, 0, 1),
        Vector4d(0, 1, 0, 1)
      };

      for(int i = 0; i < 4; i++) {
        // position
        *(pVB++) = vr[i].x;
        *(pVB++) = vr[i].y;
        // texCoord
        *(pVB++) = vr[i].x;
        *(pVB++) = vr[i].y;
      }

      Nimble::Vector4d up(0,1,0,1);
      Nimble::Vector4d right(1,0,0,1);
      up *= (10.0/m_data->m_screenSize.y);
      right *= (10.0/m_data->m_screenSize.x);

      *(pVB++) = vr[0].x - up.x - right.x;
      *(pVB++) = vr[0].y - up.y - right.y;
      *(pVB++) = vr[0].x;
      *(pVB++) = vr[0].y;

      *(pVB++) = vr[1].x - up.x + right.x;
      *(pVB++) = vr[1].y - up.y + right.y;
      *(pVB++) = vr[1].x;
      *(pVB++) = vr[1].y;

      *(pVB++) = vr[2].x + up.x + right.x;
      *(pVB++) = vr[2].y + up.y + right.y;
      *(pVB++) = vr[2].x;
      *(pVB++) = vr[2].y;

      *(pVB++) = vr[3].x + up.x - right.x;
      *(pVB++) = vr[3].y + up.y - right.y;
      *(pVB++) = vr[3].x;
      *(pVB++) = vr[3].y;

      // use indices 0-15 for rectangle
      *(pIB++) = 0;
      *(pIB++) = 1;
      *(pIB++) = 3;
      *(pIB++) = 2;

      *(pIB++) = 3;
      *(pIB++) = 2;
      *(pIB++) = 6;
      *(pIB++) = 1;
      *(pIB++) = 5;
      *(pIB++) = 0;
      *(pIB++) = 4;
      *(pIB++) = 3;
      *(pIB++) = 7;
      *(pIB++) = 6;



      // circle
      // vertex count in level n = 2^n + 2
      int level = LOD_MAXIMUM;  // level of detail
      int segments = 1 << LOD_MAXIMUM;

      float delta = Math::TWO_PI / segments;
      GLuint offset = 16; // 32 floats already in vbo
      //info("SEGMENTS:%d DELTA:%f", segments, delta);

      *(pVB++) = 0.0f;
      *(pVB++) = 0.0f;

      for(int i = 0; i <= segments; i++) {
        float a = i * delta;
        float sa = sinf(a);
        float ca = -cosf(a);
        *(pVB++) = sa;
        *(pVB++) = ca;
      }

      int step;
      for(int i = 2; i <= level; i++) {
        // every 2^(level-i):th vertex
        step = 1 << (level-i);
        *(pIB++) = offset; // for each level, push index for center first
        for(GLuint ind = offset + 1; ind <= segments + offset + 1; ind += step) {
          *(pIB++) = ind;
        }
      }
      m_vb.unmap();
      m_ib.unmap();

      once = false;
    }
  }

  void RenderContext::finish()
  {
  }

  void RenderContext::setRecursionLimit(size_t limit)
  {
    m_data->m_recursionLimit = limit;
  }

  size_t RenderContext::recursionLimit() const
  {
    return m_data->m_recursionLimit;
  }

  void RenderContext::setRecursionDepth(size_t rd)
  {
    m_data->m_recursionDepth = rd;
  }

  size_t RenderContext::recursionDepth() const
  {
    return m_data->m_recursionDepth;
  }

  void RenderContext::pushClipRect(const Nimble::Rectangle & r)
  {
    m_data->m_clipStack.push(r);
  }

  void RenderContext::popClipRect()
  {
    m_data->m_clipStack.pop();
  }

  bool RenderContext::isVisible(const Nimble::Rectangle & area)
  {
    if(m_data->m_clipStack.empty())
      return true;
    else
      return m_data->m_clipStack.top().intersects(area);
  }

  const Nimble::Rectangle & RenderContext::visibleArea() const
  {
    return m_data->m_clipStack.top();
  }

  void RenderContext::setScreenSize(Nimble::Vector2i size)
  {
    m_data->m_screenSize = size;
  }

  RenderContext::FBOHolder RenderContext::getTemporaryFBO
      (Nimble::Vector2f basicsize, float scaling, uint32_t flags)
  {
    Nimble::Vector2i minimumsize = basicsize * scaling;

    /* First we try to find a reasonable available FBO, that is not more than
       100% too large.
    */

    long maxpixels = 2 * minimumsize.x * minimumsize.y;

    FBOHolder ret;
    FBOPackage * fbo = 0;

    for(Internal::FBOPackages::iterator it = m_data->m_fbos.begin();
    it != m_data->m_fbos.end(); it++) {
      fbo = (*it).ptr();

      if(flags & FBO_EXACT_SIZE) {
        if(fbo->userCount() ||
           fbo->m_tex.width() != minimumsize.x ||
           fbo->m_tex.height() != minimumsize.y)
          continue;
      }
      else if(fbo->userCount() ||
              fbo->m_tex.width() < minimumsize.x ||
              fbo->m_tex.height() < minimumsize.y ||
              fbo->m_tex.pixelCount() > maxpixels)
        continue;

      ret = FBOHolder(this, fbo);
      break;
    }

    if(!ret.m_package) {
      // Nothing available, we need to create a new FBOPackage
      // info("Creating a new FBOPackage");
      fbo = new FBOPackage();
      Vector2i useSize = minimumsize;
      if(!(flags & FBO_EXACT_SIZE))
        useSize += minimumsize / 4;
      fbo->setSize(useSize);
      m_data->m_fbos.push_back(fbo);

      ret = FBOHolder(this, fbo);
    }

    /* We now have a valid FBO, next job is to set it up for rendering.
    */

    glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

    for(int i = 0; i < 6; i++)
      glDisable(GL_CLIP_PLANE0 + i);

    ret.m_package->activate();

    // Draw into color attachment 0
    glDrawBuffer(Luminous::COLOR0);

    // Save and setup viewport to match the FBO
    glViewport(0, 0, fbo->m_tex.width(), fbo->m_tex.height());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, minimumsize.x, minimumsize.y);

    // Save matrix stack
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, minimumsize.x, 0, minimumsize.y);

    m_data->pushFBO(ret.m_package);

    // Lets adjust the matrix stack to take into account the new
    // reality:
    pushTransform(Nimble::Matrix3::scale2D(
        minimumsize.x / basicsize.x,
        minimumsize.y / basicsize.y));

    ret.m_texUV.make(minimumsize.x / (float) fbo->m_tex.width(),
                     minimumsize.y / (float) fbo->m_tex.height());

    // info("texuv = %f %f", ret.m_texUV.x, ret.m_texUV.y);

    return ret;
  }

  void RenderContext::drawLineRect(const Nimble::Rectf & r,
                                   float thickness, const float * rgba)
  {
    thickness *= 0.5f;

    Vector2 v1(thickness, thickness);

    Nimble::Rectf inside(r.low() + v1, r.high() - v1);
    Nimble::Rectf outside(r.low() - v1, r.high() + v1);

    Utils::glRectWithHoleAA(outside, inside, transform(), rgba);
  }


  void RenderContext::drawRect(const Nimble::Rectf & rect, const float * rgba)
  {
    Utils::glTexRectAA(rect.size(),
                       transform() * Matrix3::translate2D(rect.low()), rgba);
  }


  void RenderContext::drawCircle(Nimble::Vector2f center, float radius,
                                 const float * rgba, int segments) {
    if (segments < 0) {
      drawCircleImpl(center, radius, rgba);
    } else {
      drawCircleWithSegments(center, radius, rgba, segments);
    }
  }

  void RenderContext::drawCircleImpl(Nimble::Vector2f center, float radius,
                                 const float * rgba) {
    const Matrix3f& m = transform();
    const float tx = center.x;
    const float ty = center.y;

    // translate(tx, ty) & scale(radius)
    Matrix4f t(m[0][0]*radius, m[0][1]*radius, 0, m[0][2] + m[0][1]*ty+m[0][0]*tx,
               m[1][0]*radius, m[1][1]*radius, 0, m[1][2] + m[1][1]*ty+m[1][0]*tx,
               0         , 0         , 1, 0,
               m[2][0]*radius, m[2][1]*radius, 0, m[2][2]  + m[2][1]*ty+m[2][0]*tx);

    if(rgba)
      glColor4fv(rgba);

    static bool shader_inited = false;
    static Luminous::GLSLProgramObject * program;

    static const GLfloat vertices[] = {
      -1.0, -1.0,
      1.0, -1.0,
      1.0, 1.0,
      -1.0, 1.0
    };

    if (!shader_inited) {
      static const char * vert_shader = ""\
          "uniform mat4 matrix;"\
          "varying vec2 pos;"\
          "void main(void) {"\
          "  pos = gl_Vertex; "\
          "  mat4 transform = gl_ProjectionMatrix * matrix;"\
          "  gl_Position = transform * gl_Vertex;"\
          "  gl_FrontColor = gl_Color;"\
          "}";
      static const char * frag_shader = ""\
          "varying vec2 pos;"\
          "uniform float border_start;"\
          "void main(void) {"\
          "  float r = length(pos);"\
          "  gl_FragColor = gl_Color;"\
          "  gl_FragColor.w *= smoothstep(1.00, border_start, r);"\
          "}";

      shader_inited = true;
      program = new GLSLProgramObject();
      program->loadStrings(vert_shader, frag_shader);
    }



    program->bind();

    // uniform scaling assumed, should work fine with "reasonable" non-uniform scaling
    float totalRadius = m.extractScale() * radius;
    float border = Nimble::Math::Min(1.0f, totalRadius-2.0f);
    program->setUniformFloat("border_start", (totalRadius-border)/totalRadius);
    GLint matrixLoc = program->getUniformLoc("matrix");
    glUniformMatrix4fv(matrixLoc, 1, GL_TRUE, t.data());

    // using a VBO with 4 vertices is actually slower than this
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    program->unbind();
  }

  void RenderContext::drawCircleWithSegments(Nimble::Vector2f center, float radius,
                                 const float * rgba, int segments)
  {
    if(segments < 0) {
      float realRad = radius * transform().extractScale();
      segments = Math::Clamp((int) realRad * 2, 6, 60);
    }

    Nimble::Matrix3 m(transform() * Nimble::Matrix3::translate2D(center));

    Utils::glSolidSoftCircle(m, radius, 1.0f, segments, rgba);
  }

  void RenderContext::drawPolyLine(const Nimble::Vector2f * vertices, int n,
                                   float width, const float * rgba)
  {
    if(n < 2)
      return;

    float r = rgba[0];
    float g = rgba[1];
    float b = rgba[2];
    float a = rgba[3];

    width *= scale() * 0.5f;
    float fullw = width + 1;

    Matrix3 m(transform());

    Vector2f cprev = m.project(vertices[0]);

    Vector2f dir0 = m.project(vertices[1]) - cprev;

    Vector2 p01 = dir0.perpendicular();

    float len01 = p01.length();

    if(len01 < 1.0e-5f)
      p01.make(1,0);
    else
      p01.normalize();

    for(int i = 1; i < n; i++) {

      Vector2f cnow  = m.project(vertices[i]);

      if((cnow - cprev).length() < 3.0f && i != (n-1))
        continue;

      Vector2f cnext;

      if(i < (n - 1))
        cnext = m.project(vertices[i + 1]);
      else
        cnext = 2.0f * cnow - cprev;

      Vector2f dir1 = cnext - cnow;
      Vector2f dir2 = cnow - cprev;

      float l1 = dir1.length();
      float l2 = dir2.length();

      if(l1 < 1.0e-5f)
        dir1.make(1, 0);
      else
        dir1 /= l1;

      if(l2 < 1.0e-5f)
        dir2.make(1, 0);
      else
        dir2 /= l2;

      Vector2 q = dir1.perpendicular();

      Vector2 p12 = (dir2 + dir1).perpendicular();
      p12.normalize();

      float q12 = dot(p12, q);
      if(q12 > 0.1f)
        p12 /= q12;

      glBegin(GL_QUAD_STRIP);

      glColor4f(r, g, b, 0.0f);
      glVertex2fv((cprev + p01 * fullw).data());
      glVertex2fv((cnow + p12 * fullw).data());

      glColor4f(r, g, b, a);
      glVertex2fv((cprev + p01 * width).data());
      glVertex2fv((cnow + p12 * width).data());

      glVertex2fv((cprev - p01 * width).data());
      glVertex2fv((cnow - p12 * width).data());

      glColor4f(r, g, b, 0.0f);
      glVertex2fv((cprev - p01 * fullw).data());
      glVertex2fv((cnow - p12 * fullw).data());

      glEnd();

      p01 = p12;

      cprev = cnow;
    }

  }

  void RenderContext::drawTexRect(Nimble::Vector2 size, const float * rgba)
  {
    Nimble::Matrix3 m = transform();

    const Vector4 v[4] = {
      Utils::project(m, Vector2(0,       0)),
      Utils::project(m, Vector2(size.x,  0)),
      Utils::project(m, Vector2(size.x,  size.y)),
      Utils::project(m, Vector2(0,       size.y))
    };

    if(rgba)
      glColor4fv(rgba);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex4fv(v[0].data());

    glTexCoord2f(1.0f, 0.0f);
    glVertex4fv(v[1].data());

    glTexCoord2f(1.0f, 1.0f);
    glVertex4fv(v[2].data());

    glTexCoord2f(0.0f, 1.0f);
    glVertex4fv(v[3].data());

    glEnd();

  }

  void RenderContext::drawTexRect(Nimble::Vector2 size, const float * rgba,
                                  const Nimble::Rect & texUV)
  {
    Nimble::Matrix3 m = transform();

    const Vector4 v[4] = {
      Utils::project(m, Vector2(0,       0)),
      Utils::project(m, Vector2(size.x,  0)),
      Utils::project(m, Vector2(size.x,  size.y)),
      Utils::project(m, Vector2(0,       size.y))
    };

    if(rgba)
      glColor4fv(rgba);

    glBegin(GL_QUADS);

    glTexCoord2f(texUV.low().x, texUV.low().y);
    glVertex4fv(v[0].data());

    glTexCoord2f(texUV.high().x, texUV.low().y);
    glVertex4fv(v[1].data());

    glTexCoord2f(texUV.high().x, texUV.high().y);
    glVertex4fv(v[2].data());

    glTexCoord2f(texUV.low().x, texUV.high().y);
    glVertex4fv(v[3].data());

    glEnd();

  }

  void RenderContext::drawTexRect(Nimble::Vector2 size, const float * rgba,
                                  Nimble::Vector2 texUV)
  {
    drawTexRect(size, rgba, Rect(Vector2(0,0), texUV));
  }

  void RenderContext::drawLineVBO(Nimble::Vector2f start, Nimble::Vector2f end)
  {
    Matrix3f m = transform();
    Matrix4f t(m[0][0], m[0][1], 0, m[0][2],
               m[1][0], m[1][1], 0, m[1][2],
                     0,       0, 1,       0,
               m[2][0], m[2][1], 0, m[2][2]);

    Vector2f v = end - start;
    float angle = Math::radToDeg(Math::ATan2(v.x, v.y));

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPushMatrix();

    glTranslatef(start.x, start.y, 0.f);
    glMultTransposeMatrixf(t.data());
    glRotatef(angle, 0, 0, 1);
    glScalef(v.x, v.y, 1.0f);

    glEnableClientState(GL_VERTEX_ARRAY);

    m_vb.bind();
    glVertexPointer(2, GL_FLOAT, 4*sizeof(GL_FLOAT), BUFFER_OFFSET(0));

    m_ib.bind();
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    m_ib.unbind();
    m_vb.unbind();

    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();

  }
  void RenderContext::drawLineRectVBO(const Nimble::Rectf & rect, float thickness, const float * rgba)
  {
  }

  void RenderContext::drawRectVBO(const Nimble::Rectf & rect, const float * rgba)
  {
    Matrix3f m = transform();
    Matrix4f t(m[0][0], m[0][1], 0, m[0][2],
               m[1][0], m[1][1], 0, m[1][2],
                     0,       0, 1,       0,
               m[2][0], m[2][1], 0, m[2][2]);

    glPushMatrix();

    glTranslatef(rect.low().x, rect.low().y, 0.f);
    glMultTransposeMatrixf(t.data());
    glScalef(rect.width(), rect.height(), 1.0f);

    glEnableClientState(GL_VERTEX_ARRAY);

    m_vb.bind();
    glVertexPointer(2, GL_FLOAT, 4*sizeof(GL_FLOAT), BUFFER_OFFSET(0));

    m_ib.bind();
    if(rgba)
      glColor4f(rgba[0], rgba[1], rgba[2], 0.1f);
    glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_INT, BUFFER_OFFSET(4*sizeof(GLuint)));
    if(rgba)
      glColor4fv(rgba);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    m_ib.unbind();
    m_vb.unbind();

    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
  }

  void RenderContext::drawCircleVBO(Nimble::Vector2f center, float radius, const float * rgba)
  {
    const Matrix3f& m = transform();
    const float tx = center.x;
    const float ty = center.y;

    // translate(tx, ty) & scale(radius)
    Matrix4f t(m[0][0]*radius, m[0][1]*radius, 0, m[0][2] + m[0][1]*ty+m[0][0]*tx,
               m[1][0]*radius, m[1][1]*radius, 0, m[1][2] + m[1][1]*ty+m[1][0]*tx,
               0         , 0         , 1, 0,
               m[2][0]*radius, m[2][1]*radius, 0, m[2][2] + m[2][1]*ty+m[2][0]*tx);

    if(rgba)
      glColor4fv(rgba);
    glPushMatrix();

    glMultTransposeMatrixf(t.data());

    glEnableClientState(GL_VERTEX_ARRAY);

    m_vb.bind();
    glVertexPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

    // LOD
    int level = LOD_MAXIMUM + Nimble::Math::Log2(radius*m.extractScale()/m_data->m_screenSize.x);
    level = Nimble::Math::Clamp(level, (int)LOD_MINIMUM, (int)LOD_MAXIMUM);

    //int offset = sizeof(GLuint)*(6+(pow2-4 + 2*(level-2)));  // 4 * (Sigma i ^ n - 2)
    const int circle_begin_offset = 16;
    // offset o(n) of level n:
    //  o(LOD_MINIMUM) = circle_begin_offset
    //  o(n+1) = o(n) + 2^n + 2
    // => o(n) = 2^n + 2n - (2*LOD_MINIMUM+2^LOD_MINIMUM) + circle_begin_offset
    const int offset = sizeof(GLuint) * ((1 << level) + 2*level
                                         - ((1<<LOD_MINIMUM) + 2*LOD_MINIMUM) + circle_begin_offset);
    int count = (1 << level);

    m_ib.bind();
    glDrawElements(GL_TRIANGLE_FAN, count, GL_UNSIGNED_INT, BUFFER_OFFSET(offset));

    m_ib.unbind();
    m_vb.unbind();

    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
  }

  void RenderContext::drawArcVBO(Nimble::Vector2f center, float radius, float fromRadians, float toRadians, const float * rgba)
  {
    Matrix3f m = transform();
    Matrix4f t(m[0][0], m[0][1], 0, m[0][2],
               m[1][0], m[1][1], 0, m[1][2],
                     0,       0, 1,       0,
               m[2][0], m[2][1], 0, m[2][2]);

    if(rgba)
      glColor4fv(rgba);
    glPushMatrix();

    glTranslatef(center.x, center.y, 0.f);
    glMultTransposeMatrixf(t.data());
    glScalef(radius, radius, 1.0f);
    glRotatef(Math::radToDeg(fromRadians), 0.f, 0.f, 1.f);

    glEnableClientState(GL_VERTEX_ARRAY);

    m_vb.bind();
    glVertexPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

    // LOD
    int level = LOD_MAXIMUM + Math::Log2(radius*m.extractScale()*2/m_data->m_screenSize.x);
    if (level < LOD_MINIMUM) level = LOD_MINIMUM;
    int pow2 = Math::Floor(Math::Pow(2.0f,level));
    int count = pow2 + 2;
    int offset = sizeof(GLuint)*(6+(pow2-4 + 2*(level-2)));  // 4 * (Sigma i ^ n - 2)
    count =int(count*(toRadians-fromRadians)/Math::TWO_PI);

    m_ib.bind();
    glDrawElements(GL_TRIANGLE_FAN, count, GL_UNSIGNED_INT, BUFFER_OFFSET(offset));

    m_ib.unbind();
    m_vb.unbind();

    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
  }

  void RenderContext::drawPolyLineVBO(const Nimble::Vector2f * vertices, int n,
                                      float width, const float * rgba)
  {
  }
  void RenderContext::drawTexRectVBO(Nimble::Vector2 size, const float * rgba)
  {
    const Matrix3f& m = transform();
    const float sx = size.x;
    const float sy = size.y;

    Matrix4f t(m[0][0]*sx, m[0][1]*sy, 0, m[0][2],
               m[1][0]*sx, m[1][1]*sy, 0, m[1][2],
               0         , 0         , 1, 0,
               m[2][0]*sx, m[2][1]*sy, 0, m[2][2]);

    if(rgba)
      glColor4fv(rgba);

    glPushMatrix();
    glMultTransposeMatrixf(t.data());

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    m_vb.bind();
    glVertexPointer(2, GL_FLOAT, 4*sizeof(GL_FLOAT), BUFFER_OFFSET(0));
    glTexCoordPointer(2, GL_FLOAT, 4*sizeof(GL_FLOAT), BUFFER_OFFSET(2*sizeof(GL_FLOAT)));
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    m_vb.unbind();

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
  }

  void RenderContext::drawTexRectAAVBO(Nimble::Vector2 size, const float * rgba)
  {
    const Matrix3f& m = transform();
    const float sx = size.x;
    const float sy = size.y;

    Matrix4f t(m[0][0]*sx, m[0][1]*sy, 0, m[0][2],
               m[1][0]*sx, m[1][1]*sy, 0, m[1][2],
               0         , 0         , 1, 0,
               m[2][0]*sx, m[2][1]*sy, 0, m[2][2]);

    if(rgba)
      glColor4fv(rgba);

    glPushMatrix();

    glMultTransposeMatrixf(t.data());

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    m_vb.bind();
    glVertexPointer(2, GL_FLOAT, 4*sizeof(GL_FLOAT), BUFFER_OFFSET(0));
    glTexCoordPointer(2, GL_FLOAT, 4*sizeof(GL_FLOAT), BUFFER_OFFSET(2*sizeof(GL_FLOAT)));

    m_ib.bind();
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    if (rgba) {
      glColor4f(rgba[0], rgba[1], rgba[2], 0.0f);
    }

    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glColor4f(rgba[0], rgba[1], rgba[2], 0.0f);

    glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_INT, BUFFER_OFFSET(4*sizeof(GLuint)));

    m_ib.unbind();
    m_vb.unbind();

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
  }
  void RenderContext::drawTexRectVBO(Nimble::Vector2 size, const float * rgba,
                                     const Nimble::Rect & texUV)
  {
  }
  void RenderContext::drawTexRectVBO(Nimble::Vector2 size, const float * rgba,
                                     Nimble::Vector2 texUV)
  {
  }

  void RenderContext::setBlendFunc(BlendFunc f)
  {
    if(f == BLEND_NONE) {
      glDisable(GL_BLEND);
      return;
    }

    glEnable(GL_BLEND);

    if(f == BLEND_USUAL)
      Utils::glUsualBlend();
    else if(f == BLEND_ADDITIVE)
      Utils::glAdditiveBlend();
    else if(f == BLEND_SUBTRACTIVE) {
      Utils::glSubtractiveBlend();
    }

  }

  const char ** RenderContext::blendFuncNames()
  {
    static const char * names [] = {
      "usual",
      "none",
      "additive",
      "subtractive"
    };

    return names;
  }

  void RenderContext::clearTemporaryFBO(FBOPackage * fbo)
  {
    assert(fbo->userCount() == 0);

    fbo->m_fbo.unbind();

    fbo = m_data->popFBO(fbo);

    if(fbo) {
      fbo->activate();
    }
    else {
      // info("Back to back-buffer");
      glDrawBuffer(GL_BACK);
    }

    glPopAttrib();

    // Restore matrix stack
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    popTransform();
  }


}

