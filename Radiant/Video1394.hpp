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

#ifdef WIN32
#include <Radiant/Video1394cmu.hpp>
#else

#ifndef RADIANT_VIDEO_1394_HPP
#define RADIANT_VIDEO_1394_HPP

#include <Nimble/Rect.hpp>

#include <Radiant/VideoInput.hpp>

#include <dc1394/control.h>

#include <string>
#include <stdint.h>

#ifdef WIN32
#include <WinPort.h>
#endif

namespace Radiant {

  /// FireWire video capture
  /** This class works on top of the libdc1394 library.

      For more information on libdc1394, see:
      http://damien.douxchamps.net/ieee1394/libdc1394/ */
  class Video1394 : public VideoInput
  {
  public:
  
    Video1394();
    virtual ~Video1394();

    /** A container of basic camera information. #CameraInfo objects
	are used to store information about a particular camera. */
    class CameraInfo {
    public:
      /// The 64-bit unique FireWire identifier
      int64_t m_euid64;
      /// Vendor name, in a human-readable format
      std::string m_vendor;
      /// Camera model, in a human-readable format
      std::string m_model;
    };

    /// Get the list of cameras that are connected.
    /** @param cameras List of camera objects, which is filled by this
	function. */
    static bool queryCameras(std::vector<CameraInfo> * cameras);

    /// Gets information about a particular camera.

    /** @param euid64 The 64-bit unique camera identifier.
	@camera The camera information object which is updated with new value.
     */
    static bool queryCamera(uint64_t euid64, CameraInfo * camera);

    virtual const VideoImage * captureImage(int timeoutus = -1);
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
    static bool hasMode(const dc1394feature_info_t & feature,
				   dc1394feature_mode_t mode);
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

    /** Sets a given feature to relative value, in range 0-1.*/
    void setFeature1394(dc1394feature_t feature, float value);
    /** Sets a given feature to an absolute integer value. 
	
	The minimum/maximum range of the value depends on the
	parameter, and the camera model.
     */
    void setFeature1394Raw(dc1394feature_t feature, int32_t value);
    void getFeatures(std::vector<dc1394feature_info_t> *);
    static const char * featureName(dc1394feature_t);

    /// Sets the triggering mode for the camera
    bool enableTrigger(dc1394trigger_source_t source);
    bool setTriggerMode(dc1394trigger_mode_t mode);
    bool setTriggerPolarity(dc1394trigger_polarity_t polarity);
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

    /// Initializes the FireWire camera to format 7 mode
    virtual bool openFormat7(const char * cameraeuid,
			     Nimble::Recti roi,
			     float fps,
			     int mode = 0);
    
    /** Prints the Format-7 modes for a particular camera. */
    bool printFormat7Modes(const char * cameraeuid);

    virtual bool isInitialized() const;
    virtual bool start();
    virtual bool stop();
    virtual bool close();

    /// Sets the camera EUID that will be used to select the camera
    void setCameraEuid64(uint64_t euid) { m_euid = euid; }

    /// Returns information about this particular camera object
    CameraInfo cameraInfo();

    dc1394camera_t * dc1394Camera() { return m_camera; }

    /** Returns the number of frames that would be immediately readable.
	
	This function is not implemented on all platforms, so use it
	with some care.
     */
    int framesBehind() const { return m_framesBehind; }
 
  private:

    bool enableCameraFeature(unsigned int feature, 
			     const std::string & description, 
			     bool automatic_mode,
			     unsigned int * feature_min_value,
			     unsigned int * feature_max_value);

    bool findCamera(const char * euid);
    void captureSetup(int buffers);

    std::string    m_videodevice;

    /** camera capture information. */
    dc1394camera_t * m_camera;
    dc1394video_frame_t * m_frame;

    /** camera feature information. */
    dc1394featureset_t m_features;
    dc1394speed_t      m_speed; // FW400, FW800 etc.

    uint64_t m_euid;
    int m_cameraNum;

    int m_isoChannel;

    /// Numberof images the user is holding.
    int m_outside;
    int m_framesBehind;

    bool  m_initialized;
    bool  m_started;
    unsigned  m_fps;

    VideoImage m_image;
  };

}


#endif

#endif

