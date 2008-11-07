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

#include <Radiant/Trace.hpp>

#include "Video1394cmu.hpp"
#include "cmu_dc1394.hpp"


#include <Radiant/Mutex.hpp>
#include <Radiant/Sleep.hpp>
#include <Radiant/Types.hpp>

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <iostream>
#include <set>

#include <1394Camera.h>


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

	static cmu_dc1394_t* __dcs = 0;
	static int __count = 0;

Video1394::Video1394() : m_camera(0)
{
    m_initialized = false;
    m_euid = 0;
    m_cameraNum = 0;
    m_started = false;

    if (!__dcs)
      __dcs = cmu1394_new();

    __count++;
  }

  Video1394::~Video1394()
  {
    if (m_initialized)
      close();

    __count--;
    if(!__count)
      cmu1394_free(__dcs, __infos);
  }


// ---------------------------------------------------------------------------
//
ImageFormat Video1394::imageFormat() const
{
	return m_image.m_format;
}

// ---------------------------------------------------------------------------
//
unsigned int Video1394::size() const
{
	return m_image.size();
}

// ---------------------------------------------------------------------------
//
bool Video1394::hasMode(const dc1394feature_info_t & feature, dc1394feature_mode_t mode)
{
	for (uint i = 0; i < feature.modes.num; i++)
		if (feature.modes.modes[i] == mode)
			return true;
	return false;
}

// ---------------------------------------------------------------------------
//
dc1394error_t has_mode(C1394Camera *cmu_camera, dc1394feature_t feature, dc1394feature_mode_t mode, dc1394bool_t * val)
{
	assert(val);
	assert(cmu_camera);
	CAMERA_FEATURE cmu_feature = cmu_dc1394_convertFeatureID(feature);
	C1394CameraControl * pcc = cmu_camera->GetCameraControl(cmu_feature);
	if (!pcc) return DC1394_INVALID_FEATURE;

	dc1394bool_t hasit = DC1394_FALSE;
	if ((mode == DC1394_FEATURE_MODE_MANUAL) && pcc->HasManualMode())				hasit = DC1394_TRUE;
	else if ((mode == DC1394_FEATURE_MODE_AUTO) && pcc->HasAutoMode())			hasit = DC1394_TRUE;
	else if ((mode == DC1394_FEATURE_MODE_ONE_PUSH_AUTO) && pcc->HasOnePush())	hasit = DC1394_TRUE;

	*val = hasit;
	return DC1394_SUCCESS;
}

// ---------------------------------------------------------------------------
// 
bool cmu_has_auto_mode(C1394Camera *camera, CAMERA_FEATURE feature)
{
	C1394CameraControl *pControl = camera->GetCameraControl(feature);
	if (pControl)	return pControl->HasAutoMode();
	else				return false;
}

// ---------------------------------------------------------------------------
// 
bool cmu_has_manual_mode(C1394Camera *camera, CAMERA_FEATURE feature)
{
	C1394CameraControl *pControl = camera->GetCameraControl(feature);
	if (pControl)	return pControl->HasManualMode();
	else				return false;
}

// ---------------------------------------------------------------------------
// 
void Video1394::setGamma(float value)
{
	cmu_setFeature1394(FEATURE_GAMMA, value);
}

// ---------------------------------------------------------------------------
// 
void Video1394::setShutter(float value)
{
	cmu_setFeature1394(FEATURE_SHUTTER, value);
}

// ---------------------------------------------------------------------------
// 
void Video1394::setGain(float value)
{
	cmu_setFeature1394(FEATURE_GAIN, value);
}

// ---------------------------------------------------------------------------
// 
void Video1394::setExposure(float value)
{
	cmu_setFeature1394(FEATURE_AUTO_EXPOSURE, value);
}

// ---------------------------------------------------------------------------
// 
void Video1394::setBrightness(float value)
{
	cmu_setFeature1394(FEATURE_BRIGHTNESS, value);
}

// ---------------------------------------------------------------------------
// 
void Video1394::setFocus(float value)
{
	cmu_setFeature1394(FEATURE_FOCUS, value);
}


