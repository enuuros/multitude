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

#ifndef RADIANT_VIDEO_1394_HPP
#define RADIANT_VIDEO_1394_HPP

#include <Radiant/VideoInput.hpp>

#include <dc1394/control.h>

#include <string>
#include <WinPort.h>

class C1394Camera;
#define _WINSOCKAPI_		// timeval redefinition
#include <windows.h>
#include <1394camapi.h>

namespace Radiant {

  /// FireWire video capture
  /** This class works on top of the CMU 1394 library.

      For more information on libdc1394, see:
      http://www-2.cs.cmu.edu/~iwan/1394/ */
class MTEXPORT Video1394 : public VideoInput
{
public:
  
    Video1394();
    virtual ~Video1394();

    class CameraInfo {
    public:
      int64_t m_euid64;
      std::string m_vendor;
      std::string m_model;
    };

    /// Get the list of cameras that are connected.
    static void queryCameras(std::vector<CameraInfo> * cameras);
    static bool queryCamera(u_int64_t euid64, CameraInfo * camera);

    virtual const VideoImage * captureImage();
    virtual void doneImage();

    /// The width of the video stream images.
    virtual int width() const;
    /// The height of the video stream images.
    virtual int height() const;
    /// The frame rate of the video stream.
    virtual float fps() const;
    /// Native image format of the stream.
    virtual ImageFormat imageFormat() const;  
    virtual unsigned int size() const;
    static bool hasMode(const dc1394feature_info_t & feature, dc1394feature_mode_t mode);
    static bool hasAutoMode(const dc1394feature_info_t & feature)
    { return hasMode(feature, DC1394_FEATURE_MODE_AUTO); }
    static bool hasManualMode(const dc1394feature_info_t & feature)
    { return hasMode(feature, DC1394_FEATURE_MODE_MANUAL); }

    virtual void setGamma(float value);
    virtual void setShutter(float value);
    virtual void setGain(float value);
    virtual void setExposure(float value);
    virtual void setBrightness(float value);
    virtual void setFocus(float value);
    virtual void setWhiteBalance(float u_to_blue, float v_to_red);
    void setFeature1394(dc1394feature_t feature, float value);
    void setFeature1394Raw(dc1394feature_t feature, int32_t value);
    void getFeatures(std::vector<dc1394feature_info_t> *);
    static const char * featureName(dc1394feature_t);

    /// Sets the triggering mode for the camera
    bool enableTrigger(dc1394trigger_source_t source);
    bool setTriggerMode(dc1394trigger_mode_t mode);
    bool disableTrigger();
    /// Sends a software trigger signal to the camera
    void sendSoftwareTrigger();
    
    /// Initializes the FireWire camera
    /** Some of the arguments are frankly ignored at the moment, for
	the example the device name (which is selected
	automatically). */
    virtual bool open(const char * device = 0, 
		      const char * camera = 0,
		      const char * euid = 0,
		      ImageFormat fmt = IMAGE_UNKNOWN, 
		      int width = DONT_CARE, 
		      int height = DONT_CARE, 
		      FrameRate framerate = FPS_IGNORE);
    
    virtual bool isInitialized() const;
    virtual bool start();
    virtual bool stop();
    virtual bool close();

    /// Sets the camera EUID that will be used to select the camera
    void setCameraEuid64(u_int64_t euid) { m_euid = euid; }

    CameraInfo cameraInfo();

    dc1394camera_t * dc1394Camera() { return m_camera; }
 
  protected:
	void cmu_setFeature1394(CAMERA_FEATURE feature, float value);

    bool enableCameraFeature(unsigned int feature, 
			     const std::string & description, 
			     bool automatic_mode,
			     unsigned int * feature_min_value,
			     unsigned int * feature_max_value);

    std::string    m_videodevice;

    /** camera capture information. */
    dc1394camera_t * m_camera;

	C1394Camera * cmu_camera;

    /** camera feature information. */
    dc1394featureset_t m_features;

    u_int64_t m_euid;
    int m_cameraNum;

    int m_isoChannel;

    bool  m_initialized;
    bool  m_started;
    unsigned  m_fps;

    VideoImage m_image;
  };

}


#endif
