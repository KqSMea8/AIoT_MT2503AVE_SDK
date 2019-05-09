/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "kal_release.h"
#include "drv_comm.h"
#include "stdio.h"
#include "string.h" // memcpy
#include "sensor_alg_interface.h"
#include "intrCtrl.h"
#include "us_timer.h"


#define LOGE(fmt,arg...)   kal_prompt_trace(MOD_MMI, "SMD: "fmt, ##arg)
#define LOGW(fmt,arg...)   kal_prompt_trace(MOD_MMI, "SMD: "fmt, ##arg) 
#define LOGI(fmt,arg...)   kal_prompt_trace(MOD_MMI, "SMD: "fmt, ##arg)
#define LOGD(fmt,arg...)   kal_prompt_trace(MOD_MMI, "SMD: "fmt, ##arg)

struct sensor_driver_context { /*sensor context*/
    uint32_t enable;
    uint32_t delay;
    sensor_driver_object_t obj;
};

static struct sensor_driver_context sdc[SENSOR_PHYSICAL_TYPE_ALL];

int32_t sensor_driver_attach(uint32_t sensor_type, sensor_driver_object_t *obj)
{
    LOGI("sensor_driver_attach sensor_type= %d \r\n", sensor_type);
    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }

    sdc[sensor_type].enable = 0;
    if (obj->polling) {
        sdc[sensor_type].delay = SENSOR_DEFAULT_DELAY; //default delay.
    } else {
        sdc[sensor_type].delay = 0x7FFFFFFF; //max delay
    }
    memcpy(&sdc[sensor_type].obj, obj, sizeof(sensor_driver_object_t));

    return 0;
}

int32_t sensor_driver_enable(uint32_t sensor_type, int32_t enable)
{
    int err = 0;

    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }
    if (NULL == sdc[sensor_type].obj.sensor_operate) {
        LOGD("sensor_driver_set_delay(%d) null pointer\r\n", sensor_type);
        return -1;
    }

    // redundant check
    if ( sdc[sensor_type].enable == enable ) {
        return 0;
    }

    sdc[sensor_type].obj.sensor_operate(sdc[sensor_type].obj.self, \
                                        SENSOR_ENABLE, NULL, 0, NULL, &enable, sizeof(int32_t));

    if (err >= 0) {
        sdc[sensor_type].enable = enable;
    }

    return err;
}

/*------------get data--------------------------------------------------------*/
int32_t sensor_driver_get_data(uint32_t sensor_type, void *data_buf, int32_t size, int32_t *actual_out_size)
{
    int err = 0;

    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }
    if (NULL == sdc[sensor_type].obj.sensor_operate) {
        LOGD("sensor_driver_get_data(%d) null pointer\r\n", sensor_type);
        return -1;
    }

    err = sdc[sensor_type].obj.sensor_operate(sdc[sensor_type].obj.self, SENSOR_GET_DATA,
            data_buf, size, actual_out_size, NULL, 0);

    if (err < 0) {
        LOGD("sensor_driver_get_data type(%d) err(%d) \r\n", sensor_type, err);
    }

    return err;
}

/*------------set delay--------------------------------------------------------*/
// delay in ms unit
int32_t sensor_driver_set_delay(uint32_t sensor_type, int32_t delay)
{
    int err = 0;

    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }
    if (NULL == sdc[sensor_type].obj.sensor_operate) {
        LOGD("sensor_driver_set_delay(%d) null pointer \r\n", sensor_type);
        return -1;
    }

    // redundant check
    if ( sdc[sensor_type].delay == delay ) {
        return 0;
    }

    err = sdc[sensor_type].obj.sensor_operate(sdc[sensor_type].obj.self, SENSOR_DELAY, \
            NULL, 0, NULL, &delay, sizeof(int32_t));

    if (err >= 0) {
        sdc[sensor_type].delay = delay;
    }

    return err;
}

int32_t sensor_driver_set_cust(uint32_t sensor_type, void *out_data_buf, int32_t out_size, int32_t *actual_out_size, \
                               void *in_data_buf, int32_t in_size)
{
    int err = 0;

    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }
    if (NULL == sdc[sensor_type].obj.sensor_operate) {
        LOGD("sensor_driver_set_cust(%d) null pointer \r\n", sensor_type);
        return -1;
    }
    err = sdc[sensor_type].obj.sensor_operate(sdc[sensor_type].obj.self, SENSOR_CUST, \
            out_data_buf, out_size, actual_out_size, in_data_buf, in_size);
    return err;
}

uint32_t sensor_driver_get_enable(int sensor_type)
{
    return sdc[sensor_type].enable;
}

uint32_t sensor_driver_get_delay(int sensor_type)
{
    return sdc[sensor_type].delay;
}

int32_t sensor_driver_get_event_type(int sensor_type)
{
    return sdc[sensor_type].obj.polling;
}


/* No thread safe. Use in task context. */
uint32_t sensor_driver_get_ms_tick(void)
{
#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
    return ust_get_current_time();
}

