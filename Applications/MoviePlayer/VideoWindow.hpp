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

#ifndef VIDEO_WINDOW_HPP
#define VIDEO_WINDOW_HPP

#include <Dyslexic/CPUFont.hpp>
#include <Dyslexic/GPUFont.hpp>

#include <Luminous/GLResources.hpp>

#include <Resonant/DSPNetwork.hpp>

#include <VideoDisplay/ShowGL.hpp>

#include <QGLWidget>
#include <QTimer>

class VideoWindow : public QGLWidget
{
public:
  VideoWindow();
  virtual ~VideoWindow();

  bool open(const char * filename, const char * audiodev);

protected:

  class Item {
  public:
    Item() {}
    ~Item() {}
    
    VideoDisplay::ShowGL    m_show;
  };

  typedef std::list<Radiant::RefPtr<Item> > container;
  typedef container::iterator iterator;

  virtual void keyPressEvent(QKeyEvent * event);

  virtual void mousePressEvent(QMouseEvent * e);
  virtual void mouseReleaseEvent(QMouseEvent * e);

  virtual void initializeGL();
  virtual void paintGL();

  void toggleFullScreen();

  container m_movies;
  
  Resonant::DSPNetwork    m_dsp;

  QTimer m_timer;
  Radiant::TimeStamp m_lastActivity;
  Dyslexic::CPUFont * m_subCPUFont;
  Dyslexic::GPUFont * m_subGPUFont;

  Luminous::GLResources m_glResources;

  bool   m_showProgress;
  bool   m_showSteps;
};

#endif

