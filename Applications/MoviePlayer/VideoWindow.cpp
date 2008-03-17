/* COPYRIGHT
 *
 * This file is part of Applications/MoviePlayer.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Applications/MoviePlayer.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "VideoWindow.hpp"

#include <Radiant/Trace.hpp>
#include <Radiant/VideoFFMPEG.hpp>

#include <Luminous/MatrixStep.hpp>
#include <Luminous/Utils.hpp>

#include <Radiant/FileUtils.hpp>
#include <Radiant/Trace.hpp>

#include <VideoDisplay/SubTitles.hpp>

#include <Dyslexic/CPUBitmapFont.hpp>
#include <Dyslexic/GPUTextureFont.hpp>

#include <QCoreApplication>
#include <QKeyEvent>
#include <QMouseEvent>

using Radiant::error;
using Radiant::trace;

using namespace Nimble;

VideoWindow::VideoWindow()
  : m_subCPUFont(0),
    m_subGPUFont(0),
    m_showProgress(true),
    m_showSteps(false)
{
  connect( & m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
  m_timer.start(10);
}

VideoWindow::~VideoWindow()
{
  m_dsp.stop();
}

bool VideoWindow::open(const char * filename, const char * audiodev)
{
  if(!m_dsp.isRunning()) {
    bool ok = m_dsp.start(audiodev);
    if(!ok)
      return false;
  }

  Radiant::RefPtr<Item> item = new Item();

  std::string srtfile = Radiant::FileUtils::baseFilename(filename) + ".srt";

  // VideoDisplay::SubTitles subs;
  // subs.readSrt(srtfile.c_str());

  item->m_show.loadSubTitles(srtfile.c_str());

  if(!item->m_show.init(filename, & m_dsp))
    return false;
  
  m_movies.push_back(item);

  trace("VideoWindow::open # EXIT OK");

  return true;
}

#define ALL_MOVIES(X) for(iterator it = m_movies.begin(); it != m_movies.end(); it++) (*it)->m_show.X

void VideoWindow::keyPressEvent(QKeyEvent * e)
{
  m_lastActivity = Radiant::TimeStamp::getTime();

  if(e->key() == Qt::Key_G) {
    VideoDisplay::VideoIn::toggleDebug();
    RadiantIO::VideoInputFFMPEG::setDebug(1);
  }
  else if(e->key() == Qt::Key_F)
    toggleFullScreen();
  else if(e->key() == Qt::Key_S)
    m_showSteps = !m_showSteps;
  else if(e->key() == Qt::Key_Escape)
    QCoreApplication::exit();
  else if(e->key() == Qt::Key_Space)
    ALL_MOVIES(togglePause());
  else if(e->key() == Qt::Key_Left)
    ALL_MOVIES(seekBy(Radiant::TimeStamp::createSecondsD(-6)));
  else if(e->key() == Qt::Key_Right)
    ALL_MOVIES(seekBy(Radiant::TimeStamp::createSecondsD( 6)));
  else
    e->ignore();
}

void VideoWindow::mousePressEvent(QMouseEvent * e)
{
  m_lastActivity = Radiant::TimeStamp::getTime();

  if(!m_showProgress)
    return;

  if(e->pos().y() < (height() - 35))
    return;

  //puts("REL POS");

  double rel = e->pos().x() / (double) this->width();

  ALL_MOVIES(seekToRelative(rel));
}
 
void VideoWindow::mouseReleaseEvent(QMouseEvent *)
{}

void VideoWindow::initializeGL()
{
  ALL_MOVIES(contextInit());
  const char * ttf = "DejaVuSans.ttf";
  const char * path = ".:/Users/tommi/screenapps/Fonts/";
  std::string filename = Radiant::FileUtils::findFile(ttf, path);

  if(filename.size()) {
    m_subCPUFont = new Dyslexic::CPUBitmapFont();
    if(m_subCPUFont->load(filename.c_str())) {
      m_subCPUFont->setFaceSize(25, 72);
      m_subGPUFont = new Dyslexic::GPUTextureFont(m_subCPUFont);
      Radiant::trace("VideoWindow::initializeGL # Got font");
    }
  }
}

void VideoWindow::paintGL()
{
  // puts("VideoWindow::paintGL");
  
  int w = width();
  int h = height();

  glViewport (0, 0, w, h);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  
  glDisable(GL_LIGHTING);
  
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor4f(1, 1, 1, 1);
  gluOrtho2D(0, width(), height(), 0);
  
  ALL_MOVIES(update());

  int n = m_movies.size();

  int rows = (int) ceilf(sqrtf(n));
  int cols = 1;
  while(rows * cols < n)
    cols++;

  int index = 0;
  
  int itemw = w / cols;
  int itemh = h / rows;

  float inactsecs =
    Radiant::TimeStamp(Radiant::TimeStamp::getTime() - m_lastActivity).secondsD();

  float maxDisplay = 6.0f;
  
  m_showProgress = inactsecs < maxDisplay;

  for(iterator it = m_movies.begin(); it != m_movies.end(); it++) {
  
    VideoDisplay::ShowGL & show = (*it)->m_show;

    glColor4f(1, 1, 1, 1);

    // Aspect ratios
    float wa = itemw / (float) itemh;
    Nimble::Vector2i ss = show.size();
    float sa = ss.x / (float) ss.y;
  
    float realw;
    float realh;

    if(wa <= sa) {
      realw = itemw;
      realh = realw / sa;
    }
    else {
      realh = itemh;
      realw = realh * sa;
    }

    Vector2f center(itemw, itemh);
    center *= 0.5f;
    
    Vector2f span(realw, realh);
    span *= 0.5f;

    Luminous::MatrixStep mstep;
    glTranslatef((index % cols) * itemw, (index / cols) * itemh, 0);
    index++;

    show.render(center - span, center + span, 0, m_subGPUFont, h);
    
    if(!m_showProgress)
      continue;

    // Now draw the progress bar etc...

    Luminous::Utils::glUsualBlend();

    glDisable(GL_TEXTURE_2D);
    
    float mainAlpha = 1.0f;

    float fadeTime = 3.0f;
    float startFade = maxDisplay - fadeTime;

    if(inactsecs > startFade)
      mainAlpha = 1.0f - (inactsecs - startFade) / fadeTime;

    if(m_showSteps) {
      glBegin(GL_LINE_STRIP);

      for(int i = 0; i < VideoDisplay::ShowGL::HISTOGRAM_POINTS; i++) {
	int delta =
	  (show.histogramIndex() + VideoDisplay::ShowGL::HISTOGRAM_POINTS-i) %
	  VideoDisplay::ShowGL::HISTOGRAM_POINTS;
	
	float a = 1.0f - 0.7f *
	  powf(delta / (float) VideoDisplay::ShowGL::HISTOGRAM_POINTS, 0.25);
      
	glColor4f(a, 0, 0, a * mainAlpha);
	
	glVertex2f(i * 2, itemh - 1 - show.histogramPoint(i) * 35 * 0.5f);
      }
      
      glEnd();
    }
    
    glColor4f(0.3, 0.3, 0.3, 0.7 * mainAlpha);
    glRectf(0, itemh - 35, itemw, itemh);
    
    glColor4f(1, 1, 1, 0.7 * mainAlpha);
    
    float relative = show.relativePosition();

    glRectf(0, itemh - 30, itemw * relative, itemh - 5);
  }
}

void VideoWindow::toggleFullScreen()
{
  setWindowState(windowState() ^ Qt::WindowFullScreen);
}

