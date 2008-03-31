/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

// Some original source code by Juha Laitinen still may be around.

#include "Video1394.hpp"

#include <Radiant/Trace.hpp>
#include <Radiant/Mutex.hpp>
#include <Radiant/Sleep.hpp>
#include <Radiant/Types.hpp>

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <iostream>
#include <set>

#include <dc1394/camera.h>

#include <sys/utsname.h>

#define NUM_BUFFERS 10

namespace Radiant {

  static std::vector<dc1394camera_t *> __infos;

  static std::set<int> __iso_channels;

  inline int diFPS2dcFPS(FrameRate fps)
  {

    switch(fps) {
    case FPS_IGNORE:
      return DC1394_FRAMERATE_15;
    case FPS_5:
      return DC1394_FRAMERATE_3_75;
    case FPS_10:
      return DC1394_FRAMERATE_7_5;
    case FPS_15:
      return DC1394_FRAMERATE_15;
    case FPS_30:
      return DC1394_FRAMERATE_30;
    case FPS_60:
      return DC1394_FRAMERATE_60;
    case FPS_120:
      return DC1394_FRAMERATE_120;
    case FPS_COUNT:
      ;
    }

    return DC1394_FRAMERATE_15;
  }

  const char * fps_labels[] =
    {
      "1.875 fps",
      "3.75 fps",
      "7.5 fps",
      "15 fps",
      "30 fps",
      "60 fps",
      "120 fps",
      "240 fps"
    };

  const char * format0_labels[]=
    {
      "Format 0, Mode 0: 160x120 YUV (4:4:4)",
      "Format 0, Mode 1: 320x240 YUV (4:2:2)",
      "Format 0, Mode 2: 640x480 YUV (4:1:1)",
      "Format 0, Mode 3: 640x480 YUV (4:2:2)",
      "Format 0, Mode 4: 640x480 RGB 24bpp",
      "Format 0, Mode 5: 640x480 Mono 8bpp",
      "Format 0, Mode 6: 640x480 Mono 16bpp"
    };

