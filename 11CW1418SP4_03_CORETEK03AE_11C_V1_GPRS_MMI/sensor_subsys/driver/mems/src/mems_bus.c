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
#include "dcl.h"
#include "dcl_i2c.h"

#define LOGE(fmt,arg...)   kal_prompt_trace(MOD_MMI, "MEMS_BUS: "fmt, ##arg)
#define LOGW(fmt,arg...)   kal_prompt_trace(MOD_MMI, "MEMS_BUS: "fmt, ##arg) 
#define LOGI(fmt,arg...)   kal_prompt_trace(MOD_MMI, "MEMS_BUS: "fmt, ##arg)
#define LOGD(fmt,arg...)   kal_prompt_trace(MOD_MMI, "MEMS_BUS: "fmt, ##arg)

#if 1 /*bus select : I2C */
static DCL_HANDLE  	mems_i2c_handle;
static kal_bool 	mems_i2c_configure_done = KAL_FALSE;
static kal_uint8 	mems_i2c_send_reg;

void mems_i2c_init(kal_uint8 i2c_port, kal_uint32 i2c_freq)
{
	DclSI2C_Initialize();
}

kal_int32 mems_i2c_open(kal_uint8 slave_addr)
{
	I2C_CONFIG_T cfg;
	DCL_STATUS status = STATUS_OK;
	
	cfg.eOwner = DCL_I2C_OWNER_GS;
	cfg.fgGetHandleWait = KAL_TRUE;
	cfg.u1SlaveAddress = slave_addr << 1;
	cfg.u1DelayLen = 0;
	cfg.eTransactionMode = DCL_I2C_TRANSACTION_FAST_MODE;
	cfg.u4FastModeSpeed = 400;
	cfg.u4HSModeSpeed = 0;
#if 0
/* under construction !*/
#else
	cfg.fgEnableDMA = DCL_FALSE;
#endif

	mems_i2c_handle = DclSI2C_Open(DCL_I2C, DCL_I2C_OWNER_GS);
	DclSI2C_Configure(mems_i2c_handle, (DCL_CONFIGURE_T *)&cfg);

	mems_i2c_configure_done = KAL_TRUE;

	return status;
}

kal_int32 mems_i2c_write(kal_uint8 slave_addr, kal_uint8 *data, kal_uint16 len)
{
	I2C_CTRL_SINGLE_WRITE_T write;
    DCL_STATUS status = STATUS_OK;

	if (!mems_i2c_configure_done) return STATUS_NOT_OPENED;
	
#if 0 /* DMA mode*/
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
#else /* Polling */
	write.u4DataLen = len;
	write.pu1Data = data;
	status = DclSI2C_Control(mems_i2c_handle, I2C_CMD_SINGLE_WRITE, (DCL_CTRL_DATA_T *)&write);
	if(STATUS_OK != status){
		LOGE("send polling failed (%d).\r\n", status);
	}
#endif

    return status;
}

kal_int32 mems_i2c_read(kal_uint8 slave_addr, kal_uint8 *data, kal_uint16 len)
{
    I2C_CTRL_SINGLE_READ_T read;
    DCL_STATUS status;

	if (!mems_i2c_configure_done) return STATUS_NOT_OPENED;

#if 0 /* DMA mode*/
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
#else /* Polling */
	read.u4DataLen = len;
	read.pu1Data = data;
	status = DclSI2C_Control(mems_i2c_handle, I2C_CMD_SINGLE_READ, (DCL_CTRL_DATA_T *)&read);
	if(STATUS_OK != status){
		LOGE("receive polling failed (%d).\r\n", status);
	}

#endif

    return status;
}

kal_int32 mems_i2c_write_read(kal_uint8 slave_addr, kal_uint8 reg, kal_uint8 *data, kal_uint16 len)
{
	I2C_CTRL_WRITE_AND_READE_T write_read;
    DCL_STATUS status;

	if (!mems_i2c_configure_done) return STATUS_NOT_OPENED;

	mems_i2c_send_reg = reg;
	write_read.pu1OutData = &mems_i2c_send_reg; 
	write_read.u4OutDataLen = 1;
	write_read.pu1InData = data;
	write_read.u4InDataLen = len;
	status = DclSI2C_Control(mems_i2c_handle, I2C_CMD_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_read);
    if(STATUS_OK != status){
        LOGE("write read polling failed (%d).\r\n", status);
    }

    return status;
}

#else  /*bus select : SPI */
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
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
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
#ifndef __ICCARM__
/* under construction !*/
#else
/* under construction !*/
#endif
#endif /*SPI_DMA_NON_CACHEABLE_ACCESS*/
#endif /*HAL_SPI_MASTER_FEATURE_DMA_MODE*/
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
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/* under construction !*/
/* under construction !*/
#endif
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
/* under construction !*/
#ifdef HAL_SPI_MASTER_FEATURE_MACRO_CONFIG
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
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
#else
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
/* under construction !*/
/* under construction !*/
#else
/* under construction !*/
#endif
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#else
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif /*bus select */

