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

#include "CamView.hpp"
#include "MainWindow.hpp"

#include <Radiant/Trace.hpp>
#include <Radiant/Video1394.hpp>

#include <QtGui/QApplication>

enum Task {
  TASK_SHOW_CAMERAS,
  TASK_SCAN_BUS
};

void helper(const char * app)
{
  printf("USAGE:\n %s [options]\n\n", app);
  printf
    ("OPTIONS:\n"
     "--format7       - Uses Format 7 modes\n"
     " --fps  +float  - Sets arbitrary capture rate for the cameras, with SW trigger\n"
     " --help         - This help\n"
     " --listformat7modes    - List available format 7 modes\n"
     " --rate +int    - Selects one of the standard frame rates (15, 30, 60...)\n"
     " --scanbus      - Scans and reports all available cameras\n"
     " --triggermode   +int - Selects the trigger mode, range: 0-%d\n"
     " --triggersource +int - Selects the trigger source, range: 0-%d\n"
     "\nEXAMPLES:\n"
     " %s             - Run all cameras at 15 fps\n"
     " %s --scanbus   - List cameras, with IDs\n"
     " %s --fps 47    - Run all cameras at 47 fps (with SW triggering)\n"
     " %s --rate 30   - Run all cameras at 30 fps (internal triggering)\n"
     " %s --rate 60 --triggersource 0  - Run all cameras at max 60 fps with hardware trigger\n"
     " %s --rate 60 --triggersource 0 --triggermode 1 - Run all cameras at max 60 fps with trigger source 0 and trigger mode 1\n"
     , (int) DC1394_TRIGGER_MODE_NUM - 1, (int) DC1394_TRIGGER_SOURCE_NUM - 1,
     app, app, app, app, app, app);
  fflush(0);
}


int main(int argc, char ** argv)
{
  QApplication qa(argc, argv);
  
  float fps = -1.0f;

  int triggerSource = -1;
  int triggerMode = -1;
  Radiant::FrameRate rate = Radiant::FPS_15;
  Task t = TASK_SHOW_CAMERAS;
  int i, res = 0;
  bool format7 = false;
  bool listmodes = false;

  for(i = 1; i < argc; i++) {
    const char * arg = argv[i];

    if(strcmp(arg, "--format7") == 0) {
      format7 = true;
    }
    else if(strcmp(arg, "--fps") == 0 && (i+1) < argc) {
      fps = atof(argv[++i]);
    }
    else if(strcmp(arg, "--help") == 0) {
      helper(argv[0]);
      return 0;
    }
    else if(strcmp(arg, "--listformat7modes") == 0) {
      listmodes = true;
    }
    else if(strcmp(arg, "--rate") == 0 && (i+1) < argc) {
      rate = Radiant::closestFrameRate(atof(argv[++i]));
    }
    else if(strcmp(arg, "--scanbus") == 0) {
      t = TASK_SCAN_BUS;
    }
    else if(strcmp(arg, "--triggermode") == 0 && (i+1) < argc) {
      triggerMode = (atoi(argv[++i]) + DC1394_TRIGGER_MODE_0);
    }
    else if(strcmp(arg, "--triggersource") == 0 && (i+1) < argc) {
      triggerSource = (atoi(argv[++i]) + DC1394_TRIGGER_SOURCE_0);
    }
    else if(strcmp(arg, "--verbose") == 0) {
      FireView::CamView::setVerbose(true);
      Radiant::enableVerboseOutput(true);
    }
    else {
      printf("%s Could not handle argument %s\n", argv[0], arg);
      helper(argv[0]);
      return -1;
    }
  }

  if(triggerMode >= 0 && triggerSource < 0) {
    printf("%s If you set trigger mode, you also need to set trigger mode\n",
	   argv[0]);
    return -1;
  }

  if(t == TASK_SCAN_BUS) {
    std::vector<Radiant::Video1394::CameraInfo> cameras;
    Radiant::Video1394::queryCameras( & cameras);

    printf("Found %d FireWire cameras\n", (int) cameras.size());

    for(i = 0; i < (int) cameras.size(); i++) {
      const Radiant::Video1394::CameraInfo & cam = cameras[i];
      printf("Camera %d: ID = %llx, VENDOR = %s, MODEL = %s\n",
	     i + 1, (long long) cam.m_euid64,
	     cam.m_vendor.c_str(), cam.m_model.c_str());
      fflush(0);

      if(listmodes) {
/// @todo implement in win32
#ifndef WIN32
	Radiant::Video1394 tmp;
	char idbuf[32];
	sprintf(idbuf, "%llx", cam.m_euid64);
	tmp.printFormat7Modes(idbuf);
#endif
      }
    }
  }
  else {

    FireView::MainWindow * mw =
      new FireView::MainWindow(rate, fps, triggerSource, triggerMode, format7);
    
    mw->resize(800, 600);
    mw->init();
    mw->show();

    QObject::connect( & qa, SIGNAL(lastWindowClosed()), & qa, SLOT(quit()));
    res = qa.exec();
    
    delete mw;
  }

  return res;
}
