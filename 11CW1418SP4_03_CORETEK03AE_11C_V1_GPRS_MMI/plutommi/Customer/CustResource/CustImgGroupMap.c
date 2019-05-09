#include "CustDataRes.h"
#include "ResCompressConfig.h"

#ifndef __MMI_RESOURCE_IMAGE_THIRD_ROM__
#ifdef __MMI_RESOURCE_LZMA_IMAGE_GROUP_COMPRESS__

#include "CustResDef.h"
#include "CustImgGroupDataHW.h"
const unsigned short  mtk_CurrMaxGroupsNum = 35;

const CUSTOM_IMAGE	mtk_nCustGroupNames[]={
    (U8*)&image_group_0,
    (U8*)&image_group_1,
    (U8*)&image_group_2,
    (U8*)&image_group_3,
    (U8*)&image_group_4,
    (U8*)&image_group_5,
    (U8*)&image_group_6,
    (U8*)&image_group_7,
    (U8*)&image_group_8,
    (U8*)&image_group_9,
    (U8*)&image_group_10,
    (U8*)&image_group_11,
    (U8*)&image_group_12,
    (U8*)&image_group_13,
    (U8*)&image_group_14,
    (U8*)&image_group_15,
    (U8*)&image_group_16,
    (U8*)&image_group_17,
    (U8*)&image_group_18,
    (U8*)&image_group_19,
    (U8*)&image_group_20,
    (U8*)&image_group_21,
    (U8*)&image_group_22,
    (U8*)&image_group_23,
    (U8*)&image_group_24,
    (U8*)&image_group_25,
    (U8*)&image_group_26,
    (U8*)&image_group_27,
    (U8*)&image_group_28,
    (U8*)&image_group_29,
    (U8*)&image_group_30,
    (U8*)&image_group_31,
    (U8*)&image_group_32,
    (U8*)&image_group_33,
    (U8*)&image_group_34
};

#ifdef __MMI_RESOURCE_UT_SIMPLE_CACHE_SUPPORT__
const CUSTOM_IMAGE	mtk_nCustGroupCacheNames[]={
    (U8*)&cache_image_group_0,
    (U8*)&cache_image_group_1,
    (U8*)&cache_image_group_2,
    (U8*)&cache_image_group_3,
    (U8*)&cache_image_group_4,
    (U8*)&cache_image_group_5,
    (U8*)&cache_image_group_6,
    (U8*)&cache_image_group_7,
    (U8*)&cache_image_group_8,
    (U8*)&cache_image_group_9,
    (U8*)&cache_image_group_10,
    (U8*)&cache_image_group_11,
    (U8*)&cache_image_group_12,
    (U8*)&cache_image_group_13,
    (U8*)&cache_image_group_14,
    (U8*)&cache_image_group_15,
    (U8*)&cache_image_group_16,
    (U8*)&cache_image_group_17,
    (U8*)&cache_image_group_18,
    (U8*)&cache_image_group_19,
    (U8*)&cache_image_group_20,
    (U8*)&cache_image_group_21,
    (U8*)&cache_image_group_22,
    (U8*)&cache_image_group_23,
    (U8*)&cache_image_group_24,
    (U8*)&cache_image_group_25,
    (U8*)&cache_image_group_26,
    (U8*)&cache_image_group_27,
    (U8*)&cache_image_group_28,
    (U8*)&cache_image_group_29,
    (U8*)&cache_image_group_30,
    (U8*)&cache_image_group_31,
    (U8*)&cache_image_group_32,
    (U8*)&cache_image_group_33,
    (U8*)&cache_image_group_34
};

#endif //__MMI_RESOURCE_UT_SIMPLE_CACHE_SUPPORT__


#endif  //__MMI_RESOURCE_LZMA_IMAGE_GROUP_COMPRESS__


#endif  // __MMI_RESOURCE_IMAGE_THIRD_ROM__
