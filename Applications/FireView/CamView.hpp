/* COPYRIGHT
 *
 * This file is part of Applications/FireView.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Applications/FireView.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef FIREVIEW_CAMVIEW_HPP
#define FIREVIEW_CAMVIEW_HPP

#ifdef WIN32
#include <WinPort.h>
#include <Radiant/Video1394cmu.hpp>
#define _WINSOCKAPI_	// timeval struct redefinition
#endif

#include <Luminous/Texture.hpp>

#include <Nimble/Rect.hpp>

#include <Radiant/Mutex.hpp>
#include <Radiant/Thread.hpp>
#include <Radiant/TimeStamp.hpp>
#include <Radiant/Video1394.hpp>

#include <QGLWidget>
#include <QTimer>

namespace FireView {

  using namespace Nimble;

  class ParamView;

  class CamView : public QGLWidget
  {
    Q_OBJECT;
  public:
    CamView(QWidget * parent);
    virtual ~CamView();

    bool start(u_int64_t euid64, Radiant::FrameRate fps, float customFps = 0.0f,
	       int triggerSource = -1, int triggerMode = -1);

    std::vector<dc1394feature_info_t> & features()
    { return m_thread.m_features; }

    void updateParam(int i) { m_thread.m_featureSend[i] = true;}
    void autoParam(int i) { m_thread.m_autoSend[i] = true;}

    static void setVerbose(bool verbose) { m_verbose = verbose; }

  public slots:
    
    void openParams();
    void showAverages();
    void toggleHalfInchToThirdInch();
    void locate();
    void triggerAnalysis();
    void updateScreen();
    void toggleFiltering()
    { m_filtering = !m_filtering; }

  protected:
    
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mousePressEvent(QMouseEvent * e);

    virtual void hideEvent ( QHideEvent * e );
    virtual void closeEvent ( QCloseEvent * e );
    virtual void keyPressEvent ( QKeyEvent * e );
    virtual void paintGL();

    void grabImageLuminosity(int screenx, int screeny);
    Nimble::Vector2i screenToImage(int screenx, int screeny) const;
    Nimble::Vector2i imageToScreen(Nimble::Vector2i) const;
    
    Rect getEffectiveArea();
    void doAnalysis();

    class InputThread : public Radiant::Thread
    {
    public:

      enum State {
	UNINITIALIZED,
	STARTING,
	FAILED,
	RUNNING
      };
      
      friend class CamView;
      InputThread();
      virtual ~InputThread();

      bool start(u_int64_t euid64, Radiant::FrameRate fps, 
		 float customFps, int triggerSource, int triggerMode);
      void stop();

      bool isRunning() const { return m_state == RUNNING; }
      
    protected:

      virtual void childLoop();

      Radiant::Video1394 m_video;
      Radiant::MutexAuto m_mutex;
      Radiant::VideoImage m_frame;
      Radiant::FrameRate m_fps;
      float           m_customFps;
      int             m_triggerSource;
      int             m_triggerMode;

      std::vector<dc1394feature_info_t> m_features;
      std::vector<bool> m_featureSend;
      std::vector<bool> m_autoSend;
      
      volatile State m_state;
      volatile bool  m_continue;
      volatile int   m_frameCount;

      Radiant::TimeStamp m_lastCheckTime;
      int                m_lastCheckFrame;
      float              m_lastCheckFps;
    };

    class Analysis
    {
    public:

      float average;
      Vector2 center;
    };

    enum {
      AREA_DIVISION = 5,
      AREA_COUNT = AREA_DIVISION * AREA_DIVISION
    };

    int m_texFrame;
    bool m_filtering;

    Luminous::Texture2D * m_tex;

    InputThread m_thread;
    QTimer      m_timer;

    ParamView  *m_params;

    bool        m_showAverages;
    bool        m_halfToThird;
    bool        m_doAnalysis;
    float       m_imageScale;

    QString     m_text;
    Nimble::Vector2i m_textLoc;
    float       m_textColor;

    Analysis   m_averages[AREA_COUNT]; // Grid.
    QImage     m_foo;
    static bool m_verbose;
  };

}

#endif