// ---------------------------------------------------------------------------
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
	assert(cmu_camera);
	const char * fname = "Video1394::setWhiteBalance";

	C1394CameraControl * pcc = cmu_camera->GetCameraControl(FEATURE_WHITE_BALANCE);
	if (!pcc) return;

	// -- automatic
	if (u_to_blue < 0 || v_to_red < 0)
	{
		if (pcc->HasAutoMode())		pcc->SetAutoMode(TRUE);
		else								{ error("%s # no auto mode for white balance", fname); return; }
		if (u_to_blue < 0)	u_to_blue = -u_to_blue;
		if (v_to_red  < 0)	v_to_red  = -v_to_red;
	}
	else
	{
		if (pcc->HasManualMode())	pcc->SetAutoMode(FALSE);
		else								{ error("%s # no manual mode for white balance", fname); return; }
	}

	unsigned short low  = 0;
	unsigned short high = 0;
	unsigned short ublue,vred;

	pcc->GetRange(&low, &high);
	if(low != high)
	{
		float s = float(high - low);
		ublue = (unsigned short) (s * u_to_blue + low);
		vred  = (unsigned short) (s * v_to_red  + low);
		if (ublue > high)	ublue = high;
		if(vred > high)	vred = high;
	}
	else
	{
		ublue = (unsigned short) round(u_to_blue);
		vred  = (unsigned short) round(v_to_red);
	}

	pcc->SetValue(ublue, vred);
}

// ---------------------------------------------------------------------------
/** 
* Set a camera feature (=control parameter) to some value. Possible
* features can be found from dc1394_control.h. If the "value" is less
* than zero, then the camera will be switched to automatic control of
* the feature. 
*/
void Video1394::setFeature1394(dc1394feature_t feature, float value)
{
	// const char * fname = "Video1394::setFeature1394";
	CAMERA_FEATURE cmu_feature = cmu_dc1394_convertFeatureID(feature);
	cmu_setFeature1394(cmu_feature, value);
}

// ---------------------------------------------------------------------------
//
void Video1394::cmu_setFeature1394(CAMERA_FEATURE feature, float value)
{
	assert(cmu_camera);
	const char * fname = "Video1394::cmu_setFeature1394";

	C1394CameraControl * pcc = cmu_camera->GetCameraControl(feature);
	if (!pcc) return;

	const char * name = cmu_dc1394_feature_get_string(feature);
	// trace2("%s # %s %f", fname, name, value);

	bool automatic = value < 0.0f;
	if (automatic)
	{
		if (pcc->HasAutoMode())		pcc->SetAutoMode(TRUE);
		else								{ error("%s # no auto mode for %s", fname, name); return; }
		value = -value;
	}
	else
	{
		if (pcc->HasManualMode())	pcc->SetAutoMode(FALSE);
		else								{ error("%s # no manual mode for %s", fname, name); return; }
	}

	unsigned short low  = 0;
	unsigned short high = 0;

	pcc->GetRange(&low, &high);
	if (low != high)
	{
		value = (high - low) * value + low;
		if (value > high)
			value = high;
	}

	pcc->SetValue(unsigned short(value));
}

// ---------------------------------------------------------------------------
//
void Video1394::setFeature1394Raw(dc1394feature_t feature, int32_t value)
{
	// const char * fname = "Video1394::setFeature1394";
	CAMERA_FEATURE cmu_feature = cmu_dc1394_convertFeatureID(feature);
	cmu_setFeature1394(cmu_feature, float(value));

	assert(cmu_camera);
	C1394CameraControl * pcc = cmu_camera->GetCameraControl(cmu_feature);
	if (pcc)
	{
		pcc->SetAutoMode(FALSE);
		pcc->SetValue(unsigned short(value));
	}
}

