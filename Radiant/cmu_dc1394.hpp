#ifndef _cmu_dc1394_hpp_
#define _cmu_dc1394_hpp_

typedef struct
{
	int num_cameras;
	// camera_info_t* cameras;
	C1394Camera** cmucams;			// array of cameras in bus
} cmu_dc1394_t;

dc1394camera_t* cmu_dc1394_camera_new(C1394Camera* pcam, uint64_t guid);
cmu_dc1394_t* cmu1394_new();
void cmu1394_free(cmu_dc1394_t* dcs, std::vector<dc1394camera_t *>& infos);
void cmu_dc1394_camera_free_list(dc1394camera_list_t* camlist);
bool cmu_dc1394_feature_get_all(Radiant::Video1394* videoin, dc1394featureset_t* featureSet);
//void cmu_dc1394_feature_get_all(dc1394camera_t* pcam, dc1394featureset_t* pfeatures);

dc1394error_t get_format_from_mode(dc1394video_mode_t mode, uint32_t *format);

const char * cmu_dc1394_feature_get_string(CAMERA_FEATURE cmu_feature);
CAMERA_FEATURE cmu_dc1394_convertFeatureID(dc1394feature_t feature);
dc1394feature_t cmu_dc1394_convertFeatureID(CAMERA_FEATURE feature);
const char * cmu_dc1394_feature_get_string(CAMERA_FEATURE feature);

#endif
