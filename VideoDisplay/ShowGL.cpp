/* COPYRIGHT
 *
 * This file is part of VideoDisplay.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "VideoDisplay.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "ShowGL.hpp"

#include "AudioTransfer.hpp"
#include "VideoInFFMPEG.hpp"

#include <Radiant/Trace.hpp>
#include <Radiant/ImageConversion.hpp>
#include <Radiant/Sleep.hpp>

#include <Poetic/GPUFont.hpp>
#include <Poetic/CPUFont.hpp>

#include <Luminous/Utils.hpp>

namespace VideoDisplay {

  ShowGL::YUVProgram::YUVProgram(Luminous::GLResources * resources)
    : Luminous::GLSLProgramObject(resources)
  {
    for(uint i = 0; i < PARAM_SIZEOF; i++)
      m_uniforms[i] = -1;
    init();
  }

  ShowGL::YUVProgram::~YUVProgram()
  {
    clear();
  }

  bool ShowGL::YUVProgram::init()
  {
    static const char * shadersource = 
      "uniform sampler2D ytex;\n"
      "uniform sampler2D utex;\n"
      "uniform sampler2D vtex;\n"
      "uniform mat3 zm;\n"
      "void main (void) {\n"
      "  vec4 ycolor = texture2D(ytex, gl_TexCoord[0].st);\n"
      "  vec4 ucolor = texture2D(utex, gl_TexCoord[0].st);\n"
      "  vec4 vcolor = texture2D(vtex, gl_TexCoord[0].st);\n"
      "  vec3 yuv = vec3(ycolor.r, ucolor.r - 0.5, vcolor.r - 0.5);\n"
      "  gl_FragColor.rgb = zm * yuv;\n"
      "  gl_FragColor.a = gl_Color.a;\n"
      "}\n";
    /*
    static const char * shadersource = 
      "uniform sampler2D ytex;\n"
      "uniform sampler2D utex;\n"
      "uniform sampler2D vtex;\n"
      "uniform vec2 offset;\n"
      "uniform mat3 zm;\n"
      "void main (void) {\n"
      "  vec4 ycolor = texture2D(ytex, gl_TexCoord[0].st + offset);\n"
      "  ycolor = texture2D(ytex, gl_TexCoord[0].st - offset);\n"
      "  vec4 ucolor = texture2D(utex, gl_TexCoord[0].st);\n"
      "  vec4 vcolor = texture2D(vtex, gl_TexCoord[0].st);\n"
      "  vec3 yuv = vec3(ycolor.r * 0.25, ucolor.r - 0.5, vcolor.r - 0.5);\n"
      "  gl_FragColor.rgb = zm * yuv;\n"
      "  gl_FragColor.a = gl_Color.a;\n"
      "}\n";
    */
    clear();

    Luminous::GLSLShaderObject * fragShader =
      new Luminous::GLSLShaderObject(GL_FRAGMENT_SHADER, resources());

    fragShader->setSource(shadersource);
    if(!fragShader->compile()) {

		Radiant::error("ShowGL::YUVProgram::init # compile: %s",
          fragShader->compilerLog());
      return false;
    }

    addObject(fragShader);

    return link();
  }

  void ShowGL::YUVProgram::bind()
  {
    Luminous::GLSLProgramObject::bind();


    static const float yuv2rgb[9] = {
      1.0f,  0.0f,    1.403f,
      1.0f, -0.344f, -0.714f,
      1.0f,  1.77f,   0.0f
    };

    glUniformMatrix3fv(m_uniforms[PARAM_MATRIX], 1, true, yuv2rgb);
    glUniform1i(m_uniforms[PARAM_YTEX], 0);
    glUniform1i(m_uniforms[PARAM_UTEX], 1);
    glUniform1i(m_uniforms[PARAM_VTEX], 2);
  }

  void ShowGL::YUVProgram::unbind()
  {
    Luminous::GLSLProgramObject::unbind();
  }

  bool ShowGL::YUVProgram::link()
  {
    if(!Luminous::GLSLProgramObject::link()) {
		Radiant::error("ShowGL::YUVProgram::link # %s", linkerLog());
      return false;
    }

    const char * params [PARAM_SIZEOF] = {
      "ytex",
      "utex",
      "vtex",
      "zm"
    };

    bool ok = true;

    for(uint i = 0; i < PARAM_SIZEOF; i++) {
      int tmp = getUniformLoc(params[i]);
      m_uniforms[i] = tmp;
      ok = ok && (tmp >= 0);
      trace(DEBUG, "ShowGL::YUVProgram::link # %s -> %d", params[i], i);
    }

    return ok;
  }

  void ShowGL::YUVProgram::clear()
  {
    Luminous::GLSLProgramObject::clear();
  }
  
  ShowGL::MyTextures::MyTextures(Luminous::GLResources * resources)
    : GLResource(resources)
  {
    m_frame = -1;
  }

  void ShowGL::MyTextures::bind()
  {
    for(int i = 0; i < 3; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glEnable(GL_TEXTURE_2D);
      m_texIds[i].bind();
    }
  }

  void ShowGL::MyTextures::unbind()
  {
    for(int i = 0; i < 3; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glDisable(GL_TEXTURE_2D);
    }
    glActiveTexture(GL_TEXTURE0);
  }

  void ShowGL::MyTextures::doTextures(int frame, Radiant::VideoImage * img)
  {
    if(m_frame == frame)
      return;

    for(uint i = 0; i < 3; i++) {

      glActiveTexture(GL_TEXTURE0 + i);
      glEnable(GL_TEXTURE_2D);
      Luminous::Texture2D * tex = & m_texIds[i];
      tex->bind();

      Vector2i & ts = m_texSizes[i];

      if(m_frame < 0) {

        Vector2i area = planeSize(img, i);

        // Texture dimensions must be even numbers:
        /*if(area.x & 0x1)
          area.x--; */

        // area.x -= 4;

        /* if(area.y & 0x1)
           area.y--;
           */

        // if(i) area.y /= 2;
        // if(i) area.x *= 2;

        trace(DEBUG, "ShowGL::YUVProgram::doTextures # area = [%d %d]",
            area.x, area.y);

        ts = area;

        tex->setWidth(area.x);
        tex->setHeight(area.y);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
            area.x, area.y, 0, 
            GL_LUMINANCE, GL_UNSIGNED_BYTE, img->m_planes[i].m_data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);        

        Luminous::Utils::glCheck
          ("ShowGL::YUVProgram::doTextures # glTexImage2D");
      }
      else {

        // memset(img->m_planes[i].m_data, 128, ts.x * ts.y);

        /* trace("ShowGL::YUVProgram::doTextures # ts = [%d %d]",
           ts.x, ts.y); */

        /* glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
           ts.x, ts.y, 0, 
           GL_LUMINANCE, GL_UNSIGNED_BYTE, img->m_planes[i].m_data);
           */
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
            ts.x, ts.y,
            GL_LUMINANCE, GL_UNSIGNED_BYTE,
            img->m_planes[i].m_data);

        Luminous::Utils::glCheck
          ("ShowGL::YUVProgram::doTextures # glTexSubImage2D");
      }
    }
    m_frame = frame;
  }

  Vector2i ShowGL::MyTextures::planeSize(const Radiant::VideoImage *img, uint i)
  {
    Vector2i area(img->m_width, img->m_height);

    if(i) {
      if(img->m_format == Radiant::IMAGE_YUV_411P) {
        area.x /= 4;
      }
      else if(img->m_format == Radiant::IMAGE_YUV_420P) {
        area /= 2;
      }
      else if(img->m_format == Radiant::IMAGE_YUV_422P) {
        area.x /= 2;
      }
    }

    return area;
  }


  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  ShowGL::ShowGL()
    : m_video(0),
    m_frame(0),
    m_dsp(0),
    m_audio(0),
    m_state(PAUSE),
    m_updates(0),
    m_blankReload(false),
    m_useBlank(false)
  {
    clearHistogram();
  }

  ShowGL::~ShowGL()
  {
    m_blankDisplay.freeMemory();
  }


  bool ShowGL::loadSubTitles(const char * filename, const char * )
  {
    return m_subTitles.readSrt(filename);
  }

  bool ShowGL::init(const char * filename, Resonant::DSPNetwork  * dsp)
  {
    m_filename = filename;
    m_dsp = dsp;

    Screenplay::VideoInputFFMPEG video;

    if(!video.open(filename))
      return false;

    // Seek a bit into the file (at most 60 secs)
    double len = video.durationSeconds();
    double pos = len * 0.05;
    if(pos > 60)
      pos = 60;

    video.seekPosition(pos);

    const Radiant::VideoImage * img = video.captureImage();

    if(!img)
      return false;

    m_blankDisplay.allocateMemory
      (Radiant::IMAGE_RGBA, img->m_width, img->m_height);

    if(!Radiant::ImageConversion::convert(img, & m_blankDisplay)) {
      return false;
    }

    m_blankReload = true;
    m_useBlank = true;
    m_position = 0;
    m_duration = Radiant::TimeStamp::createSecondsD(video.durationSeconds());

    trace(DEBUG, "ShowGL::init # Opened %s", filename);

    return true;
  }

  bool ShowGL::open(const char * filename, Resonant::DSPNetwork  * dsp,
		    Radiant::TimeStamp pos)
  {
    m_filename = filename;
    m_dsp = dsp;
    delete m_video;

    VideoInFFMPEG * ffmpg = new VideoInFFMPEG();

    bool ok = ffmpg->startDecoding(filename, pos);

    if(!ok) {
      Radiant::error("ShowGL::open # Could not open %s", filename);
      delete ffmpg;
      return false;
    }

    m_video = ffmpg;

    AudioTransfer * au = new AudioTransfer(0, m_video);

    char buf[128];
    sprintf(buf, "showgl-audiotransfer-%p", this);

    au->setId(buf);

    m_dspItem = Resonant::DSPNetwork::Item();
    m_dspItem.m_module = au;

    m_dsp->addModule(m_dspItem);

    m_audio = au;

    m_frameTime = 0;
    m_count = 0;
    m_frame = 0;

    m_state = PLAY;

    clearHistogram();

    return true;
  }

  bool ShowGL::stop()
  {
    if(m_state != PLAY)
      return false;

    if(m_frame && m_frame != & m_preview) {
      m_preview.m_image.allocateMemory(m_frame->m_image);
      m_preview.m_image.copyData(m_frame->m_image);
      m_preview.m_time = m_frame->m_time;
      m_frame = & m_preview;
    }

    int i = 0;

    while(!m_audio->stopped() && !m_audio->started() && i < 10) {
      Radiant::Sleep::sleepMs(5);
      i++;
    }

    m_dsp->markDone(m_dspItem);

    i = 0;
    while(!m_audio->stopped() && i < 20) {
      Radiant::Sleep::sleepMs(5);
      i++;
    }

    if(i >= 10) {
      trace(DEBUG, "ShowGL::stop # Forcing quit");
    }

    delete m_audio;
    m_audio = 0;

    m_video->stopDecoding();
    delete m_video;
    m_video = 0;

    m_dspItem = Resonant::DSPNetwork::Item();

    m_state = PAUSE;

    return true;
  }

  bool ShowGL::togglePause()
  {
    if(m_state == PLAY) {
      return stop();
    }
    else {

      Radiant::TimeStamp pos = m_position;
      if(Radiant::TimeStamp(m_duration - m_position).secondsD() < 2.5)
        pos = 0;

      return open(m_filename.c_str(), m_dsp, pos);
    }
  }

  void ShowGL::update()
  {
    Radiant::TimeStamp targetTime = m_audio ?
      m_audio->audioTime() : Radiant::TimeStamp(0);

    int tries = 0;

    while(targetTime > m_frameTime) {

      if(m_video->atEnd()) {
        stop();
        break;
      }

      m_frame = m_video->nextFrame();

      if(!m_frame)
        break;

      m_frameTime = m_frame->m_time;
      tries++;
      m_count++;
      m_useBlank = false;
      m_position = m_frame->m_absolute;
    }

    m_histogram[m_updates % HISTOGRAM_POINTS] = tries;

    if(m_video)
      if(!m_video->atEnd())
        m_updates++;

    m_subTitles.update(m_position);
  }

  static int yuvkey = 0;
  // static int yuvkeyaa = 0;

  void ShowGL::render(Luminous::GLResources * resources,
		      Vector2 topleft, Vector2 bottomright,
		      const Nimble::Matrix3f * transform,
		      Poetic::GPUFont * subtitleFont,
		      float subTitleSpace)
  {
    GLRESOURCE_ENSURE(YUVProgram, yuv2rgb, &yuvkey, resources);
    GLRESOURCE_ENSURE(MyTextures, textures, this, resources);
 
    assert(yuv2rgb != 0);

    Vector2i s = size();

    if(bottomright == topleft) {
      bottomright.x = topleft.x + s.x;
      bottomright.y = topleft.y + s.y;
    }

    if(m_frame && !m_useBlank) {

      // trace("ShowGL::render # %p", m_frame);

      textures->doTextures(m_count, & m_frame->m_image);
      textures->bind();
      yuv2rgb->bind();

    }
    else {

      glEnable(GL_TEXTURE_2D);

      if(s.x != 0 && (textures->blankTex().width() == 0 || m_blankReload)) {
	textures->blankTex().loadBytes(GL_RGBA, s.x, s.y, 
				      m_blankDisplay.m_planes[0].m_data,
				      Luminous::PixelFormat::rgbaUByte());
        m_blankReload = false;
      }
      textures->blankTex().bind();
    }

    glBegin(GL_QUADS);

    Vector2 txcoord[4] = {
      Vector2(0, 0),
      Vector2(1, 0),
      Vector2(1, 1),
      Vector2(0, 1)
    };

    Vector2 corners[4] = {
      Vector2(topleft.x, topleft.y),
      Vector2(bottomright.x, topleft.y),
      Vector2(bottomright.x, bottomright.y),
      Vector2(topleft.x, bottomright.y)
    };

    for(int i = 0; i < 4; i++) {
      Vector2 co = corners[i];

      glTexCoord2fv(txcoord[i].data());

      if(transform)
	glVertex4fv(Luminous::Utils::project(*transform , co).data());
      else
	glVertex2fv(co.data());
    }

    glEnd();

    yuv2rgb->unbind();
    textures->unbind();

    const SubTitles::Text * sub = m_subTitles.current();

    if(!subtitleFont && sub) {
		Radiant::error("ShowGL::render # Missing the subtitle font");
    }

    if(subtitleFont && sub) {

      glEnable(GL_BLEND);
      glEnable(GL_TEXTURE_2D);

      // puts("RENDERING SUBS");

      Poetic::CPUFont * cpuFont = subtitleFont->cpuFont();

      float fontH = cpuFont->lineHeight();
      float subH = fontH * 2.2f;

      int linecount = sub->lineCount();
      
      bool below = false;
      
      if(subTitleSpace <= 0)
        subTitleSpace = bottomright.y;
      else if((subTitleSpace - subH) <  bottomright.y)
        subTitleSpace = bottomright.y;
      else {
        subTitleSpace = bottomright.y + subH;
	below = true;
      }
      Nimble::Vector2f loc(topleft.x + fontH, subTitleSpace - fontH * 0.2f);

      if(linecount == 1) {
	if(below)
	  loc.y -= fontH;
        subtitleFont->render(sub->m_lines[0], loc);
      }
      else if(linecount  == 2) {
        subtitleFont->render(sub->m_lines[1], loc);
        loc.y -= fontH;
        subtitleFont->render(sub->m_lines[0], loc);
      }
    }

    Luminous::Utils::glCheck("ShowGL::render");
  }

  Nimble::Vector2i ShowGL::size() const
  {
    return Nimble::Vector2i(m_blankDisplay.m_width, m_blankDisplay.m_height);
  }

  void ShowGL::seekTo(Radiant::TimeStamp time)
  {
    if(time < 0)
      time = 0;
    else if(time >= m_duration)
      time = m_duration;

    m_position = time;

    if(!m_video) {
      getThumbnail(time.secondsD());
      return;
    }

    if(time < 0)
      time = 0;
    else if(time >= m_duration)
      time = m_duration - Radiant::TimeStamp::createSecondsD(2);

    trace(DEBUG, "ShowGL::seekTo # %lf", time.secondsD());

    m_video->seekTo(time.secondsD());
  }

  void ShowGL::seekToRelative(double relative)
  {
    seekTo(static_cast<int64_t> (duration() * relative));
  }

  void ShowGL::clearHistogram()
  {
    bzero(m_histogram, sizeof(m_histogram));
  }

  /// @todo this should be done in another thread, now it hangs the app
  /// everytime you call it
  void ShowGL::getThumbnail(double pos)
  {
    trace(DEBUG, "ShowGL::getThumbnail # %lf", pos);

    Screenplay::VideoInputFFMPEG video;
    if(!video.open(m_filename.c_str()))
      return;

    if(pos > 0.01)
      video.seekPosition(pos);

    const Radiant::VideoImage * img = video.captureImage();

    if(!img)
      return;

    m_blankDisplay.allocateMemory
      (Radiant::IMAGE_RGBA, img->m_width, img->m_height);

    if(!Radiant::ImageConversion::convert(img, & m_blankDisplay))
      return;

    trace(DEBUG, "ShowGL::getThumbnail # EXIT OK");

    m_blankReload = true;
    m_useBlank = true;
  }
}