// ---------------------------------------------------------------------------
//
void Video1394::getFeatures(std::vector<dc1394feature_info_t> * feats)
{
	assert(isInitialized());
	assert(cmu_camera);
	cmu_camera->RefreshControlRegisters(TRUE);

	feats->resize(DC1394_FEATURE_NUM);
	// int dc1394Feature = 0;
	for (int i = 0; i < FEATURE_NUM_FEATURES; i++)
	{
		CAMERA_FEATURE cmu_feature = (CAMERA_FEATURE)i;
		dc1394feature_t dc1394Feature = cmu_dc1394_convertFeatureID(cmu_feature);
		if (dc1394Feature <= DC1394_FEATURE_MAX)
		{
			int iFeature = (int)dc1394Feature - (int)DC1394_FEATURE_MIN;
			dc1394feature_info_t info = (*feats)[iFeature];
			bzero(&info, sizeof(dc1394feature_info_t));
			info.id = dc1394Feature;
			C1394CameraControl * pcc = cmu_camera->GetCameraControl(cmu_feature);
			if (pcc)
			{
				info.available = pcc->HasPresence() ? DC1394_TRUE : DC1394_FALSE;
				info.absolute_capable = pcc->HasAbsControl() ? DC1394_TRUE : DC1394_FALSE;
				info.readout_capable = pcc->HasReadout() ? DC1394_TRUE : DC1394_FALSE;
				info.on_off_capable = pcc->HasOnOff() ? DC1394_TRUE : DC1394_FALSE;
				info.is_on = pcc->StatusOnOff() ? DC1394_ON : DC1394_OFF;
				if (pcc->StatusOnePush())	info.current_mode = DC1394_FEATURE_MODE_ONE_PUSH_AUTO;
				else info.current_mode = pcc->StatusAutoMode() ? DC1394_FEATURE_MODE_ONE_PUSH_AUTO : DC1394_FEATURE_MODE_MANUAL;
				info.modes.num = 0;
				if (pcc->HasManualMode())	info.modes.modes[info.modes.num++] = DC1394_FEATURE_MODE_MANUAL;
				if (pcc->HasAutoMode())		info.modes.modes[info.modes.num++] = DC1394_FEATURE_MODE_AUTO;
				if (pcc->HasOnePush())		info.modes.modes[info.modes.num++] = DC1394_FEATURE_MODE_ONE_PUSH_AUTO;
				unsigned short min,max;
				pcc->GetRange(&min, &max);
				info.min = (uint32_t)min;
				info.max = (uint32_t)max;
				unsigned short v_lo,v_hi;
				pcc->GetValue(&v_lo, &v_hi);
				info.value = (uint32_t)v_lo;
				info.BU_value = (uint32_t)v_lo;
				info.RV_value = (uint32_t)v_hi;
				// ?? uint32_t           B_value;
				// ?? uint32_t           R_value;
				// ?? uint32_t           G_value;
				// ?? uint32_t           target_value;
				info.abs_control = pcc->StatusAbsControl() ? DC1394_ON : DC1394_OFF;
				pcc->GetValueAbsolute(&info.abs_value);
				pcc->GetRangeAbsolute(&info.abs_max, &info.abs_max);

				C1394CameraControlTrigger * pcct = cmu_camera->GetCameraControlTrigger();
				if (pcct)
				{
					info.polarity_capable = pcct->HasPolarity() ? DC1394_TRUE : DC1394_FALSE;
					info.trigger_modes.num = 0;
					unsigned short uMode;
					for (int uMode = 0; uMode <= 5; uMode++)
						if (pcct->HasMode(unsigned short(uMode)))
							info.trigger_modes.modes[info.trigger_modes.num++] = (dc1394trigger_mode_t)(DC1394_TRIGGER_MODE_0 + uMode);
					if (pcct->HasMode(14))	info.trigger_modes.modes[info.trigger_modes.num++] = DC1394_TRIGGER_MODE_14;
					if (pcct->HasMode(15))	info.trigger_modes.modes[info.trigger_modes.num++] = DC1394_TRIGGER_MODE_15;
					pcct->GetMode(&uMode, 0);
					if (uMode <= 5)			info.trigger_mode = (dc1394trigger_mode_t)(DC1394_TRIGGER_MODE_0 + uMode);
					else if (uMode == 14)	info.trigger_mode = DC1394_TRIGGER_MODE_14;
					else if (uMode == 15)	info.trigger_mode = DC1394_TRIGGER_MODE_15;

					// ?? dc1394trigger_polarity_t trigger_polarity;

					info.trigger_sources.num = 0;
					unsigned short uSource;
					for (uSource = 0; uSource <= 3; uSource++)
						if (pcct->HasTriggerSource(uSource))
							info.trigger_sources.sources[info.trigger_sources.num++] = (dc1394trigger_source_t)(DC1394_TRIGGER_SOURCE_0 + uSource);
					if (pcct->HasSoftwareTrigger())
						info.trigger_sources.sources[info.trigger_sources.num++] = DC1394_TRIGGER_SOURCE_SOFTWARE;
					pcct->GetTriggerSource(&uSource);
					if (uSource <= 3)			info.trigger_source = (dc1394trigger_source_t)(DC1394_TRIGGER_SOURCE_0 + uSource);
					else if (uSource == 7)	info.trigger_source = DC1394_TRIGGER_SOURCE_SOFTWARE;
				}
				(*feats)[iFeature] = info;
			}
		}
	}
}

