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
#include "vsm_platform_function.h"
#include "dcl.h"

#define LOGE(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511]"fmt, ##arg)
#define LOGW(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511]"fmt, ##arg) 
#define LOGI(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511]"fmt, ##arg)
#define LOGD(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511]"fmt, ##arg)

void vsm_platform_gpio_get_output(kal_int32 gpio_pin, kal_int32 *gpio_data)
{
	DCL_HANDLE gpio_handle;
	GPIO_CTRL_RETURN_OUT_T data;
	
	if (gpio_pin >= 0xFF) {
		LOGI("vsm_platform_gpio_get_output : incorrect pin:0x%x", gpio_pin);
		return;
	}
	
	gpio_handle = DclGPIO_Open(DCL_GPIO, gpio_pin);
	DclGPIO_Control(gpio_handle, GPIO_CMD_RETURN_OUT, (DCL_CTRL_DATA_T *)&data);
	*gpio_data = (kal_int32)data.u1RetOutData;
	DclGPIO_Close(gpio_handle);
}

void vsm_platform_gpio_set_output(kal_int32 gpio_pin, kal_int32 gpio_data)
{
	DCL_HANDLE gpio_handle;
	
	if (gpio_pin >= 0xFF) {
		LOGI("vsm_platform_gpio_set_output : incorrect pin:0x%x", gpio_pin);
		return;
	}

	gpio_handle = DclGPIO_Open(DCL_GPIO, gpio_pin);
	if (gpio_data == HAL_GPIO_DATA_HIGH)
		DclGPIO_Control(gpio_handle, GPIO_CMD_WRITE_HIGH, NULL);
	else
		DclGPIO_Control(gpio_handle, GPIO_CMD_WRITE_LOW, NULL);
	DclGPIO_Close(gpio_handle);
}

void vsm_platform_gpio_get_direction(kal_int32 gpio_pin, kal_int32 *gpio_direction)
{
	DCL_HANDLE gpio_handle;
	GPIO_CTRL_RETURN_OUT_T data;
	
	if (gpio_pin >= 0xFF) {
		LOGI("vsm_platform_gpio_get_direction : incorrect pin:0x%x", gpio_pin);
		return;
	}
	
	gpio_handle = DclGPIO_Open(DCL_GPIO, gpio_pin);
	DclGPIO_Control(gpio_handle, GPIO_CMD_RETURN_DIR, (DCL_CTRL_DATA_T *)&data);
	*gpio_direction = (kal_int32)data.u1RetOutData;
	DclGPIO_Close(gpio_handle);
}

void vsm_platform_gpio_set_direction(kal_int32 gpio_pin, kal_int32 gpio_direction)
{
	DCL_HANDLE gpio_handle;
	
	if (gpio_pin >= 0xFF) {
		LOGI("vsm_platform_gpio_set_direction : incorrect pin:0x%x", gpio_pin);
		return;
	}
	
	gpio_handle = DclGPIO_Open(DCL_GPIO, gpio_pin);
	if (gpio_direction == HAL_GPIO_DIRECTION_OUTPUT)
		DclGPIO_Control(gpio_handle, GPIO_CMD_SET_DIR_OUT, NULL);
	else if (gpio_direction == HAL_GPIO_DIRECTION_INPUT)
		DclGPIO_Control(gpio_handle, GPIO_CMD_SET_DIR_IN, NULL);
	DclGPIO_Close(gpio_handle);
}

void vsm_platform_gpio_set_pinmux(kal_int32 gpio_pin, uint8_t function_index)
{
	DCL_HANDLE gpio_handle;
	DCL_STATUS status = STATUS_OK;

	if (gpio_pin >= 0xFF) {
		LOGI("vsm_platform_gpio_set_pinmux : incorrect pin:0x%x", gpio_pin);
		return;
	}

	gpio_handle = DclGPIO_Open(DCL_GPIO, gpio_pin);
	switch(function_index) {
		case 0:
			status = DclGPIO_Control(gpio_handle, GPIO_CMD_SET_MODE_0, NULL); 
			break;
		case 1:
			status = DclGPIO_Control(gpio_handle, GPIO_CMD_SET_MODE_1, NULL);
			break;
		case 2:
			status = DclGPIO_Control(gpio_handle, GPIO_CMD_SET_MODE_2, NULL);			
			break;
		case 3:
			status = DclGPIO_Control(gpio_handle, GPIO_CMD_SET_MODE_3, NULL);
			break;
		case 4:
			status = DclGPIO_Control(gpio_handle, GPIO_CMD_SET_MODE_4, NULL);
			break;
		case 5:
			status = DclGPIO_Control(gpio_handle, GPIO_CMD_SET_MODE_5, NULL);
			break;
		case 6:
			status = DclGPIO_Control(gpio_handle, GPIO_CMD_SET_MODE_6, NULL);
			break;
		case 7:
			status = DclGPIO_Control(gpio_handle, GPIO_CMD_SET_MODE_7, NULL);
			break;
		case 8:
			status = DclGPIO_Control(gpio_handle, GPIO_CMD_SET_MODE_8, NULL);
			break;			
		default:
			break;
	}

	//LOGI("vsm_platform_gpio_set_pinmux : end with status=%d", status);
	DclGPIO_Close(gpio_handle);
}

void vsm_platform_gpio_init(kal_int32 gpio_pin)
{
	if (gpio_pin >= 0xFF)
		return;

}

void vsm_platform_gpio_deinit(kal_int32 gpio_pin)
{
	if (gpio_pin >= 0xFF)
		return;
}

void ms_delay(kal_uint32 ms)
{
#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#else
    kal_sleep_task(ms/KAL_MILLISECS_PER_TICK+1);
#endif
    
}


kal_uint32 vsm_platform_get_ms_tick()
{
	kal_uint32 curr_tick, curr_ms;

	kal_get_time(&curr_tick);	 
	curr_ms = kal_ticks_to_milli_secs(curr_tick);
	//LOGI("curr_tick(%u), curr_ms(%u)\n", curr_tick, curr_ms);

	return curr_ms;
}

