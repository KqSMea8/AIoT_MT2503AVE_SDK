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
#include "vsm_driver.h"
#include "vsm_i2c_operation.h"
#include "drv_comm.h"
#include "dcl.h"
#include "dcl_i2c.h"

#ifndef MT2511_USE_SPI

#define LOGE(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511]"fmt, ##arg)
#define LOGW(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511]"fmt, ##arg) 
#define LOGI(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511]"fmt, ##arg)

#define	I2C_BUFFER_LEN 8
 
 //i2c parameter
static DCL_HANDLE  	vsm_i2c_handle = -1;
static kal_uint32 	vsm_i2c_work_freq = 200;

#ifdef MT2511_BUS_USE_DMA
#pragma arm section rwdata = "NONCACHEDRW", zidata = "NONCACHEDZI"
kal_uint8 vsm_array[I2C_BUFFER_LEN];
kal_uint8 vsm_reg;
#pragma arm section rwdata, zidata
#else
kal_uint8 vsm_array[I2C_BUFFER_LEN];
kal_uint8 vsm_reg;
#endif

DCL_STATUS i2c_configure(kal_uint8 slave_address)
{
	I2C_CONFIG_T cfg;
	
	cfg.eOwner = DCL_I2C_USER_DEV1;
	cfg.fgGetHandleWait = KAL_TRUE;
	cfg.u1SlaveAddress = slave_address << 1;
	cfg.u1DelayLen = 0;
	cfg.eTransactionMode = DCL_I2C_TRANSACTION_FAST_MODE;
	cfg.u4FastModeSpeed = vsm_i2c_work_freq;
	cfg.u4HSModeSpeed = 0;
#ifdef MT2511_BUS_USE_DMA	
	cfg.fgEnableDMA = DCL_TRUE;
#else
	cfg.fgEnableDMA = DCL_FALSE;
#endif

	return DclSI2C_Configure(vsm_i2c_handle, (DCL_CONFIGURE_T *)&cfg);
}


void i2c_init(kal_uint8 slave_address, kal_uint32 i2c_freq)
{
	DCL_STATUS status = STATUS_OK;

	vsm_i2c_work_freq = i2c_freq;
	vsm_i2c_handle = DclSI2C_Open(DCL_I2C, DCL_I2C_USER_DEV1);
}

vsm_status_t vsm_i2c_write(kal_uint8 slave_addr, kal_uint8 *data, uint16_t len)
{
    DCL_STATUS status;
	I2C_CTRL_SINGLE_WRITE_T write;
	int i;

	if (vsm_i2c_handle < 0)
		return VSM_STATUS_UNINITIALIZED;
	
	status = i2c_configure(slave_addr);
    if (status != STATUS_OK) {
        LOGE("vsm_i2c_write:i2c_configure failed:status = %d\r\n", status);
        return VSM_STATUS_ERROR;
    }

	#ifdef MT2511_BUS_USE_DMA
	for (i=0; i<len; i++) {
        vsm_array[i] = data[i];
    }
	write.u4DataLen = len;
	write.pu1Data = vsm_array;
	#else
	write.u4DataLen = len;
	write.pu1Data = data;
	#endif
	
	status = DclSI2C_Control(vsm_i2c_handle, I2C_CMD_SINGLE_WRITE, (DCL_CTRL_DATA_T *)&write);	
	if(STATUS_OK != status){
		LOGE("vsm:vsm_i2c_write:failed (%d).\r\n", status);
		return VSM_STATUS_ERROR;
	}


	return VSM_STATUS_OK;    
}

vsm_status_t vsm_i2c_write_read(kal_uint8 slave_addr, kal_uint8 reg, kal_uint8 *data, uint16_t len)
{
	DCL_STATUS status;
	I2C_CTRL_WRITE_AND_READE_T write_read;

	if (vsm_i2c_handle < 0)
		return VSM_STATUS_UNINITIALIZED;

	status = i2c_configure(slave_addr);
    if (status != STATUS_OK) {
        LOGE("vsm_i2c_write_read:i2c_configure failed:status = %d\r\n", status);
        return VSM_STATUS_ERROR;
    }

	#ifdef MT2511_BUS_USE_DMA
	vsm_reg = reg;
	write_read.pu1OutData = &vsm_reg; 
	write_read.u4OutDataLen = 1;
	write_read.pu1InData = vsm_array;
	write_read.u4InDataLen = len;
	#else
	write_read.pu1OutData = &reg; 
	write_read.u4OutDataLen = 1;
	write_read.pu1InData = data;
	write_read.u4InDataLen = len;
	#endif
	
	status = DclSI2C_Control(vsm_i2c_handle, I2C_CMD_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_read);
	if(STATUS_OK != status){
		LOGE("write read polling failed (%d).\r\n", status);
		return VSM_STATUS_ERROR;
	}
	else {
		#ifdef MT2511_BUS_USE_DMA
		int i;
		for (i = 0; i < len; i++) {
        	data[i] = vsm_array[i];
        }
		#endif
	}
	return VSM_STATUS_OK;
}

#endif

