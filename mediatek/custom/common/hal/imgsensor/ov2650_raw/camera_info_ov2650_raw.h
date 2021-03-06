

#ifndef _CAMERA_INFO_OV2650_H
#define _CAMERA_INFO_OV2650_H

#define SENSOR_ID                           OV2650_SENSOR_ID
#define SENSOR_DRVNAME                      SENSOR_DRVNAME_OV2650_RAW
#define INCLUDE_FILENAME_ISP_REGS_PARAM     "camera_isp_regs_ov2650raw_mt6573.h"
#define INCLUDE_FILENAME_ISP_PCA_PARAM      "camera_isp_pca_ov2650raw_mt6573.h"
#include "camera_3A_para_ov2650_raw_mt6573.h"
#define CAMERA_3A_PARA                      OV2650_RAW_CAMERA_3A_PARA
#define CAMERA_3A_STAT_CONFIG_PARAM         OV2650_RAW_CAMERA_3A_STAT_CONFIG_PARAM


#if defined(ISP_SUPPORT)


#define OV2650_CAMERA_AUTO_DSC CAM_AUTO_DSC
#define OV2650_CAMERA_PORTRAIT CAM_PORTRAIT
#define OV2650_CAMERA_LANDSCAPE CAM_LANDSCAPE
#define OV2650_CAMERA_SPORT CAM_SPORT
#define OV2650_CAMERA_FLOWER CAM_FLOWER
#define OV2650_CAMERA_NIGHTSCENE CAM_NIGHTSCENE
#define OV2650_CAMERA_DOCUMENT CAM_DOCUMENT
#define OV2650_CAMERA_ISO_ANTI_HAND_SHAKE CAM_ISO_ANTI_HAND_SHAKE
#define OV2650_CAMERA_ISO100 CAM_ISO100
#define OV2650_CAMERA_ISO200 CAM_ISO200
#define OV2650_CAMERA_ISO400 CAM_ISO400
#define OV2650_CAMERA_ISO800 CAM_ISO800
#define OV2650_CAMERA_ISO1600 CAM_ISO1600
#define OV2650_CAMERA_VIDEO_AUTO CAM_VIDEO_AUTO
#define OV2650_CAMERA_VIDEO_NIGHT CAM_VIDEO_NIGHT
#define OV2650_CAMERA_NO_OF_SCENE_MODE CAM_NO_OF_SCENE_MODE

#endif
#endif