// ---------------------------------------------------------------------------
//
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
	if (index < 0 || index >= DC1394_FEATURE_NUM)
		return 0;
	return names[index];
}

// ---------------------------------------------------------------------------
//
bool Video1394::enableTrigger(dc1394trigger_source_t source)
{
	assert(cmu_camera);
	C1394CameraControlTrigger * pcct = cmu_camera->GetCameraControlTrigger();
	if (pcct)
	{
		/* ?? if(dc1394_external_trigger_set_power(m_camera, DC1394_ON) != DC1394_SUCCESS) {
			error("Video1394::enableTrigger # Could not turn trigger power on");
			return false;
			} */

		unsigned short src = unsigned short(source - DC1394_TRIGGER_SOURCE_MIN);
		if (source == DC1394_TRIGGER_SOURCE_SOFTWARE)
			src = 7;
		if (pcct->SetTriggerSource(src) == CAM_SUCCESS)
			return true;
		else
			error("Video1394::enableTrigger # Could not set trigger source");
	}
	return false;
}
  
// ---------------------------------------------------------------------------
//
bool Video1394::setTriggerMode(dc1394trigger_mode_t mode)
{
	assert(cmu_camera);
	C1394CameraControlTrigger * pcct = cmu_camera->GetCameraControlTrigger();
	if (pcct)
	{
		unsigned short uMode = (unsigned short)mode - DC1394_TRIGGER_MODE_0;
		if (uMode == 6)		uMode = 14;
		else if (uMode == 7)	uMode = 15;
		if (pcct->SetMode(uMode) == CAM_SUCCESS)
			return true;
		else
			error("Video1394::setTriggerMode # Could not set trigger mode");
	}
	return false;
}

// ---------------------------------------------------------------------------
//
bool Video1394::disableTrigger()
{
	/* ?? assert(m_camera != 0);
	return dc1394_external_trigger_set_power(m_camera, DC1394_OFF) == DC1394_SUCCESS; */
	return false;
}

void checkCamError(const char * what, int err) {
	switch(err) {
			case CAM_SUCCESS:
				trace(Radiant::DEBUG, "%s : success", what);
				break;
			case CAM_ERROR_NOT_INITIALIZED:
				error("%s : no camera selected/camera not initialized", what);
				break;
			case CAM_ERROR_BUSY:
				error("%s : camera is busy", what);
				break;
			case CAM_ERROR:
				error("%s : camera error", what);
				break;
			case CAM_ERROR_INVALID_VIDEO_SETTINGS:
				error("%s : invalid video settings", what);
				break;
			case CAM_ERROR_INSUFFICIENT_RESOURCES:
				error("%s : insufficient resources", what);
				break;
			case ERROR_OUTOFMEMORY:
				error("%s : out of memory", what);
				break;
			default:
				error("%s : unknown error (%d)", what, err);
				break;
	};
}

