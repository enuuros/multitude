/* COPYRIGHT
 *
 * This file is part of Applications/FireView.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Applications/FireView.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "CamView.hpp"

#include "ParamView.hpp"

#include <Radiant/Sleep.hpp>
#include <Radiant/TimeStamp.hpp>

#include <Luminous/Utils.hpp>
#include <Luminous/PixelFormat.hpp>

#include <Radiant/StringUtils.hpp>
#include <Radiant/Trace.hpp>

#include <QAction>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

namespace FireView {

  using namespace Radiant;

  CamView::InputThread::InputThread()
    : m_state(UNINITIALIZED),
      m_continue(false),
      m_frameCount(0),
      m_lastCheckFrame(0),
      m_lastCheckFps(0)
  {
  }

  CamView::InputThread::~InputThread()
  {}

  bool CamView::InputThread::start(u_int64_t euid64, Radiant::FrameRate fps,
				   float customFps,
				   int triggerSource, int triggerMode)
  {
    m_video.setCameraEuid64(euid64);
    /*
    Radiant::trace("CamView::InputThread::start # %f %f # %d %d",
		   customFps, Radiant::asFloat(fps),
		   triggerSource, triggerMode);
    */
    while(customFps > Radiant::asFloat(fps) && fps < Radiant::FPS_60) {
      puts("Adjusting the core FPS");
      fps = (Radiant::FrameRate) (fps + 1);
    }
    m_fps = fps;
    m_customFps = customFps;
    m_triggerSource = triggerSource;
    m_triggerMode = triggerMode;
    m_state = STARTING;
    m_continue = true;
    m_frameCount = 0;

    m_lastCheckTime = Radiant::TimeStamp::getTime();
    m_lastCheckFrame = 0;
    m_lastCheckFps = 0;
    if(!run())
      return false;
    
    while(m_state == STARTING) {
      Radiant::Sleep::sleepMs(20);
    }
    
    return m_state == RUNNING;
  }
  
  void CamView::InputThread::stop()
  {
    if(!m_continue)
      return;
    
    m_continue = false;
    waitEnd();
  }

  static const char * modeName(dc1394feature_mode_t mode)
  {
    if(mode == DC1394_FEATURE_MODE_MANUAL)
      return "manual";
    else if(mode == DC1394_FEATURE_MODE_AUTO)
      return "auto";
    else if(mode == DC1394_FEATURE_MODE_ONE_PUSH_AUTO)
      return "one-push-auto";

    return "unknown";
  }
  void CamView::InputThread::childLoop()
  {
    using Radiant::StringUtils::yesNo;
    
    bool ok = m_video.open(0, 0, 0, Radiant::IMAGE_UNKNOWN, 640, 480, m_fps);
    
    if(!ok) {
      m_state = FAILED;
      return;
    }

    if(m_verbose) {
      // Try to print some information
      std::vector<dc1394feature_info_t> features;
      m_video.getFeatures( & features);

#ifndef WIN32
      dc1394camera_t * cam = m_video.dc1394Camera();
      printf("Preparing camera %llx %s %s\n",
	     (long long) cam->guid, cam->vendor, cam->model);
#endif

      for(uint i = 0; i < features.size(); i++) {
	dc1394feature_info_t & info = features[i];

        if(!info.available)
          continue;
	printf(" Feature %u = %s: \n"
	       "  Capabilities:\n"
               "   absolute = %s\n   readout = %s\n"
	       "   on-off = %s\n   polarity = %s\n"
	       "  On = %s\n",
	       i, Radiant::Video1394::featureName(info.id),
	       yesNo(info.absolute_capable),
	       yesNo(info.readout_capable),
	       yesNo(info.on_off_capable),
	       yesNo(info.polarity_capable),
	       yesNo(info.is_on));
	printf("  Mode = %s in [", modeName(info.current_mode));
        for(uint j = 0; j < info.modes.num; j++)
          printf(" %s ", modeName(info.modes.modes[j]));
        printf("]\n");
        printf("  Value = %u in [%u %u]\n",
	       info.value, info.min, info.max);
	printf("  Abs value = %f in [%f %f]\n",
	       info.abs_value, info.abs_min, info.abs_max);
	/*printf("  Trigger mode = %d in [", (int) info.trigger_mode);
	for(uint j = 0; j < info.trigger_modes.num; j++) {
	  printf(" %d ", (int) info.trigger_modes.modes[j]);
	}
	printf("]\n");
	printf("  Trigger source = %d in [", (int) info.trigger_source);
	for(uint j = 0; j < info.trigger_sources.num; j++) {
	  printf(" %d ", (int) info.trigger_sources.sources[j]);
	}
	printf("]\n");*/
      }
      fflush(0);
    }

    dc1394trigger_source_t trig = m_triggerSource > 0 ?
      (dc1394trigger_source_t) m_triggerSource : DC1394_TRIGGER_SOURCE_SOFTWARE;

    if(m_customFps > 0.0f && trig != DC1394_TRIGGER_SOURCE_SOFTWARE)
      error("Cannot have custom FPS combined with anything but SW trigger (%d)",
	    (int) trig);

    if(m_customFps > 0.0f || m_triggerSource > 0) {
      if(!m_video.enableTrigger(trig)) {
	m_state = FAILED;
	return;
      }
      else
	trace("Enabled trigger source %d", m_triggerSource);

      if(m_triggerMode >= 0) {
	if(!m_video.setTriggerMode
	   ((dc1394trigger_mode_t) m_triggerMode))
	  error("Could not set trigger mode %d", m_triggerMode);
	else
	  trace("Enabled trigger mode %d", m_triggerMode);
      }
    }
    else
      m_video.disableTrigger();

    trace("Getting features");

    m_video.getFeatures( & m_features);

    {
      m_featureSend.resize(m_features.size());
      
      for(unsigned i = 0; i < m_features.size(); i++) {
	dc1394feature_info_t & info = m_features[i];
	if(info.id == DC1394_FEATURE_GAMMA && 
	   info.value > ((info.max * 3 + info.min) / 4)) {
	  /* If gamma appears to be too high, bring it down. This is
	     done because some cameras when powered up (Unibrain 521b
	     for example), initialize to maximum gamma value, which
	     makes the image look strange, sometimes even fully
	     white/gray.
	  */
	  info.value = (info.max + info.min) / 2;
	  m_featureSend[i] = true;
	}
	else
	  m_featureSend[i] = false;
      }

      m_autoSend = m_featureSend;
    }

    // m_video.start();

    trace("Starting video capture");

    if(!m_video.start()) {
      m_state = UNINITIALIZED;
      error("Could not start video capture");
      return;
    }

#ifdef WIN32
	const Radiant::VideoImage * initialGrab = m_video.captureImage();
	m_frame.allocateMemory(*initialGrab);
	m_video.doneImage();
#endif

    m_state = RUNNING;

    Radiant::TimeStamp starttime(Radiant::TimeStamp::getTime());
    Radiant::SleepSync sync;
    sync.resetTiming();

    trace("Capturing video");

    m_lastCheckTime = Radiant::TimeStamp::getTime();
    while(m_continue) {

      // printf("<"); fflush(0);

      if(m_customFps > 0.0f) {
	sync.sleepSynchroUs((long) (1000000 / m_customFps));
	m_video.sendSoftwareTrigger();
      }

      const Radiant::VideoImage * img = m_video.captureImage();

#ifdef WIN32
	  if (img == 0)
	  {
		  m_continue = false;
		  m_frameCount = m_frameCount;
		  break;
	  }
#endif

      // qDebug("CamView::InputThread::childLoop # Captured");

      Radiant::Guard g( & m_mutex);

#ifndef WIN32
	  m_frame.allocateMemory(*img);
#endif
	  m_frame.copyData(*img);
	  m_frameCount++;
      m_video.doneImage();
     
      for(unsigned i = 0; i < m_features.size(); i++) {
	if(m_featureSend[i]) {
	  m_video.setFeature1394Raw(m_features[i].id, m_features[i].value);
	  m_featureSend[i] = false;
	}
	else if(m_autoSend[i]) {
	  m_video.setFeature1394(m_features[i].id, -1);
	  m_autoSend[i] = false;
	}
      }

      Radiant::TimeStamp now = Radiant::TimeStamp::getTime();

      double dt = Radiant::TimeStamp(now - m_lastCheckTime).secondsD();

      if(dt > 3.0f) {
        int frames = m_frameCount - m_lastCheckFrame;
        m_lastCheckFps = frames / dt;
        m_lastCheckFrame = m_frameCount;
        m_lastCheckTime = now;

        // qDebug("FPS = %f", m_lastCheckFps);
      }

      // printf(">"); fflush(0);
      // qDebug("CamView::InputThread::childLoop # Frame");
    }

    // qDebug("CamView::InputThread::childLoop # DONE");

    float fps = m_frameCount /
      Radiant::TimeStamp(Radiant::TimeStamp::getTime() - starttime).secondsD();

    m_frame.freeMemory();


    qDebug("CamView::InputThread::childLoop # camid = %llx # EXIT (%.2f fps, %d frames)",
	   (long long) m_video.cameraInfo().m_euid64, fps, (int) m_frameCount);

    m_video.stop();
    m_video.close();

    m_state = UNINITIALIZED;

  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool CamView::m_verbose = false;

  static int __interval = 50;

  CamView::CamView(QWidget * parent)
    : QGLWidget(parent),
      m_tex(0),
      m_params(0),
      m_showAverages(false),
      m_halfToThird(false),
      m_doAnalysis(false),
      m_imageScale(1),
      m_foo(300, 100, QImage::Format_ARGB32)
  {
    // QTimer::singleShot(1000, this, SLOT(locate()));
    connect( & m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    bzero(m_averages, sizeof(m_averages));
    setFocusPolicy(Qt::ClickFocus);

    // setAttribute(Qt::WA_DeleteOnClose, true);

    QTimer * t = new QTimer(this);

    connect(t, SIGNAL(timeout()), this, SLOT(triggerAnalysis()));
    t->start(1000);
  }
  
  CamView::~CamView()
  {
    delete m_params;
    m_thread.stop();
  }
  
  bool CamView::start(u_int64_t euid64, Radiant::FrameRate fps,
						float customFps,
		      int triggerSource, int triggerMode)
  {
    Radiant::Video1394::CameraInfo info;
    
    bool ok = Radiant::Video1394::queryCamera(euid64, & info);

    if(!ok) {
      Radiant::error("CamView::start # Could not query camera");
      return false;
    }

    QString title;

    title.sprintf("%s: %s (%llx)", 
		  info.m_vendor.c_str(), info.m_model.c_str(),
		  (long long) euid64);
    
    ((QWidget *) parent())->setWindowTitle(title);

    m_texFrame = -1;
    m_filtering = false;

    ok = m_thread.start(euid64, fps, customFps, triggerSource,  triggerMode);

    if(ok) {
      Radiant::Guard g( & m_thread.m_mutex);
      Radiant::VideoImage frame = m_thread.m_frame;
      move(100, 100);
      resize(frame.width(), frame.height());
      m_timer.start(__interval);
    }
    else
      m_timer.stop();

    return ok;
  }

  void CamView::openParams()
  {
    if(!m_params) {
      m_params = new ParamView(this);
      m_params->init();
    }

    m_params->raise();
    m_params->show();
  }
   
  void CamView::showAverages()
  {
    m_showAverages = !m_showAverages;
    m_doAnalysis = true;
  }
  
  void CamView::toggleHalfInchToThirdInch()
  {
    m_halfToThird = !m_halfToThird;
    m_doAnalysis = true;
  }
 
  void CamView::locate()
  {
    QWidget * p = dynamic_cast<QWidget *> (parent());
    if(!p)
      p = this;
    // p->raise();
    // p->show();
    Radiant::VideoImage frame = m_thread.m_frame;
    p->resize(frame.width(), frame.height());
  }

  void CamView::triggerAnalysis()
  {
    if(m_showAverages)
      m_doAnalysis = true;
  }

  void CamView::updateScreen()
  {
    if(m_timer.isActive())
      m_timer.stop();
    else
      m_timer.start(__interval);
  }

  void CamView::mouseMoveEvent(QMouseEvent * e)
  {
    mousePressEvent(e);
  }

  void CamView::mousePressEvent(QMouseEvent * e)
  {
    grabImageLuminosity(e->x(), e->y());
  }


  void CamView::hideEvent ( QHideEvent * event )
  {
    // qDebug("CamView::hideEvent");
    QGLWidget::hideEvent(event);
    // close();
  }

  void CamView::closeEvent ( QCloseEvent * event )
  {
    // qDebug("CamView::closeEvent");
    QGLWidget::closeEvent(event);
  }

  void CamView::keyPressEvent ( QKeyEvent * e )
  {
    if(e->key() == Qt::Key_Space) {
      m_doAnalysis = true;
    }
    else
      e->ignore();
  }

  void CamView::paintGL()
  {
    using Luminous::PixelFormat;

    if(!m_tex)
      m_tex = new Luminous::Texture2D;

    if(m_thread.m_frameCount && m_texFrame != m_thread.m_frameCount) {

      Radiant::Guard g( & m_thread.m_mutex);
      Radiant::VideoImage frame = m_thread.m_frame;
      
      if((m_tex->width()  != frame.width()) ||
	 (m_tex->height() != frame.height())) {
		m_tex->loadBytes(GL_LUMINANCE, frame.width(), frame.height(), 
			 frame.m_planes[0].m_data,
			 PixelFormat(PixelFormat::LAYOUT_LUMINANCE,
				     PixelFormat::TYPE_UBYTE),
			 false);
      }
      else {
	m_tex->bind();
	
        // puts("subimage");

	// bzero(frame.m_planes[0].m_data, 640 * 20); // black strip
        
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
			frame.width(), frame.height(),
			GL_LUMINANCE, GL_UNSIGNED_BYTE,
			frame.m_planes[0].m_data);
      }

      m_texFrame = m_thread.m_frameCount;

      if(m_doAnalysis)
	doAnalysis();
    }


    // trace("Frame = %d", (int) m_texFrame);

    int dw = width();
    int dh = height();

    glViewport (0, 0, dw, dh);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    gluOrtho2D(0, dw, dh, 0);
    
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    if(m_texFrame >= 0) {
      glEnable(GL_TEXTURE_2D);
      m_tex->bind();
      glColor3f(1, 1, 1);
      
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
      float aspect = m_tex->width() / (float) m_tex->height();
      float myaspect = width() / (float) height();
      float imw, imh;

      if(myaspect < aspect) {
	imw = width();
	imh = imw / aspect;
      }
      else {
	imh = height();
	imw = imh * aspect;
      }

      m_imageScale = imw / m_tex->width();
      
      if(m_filtering) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
      else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      }

      Luminous::Utils::glTexRect(0, 0, imw, imh);
    }

    glDisable(GL_TEXTURE_2D);

    if(m_halfToThird) {
      Rect sq = getEffectiveArea();

      float phase = (m_texFrame % 60) / 60.0f;
      float green = sinf(phase * Math::TWO_PI) * 0.5f + 0.5f;
      glColor3f(green * 0.5f, green, 0);

      glLineWidth(1);
      glBegin(GL_LINE_STRIP);

      glVertex2fv(sq.low().data());
      glVertex2f(sq.high().x, sq.low().y);
      glVertex2fv(sq.high().data());
      glVertex2f(sq.low().x, sq.high().y);
      glVertex2fv(sq.low().data());

      glEnd();
    }

    if(m_showAverages || m_texFrame < 0) {

      QPainter foo( & m_foo);
      QString tmp;
      // QFont fnt = font();
    
      for(int i = 0; i < AREA_COUNT && m_showAverages; i++) {
      
        Analysis & an = m_averages[i];	
        tmp.sprintf("%.1f", an.average);
      
        if(an.average < 128)
          glColor3f(1, 1, 1);
        else
          glColor3f(0, 0, 0);

        float w = foo.boundingRect(0, 0, 500, 500, Qt::AlignLeft, tmp).width();
        renderText((int) (an.center.x - w * 0.5f), (int) an.center.y, tmp);
      }

      if(m_texFrame < 0) {
        glColor3f(1, 1, 1);
        const char * warningtext = "Waiting for camera frames";
        float w = foo.boundingRect(0, 0, 500, 500,
                                   Qt::AlignLeft, warningtext).width();
        renderText((int) (width() * 0.5f - w * 0.5f), height() / 2, 
                   warningtext);
      }
    }
    glColor3f(1, 1, 1);

    char state[64];
    sprintf(state, "%.4f FPS %d frames", m_thread.m_lastCheckFps,
            m_thread.m_frameCount);

    renderText(5, 18, state);

    if(!m_text.isEmpty()) {
      glEnable(GL_TEXTURE_2D);
      Nimble::Vector2i sp = imageToScreen(m_textLoc);
      Luminous::Utils::glGrayf(m_textColor);
      renderText(sp.x + 10, sp.y, m_text);
    }
  }
  
  void CamView::grabImageLuminosity(int screenx, int screeny)
  {
    if(m_thread.m_frameCount < 2)
      return;

    Nimble::Vector2i p = screenToImage(screenx, screeny);

    // Radiant::Guard g( & m_thread.m_mutex);

    Radiant::VideoImage frame = m_thread.m_frame;

    if((uint) p.x >= (uint) frame.width() ||
       (uint) p.y >= (uint) frame.height()) {
      m_text.clear();
      return;
    }

    int lumi = frame.m_planes[0].line(p.y)[p.x];
    
    char buf[64];
    
    sprintf(buf, "%d [%d %d]", lumi, p.x, p.y);

    m_text = buf;
    m_textLoc = p;
    m_textColor = lumi < 128 ? 1.0f : 0.0f;
  }

  Nimble::Vector2i CamView::screenToImage(int screenx, int screeny) const
  {
    return Nimble::Vector2i((int) (screenx / m_imageScale),
                            (int) (screeny / m_imageScale));
  }

  Nimble::Vector2i CamView::imageToScreen(Nimble::Vector2i p) const
  {
    return Nimble::Vector2i((int) (p.x * m_imageScale),
                            (int) (p.y * m_imageScale));    
  }

  /** Returns the effective imaging area. The area includes the border
      pixels. */
  Rect CamView::getEffectiveArea()
  {
    if(!m_tex->width() || !m_tex->height())
      return Rect(0, 0, 0, 0);

    int w = m_tex->width() - 1;
    int h = m_tex->height() - 1;
    
    if(m_halfToThird) {
      /* We assume that the pixels in half inch sensor are 9.9f / 7.4f times
	 the size of pixels in third-inch sensor. Half-inch sensor has
	 9.9um pixels (for VGA) and third-inch comes with 7.4um
	 pixels.*/
      float scale = 9.9f / 7.4f;
      float remove = (scale - 1.0f) * 0.5f;
      float keep = 1.0f - remove;
      
      int lx = Nimble::Math::Round(remove * w);
      int ly = Nimble::Math::Round(remove * h);
      int hx = Nimble::Math::Round(keep * w);
      int hy = Nimble::Math::Round(keep * h);

      return Rect(lx, ly, hx, hy);
    }

    return Rect(0, 0, w, h);
  }

  void CamView::doAnalysis()
  {
    Rect a = getEffectiveArea();
    Vector2 span = a.span();
    
    Radiant::VideoImage frame = m_thread.m_frame; // Already mutex locked, safe access

    float s = 1.0f / AREA_DIVISION;

    for(int i = 0; i < AREA_DIVISION; i++) {

      int ly = (int) (a.low().y + span.y * (i * s));
      int hy = (int) (a.low().y + span.y * ((i + 1) * s));

      for(int j = 0; j < AREA_DIVISION; j++) {

	int lx = (int) (a.low().x + span.x * (j * s));
	int hx = (int) (a.low().x + span.x * ((j + 1) * s));
	
	int sum = 0;

	for(int y = ly; y <= hy; y++) {
	  const uint8_t * pixel = frame.m_planes[0].line(y) + lx;
	  const uint8_t * sentinel = pixel + hx - lx;

	  while(pixel <= sentinel)
	    sum += *pixel++;
	}

	int xx = hx - lx + 1;
	int yy = hy - ly + 1;

	Analysis & an = m_averages[i * AREA_DIVISION + j];

	an.average = sum / (float) (xx * yy);
	an.center.make((lx + hx) * 0.5, (ly + hy) * 0.5f);
      }
    }

    m_doAnalysis = false;
  }
}
