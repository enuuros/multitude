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
      debug("ShowGL::YUVProgram::link # %s -> %d", params[i], i);
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

      Vector2i area = planeSize(img, i);
      Vector2i & ts = m_texSizes[i];
      ts = area;
      
      if(m_frame < 0 || area != tex->size()) {

	debug("ShowGL::YUVProgram::doTextures # area = [%d %d] ptr = %p",
	     area.x, area.y, img->m_planes[i].m_data);

        tex->setWidth(area.x);
        tex->setHeight(area.y);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
		     area.x, area.y, 0, 
		     GL_LUMINANCE, GL_UNSIGNED_BYTE, img->m_planes[i].m_data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);        

        Luminous::Utils::glCheck
          ("ShowGL::YUVProgram::doTextures # glTexImage2D");
      }
      else {

	debug("ShowGL::YUVProgram::doTextures # frame = %d, ts = [%d %d]",
	     frame, ts.x, ts.y);

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
      m_targetChannel(-1),
      m_videoFrame(-1),
      m_state(PAUSE),
      m_updates(0)
  {
    clearHistogram();
  }

  ShowGL::~ShowGL()
  {
    stop();
    delete m_video;
  }

  bool ShowGL::loadSubTitles(const char * filename, const char * )
  {
    return m_subTitles.readSrt(filename);
  }

  bool ShowGL::init(const char * filename, Resonant::DSPNetwork  * dsp,
		    float /*previewpos*/,
                    int targetChannel)
  {
    // debug("ShowGL::init # %f", previewpos);

    m_filename = filename;
    m_dsp = dsp;
    m_targetChannel = targetChannel;
    delete m_video;

    VideoInFFMPEG * ffmpg = new VideoInFFMPEG();

    bool ok = ffmpg->init(filename, 0);

    if(!ok) {
      Radiant::error("ShowGL::open # Could not open %s", filename);
      delete ffmpg;
      return false;
    }

    m_video = ffmpg;

    m_position = 0;
    m_duration = Radiant::TimeStamp::createSecondsD(m_video->durationSeconds());

    debug("ShowGL::init # Opened %s (%lf secs)",
	  filename, m_duration.secondsD());

    return true;
  }


  bool ShowGL::start(bool fromOldPos)
  {
    debug("ShowGL::start");
    
    if(m_state == PLAY) {
      return false;
    }

    AudioTransfer * au = new AudioTransfer(0, m_video);

    char buf[128];
    sprintf(buf, "showgl-audiotransfer-%p", this);

    au->setId(buf);

    m_dspItem = Resonant::DSPNetwork::Item();
    m_dspItem.setModule(au);
    m_dspItem.setTargetChannel(m_targetChannel);

    m_dsp->addModule(m_dspItem);

    m_audio = au;

    if(fromOldPos)
      m_video->play();
    else
      m_video->play(0);

    m_state = PLAY;

    return true;
  }

  bool ShowGL::stop()
  {
    debug("ShowGL::stop");

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

    m_audio = 0;

    m_video->stop();

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

      start();

      return false;
    }
  }

  bool ShowGL::pause()
  {
    if(m_state == PLAY) {
      return stop();
    }
    return true;
  }

  bool ShowGL::unpause()
  {
    if(m_state == PLAY)
      return true;

    Radiant::TimeStamp pos = m_position;

    if(Radiant::TimeStamp(m_duration - m_position).secondsD() < 2.5)
      pos = 0;
    
    return false; // play(pos);
  }

  void ShowGL::update()
  {
    int videoFrame;

    if(m_audio) {
      videoFrame = m_audio->videoFrame();
      if(m_audio->atEnd()) {
	info("ShowGL::update # At end");
	stop();
      }
    }
    else
      videoFrame = m_video->latestFrame();

    if(videoFrame < 0)
      return;

    VideoIn::Frame * f = m_video->getFrame(videoFrame, true);

    if(!f) {
      debug("ShowGL::update # NO FRAME %d", videoFrame);
      return;
    }

    m_position = f->m_absolute;

    m_histogram[m_updates % HISTOGRAM_POINTS] = videoFrame - m_videoFrame;

    m_updates++;
    m_frame = f;

    if(m_videoFrame != videoFrame) {
      debug("ShowGL::update # Move %d -> %d (%lf, %d x %d)",
            m_videoFrame, videoFrame, m_position.secondsD(),
            m_frame->m_image.m_width, m_frame->m_image.m_height);
      m_count++;
      m_videoFrame = videoFrame;
    }

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
    GLRESOURCE_ENSURE(YUVProgram, yuv2rgb, & yuvkey, resources);
    GLRESOURCE_ENSURE(MyTextures, textures, this, resources);
 
    assert(yuv2rgb != 0);

    Vector2i s = size();

    if(bottomright == topleft) {
      bottomright.x = topleft.x + s.x;
      bottomright.y = topleft.y + s.y;
    }

    if(m_frame) {

      // debug("ShowGL::render # %p", m_frame);

      textures->doTextures(m_count, & m_frame->m_image);
      textures->bind();
      yuv2rgb->bind();
    }

    glEnable(GL_BLEND);
    
    Nimble::Vector4 white(1, 1, 1, 1);

    if(transform) {
      Nimble::Matrix3 m = *transform * Nimble::Matrix3::translate2D(topleft);

      Luminous::Utils::glTexRectAA(bottomright - topleft, *transform,
				   white.data());
    }
    else {
      Nimble::Rect r(topleft, bottomright);
      Luminous::Utils::glTexRectAA(r, white.data());
    }
    /*
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

    // The base rectangle:
    for(int i = 0; i < 4; i++) {
      Vector2 & co = corners[i];

      glTexCoord2fv(txcoord[i].data());

      if(transform)
	co = Luminous::Utils::project(*transform , co).xy();

      glVertex2fv(co.data());
    }

    glEnd();

    Nimble::Vector4 opaque(1, 1, 1, 1);
    Nimble::Vector4 transparent(1, 1, 1, 0);

    Vector2 up = corners[0] - corners[1];
    up.normalize();

    Vector2 right = corners[3] - corners[0];
    right.normalize();

    glBegin(GL_TRIANGLE_STRIP);
    
    glTexCoord2fv(txcoord[0].data());
    glColor4fv(opaque.data());
    glVertex2fv(corners[0].data());
    glColor4fv(transparent.data());
    glVertex2fv((corners[0] + up - right).data());

    glTexCoord2fv(txcoord[1].data());
    glColor4fv(opaque.data());
    glVertex2fv(corners[1].data());
    glColor4fv(transparent.data());
    glVertex2fv((corners[1] - up - right).data());

    glTexCoord2fv(txcoord[2].data());
    glColor4fv(opaque.data());
    glVertex2fv(corners[2].data());
    glColor4fv(transparent.data());
    glVertex2fv((corners[2] - up + right).data());

    glTexCoord2fv(txcoord[3].data());
    glColor4fv(opaque.data());
    glVertex2fv(corners[3].data());
    glColor4fv(transparent.data());
    glVertex2fv((corners[3] + up + right).data());

    glTexCoord2fv(txcoord[0].data());
    glColor4fv(opaque.data());
    glVertex2fv(corners[0].data());
    glColor4fv(transparent.data());
    glVertex2fv((corners[0] + up - right).data());

    glEnd();

    */

    // Then a thin strip around to anti-alias:

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
    assert(m_video != 0);

    return m_video->vdebug().m_videoFrameSize;
  }

  void ShowGL::seekTo(Radiant::TimeStamp time)
  {
    if(time < 0)
      time = 0;
    else if(time >= m_duration)
      time = m_duration;

    m_position = time;

    if(time < 0)
      time = 0;
    else if(time >= m_duration)
      time = m_duration - Radiant::TimeStamp::createSecondsD(2);

    debug("ShowGL::seekTo # %lf", time.secondsD());

    m_video->seek(time);
  }

  void ShowGL::seekToRelative(double relative)
  {
    seekTo(TimeStamp(duration() * relative));
  }

  void ShowGL::clearHistogram()
  {
    bzero(m_histogram, sizeof(m_histogram));
  }
  
}