// ---------------------------------------------------------------------------
//
void Video1394::sendSoftwareTrigger()
{
	assert(cmu_camera);
	C1394CameraControlTrigger * pcct = cmu_camera->GetCameraControlTrigger();
	if (pcct)
		pcct->DoSoftwareTrigger();
}

	/** --------------------------------------------------------------------------
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
		Guard guard(&mutex);

		width = width;
		height = height;
		framerate = framerate;

		m_videodevice = device ? device : "/dev/video1394";		// needed ?? 
		m_cameraNum = camera ? atoi(camera) : 0;
		m_euid = euid ? strtoll(euid, 0, 16) : m_euid;

		if (euid != 0)
			trace(Radiant::DEBUG, "Video1394::open # %.8x%.8x (%s)", (int) (m_euid >> 32), (int) m_euid, euid);

		const char * fname = "Video1394::initialize";

		// uint channel;
		// uint speed;

		//JJK bzero( & m_camera, sizeof(m_camera));
		// bzero( & m_features, sizeof(m_features));

		if (m_initialized)
			close();

		std::vector<CameraInfo> cameras;
		queryCameras(&cameras);  
		if (!__infos.size())
		{
			error("%s # No FireWire cameras found", fname);
			return false;
		}

#ifndef WIN32
		struct utsname sn;
		uname( & sn);

		// trace("System: %s # %s # %s", sn.sysname, sn.release, sn.version);
		bool isleopard = strcmp(sn.sysname, "Darwin") == 0 &&
			sn.release[0] == '9' && sn.release[1] == '.';
#endif

		// Clean up in the first start:
		static int initcount = 0;
#ifdef JJK_TODO
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
#endif
		initcount++;

		// Now seek the camera we are interested in:
		uint32_t i;
		bool foundCorrect = false;

		for (i = 0; i < __infos.size() && m_euid != 0; i++)
		{
			if (__infos[i]->guid == m_euid)
			{
				m_cameraNum = (int) i;
				foundCorrect = true;
				trace(Radiant::DEBUG, "%s # Got camera %d based on euid", fname, (int) i);
				break;
			}
		}

		if (m_euid != 0 && !foundCorrect)
		{
			error("%s # Could not find the camera with euid = %llx", (long long) m_euid);
			return false;
		}

		assert(m_cameraNum < (int) __infos.size());

		//JJK m_camera = __infos[m_cameraNum];
		cmu_camera = new C1394Camera;
		cmu_camera->CheckLink();
		cmu_camera->SelectCamera(m_cameraNum);
		cmu_camera->InitCamera(TRUE);
		m_initialized = true;

	/* int isochan = m_cameraNum + 2;
		if(dc1394_video_specify_iso_channel(m_camera, isochan) !=DC1394_SUCCESS){
		error(ERR_UNKNOWN, "%s # unable to set ISO channel to %d", 
		fname, isochan);
		}
	*/

		if (!cmu_dc1394_feature_get_all(this, &m_features))
			error("%s # unable to get feature set %d", fname, m_cameraNum);
		// else
			// dc1394_print_feature_set(& m_features);

	/*	dc1394speed_t speed;
		if(dc1394_video_get_iso_channel_and_speed(m_camera, &channel, &speed) 
		!= DC1394_SUCCESS) 
		fatal(ERR_UNKNOWN, 
		"%s # unable to get the iso channel number", fname);
	*/
  
		dc1394video_mode_t video_mode = DC1394_VIDEO_MODE_640x480_MONO8;
		// dc1394video_mode_t video_mode = DC1394_VIDEO_MODE_320x240_YUV422;
  
#ifdef JJK_TODO
		dc1394video_mode_t video_modes[] =
		{
			difmt2dcfmt(fmt, width, height),
			DC1394_VIDEO_MODE_1024x768_MONO8,
			DC1394_VIDEO_MODE_640x480_MONO8,
			DC1394_VIDEO_MODE_640x480_YUV411,
			(dc1394video_mode_t) 0
		};

		dc1394framerates_t framerates;
		//BOOL HasVideoFrameRate(unsigned	long format, unsigned long mode, unsigned long rate);

		for (i = 0; video_modes[i] != 0; i++)
		{
			video_mode = video_modes[i];
			if (dc1394_video_get_supported_framerates(m_camera, video_mode, &framerates) != DC1394_SUCCESS)
			{
				fatal("%s # dc1394_video_get_supported_framerates", fname);
				// cleanup_and_exit(m_camera);
			}
			if (framerates.num != 0)
				break;
		}

		assert(framerates.num);

		int targetfps = diFPS2dcFPS(framerate);
		dc1394framerate_t fps = (dc1394framerate_t) targetfps;
  
		for(i = 0; i < framerates.num; i++)
		{
			fps = framerates.framerates[i];
			if (fps == targetfps)
			break;
		}
  
		trace("%s # The video mode id = %d", fname, (int) video_mode);
		trace("%s # The frame rate id = %d (target = %d)", fname, (int) fps, targetfps);
    
