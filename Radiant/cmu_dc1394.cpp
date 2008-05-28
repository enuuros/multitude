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
#include <Radiant/Types.hpp>
#include <Radiant/Video1394cmu.hpp>

#include <dc1394/control.h>
#include <math.h>
#define _INC_STDLIB			// __restrict
#include <windows.h>
#include <1394Camera.h>
#include "cmu_dc1394.hpp"
#include <WinPort.h>


// ----------------------------------------------------------------------------
// -- the return value is cached and used later, pcam is not
//
dc1394camera_t* cmu_dc1394_camera_new(C1394Camera* pcam, uint64_t guid)
{
	dc1394camera_t* dccam = 0;
	if (pcam != 0)
	{
		dccam = new dc1394camera_t;
		if (dccam != 0)
		{
			bzero(dccam, sizeof(dc1394camera_t));
			dccam->guid = guid;
			dccam->vendor = new char[256];
			dccam->model  = new char[256];
			pcam->GetCameraVendor(dccam->vendor, 256);
			pcam->GetCameraName(dccam->model, 256);
		}
	}
	return dccam;
}

// ----------------------------------------------------------------------------
//
cmu_dc1394_t* cmu1394_new()
{
	cmu_dc1394_t* pcam = new cmu_dc1394_t;
	if (pcam)
		bzero(pcam, sizeof(cmu_dc1394_t));
	return pcam;
}

// ----------------------------------------------------------------------------
//
void cmu1394_free(cmu_dc1394_t* dcs, std::vector<dc1394camera_t *>& infos)
{
	for (int c = 0; c < (int)infos.size(); c++)
	{
		delete [] infos[c]->vendor;
		delete [] infos[c]->model;
	}
	delete [] dcs->cmucams;
	delete dcs;
}

// ----------------------------------------------------------------------------
//
void cmu_dc1394_camera_free_list(dc1394camera_list_t* camlist)
{
	delete [] camlist->ids;
	delete camlist;
}

// ----------------------------------------------------------------------------
//
bool cmu_dc1394_feature_get_all(Radiant::Video1394* videoin, dc1394featureset_t* featureSet)
{
	std::vector<dc1394feature_info_t> featureList;
	videoin->getFeatures(&featureList);
	for (unsigned i=0; i<featureList.size(); i++)
	{
		if (i >= DC1394_FEATURE_NUM) break;
		featureSet->feature[i] = featureList[i];
	}
	return true;
}

// ----------------------------------------------------------------------------
// feature id's are incompatible between cmu1394 and libdc1394, gotta convert
// here
//
const char * cmu_dc1394_feature_get_string(CAMERA_FEATURE cmu_feature)
{
	dc1394feature_t feature = cmu_dc1394_convertFeatureID(cmu_feature);
	return Radiant::Video1394::featureName(feature);	// ::dc1394GetFeatureName(feature);
}

// ----------------------------------------------------------------------------
// feature id's are incompatible between cmu1394 and libdc1394, gotta convert
//
CAMERA_FEATURE cmu_dc1394_convertFeatureID(dc1394feature_t feature)
{
	unsigned uFeature = feature - 416;
	if ((16 <= uFeature) && (uFeature < 20))			uFeature += 16;
	else if ((20 <= uFeature) && (uFeature < 22))	uFeature += 28;
	return (CAMERA_FEATURE)uFeature;
}

// ----------------------------------------------------------------------------
// feature id's are incompatible between cmu1394 and libdc1394, gotta convert
//
dc1394feature_t cmu_dc1394_convertFeatureID(CAMERA_FEATURE cmu_feature)
{
	int feature = (int)cmu_feature;
	if ((0 <= feature) && (feature < 16))			feature = feature;
	else if ((32 <= feature) && (feature < 36))	feature -= 16;
	else if ((48 <= feature) && (feature < 50))	feature -= 28;
	else														feature = DC1394_FEATURE_NUM + 1;
	dc1394feature_t dcFeature = (dc1394feature_t)(feature + DC1394_FEATURE_MIN);
	return dcFeature;
}

// ----------------------------------------------------------------------------
// ripped from dc1394/internal.c
//
dc1394error_t get_format_from_mode(dc1394video_mode_t mode, uint32_t *format)
{
    dc1394error_t err = DC1394_SUCCESS;

	 if ((DC1394_VIDEO_MODE_160x120_YUV444 <= mode) && (mode <= DC1394_VIDEO_MODE_640x480_MONO16))
        *format = 0;
    else if ((DC1394_VIDEO_MODE_800x600_YUV422 <= mode) && (mode <= DC1394_VIDEO_MODE_1024x768_MONO16))
        *format = 1;
    else if ((DC1394_VIDEO_MODE_1280x960_YUV422 <= mode) && (mode <= DC1394_VIDEO_MODE_1600x1200_MONO16))
        *format = 2;
    else if ((DC1394_VIDEO_MODE_EXIF <= mode) && (mode <= DC1394_VIDEO_MODE_EXIF))
        *format = 6;
    else if ((DC1394_VIDEO_MODE_FORMAT7_0 <= mode) && (mode <= DC1394_VIDEO_MODE_FORMAT7_7))
        *format = 6;
    else err = DC1394_INVALID_VIDEO_MODE;

    return err;
}

// ----------------------------------------------------------------------------
// void cmu_dc1394_feature_set_mode(m_camera, feature, DC1394_FEATURE_MODE_AUTO);
/*
void cmu_dc1394_feature_set_mode(1394Camera* camera, CAMERA_FEATURE feature, DC1394_FEATURE_MODE_AUTO);
{
}
*/

