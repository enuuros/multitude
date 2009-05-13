/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
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
#include <stdlib.h>
#include <string.h>


#include <iostream>
#include <set>

#include <dc1394/camera.h>

#ifndef WIN32
#include <sys/utsname.h>
#endif

#define NUM_BUFFERS 10

namespace Radiant {

  static std::vector<dc1394camera_t *> __infos;

  static std::set<int> __iso_channels;
  static std::set<int> __taken;

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
    m_framesBehind = 0;
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

    /*if(!__count) {
      dc1394_free(__dc);
      __dc = 0;
    }
    */
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

  dc1394error_t has_mode(dc1394camera_t * camera,
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
        Radiant::error("%s # no auto mode for white balance", fname);
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
        Radiant::error("%s # no manual mode for white balance", fname);
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
        Radiant::error("%s # no auto mode for %s", fname, name);
    }
    else {
      has_manual_mode(m_camera, feature, & b);

      if(b)
        dc1394_feature_set_mode(m_camera, feature, DC1394_FEATURE_MODE_MANUAL);
      else {
        Radiant::error("%s # no manual mode for %s", fname, name);
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
      Radiant::error("Video1394::enableTrigger # Could not turn trigger power on");
      return false;
    }

    if(dc1394_external_trigger_set_source(m_camera, source)
        != DC1394_SUCCESS) {
      Radiant::error("Video1394::enableTrigger # Could not set trigger source");
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

  bool Video1394::setTriggerPolarity(bool up)
  {
    debug("Video1394::setTriggerPolarity # %d", (int) up);

    dc1394trigger_polarity_t polarity = up ? 
      DC1394_TRIGGER_ACTIVE_HIGH : DC1394_TRIGGER_ACTIVE_LOW;

    dc1394error_t e = dc1394_external_trigger_set_polarity(m_camera, polarity);
    
    if(e != DC1394_SUCCESS) {
      error("Video1394::setTriggerPolarity # %s", dc1394_error_get_string(e));
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
  
  static MutexAuto __mutex;

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
    // Only one thread at a time, just to make things sure.
    Guard guard( & __mutex);

    m_videodevice = device ? device : "/dev/video1394";
    m_cameraNum = camera ? atoi(camera) : 0;
    m_framesBehind = 0;

    uint32_t i;

    m_outside = 0;

    const char * fname = "Video1394::initialize";

    if(!findCamera(euid))
      return false;

    /* else
       dc1394_print_feature_set(& m_features);
       */

    dc1394video_mode_t video_modes[] = {
      difmt2dcfmt(fmt, width, height),
      DC1394_VIDEO_MODE_1024x768_MONO8,
      DC1394_VIDEO_MODE_640x480_MONO8,
      DC1394_VIDEO_MODE_640x480_YUV411,
      (dc1394video_mode_t) 0
    };

    dc1394video_mode_t video_mode;

    // if(width >= 500)
    video_mode = DC1394_VIDEO_MODE_640x480_MONO8;
    // else
    // video_mode = DC1394_VIDEO_MODE_320x240_MONO8;

    dc1394framerates_t framerates;
    framerates.num = 0;

    for(i = 0; video_modes[i] != 0; i++) {
      video_mode = video_modes[i];
      if (dc1394_video_get_supported_framerates(m_camera, 
            video_mode, &framerates)
          != DC1394_SUCCESS) {
        Radiant::error("%s # dc1394_video_get_supported_framerates",
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

    debug("%s # The video mode id = %d", fname, (int) video_mode);
    debug("%s # The frame rate id = %d (target = %d)", 
        fname, (int) fps, targetfps);

    if(dc1394_video_set_mode(m_camera, video_mode)
        != DC1394_SUCCESS) {
      error("%s # dc1394_video_set_mode failed",
            fname);
      return false;
    }

    if(dc1394_video_set_framerate(m_camera, fps) != DC1394_SUCCESS) {
      error("%s # dc1394_video_set_framerate failed",
            fname);
      return false;
    }

    // If the camera is already running (eg. unclean exit), stop it
    dc1394switch_t isoWasOn;
    if(dc1394_video_get_transmission(m_camera, &isoWasOn) != DC1394_SUCCESS)     
      Radiant::error("%s # dc1394_video_get_transmission failed", fname);

    if(isoWasOn == DC1394_ON) {
      debug("%s # Camera is already running, stopping it", fname);

      if(dc1394_video_set_transmission(m_camera, DC1394_OFF) !=DC1394_SUCCESS)
        Radiant::error("%s # dc1394_video_set_transmission failed", fname);      
    }

    captureSetup(NUM_BUFFERS);

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
      Radiant::error("%s # unsupported image format", fname);
    }

    debug("%s # EXIT OK with difmt = %d", fname, (int) m_image.m_format);

    return true;
  }

  bool Video1394::openFormat7(const char * cameraeuid,
			      Nimble::Recti roi,
			      float fps,
			      int mode)
  {
    Guard guard( & __mutex);

    const char * fname = "Video1394::openFormat7";

    if(!findCamera(cameraeuid))
      return false;

    int err;
    unsigned minbytes, maxbytes;

    dc1394video_mode_t vmode = (dc1394video_mode_t)
      (DC1394_VIDEO_MODE_FORMAT7_0 + mode);
    
    err = dc1394_video_set_mode(m_camera, vmode);
    if(err != DC1394_SUCCESS) {
      Radiant::error("%s # Could not set mode to format7_0", fname);
      return false;
    }

    dc1394format7modeset_t modeset;

    bzero( & modeset, sizeof(modeset));

    err = dc1394_format7_get_modeset(m_camera, & modeset);
    
    uint32_t maxw = 0;
    uint32_t maxh = 0;
    
    err = dc1394_format7_get_max_image_size
      (m_camera, vmode, & maxw, & maxh);

    debug("%s # fps = %f", fname, fps);
    debug("%s # Maximum image size = %d x %d", fname, (int) maxw, (int) maxh);

    if(roi.high().x > (int) maxw)
      roi.high().x = maxw;

    if(roi.high().y > (int) maxh)
      roi.high().y = maxh;

    // Make the image size multiple of four...
    for(int i = 0; i < 2; i++) {
      while(roi.high()[i] & 0x3)
	roi.high()[i]--;
    }

    err = dc1394_format7_get_packet_parameters
      (m_camera, vmode, & minbytes, & maxbytes);

    if(err != DC1394_SUCCESS) {
      Radiant::error("%s # Could not get packet parameters", fname);
      return false;
    }

    /* Tricky to get the frame-rate right:

       http://damien.douxchamps.net/ieee1394/libdc1394/v2.x/faq/#How_can_I_work_out_the_packet_size_for_a_wanted_frame_rate
*/

    float busPeriod; // Bus period in seconds

    if(m_speed == DC1394_ISO_SPEED_400)
      busPeriod = 0.000125f;
    else if(m_speed == DC1394_ISO_SPEED_800)
      busPeriod = 0.0000625f;
    else {
      Radiant::error("%s # Cannot calculate bus speed as the speed (%d) is unknown",
          fname, (int) m_speed);
      return false;
    }

    int numPackets = (int) (1.0f / (busPeriod * fps));
    int denom = numPackets * 8;

    int packetSize = 2.01 * (roi.area() * 8 + denom - 1) / denom;

    if(packetSize > (int) maxbytes) {
      
      debug("%s # Limiting packet size to %u", fname, maxbytes);
      packetSize = maxbytes;
    }

    dc1394_format7_set_color_coding(m_camera,
				    vmode,
				    DC1394_COLOR_CODING_MONO8);

    err = dc1394_format7_set_roi(m_camera,
				 vmode,
				 DC1394_COLOR_CODING_MONO8,
				 packetSize,
				 roi.low().x, roi.low().y,
				 roi.width(), roi.height());

    if(err != DC1394_SUCCESS) {
      Radiant::error("%s # Could not set ROI", fname);
      return false;
    }

    captureSetup(NUM_BUFFERS);

    // Here we only support grayscale for the time being...
    m_image.m_format = IMAGE_GRAYSCALE;
    m_image.m_planes[0].m_type = PLANE_GRAYSCALE;
    m_image.m_planes[0].m_linesize = roi.width();
    m_image.m_width  = roi.width();
    m_image.m_height = roi.height();

    debug("%s # initialized format-7 mode with resolution %d x %d",
	  fname, m_image.m_width, m_image.m_height);

    m_initialized = true;

    return true;
  }

  bool Video1394::printFormat7Modes(const char * cameraeuid)
  {
    const char * fname = "Video1394::printFormat7Modes";

    if(!findCamera(cameraeuid))
      return false;

    int err;
    dc1394format7modeset_t modeset;

    bzero( & modeset, sizeof(modeset));

    err = dc1394_format7_get_modeset(m_camera, & modeset);
    
    if(err != DC1394_SUCCESS) {
      Radiant::error("%s # Could not get modeset", fname);
      close();
      return false;
    }

    CameraInfo ci(cameraInfo());
    
    info("Format 7 mode information for %s %s (id = %s)",
	 ci.m_vendor.c_str(), ci.m_model.c_str(), cameraeuid);
    
    for(int i = 0; i < DC1394_VIDEO_MODE_FORMAT7_NUM; i++) {

      dc1394format7mode_t & mode = modeset.mode[i];

      if(!mode.present) {
	info(" Format7 mode %d not present", i);
	continue;
      }
      
      info(" Format7 mode %d:", i);
      info("  size    = [%d %d]\n"
	   "  maxsize = [%d %d]\n"
	   "  pos     = [%d %d]",
	   mode.size_x, mode.size_y, 
	   mode.max_size_x, mode.max_size_y,
	   mode.pos_x, mode.pos_y);
      info("  unitsize    = [%d %d]\n"
	   "  unitpos     = [%d %d]",
	   mode.unit_size_x, mode.unit_size_y, 
	   mode.unit_pos_x, mode.unit_pos_y);
      info("  pixum    = %d",
	   mode.pixnum);
    }
    
    return close();
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

    if(m_started)
      return true;

    // assert(m_started == false);

    if (dc1394_video_set_transmission(m_camera, DC1394_ON) != DC1394_SUCCESS) {
      Radiant::error("Video1394::start # unable to start camera iso transmission");

      return false;
    }
    else {
      dc1394switch_t trans;
      dc1394_video_get_transmission(m_camera, & trans);
      debug("Video1394::start # %d", (int) trans);
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
      Radiant::error("Video1394::stop # unable to stop iso transmission");
    }

    m_started = false;

    return true;
  }

  bool Video1394::queryCameras(std::vector<CameraInfo> * query)
  {
    Guard guard( & __mutex);

    const char * fname = "Video1394::queryCameras";

    uint32_t i, j;//, numCameras = 0;
    // dc1394camera_t **cameras = 0;
    dc1394error_t err;
    dc1394camera_list_t * camlist = 0;

    if(!__dc)
      __dc = dc1394_new();

#ifndef __linux__
    // For OSX

    static bool first = true;

    if(!first) {
      goto fillquery;
    }

    first = false;
    
	
#endif

    


    if((err = dc1394_camera_enumerate(__dc, & camlist))
        != DC1394_SUCCESS) {

      // if(err != DC1394_NO_CAMERA)

#ifdef __linux__      
      const char * username = getenv("USERNAME");
      
      Radiant::error("%s # dc1394_find_cameras failed (%s)\n"
          "*************************************************************\n"
          "Please check that FireWire device files exist:\n"
          "/dev/raw1394\n"
          "/dev/video (or /dev/video/0 etc)\n"
          "And that you have permissions to use them.\n"
          "\n"
          "To gain permissions, try the following commands:\n\n"
          "> sudo addgroup %s video\n"
          "> sudo addgroup %s disk\n\n"
          "You may need to log in again for the changes to be effective.\n"
          "*************************************************************\n\n",
                     fname, dc1394_error_get_string(err), username, username);
#else
      Radiant::error("%s # dc1394_find_cameras failed (%s)\n",
          fname, dc1394_error_get_string(err));
#endif
      return false;
    }
    

    debug("Getting %d FireWire cameras", (int) camlist->num);

    __infos.clear();

    for(i = 0; i < camlist->num; i++) {
      bool already = false;

      for(j = 0; j < __infos.size(); j++)
        if(__infos[j]->guid == camlist->ids[i].guid)
          already = true;

      if(!already)
        __infos.push_back(dc1394_camera_new(__dc, camlist->ids[i].guid));
    }

    debug("Copying FireWire camera information to user", (int) camlist->num);

  fillquery:

    query->clear();

    for(i = 0; i < __infos.size(); i++) {
      dc1394camera_t * c = __infos[i];
      CameraInfo ci;

      if(!c) {
	error("NULL camera");
	continue;
      }

      if(!c->guid || !c->vendor || !c->model)
        continue;
      
      debug("Got camera %p: %s %s (%llx)", c, c->vendor, c->model, c->guid);

      ci.m_euid64 = c->guid;
      ci.m_vendor = c->vendor;
      ci.m_model  = c->model;

      query->push_back(ci);
    }

    debug("Clearing camera list");

#ifdef __linux__
    dc1394_camera_free_list(camlist);
#endif

    return true;
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

    Radiant::error("Video1394::queryCamera # FAILED");

    return false;
  }


  /**
   * Capture a camera frame.
   */
  const VideoImage * Video1394::captureImage(int timeoutus)
  {
    // trace("Video1394::captureImage");

    assert(isInitialized());

    if (!m_started)
      start();

    m_frame = 0;

    if(timeoutus > 0) {

      int fd = dc1394_capture_get_fileno(m_camera);
      fd_set fds;
      struct timeval tv;

      FD_ZERO(& fds);
      FD_SET(fd, & fds);

      tv.tv_sec = timeoutus / 1000000;
      tv.tv_usec = timeoutus % 1000000;

      select(fd + 1, &fds, 0, 0, &tv);

      if(FD_ISSET(fd, & fds))
        ;
      else {
        FD_ZERO( & fds);
        // error("Video1394::captureImage # no image available fd = %d r = %d", fd, r);
        return 0;
      }
      FD_ZERO( & fds);
    }

    int err = dc1394_capture_dequeue(m_camera,
                                     DC1394_CAPTURE_POLICY_WAIT, & m_frame);

    if(err ) {
      Radiant::error("Video1394::captureImage # Unable to capture a frame!");
      close();
      return 0;
    }

    if(!m_frame)
      return 0;

    /* assert(m_frame->size[0] == (uint) m_image.m_width &&
       m_frame->size[1] == (uint) m_image.m_height); */

    m_image.m_planes[0].m_data = (uchar *) m_frame->image;

    m_framesBehind = m_frame->frames_behind;

    // trace("Video1394::captureImage # EXIT");

    m_outside++;

    if(m_outside != 1) {
      Radiant::error("Video1394::captureImage # Please release captured " 
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
    // assert(isInitialized());

    if(!m_camera)
      return false;
    
    if (m_started)
      stop();

    if(m_camera) {
      dc1394_camera_free(m_camera);
    }

    m_initialized = false;
    m_camera = 0;

    std::set<int>::iterator it = __taken.find(m_cameraNum);
    if(it == __taken.end())
      error("Video1394::close # taken mismatch %d", (int) m_cameraNum);
    else
      __taken.erase(it);

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

  bool Video1394::findCamera(const char * euid)
  {
    const char * fname = "Video1394::findCamera";

    uint32_t i;

    bzero( & m_camera, sizeof(m_camera));

    m_euid = euid ? strtoll(euid, 0, 16) : m_euid;

    // if(euid != 0)
    debug("Video1394::findCamera # m_euid = %.8x%.8x (%s)", 
          (int) (m_euid >> 32), (int) m_euid, euid);

    if (m_initialized)
      close();

    {
      std::vector<CameraInfo> cameras;

      if(!queryCameras(&cameras)) return false;
    }

    if(!__infos.size()) {
      Radiant::error("%s # No FireWire cameras found", fname);
      return false;
    }

    bool isleopard = false;

#ifndef WIN32
    struct utsname sn;
    uname(& sn);

    // trace("System: %s # %s # %s", sn.sysname, sn.release, sn.version);
    isleopard = strcmp(sn.sysname, "Darwin") == 0 &&
      sn.release[0] == '9' && sn.release[1] == '.';
#endif

    // Clean up in the first start:
    static int initcount = 0;
    if(!initcount) {

      if(isleopard)
        debug("%s # Running Leopard, no FireWire bus reset", fname);
      else {
        for(int c = 0; c < (int) __infos.size(); c++) {
          dc1394_reset_bus(__infos[c]);
          Sleep::sleepMs(100);
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
        debug("%s # Got camera %d based on euid", fname, (int) i);
        break;
      }
    }

    if(m_euid != 0 && !foundCorrect) {
      debug("%s # Could not find the camera with euid = %llx",
	    fname, (long long) m_euid);
      return false;
    }

    if(__taken.find(m_cameraNum) != __taken.end()) {
      error("%s # Camera index %d is already taken", fname, (int) m_cameraNum);
    }

    __taken.insert(m_cameraNum);

    assert(m_cameraNum < (int) __infos.size());

    m_camera = __infos[m_cameraNum];

    debug("%s # Initializing camera %s \"%s\"",
	  fname, m_camera->vendor, m_camera->model);

    /* int isochan = m_cameraNum + 2;
       if(dc1394_video_specify_iso_channel(m_camera, isochan) !=DC1394_SUCCESS){
       error(ERR_UNKNOWN, "%s # unable to set ISO channel to %d", 
       fname, isochan);
       }
       */

    if(dc1394_feature_get_all(m_camera, & m_features)
        != DC1394_SUCCESS) {
     debug("%s # unable to get feature set %d", 
          fname, m_cameraNum);
    }

    bool try1394b = false;

    if(getenv("NO_FW800") != 0)
      try1394b = false;

    if(!m_camera->bmode_capable)
      try1394b = false;

    if(strstr(m_camera->model, "Firefly") &&
       strstr(m_camera->vendor, "Point Grey")) {
      /* PTGrey Firefly is a popular camera, but it apparently reports
	 itself as FW800 camera... */
      
      debug("PTGrey Firefly camera detected, going for FW400");
      try1394b = false;
    }

    debug("%s # Try %s FW800", fname, try1394b ? "with" : "without");

    if(try1394b) {
      bool is1394b = false;

      if(dc1394_video_set_operation_mode(m_camera, DC1394_OPERATION_MODE_1394B)
          != DC1394_SUCCESS) {
        dc1394_video_set_operation_mode(m_camera, DC1394_OPERATION_MODE_LEGACY);
	debug("%s # Could not set operation mode to 1394B", fname);
      }
      else
        is1394b = true;

      info("%s # is1394b = %d", fname, (int) is1394b);

      if(is1394b) {
        if(dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_800)
            != DC1394_SUCCESS) {
	  
	  debug("%s # Could not set ISO speed to 800", fname);

          if(dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_400) 
	     != DC1394_SUCCESS) {
            error("%s # dc1394_video_set_iso_speed 400 failed",
                  fname);
            return false;
          }
        }
      }
      else if(dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_400)
	      != DC1394_SUCCESS) {
	error("%s # dc1394_video_set_iso_speed 400 failed", fname);
	return false;
      }
    }

    if (dc1394_video_get_iso_speed(m_camera, &m_speed) != DC1394_SUCCESS) {
      error("%s # dc1394_video_get_iso_speed failed", fname);
      return false;
    }
    else
      debug("%s # ISO speed = %u", fname, (uint) m_speed);

    return true;
  }

  void Video1394::captureSetup(int buffers)
  {
    int flags = DC1394_CAPTURE_FLAGS_DEFAULT;

#ifdef __linux__
    if(getenv("WITHOUT_1394_BANDWIDTH_ALLOC")) {
      flags = DC1394_CAPTURE_FLAGS_CHANNEL_ALLOC;
      debug("Video1394::captureSetup # Ignoring bandwidth allocation");
    }
#endif
    dc1394error_t res = dc1394_capture_setup(m_camera, buffers, flags);
    if(res != DC1394_SUCCESS) {

      Radiant::error("Video1394::captureSetup # "
		     "unable to setup camera- check that the video mode,"
		     "framerate and format are supported (%s)", 
		     dc1394_error_get_string(res));
    }

  }

}
