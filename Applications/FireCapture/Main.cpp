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

#include <Radiant/Sleep.hpp>
#include <Radiant/TimeStamp.hpp>
#include <Radiant/Thread.hpp>
#include <Radiant/Trace.hpp>
#include <Radiant/Video1394.hpp>

#include <stdlib.h>
#include <string.h>

int triggerSource = -1;
int triggerMode = -1;
Radiant::FrameRate rate = Radiant::FPS_15;

class CameraThread : public Radiant::Thread
{
public:

  CameraThread(uint64_t cameraId, const std::string & dir)
    : m_continue(true),
      m_cameraId(cameraId),
      m_dir(dir)
  {}

  void stop()
  {
    m_continue = false;
    waitEnd();
  }

protected:

  virtual void childLoop()
  {
    Radiant::info("Camera loop started for %llx", (long long) m_cameraId);
    
    m_video.setCameraEuid64(m_cameraId);

    m_video.open(0, 0, 0, Radiant::IMAGE_UNKNOWN, 640, 480, rate);

    if(triggerMode >= 0) {
      m_video.setTriggerMode((dc1394trigger_mode_t) triggerMode);
    }

    if(triggerSource >= 0) {
      m_video.enableTrigger((dc1394trigger_source_t) triggerSource);
    }

    m_video.start();

    int count = 0;

    Radiant::TimeStamp start(Radiant::TimeStamp::getTime());

    while(m_continue) {

      const Radiant::VideoImage * im = m_video.captureImage(5000000);

      if(!im) {
        Radiant::error("Frame capture failed for camera %llx",
                       (long long) m_cameraId);
        
        break;
      }

      count++;

      if(count % 100 == 0) {
        Radiant::info("Capture %d frames from camera %llx",
                      count, (long long) m_cameraId);
      }

      m_video.doneImage();

    }
    
    float secs = start.since().secondsD();

    Radiant::info("Captured %d frames in %.2f seconds, %.2f FPS",
                  count, secs, (float) (count / secs));

    m_video.stop(); 
  }

private:
  
  volatile bool m_continue;
  uint64_t      m_cameraId;
  std::string   m_dir;

  Radiant::Video1394 m_video;

};


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
     " --triggerpolarity   +up/down - Selects the trigger polarity, either "
          "\"up\" or \"down\"\n"
     " --triggersource +int - Selects the trigger source, range: 0-%d\n"
     "\nEXAMPLES:\n"
     " %s             - Run all cameras at 15 fps\n"
     " %s --scanbus   - List cameras, with IDs\n"
     " %s --fps 47    - Run all cameras at 47 fps (with SW triggering)\n"
     " %s --rate 30   - Run all cameras at 30 fps (internal triggering)\n"
     " %s --rate 60 --triggersource 0  - Run all cameras at max 60 fps with hardware trigger\n"
     " %s --rate 60 --triggersource 0 --triggermode 0 - Run all cameras at max 60 fps with trigger source 0 and trigger mode 0\n"
     , (int) DC1394_TRIGGER_MODE_NUM - 1, (int) DC1394_TRIGGER_SOURCE_NUM - 1,
     app, app, app, app, app, app);
  fflush(0);
}


int main(int argc, char ** argv)
{
  int secs = 20;
  
  float fps = -1.0f;

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
    else if(strcmp(arg, "--time") == 0 && (i+1) < argc) {
      secs = atoi(argv[++i]);
    }
    else if(strcmp(arg, "--triggermode") == 0 && (i+1) < argc) {
      triggerMode = (atoi(argv[++i]) + DC1394_TRIGGER_MODE_0);
    }
    else if(strcmp(arg, "--triggerpolarity") == 0 && (i+1) < argc) {
      i++;
      if(strcmp(argv[i], "up") == 0) {
	;//FireView::CamView::setTriggerPolarity(DC1394_TRIGGER_ACTIVE_HIGH);
      }
      else if(strcmp(argv[i], "down") == 0) {
	; //FireView::CamView::setTriggerPolarity(DC1394_TRIGGER_ACTIVE_LOW);
      }
    }
    else if(strcmp(arg, "--triggersource") == 0 && (i+1) < argc) {
      triggerSource = (atoi(argv[++i]) + DC1394_TRIGGER_SOURCE_0);
    }
    else if(strcmp(arg, "--verbose") == 0) {
      puts("Verbose mode");
      Radiant::enableVerboseOutput(true);
    }
    else {
      printf("%s Could not handle argument %s\n", argv[0], arg);
      helper(argv[0]);
      return -1;
    }
  }

  if(triggerMode >= 0 && triggerSource < 0) {
    printf("%s If you set trigger mode, you also need to set trigger source\n",
	   argv[0]);
    return -1;
  }

  std::vector<Radiant::Video1394::CameraInfo> cameras;
  Radiant::Video1394::queryCameras( & cameras);

  printf("Found %d FireWire cameras\n", (int) cameras.size());

  if(! cameras.size())
    return 0;

  std::vector<CameraThread *> threads;

  for(i = 0; i < (int) cameras.size(); i++) {

    const Radiant::Video1394::CameraInfo & cam = cameras[i];
    printf("Camera %d: ID = %llx, VENDOR = %s, MODEL = %s\n",
           i + 1, (long long) cam.m_euid64,
           cam.m_vendor.c_str(), cam.m_model.c_str());
    fflush(0);

    CameraThread * thread = new CameraThread(cam.m_euid64, "foo");

    threads.push_back(thread);

    thread->run();
  }

  Radiant::Sleep::sleepS(secs);

  for(int i = 0; i < (int) threads.size(); i++) {
    
    threads[i]->stop();
    delete threads[i];
  }
  
  return res;
}
