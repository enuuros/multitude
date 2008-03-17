/* COPYRIGHT
 *
 * This file is part of Diva.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Diva.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef DIVA_VIDEO_INPUT_H
#define DIVA_VIDEO_INPUT_H

#include <Radiant/VideoImage.hpp>
#include <Radiant/IODefs.hpp>

#include <vector>


namespace Radiant {


  enum FrameRate {
    FPS_IGNORE,
    FPS_5,
    FPS_10,
    FPS_15,
    FPS_30,
    FPS_60,
    FPS_120,
    FPS_COUNT
  };

  enum {
    DONT_CARE = -1,
    WITH_VIDEO = 0x1,
    WITH_AUDIO = 0x2
  };

  float asFloat(FrameRate);
  FrameRate closestFrameRate(float fps);

  /// Base class for video input classes. 

  /** Potential child classes are: FireWire video input, USB video
      input and movie video input.

      @authors Tommi Ilmonen and Juha Laitinen
  */

  class VideoInput
  {
  public:


    virtual ~VideoInput();

    /** 
     * This method captures an image from the video source. The image
     * is returned in the native format of the device.
     * @see #imageFormat() for the current format of the device.
     * @see #Radiant::ImageConversion for conversions into application video formats 
     * @note The device has to be initialized, and the stream transmission has
     * to be started before this method can be called.
     * @see #open()
     * @see #start()
     */
    virtual const Radiant::VideoImage * captureImage() = 0;
    /// Inform the video handler that the application has used the image
    /** This function is necessary as some handlers need to release
	the resources that relate to a particular frame. */
    virtual void doneImage();

    /// Get audio data
    virtual const void * captureAudio(int * frameCount);
    /// Get audio parameters
    /** @channels The number of channels in the video sound-track. 
	@sample_rate Audio sample rate
	@format The audio sample format
     */
    virtual void getAudioParameters(int * channels, 
				    int * sample_rate,
				    AudioSampleFormat * format);

    /// Returns the current width of a frame in the video stream images.
    virtual int width() const = 0;
    /// Returns the current height of a frame in the video stream images.
    virtual int height() const = 0;
    /** Returns the current frame rate (int frames per second) of the
	video stream. */
    virtual float fps() const = 0;
    /// Returns the current image format of the stream.
    virtual ImageFormat imageFormat() const = 0;
    /// Returns the total size of one captured image frame in bytes.
    virtual unsigned int size() const = 0;

    virtual void setGamma(float);
    /** Sets the shutter time. Larger values lead to longer shutter
	times. Negative values tell the system to use automatic shutter
	timing. Manual range [0-1]. */
    virtual void setShutter(float);
    /** Sets the camera gain (if possible). Negative values tell the
	system to use automatic gain control. Manual range [0-1]. */
    virtual void setGain(float value);
    /// Set the exposure control of the device (if possible)
    virtual void setExposure(float value);
    /// Set the brightness control of the device (if possible)
    virtual void setBrightness(float value);

    /** 
     * Starts the data transmission.
     */
    virtual bool start() = 0;

    /** 
     * Stops the data transmission.
     */
    virtual bool stop() = 0;
  
    /** 
     * Close the device.
     */
    virtual bool close() = 0;
  
  protected:
    /// Disabled
    VideoInput() {}

  };

}

#endif