  dc1394video_mode_t difmt2dcfmt(ImageFormat fmt, int w, int h)
  {
    if(w == 640 && h == 480) {
      if(fmt == IMAGE_RAWBAYER || fmt == IMAGE_GRAYSCALE)
	return DC1394_VIDEO_MODE_640x480_MONO8;
      else if(fmt == IMAGE_YUV_411 || fmt == IMAGE_YUV_411P)
	return DC1394_VIDEO_MODE_640x480_YUV411;
      else if(fmt == IMAGE_YUV_422 || fmt == IMAGE_YUV_422P)
	return DC1394_VIDEO_MODE_640x480_YUV422;
      else
	return DC1394_VIDEO_MODE_640x480_YUV411;
    }
    else if(w == 1024 && h == 768) {
      if(fmt == IMAGE_RAWBAYER || fmt == IMAGE_GRAYSCALE)
	return DC1394_VIDEO_MODE_1024x768_MONO8;
      else if(fmt == IMAGE_YUV_422 || fmt == IMAGE_YUV_422P)
	return DC1394_VIDEO_MODE_1024x768_YUV422;
      else
	return DC1394_VIDEO_MODE_1024x768_YUV422;    
    }
  
    return DC1394_VIDEO_MODE_640x480_YUV411;
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  static dc1394_t * __dc = 0;

  static int __count = 0;

  Video1394::Video1394()
    : m_camera(0),
      m_frame(0),
      m_outside(0)
  {
    m_videodevice = "/dev/video1394";
    m_initialized = false;
    m_euid = 0;
    m_cameraNum = 0;
    m_started = false;

    if(!__dc)
      __dc = dc1394_new();
    
    __count++;
  }

  Video1394::~Video1394()
  {
    if (m_initialized)
      close();

    __count--;
    if(!__count)
      dc1394_free(__dc);
  }

  ImageFormat Video1394::imageFormat() const
  {
    return m_image.m_format;
  }

  unsigned int Video1394::size() const
  {
    return m_image.size();
  }

  bool Video1394::hasMode(const dc1394feature_info_t & feature,
			  dc1394feature_mode_t mode)
  {
    for(uint i = 0; i < feature.modes.num; i++)
      if(feature.modes.modes[i] == mode)
	return true;

    return false;
  }

  dc1394error_t has_mode(dc1394camera_t *camera,
			 dc1394feature_t feature,
			 dc1394feature_mode_t mode,
			 dc1394bool_t * val)
  {
    dc1394feature_modes_t modes;
    modes.num = 0;
    dc1394error_t ret = dc1394_feature_get_modes(camera, feature, & modes);
    
    *val = DC1394_FALSE;

    for(uint i = 0; i < modes.num; i++)
      if(modes.modes[i] == mode)
	*val = DC1394_TRUE;

    return ret;
  }

  dc1394error_t has_auto_mode(dc1394camera_t *camera,
			      dc1394feature_t feature,
			      dc1394bool_t * val)
  {
    return has_mode(camera, feature, DC1394_FEATURE_MODE_AUTO, val);
  }

  dc1394error_t has_manual_mode(dc1394camera_t *camera,
			      dc1394feature_t feature,
			      dc1394bool_t * val)
  {
    return has_mode(camera, feature, DC1394_FEATURE_MODE_MANUAL, val);
  }

  void Video1394::setGamma(float value)
  {
    setFeature1394(DC1394_FEATURE_GAMMA, value);
  }

  void Video1394::setShutter(float value)
  {
    setFeature1394(DC1394_FEATURE_SHUTTER, value);
  }

  void Video1394::setGain(float value)
  {
    setFeature1394(DC1394_FEATURE_GAIN, value);
  }

  void Video1394::setExposure(float value)
  {
    setFeature1394(DC1394_FEATURE_EXPOSURE, value);
  }

  void Video1394::setBrightness(float value)
  {
    setFeature1394(DC1394_FEATURE_BRIGHTNESS, value);
  }

  void Video1394::setFocus(float value)
  {
    setFeature1394(DC1394_FEATURE_FOCUS, value);
  }


  /** 
   * Try to adjust white balance feature for the camera. 
   * @note NOTE: may fail, if, for example, the camera doesn't 
   * support this feature.
   * @param u_to_blue manual U to blue ratio, setting either 
   * one of these to below zero sets auto white balance mode.
   * @param v_to_red manual V to red ratio, setting either one 
   * of these to below zero sets auto white balance mode. These values are 
   * basicly device-dependent. You should test suitable configuration
   * with for example coriander software. Setting values out-of-range 
   * gives a warning.
   */
  void Video1394::setWhiteBalance(float u_to_blue, float v_to_red)
  {
    const char * fname = "Video1394::setWhiteBalance";

    dc1394bool_t b = DC1394_FALSE;

    if(u_to_blue < 0 || v_to_red < 0) {

      has_auto_mode(m_camera, DC1394_FEATURE_WHITE_BALANCE, &b);

      if(b)
	dc1394_feature_set_mode(m_camera, 
				DC1394_FEATURE_WHITE_BALANCE, 
				DC1394_FEATURE_MODE_AUTO);
      else
	error("%s # no auto mode for white balance", fname);
    }
    else {
    
      has_manual_mode(m_camera, 
		      DC1394_FEATURE_WHITE_BALANCE,
		      & b);

      if(b)
	dc1394_feature_set_mode(m_camera, 
				DC1394_FEATURE_WHITE_BALANCE, 
				DC1394_FEATURE_MODE_MANUAL);
      else {
	error("%s # no manual mode for white balance", fname);
	return;
      }

      uint32_t low  = 0;
      uint32_t high = 0;

      dc1394_feature_get_boundaries(m_camera, DC1394_FEATURE_WHITE_BALANCE, 
				    & low, & high);
    
      uint32_t ublue, vred;

      if(low != high) {
	float s = high - low;

	ublue = (uint32_t) (s * u_to_blue + low);
	vred  = (uint32_t) (s * v_to_red  + low);

	if(ublue > high)
	  ublue = high;

	if(vred > high)
	  vred = high;
      }
      else {
	ublue = (uint32_t) round(u_to_blue);
	vred  = (uint32_t) round(v_to_red);
      }

      dc1394_feature_whitebalance_set_value(m_camera, ublue, vred);
    }
  }

  /** Set a camera feature (=control parameter) to some value. Possible
   * features can be found from dc1394_control.h. If the "value" is less
   * than zero, then the camera will be switched to automatic control of
   * the feature. */

  void Video1394::setFeature1394(dc1394feature_t feature, float value)
  {
    const char * fname = "Video1394::setFeature1394";

    const char * name = dc1394_feature_get_string(feature);

    // trace2("%s # %s %f", fname, name, value);

    bool automatic = value < 0.0f;

    dc1394bool_t b = DC1394_FALSE;

    if(automatic) {

      has_auto_mode(m_camera, feature, & b);

      if(b)
	dc1394_feature_set_mode(m_camera, feature, DC1394_FEATURE_MODE_AUTO);
      else
	error("%s # no auto mode for %s", fname, name);
    }
    else {
      has_manual_mode(m_camera, feature, & b);

      if(b)
	dc1394_feature_set_mode(m_camera, feature, DC1394_FEATURE_MODE_MANUAL);
      else {
	error("%s # no manual mode for %s", fname, name);
	return;
      }

      uint32_t low  = 0;
      uint32_t high = 0;

      dc1394_feature_get_boundaries(m_camera, feature, & low, & high);

      uint32_t uvalue;
    
      if(low != high) {

	uvalue = (uint) ((high - low) * value + low);

	if(uvalue > high)
	  uvalue = high;
      }
      else
	uvalue = (uint32_t) round(value);

      dc1394_feature_set_value(m_camera, feature, uvalue);
    }
  }

  void Video1394::setFeature1394Raw(dc1394feature_t feature, int32_t value)
  {
    dc1394_feature_set_mode(m_camera,  feature, DC1394_FEATURE_MODE_MANUAL);
    dc1394_feature_set_value(m_camera, feature, value);
  }

  void Video1394::getFeatures(std::vector<dc1394feature_info_t> * feats)
  {
    assert(isInitialized());
    dc1394featureset_t features;
    dc1394_feature_get_all(m_camera, & features);
    feats->resize(DC1394_FEATURE_NUM);
    for(int i = 0; i < DC1394_FEATURE_NUM; i++)
      (*feats)[i] = features.feature[i];
  }

  const char * Video1394::featureName(dc1394feature_t f)
  {
    static const char * names [] = {
      "brightness",
      "exposure",
      "sharpness",
      "white-balance",
      "hue",
      "saturation",
      "gamma",
      "shutter",
      "gain",
      "iris",
      "focus",
      "temperature",
      "trigger",
      "trigger delay",
      "white shading",
      "frame rate",
      "zoom",
      "pan",
      "tilt",
      "optical filter",
      "capture size",
      "capure quality"
    };

    int index = f - DC1394_FEATURE_MIN;

    if(index < 0 || index >= DC1394_FEATURE_NUM)
      return 0;

    return names[index];
  }

  
  bool Video1394::enableTrigger(dc1394trigger_source_t source)
  {
    assert(m_camera != 0);

    if(dc1394_external_trigger_set_power(m_camera, DC1394_ON)
       != DC1394_SUCCESS) {
      error("Video1394::enableTrigger # Could not turn trigger power on");
      return false;
    }

    if(dc1394_external_trigger_set_source(m_camera, source)
       != DC1394_SUCCESS) {
      error("Video1394::enableTrigger # Could not set trigger source");
      return false;
    }
    
    return true;
  }
  
  bool Video1394::setTriggerMode(dc1394trigger_mode_t mode)
  {
    assert(m_camera != 0);

    if(dc1394_external_trigger_set_mode(m_camera, mode)
       != DC1394_SUCCESS) {
      error("Video1394::setTriggerMode # Could not set trigger mode");
      return false;
    }

    return true;
  }

  bool Video1394::disableTrigger()
  {
    assert(m_camera != 0);

    return dc1394_external_trigger_set_power(m_camera, DC1394_OFF) ==
      DC1394_SUCCESS;
  }

  void Video1394::sendSoftwareTrigger()
  {
    assert(m_camera != 0);
    dc1394_software_trigger_set_power(m_camera, DC1394_ON);
  }

  /**
   * Initialize this instance and open connnection to the device to be
   * controlled.
   */
  bool Video1394::open(const char * device, 
		       const char * camera,
		       const char * euid,
		       ImageFormat fmt, 
		       int width, 
		       int height, 
		       FrameRate framerate)
  {
    static MutexAuto mutex;

    // Only one thread at a time, just to make things sure.
    Guard guard( & mutex);

    m_videodevice = device ? device : "/dev/video1394";
    m_cameraNum = camera ? atoi(camera) : 0;
    m_euid = euid ? strtoll(euid, 0, 16) : m_euid;

    if(euid != 0)
      trace("Video1394::open # %.8x%.8x (%s)", 
	     (int) (m_euid >> 32), (int) m_euid, euid);

    m_outside = 0;

    const char * fname = "Video1394::initialize";

    // uint channel;
    // uint speed;

    bzero( & m_camera, sizeof(m_camera));
    // bzero( & m_features, sizeof(m_features));

    if (m_initialized)
      close();

    uint32_t i;

    {
      std::vector<CameraInfo> cameras;
    
      queryCameras(&cameras);
    }
  
    if(!__infos.size()) {
      error("%s # No FireWire cameras found", fname);
      return false;
    }

    struct utsname sn;
    uname( & sn);

    // trace("System: %s # %s # %s", sn.sysname, sn.release, sn.version);
    bool isleopard = strcmp(sn.sysname, "Darwin") == 0 &&
      sn.release[0] == '9' && sn.release[1] == '.';

    // Clean up in the first start:
    static int initcount = 0;
    if(!initcount) {
      
      if(isleopard)
	trace("%s # Running Leopard, no FireWire bus reset");
      else {
	for(int c = 0; c < (int) __infos.size(); c++) {
	  dc1394_reset_bus(__infos[c]);
	  Sleep::sleepMs(20);
	}
      }
    }
    initcount++;

    // Now seek the camera we are interested in:

    bool foundCorrect = false;

    for(i = 0; i < __infos.size() && m_euid != 0; i++) {
      if(__infos[i]->guid == m_euid) {
	m_cameraNum = (int) i;
	foundCorrect = true;
	trace("%s # Got camera %d based on euid", fname, (int) i);
	break;
      }
    }

    if(m_euid != 0 && !foundCorrect) {
      error("%s # Could not find the camera with euid = %llx",
	    (long long) m_euid);
      return false;
    }

    assert(m_cameraNum < (int) __infos.size());

    m_camera = __infos[m_cameraNum];

    /* int isochan = m_cameraNum + 2;
       if(dc1394_video_specify_iso_channel(m_camera, isochan) !=DC1394_SUCCESS){
       error(ERR_UNKNOWN, "%s # unable to set ISO channel to %d", 
       fname, isochan);
       }
    */

    if(dc1394_feature_get_all(m_camera, & m_features)
       != DC1394_SUCCESS) {
      error("%s # unable to get feature set %d", 
	     fname, m_cameraNum);
    }
    /* else
      dc1394_print_feature_set(& m_features);
    */
    dc1394speed_t speed;
  
    /* if(dc1394_video_get_iso_channel_and_speed(m_camera, &channel, &speed) 
       != DC1394_SUCCESS) 
       fatal(ERR_UNKNOWN, 
       "%s # unable to get the iso channel number", fname);
    */
  
    // dc1394video_mode_t video_mode = DC1394_VIDEO_MODE_640x480_MONO8;
    dc1394video_mode_t video_modes[] = {
      difmt2dcfmt(fmt, width, height),
      DC1394_VIDEO_MODE_1024x768_MONO8,
      DC1394_VIDEO_MODE_640x480_MONO8,
      DC1394_VIDEO_MODE_640x480_YUV411,
      (dc1394video_mode_t) 0
    };
    dc1394video_mode_t video_mode = DC1394_VIDEO_MODE_640x480_MONO8;
    // dc1394video_mode_t video_mode = DC1394_VIDEO_MODE_320x240_YUV422;
  
    dc1394framerates_t framerates;

    for(i = 0; video_modes[i] != 0; i++) {
      video_mode = video_modes[i];
      if (dc1394_video_get_supported_framerates(m_camera, 
						video_mode, &framerates)
	  != DC1394_SUCCESS) {
	fatal("%s # dc1394_video_get_supported_framerates",
	       fname);
	// cleanup_and_exit(m_camera);
      }
      if(framerates.num != 0)
	break;
    }

    assert(framerates.num);

    int targetfps = diFPS2dcFPS(framerate);
    dc1394framerate_t fps = (dc1394framerate_t) targetfps;
  
    for(i = 0; i < framerates.num; i++) {
      fps = framerates.framerates[i];
      if(fps == targetfps)
	break;
    }
  
    trace("%s # The video mode id = %d", fname, (int) video_mode);
    trace("%s # The frame rate id = %d (target = %d)", 
	   fname, (int) fps, targetfps);
    
#ifdef __linux__
    // controlled with environment variable:
    bool try1394b = getenv("NO_FW800") == 0;
    trace("%s # Try %s FW800", fname, try1394b ? "with" : "without");
#else
    bool try1394b = true;
#endif

    if(try1394b) {
      bool is1394b = false;
      
      if(dc1394_video_set_operation_mode(m_camera, DC1394_OPERATION_MODE_1394B)
	 != DC1394_SUCCESS) {
	dc1394_video_set_operation_mode(m_camera, DC1394_OPERATION_MODE_LEGACY);
      }
      else
	is1394b = true;
      
      if(is1394b) {
	if(dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_800)
	   != DC1394_SUCCESS) {
	  if(dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_400) 
	     != DC1394_SUCCESS) {
	    fatal("%s # dc1394_video_set_iso_speed failed",
		  fname);
	  }
	}
      }
    }
    else if(dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_400) 
	    != DC1394_SUCCESS) {
      fatal("%s # dc1394_video_set_iso_speed failed",
	    fname);
    }

    if (dc1394_video_get_iso_speed(m_camera, &speed) != DC1394_SUCCESS) {
      fatal("%s # dc1394_video_get_iso_speed failed", fname);
    }
    else
      trace("%s # ISO speed = %u", fname, (uint) speed);
    
    if(dc1394_video_set_mode(m_camera, video_mode)
       != DC1394_SUCCESS) {
      fatal("%s # dc1394_video_set_mode failed",
	    fname);
    }

    if(dc1394_video_set_framerate(m_camera, fps) != DC1394_SUCCESS) {
      fatal("%s # dc1394_video_set_framerate failed",
	    fname);
    }  

    // If the camera is already running (eg. unclean exit), stop it
    dc1394switch_t isoWasOn;
    if(dc1394_video_get_transmission(m_camera, &isoWasOn) != DC1394_SUCCESS)     
      error("%s # dc1394_video_get_transmission failed", fname);
    
    if(isoWasOn == DC1394_ON) {
      trace("%s # Camera is already running, stopping it", fname);

      if(dc1394_video_set_transmission(m_camera, DC1394_OFF) !=DC1394_SUCCESS)
        error("%s # dc1394_video_set_transmission failed", fname);      
    }
  
    if(dc1394_capture_setup
#ifdef __linux__
       /* On Linux, only allocate channel, no bandwidth. This way you
	  can get more cameras into a single FW bus. */
       (m_camera, NUM_BUFFERS, DC1394_CAPTURE_FLAGS_CHANNEL_ALLOC)
#else
       // On Others, allocate both channels and bandwidth.
       (m_camera, NUM_BUFFERS, DC1394_CAPTURE_FLAGS_DEFAULT)
#endif
       != DC1394_SUCCESS) {
    
      error("%s # unable to setup camera- check that the video mode,"
	     "framerate and format are supported (%s)", fname, 
	     m_videodevice.c_str());
    }

    m_initialized = true;
    m_started = false;

    // set frame size
    if(video_mode == DC1394_VIDEO_MODE_640x480_YUV411) {
      m_image.m_format = IMAGE_YUV_411;
      m_image.m_planes[0].m_type = PLANE_YUV;
      m_image.m_planes[0].m_linesize = 640 + 640 / 2;
      m_image.m_width  = 640;
      m_image.m_height = 480;
    }
    else if(video_mode == DC1394_VIDEO_MODE_640x480_MONO8) {

      if(fmt == IMAGE_RAWBAYER)
	m_image.m_format = IMAGE_RAWBAYER;
      else 
	m_image.m_format = IMAGE_GRAYSCALE;

      m_image.m_planes[0].m_type = PLANE_GRAYSCALE;
      m_image.m_planes[0].m_linesize = 640;
      m_image.m_width  = 640;
      m_image.m_height = 480;
    }
    else if(video_mode == DC1394_VIDEO_MODE_1024x768_MONO8) {

      if(fmt == IMAGE_RAWBAYER)
	m_image.m_format = IMAGE_RAWBAYER;
      else 
	m_image.m_format = IMAGE_GRAYSCALE;

      m_image.m_format = IMAGE_GRAYSCALE;
      m_image.m_planes[0].m_type = PLANE_GRAYSCALE;
      m_image.m_planes[0].m_linesize = 1024;
      m_image.m_width  = 1024;
      m_image.m_height = 768;
    }
    else {
      m_initialized = false;
      error("%s # unsupported image format", fname);
    }

    trace("%s # EXIT OK with difmt = %d", fname, (int) m_image.m_format);

    return true;
  }

  bool Video1394::isInitialized() const
  {
    return m_initialized;
  }


  int Video1394::width() const
  {
    return m_image.m_width;
  }


  int Video1394::height() const
  {
    return m_image.m_height;
  }


  float Video1394::fps() const
  {
    return m_fps;
  }

  /**
   * Starts the camera data transmission. 
   */
  bool Video1394::start()
  {
    assert(isInitialized());

    assert(m_started == false);
  
    if (dc1394_video_set_transmission(m_camera, DC1394_ON) != DC1394_SUCCESS) {
      error("Video1394::start # unable to start camera iso transmission");

      return false;
    }
    else {
      dc1394switch_t trans;
      dc1394_video_get_transmission(m_camera, & trans);
      trace("Video1394::start # %d", (int) trans);
    }

    m_started = true;

    return true;
  }


  /**
   * Starts the camera data transmission. 
   */
  bool Video1394::stop()
  {
    assert(isInitialized());

    assert(m_started);
  
    dc1394_capture_stop(m_camera);
    if (dc1394_video_set_transmission(m_camera, DC1394_OFF) !=DC1394_SUCCESS) {
      error("Video1394::stop # unable to stop iso transmission");
    }

    m_started = false;

    return true;
  }

  void Video1394::queryCameras(std::vector<CameraInfo> * query)
  {
    if(!__dc)
      __dc = dc1394_new();

    const char * fname = "Video1394::queryCameras";

    uint32_t i, j;//, numCameras = 0;
    // dc1394camera_t **cameras = 0;
    dc1394error_t err;
    dc1394camera_list_t * camlist = 0;

    if((err = dc1394_camera_enumerate(__dc, & camlist))
       != DC1394_SUCCESS) {

      // if(err != DC1394_NO_CAMERA)
	
#ifdef __linux__      
      error("%s # dc1394_find_cameras failed (%s)\n"
	    "**********************************************\n"
	    "Please check that relevant device files exist:\n"
	    "/dev/raw1394\n"
	    "/dev/video (or /dev/video/0 etc)\n"
	    "And that you have permissions to use them\n"
	    "**********************************************\n\n",
	    fname, dc1394_error_get_string(err));
#else
      error("%s # dc1394_find_cameras failed (%s)\n",
	    fname, dc1394_error_get_string(err));
#endif
    }
  
    for(i = 0; i < camlist->num; i++) {
      bool already = false;

      for(j = 0; j < __infos.size(); j++)
	if(__infos[j]->guid == camlist->ids[i].guid)
	  already = true;
      
      if(!already)
	__infos.push_back(dc1394_camera_new(__dc, camlist->ids[i].guid));
    }

    query->clear();

    for(i = 0; i < __infos.size(); i++) {
      dc1394camera_t * c = __infos[i];
      CameraInfo ci;

      ci.m_euid64 = c->guid;
      ci.m_vendor = c->vendor;
      ci.m_model  = c->model;

      query->push_back(ci);
    }

    dc1394_camera_free_list(camlist);
  }

  bool Video1394::queryCamera(u_int64_t euid64, CameraInfo * camera)
  {
    std::vector<CameraInfo> query;

    queryCameras( & query);

    for(uint i = 0; i < query.size(); i++) {
      if((u_int64_t) query[i].m_euid64 == euid64) {
	*camera = query[i];
	return true;
      }
    }

    error("Video1394::queryCamera # FAILED");

    return false;
  }


  /**
   * Capture a camera frame.
   */
  const VideoImage * Video1394::captureImage()
  {
    // trace("Video1394::captureImage");

    assert(isInitialized());

    if (!m_started)
      start();

    m_frame = 0;
    int err;

    if((err = dc1394_capture_dequeue(m_camera,
				     DC1394_CAPTURE_POLICY_WAIT,
				     & m_frame))) {
      error("Video1394::captureImage # Unable to capture a frame!");
      close();
      return 0;
    }

    if(!m_frame)
      return 0;

    /* assert(m_frame->size[0] == (uint) m_image.m_width &&
       m_frame->size[1] == (uint) m_image.m_height); */

    m_image.m_planes[0].m_data = (uchar *) m_frame->image;

    // trace("Video1394::captureImage # EXIT");

    m_outside++;

    if(m_outside != 1) {
      error("Video1394::captureImage # Please release captured " 
            "frames with doneImage()");
    }

    return & m_image;
  }

  void Video1394::doneImage()
  {
    m_outside--;

    assert(m_outside == 0);

    dc1394_capture_enqueue(m_camera, m_frame);
    
    m_frame = 0;
  }

  /**
   * Shuts down the connection.
   */
  bool Video1394::close()
  {
    assert(isInitialized());

    if(!m_camera)
      return false;

    if (m_started)
      stop();
    
    // dc1394_release_camera(m_camera);
    // dc1394_free_camera(m_camera);

    m_initialized = false;
    m_camera = 0;

    return true;
  }

  Video1394::CameraInfo Video1394::cameraInfo()
  {
    CameraInfo info;

    if(m_camera) {
      info.m_euid64 = m_camera->guid;
      info.m_vendor = m_camera->vendor;
      info.m_model  = m_camera->model;
    }

    return info;
  }


  
}
