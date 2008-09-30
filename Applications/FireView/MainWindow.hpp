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

#ifndef FIREVIEW_MAINWINDOW_HPP
#define FIREVIEW_MAINWINDOW_HPP

#include <Radiant/VideoInput.hpp>

#include <QMainWindow>

#include <set>

#ifdef WIN32
#include <WinPort.h>
#endif

class QMdiArea;

namespace FireView {

  class MainWindow : public QMainWindow
  {
    Q_OBJECT;
  public:
    MainWindow(Radiant::FrameRate rate, float customFps,
	       int triggerSource, int triggerMode, bool format7);
    virtual ~MainWindow();

    bool init();

  public slots:
    
    void checkCameras();

  protected:
    QMdiArea * m_mdi;
    std::set<u_int64_t> m_cameras;
    Radiant::FrameRate m_rate;
    float m_customFps;
    int   m_triggerSource;
    int   m_triggerMode;
    bool  m_format7;

    std::set<QWidget *> m_displays;
  };
}

#endif