#ifdef __linux__
		// controlled with environment variable:
		bool try1394b = getenv("NO_FW800") == 0;
		trace("%s # Try %s FW800", fname, try1394b ? "with" : "without");
#else
		bool try1394b = true;
#endif

		if(try1394b)
		{
			bool is1394b = false;      
			if (dc1394_video_set_operation_mode(m_camera, DC1394_OPERATION_MODE_1394B) != DC1394_SUCCESS)
				dc1394_video_set_operation_mode(m_camera, DC1394_OPERATION_MODE_LEGACY);
			else
				is1394b = true;
      
			if(is1394b) {
				if(dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_800) != DC1394_SUCCESS) {
					if(dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_400) != DC1394_SUCCESS) {
						fatal("%s # dc1394_video_set_iso_speed failed", fname);
					}
				}
			}
		}
		else if(dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_400) != DC1394_SUCCESS)
			fatal("%s # dc1394_video_set_iso_speed failed", fname);

		if (dc1394_video_get_iso_speed(m_camera, &speed) != DC1394_SUCCESS)
			fatal("%s # dc1394_video_get_iso_speed failed", fname);
		else
			trace("%s # ISO speed = %u", fname, (uint) speed);
    
		if (cmu_dc1394_video_set_mode(cmu_camera, video_mode) != DC1394_SUCCESS)
			fatal("%s # dc1394_video_set_mode failed", fname);

		unsigned long rate = fps - DC1394_FRAMERATE_MIN;
		if (cmu_camera->SetVideoFrameRate(rate) != CAM_SUCCESS)
			fatal("%s # dc1394_video_set_framerate failed", fname);

		// If the camera is already running (eg. unclean exit), stop it
		dc1394switch_t isoWasOn;
		if(dc1394_video_get_transmission(m_camera, &isoWasOn) != DC1394_SUCCESS)     
			error("%s # dc1394_video_get_transmission failed", fname);
    
		if(isoWasOn == DC1394_ON)
		{
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
#endif

		m_initialized = true;
		m_started = false;

		// set frame size
		if (video_mode == DC1394_VIDEO_MODE_640x480_YUV411)
		{
			m_image.m_format = IMAGE_YUV_411;
			m_image.m_planes[0].m_type = PLANE_YUV;
			m_image.m_planes[0].m_linesize = 640 + 640 / 2;
			m_image.m_width  = 640;
			m_image.m_height = 480;
		}
		else if(video_mode == DC1394_VIDEO_MODE_640x480_MONO8)
		{
			if(fmt == IMAGE_RAWBAYER)	m_image.m_format = IMAGE_RAWBAYER;
			else								m_image.m_format = IMAGE_GRAYSCALE;
			m_image.m_planes[0].m_type = PLANE_GRAYSCALE;
			m_image.m_planes[0].m_linesize = 640;
			m_image.m_width  = 640;
			m_image.m_height = 480;
		}
		else if(video_mode == DC1394_VIDEO_MODE_1024x768_MONO8)
		{
			if(fmt == IMAGE_RAWBAYER)	m_image.m_format = IMAGE_RAWBAYER;
			else								m_image.m_format = IMAGE_GRAYSCALE;
			m_image.m_format = IMAGE_GRAYSCALE;
			m_image.m_planes[0].m_type = PLANE_GRAYSCALE;
			m_image.m_planes[0].m_linesize = 1024;
			m_image.m_width  = 1024;
			m_image.m_height = 768;
		}
		else
		{
			m_initialized = false;
			error("%s # unsupported image format", fname);
		}

		int mbps = cmu_camera->GetMaxSpeed();
		trace(Radiant::DEBUG, "CAMERA MAX SPEED %d", mbps);
		
		int err = cmu_camera->SetVideoFormat(0);
		checkCamError("setVideoFormat", err);

		err = cmu_camera->SetVideoMode(5);
		checkCamError("setVideoMode", err);

		err = cmu_camera->SetVideoFrameRate(5);
		checkCamError("setVideoFrameRate", err);

		trace(Radiant::DEBUG, "%s # EXIT OK with difmt = %d", fname, (int) m_image.m_format);
		return true;
	}

	// ---------------------------------------------------------------------------
	//
	bool Video1394::isInitialized() const	{ return m_initialized;	}
	int Video1394::width() const				{ return m_image.m_width; }
	int Video1394::height() const				{ return m_image.m_height; }
	float Video1394::fps() const				{ return float(m_fps); }

	/** -------------------------------------------------------------------------
	* Starts the camera data transmission. 
	*/
	bool Video1394::start()
	{
		assert(isInitialized());
		assert(m_started == false);
		assert(cmu_camera);
	
		//unsigned long flags = ACQ_START_VIDEO_STREAM | ACQ_SUBSCRIBE_ONLY /*| ACQ_ALLOW_PGR_DUAL_PACKET*/;
		//int status = cmu_camera->StartImageAcquisitionEx(1, 0, flags);
		//int status = cmu_camera->StartImageCapture();
		
		//int status = cmu_camera->StartImageAcquisitionEx(6, 1000, ACQ_START_VIDEO_STREAM);
		int status = cmu_camera->StartImageAcquisition();
		if (status != CAM_SUCCESS)
		{
			error("Video1394::start # unable to start camera iso transmission");
			return false;
		}
		/* JJK commented out tracing
		else
		{
			dc1394switch_t trans;
			dc1394_video_get_transmission(m_camera, & trans);
			trace("Video1394::start # %d", (int) trans);
		} */

		m_started = true;
		return true;
	}

	/** -----------------------------------------------------------------------
	* Stops the camera data transmission. 
	*/
	bool Video1394::stop()
	{
		assert(isInitialized());
		assert(m_started);
		assert(cmu_camera);
	  
		//if (cmu_camera->StopImageCapture() != CAM_SUCCESS)
		if (cmu_camera->StopImageAcquisition() != CAM_SUCCESS)
		{
			error("Video1394::stop # unable to stop iso transmission");
			return false;
		}

		m_started = false;
		return true;
	}

	// ----------------------------------------------------------------------------
	// Capture a camera frame.
	//
	const VideoImage* Video1394::captureImage()
	{
		// trace("Video1394::captureImage");
		assert(isInitialized());
		assert(cmu_camera);

		if (!m_started)
			start();
/*
		DWORD ret = 0;
		do {
		HANDLE hFrameEvent = cmu_camera->GetFrameEvent();
		if(hFrameEvent == NULL) {
			trace("MEEP");
			if(cmu_camera->AcquireImageEx(FALSE, NULL) != CAM_ERROR_FRAME_TIMEOUT) {
				error("Video1394::captureImage # Meh, AcquireImageEx failed.");
			}
			hFrameEvent = cmu_camera->GetFrameEvent();
		}

		ret = MsgWaitForMultipleObjects(1, &hFrameEvent, FALSE, 1000, QS_ALLINPUT);
		switch(ret) {
			case WAIT_OBJECT_0:
				{
				trace("WAIT_OBJECT_0");
				int rc = cmu_camera->AcquireImageEx(FALSE, NULL);
				switch(rc) {
					case CAM_SUCCESS:
						trace("Whee!");
						break;
					case CAM_ERROR_NOT_INITIALIZED:
						error("Camera not initialized");
						return 0;
						break;
					default:
						DWORD err = GetLastError();
						error("WAT? System error code: %d", err);
						break;
				};
				}
				break;
			case WAIT_OBJECT_0 + 1:
				trace("WAIT_OBJECT_1");
				break;
			case WAIT_TIMEOUT:
				trace("WAIT_TIMEOUT");
				break;
		};
		} while(ret == WAIT_OBJECT_0);
*/

		//int status = cmu_camera->CaptureImage();
		//int status = cmu_camera->AcquireImageEx(TRUE, NULL);
		int status = cmu_camera->AcquireImage();
		//checkCamError("captureImage", status);
		if (status != CAM_SUCCESS)
		{
			error("Video1394::captureImage # Unable to capture a frame!");
			close();
			return 0;
		}

		
		unsigned long len = 0;
		m_image.m_planes[0].m_data = cmu_camera->GetRawData(&len);
/*
		trace("CAM IMG SIZE %dx%d (raw frame was %u bytes)", m_image.m_width, m_image.m_height, len);

		unsigned long w, h;
		unsigned short bpp;
		cmu_camera->GetVideoFrameDimensions(&w, &h);
		cmu_camera->GetVideoDataDepth(&bpp);
		trace("CAM TRUE SIZE %ux%u (%u bits) %d", w, h, bpp, (w * h * bpp / 8));
		//cmu_camera->getRGB(m_image.m_planes[0].m_data, m_image.size());

		// trace("Video1394::captureImage # EXIT");
*/
		return & m_image;
	}

	// ----------------------------------------------------------------------------
	// not needed with CMU
	//
	void Video1394::doneImage()
	{
	}

	/** -----------------------------------------------------------------------
	* Shuts down the connection.
	*/
	bool Video1394::close()
	{
		assert(isInitialized());

		if (!cmu_camera)
			return false;

		if (m_started)
			stop();

		// dc1394_release_camera(m_camera);
		// dc1394_free_camera(m_camera);

		m_initialized = false;

		delete cmu_camera;
		cmu_camera = 0;

		return true;
	}

	// ----------------------------------------------------------------------------
	//
	Video1394::CameraInfo Video1394::cameraInfo()
	{
		CameraInfo info;

		if (cmu_camera)
		{
			char vendor[256],model[256];
			LARGE_INTEGER guid; 
			cmu_camera->GetCameraUniqueID(&guid);
			cmu_camera->GetCameraVendor(vendor, 256);
			cmu_camera->GetCameraName(model, 256);
			info.m_euid64 = guid.QuadPart;
			info.m_vendor = vendor;
			info.m_model  = model;
		}

		return info;
	}

	// ----------------------------------------------------------------------------
	//
	void Video1394::queryCameras(std::vector<CameraInfo> * query)
	{
		/* JJK needed??
		if (!__dcs)
			__dcs = cmu1394_new(); */

		// const char * fname = "Video1394::queryCameras";

		C1394Camera cmucam;
		cmucam.CheckLink();
		int numCams = cmucam.RefreshCameraList();

		// -- enumerate cameras
		dc1394camera_list_t camlist;
		camlist.ids = 0;
		//int numCams = cmucam.GetNumberCameras();
		for (int i = 0; i < numCams; i++)
		{
			LARGE_INTEGER guid;
			cmucam.SelectCamera(i);
			// cmucam.InitCamera();
			cmucam.GetCameraUniqueID(&guid);
			uint64_t uguid = guid.QuadPart;

			bool already = false;
			for (unsigned j = 0; j < __infos.size(); j++)
				if (__infos[j]->guid == uguid)
				{
					already = true;
					break;
				}
	      
			if (!already)
				__infos.push_back(cmu_dc1394_camera_new(&cmucam, uguid));

			//JJK: should I free cmucam resources here ?
		}

		// -- fill query
		for (unsigned i = 0; i < __infos.size(); i++)
		{
			dc1394camera_t * c = __infos[i];
			CameraInfo ci;
			ci.m_euid64 = c->guid;
			ci.m_vendor = c->vendor;
			ci.m_model  = c->model;
			query->push_back(ci);
		}

		//?? query->clear();
		//cmu_dc1394_camera_free_list(camlist);
	}

	// ----------------------------------------------------------------------------
	//
	bool Video1394::queryCamera(u_int64_t euid64, CameraInfo * camera)
	{
		std::vector<CameraInfo> query;
		queryCameras(&query);

		for (uint i = 0; i < query.size(); i++)
			if ((u_int64_t)query[i].m_euid64 == euid64)
			{
				*camera = query[i];
				return true;
			}

		error("Video1394::queryCamera # FAILED");
		return false;
	}

}	// namespace
