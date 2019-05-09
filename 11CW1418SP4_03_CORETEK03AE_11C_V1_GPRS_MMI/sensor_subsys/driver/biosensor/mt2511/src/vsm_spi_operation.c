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
#include <stdio.h>
#include <string.h>
#include "vsm_driver.h" 
#include "vsm_spi_operation.h"
#include "dcl.h"
#include "spi_hal.h"

#ifdef MT2511_USE_SPI

#define LOGE(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511_spi]"fmt, ##arg)
#define LOGW(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511_spi]"fmt, ##arg) 
#define LOGI(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511_spi]"fmt, ##arg)

//#define DEBUG_VSM_SPI
#ifdef DEBUG_VSM_SPI
#define LOGD(fmt,arg...)   kal_prompt_trace(MOD_MMI, "[mt2511_spi]"fmt, ##arg)
#else
#define LOGD(fmt,arg...)
#endif

#define	SPI_BUFFER_LEN 8
#define SPI_CHIP_SELECT_VSM 0
/*SPI protocol for mt2511*/
#define SPI_CR_CMD  0x02 //Config Read
#define SPI_CW_CMD  0x04 //Config Write
#define SPI_WD_CMD  0x06 //Write Data
#define SPI_RD_CMD  0x81 //Read Data
#define SPI_RS_CMD  0x0A //Read Status
#define SPI_WS_CMD  0x08 //Write Status
/*2523 spi power on/off cmd*/
#define	SPI_PWRO_CMD    0x0E // Power on
#define SPI_PWRF_CMD    0x0C // Power Off

#define SPISLV_STATUS_CFG_SUCCESS_OFFSET       (1)
#define SPISLV_STATUS_CFG_SUCCESS_MASK         (0x1U<<SPISLV_STATUS_CFG_SUCCESS_OFFSET)
#define SPISLV_STATUS_WR_ERR_OFFSET            (4)
#define SPISLV_STATUS_WR_ERR_MASK              (0x1U<<SPISLV_STATUS_WR_ERR_OFFSET)
#define SPISLV_STATUS_TIMOUT_ERR_OFFSET        (6)
#define SPISLV_STATUS_TIMOUT_ERR_MASK          (0x1U<<SPISLV_STATUS_TIMOUT_ERR_OFFSET)
#define SPISLV_STATUS_RD_ERR_OFFSET            (3)
#define SPISLV_STATUS_RD_ERR_MASK              (0x1U<<SPISLV_STATUS_RD_ERR_OFFSET)
#define SPISLV_STATUS_TXRX_FIFO_RDY_OFFSET     (2)
#define SPISLV_STATUS_TXRX_FIFO_RDY_MASK       (0x1U<<SPISLV_STATUS_TXRX_FIFO_RDY_OFFSET)
#define SPISLV_STATUS_CMD_ERR_OFFSET           (7)
#define SPISLV_STATUS_CMD_ERR_MASK             (0x1U<<SPISLV_STATUS_CMD_ERR_OFFSET)
#define SPI_ERR_MASK (SPISLV_STATUS_RD_ERR_MASK | SPISLV_STATUS_WR_ERR_MASK | SPISLV_STATUS_TIMOUT_ERR_MASK)

#define FAST_SPI

#pragma arm section rwdata = "NONCACHEDRW", zidata = "NONCACHEDZI"
kal_uint8 vsm_array[SPI_BUFFER_LEN];
kal_uint8 vsm_spi_send_buffer[SPI_BUFFER_LEN];
kal_uint8 vsm_spi_receive_buffer[SPI_BUFFER_LEN];
#pragma arm section rwdata, zidata
kal_uint8 vsm_reg;

static DCL_HANDLE spi_handle = DCL_HANDLE_INVALID;
static SPI_CONFIG_PARAM_T spi_cfg;

/* The same structure as spi driver */
typedef struct
{
   DCL_UINT8 *pu1InData; /* Pointer to the read data */
   DCL_UINT32 u4DataLen; /* Read data length */
   DCL_UINT8 *pu1OutData; /* Pointer to the write data */
   DCL_UINT32 uCount; /* Read data length */
}SPI_CTRL_WRITE_AND_READE_T;

void vsm_spi_speed_modify(kal_int32 speed)
{
    //g_vsm_spi_cfg.clock_frequency = speed;
	//TODO:SPI speed

#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif	
}

void vsm_spi_init(kal_uint32 spi_port, uint32_t spi_macro)
{
	DCL_STATUS spi_result;
	SPI_MODE_T mode_parameter;

	if (spi_handle == DCL_HANDLE_INVALID) {
		spi_handle = DclSPI_Open(DCL_SPI, 0);
		if (DCL_HANDLE_INVALID == spi_handle) 
			{
				LOGE("vsm spi dcl open fail");
				return;
		}
	}	

	/* SPI basic config*/
	spi_cfg.cs_setup_time = 64;		/*cs setup count*/
	spi_cfg.cs_hold_time = 64;		/*cs hold count*/
	spi_cfg.cs_idle_time = 64;		/*cs idle count*/
	spi_cfg.clk_high_time = 32;
	spi_cfg.clk_low_time = 32;
	
	spi_cfg.tx_msbf = SPI_MSBF_LSB;			/*LSB*/
	spi_cfg.rx_msbf = SPI_MSBF_LSB;			/*LSB*/
	spi_cfg.tx_endian = SPI_ENDIAN_LITTLE;
	spi_cfg.rx_endian = SPI_ENDIAN_LITTLE;
	spi_cfg.clk_polarity = SPI_CPOL_B0;		/*CLK_POLARITY=0*/
	spi_cfg.clk_fmt = SPI_CPHA_B0;			/*CLK_PHASE=0*/

	spi_result = DclSPI_Control(spi_handle, (DCL_CTRL_CMD)SPI_IOCTL_SET_CONFIG_PARA, (DCL_CTRL_DATA_T *)&spi_cfg);
	if (STATUS_OK != spi_result)
	{
		LOGE("SPI configure fail:%d", spi_result);
		return;
	}

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
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
	//TODO:DMA CFG 

	return;
}

void vsm_spi_deinit()
{

	DCL_STATUS status;

	status = DclSPI_Close(spi_handle);
	if (STATUS_OK != status)
	{
		LOGE("SPI clse fail");
		return;
	}
	spi_handle = DCL_HANDLE_INVALID;
}

vsm_status_t vsm_spi_write(kal_uint8 addr, kal_uint8 reg, kal_uint8 *data, kal_uint32 size)
{
	vsm_status_t ret = VSM_STATUS_ERROR;
    kal_int32 i = 0;
    kal_int32 write_success = 0;
	DCL_BUFF_LEN write_data_len = 1;
	SPI_CTRL_WRITE_AND_READE_T  write_and_read;

	if (!spi_handle) return VSM_STATUS_UNINITIALIZED;

    /*step 1. Power on 2523 spi*/
	vsm_spi_send_buffer[0] = SPI_PWRO_CMD;
	if (STATUS_OK != DclSPI_WriteData(spi_handle, vsm_spi_send_buffer, 1, &write_data_len, (DCL_OPTIONS)0)) {
        LOGE("[spi write]SPI master send polling failed \r\n");
    } else {
        LOGD("[spi write]SPI master is sending data \r\n");
        /*step 2. Config Write*/
        vsm_spi_send_buffer[0] = SPI_CW_CMD;
        vsm_spi_send_buffer[1] = reg;
        vsm_spi_send_buffer[2] = addr;
        vsm_spi_send_buffer[3] = (size & 0xFF) - 1;
        vsm_spi_send_buffer[4] = (size & 0xFF00)>>8;
		if (STATUS_OK != DclSPI_WriteData(spi_handle, vsm_spi_send_buffer, 5, &write_data_len, (DCL_OPTIONS)0)) {
            LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
        } else {
            LOGD("[spi write]SPI master CW ok, addr 0x%x, reg 0x%x\r\n",addr, reg);
        }

        /*Step 3. Read Status*/
		vsm_spi_receive_buffer[1] = 0;
		vsm_spi_send_buffer[0] = SPI_RS_CMD;        
    	write_and_read.pu1InData = vsm_spi_receive_buffer;
    	write_and_read.u4DataLen = 2;
    	write_and_read.pu1OutData= vsm_spi_send_buffer;
    	write_and_read.uCount=1; 
		if (STATUS_OK != DclSPI_Control(spi_handle, SPI_IOCTL_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_and_read)) {
            LOGE("SPI master send_and_receive polling failed \r\n");
        } else {
            LOGD("status after RS, status_receive[0]=0x%x, status_receive[1]=0x%x\r\n", vsm_spi_receive_buffer[0], vsm_spi_receive_buffer[1]);
            if (vsm_spi_receive_buffer[0] == (SPISLV_STATUS_CFG_SUCCESS_MASK)) {
                /*step 4. Start Data transmission*/
                vsm_array[0] = SPI_WD_CMD;
                for (i = 0; i < size; i ++) {
                    vsm_array[1+i] = data[i];
                }

				if (STATUS_OK != DclSPI_WriteData(spi_handle, vsm_array, size+1, &write_data_len, (DCL_OPTIONS)0)) {
                    LOGE("[spi read]hal_spi_master_send_and_receive_polling failed .\r\n");
                } else {
                    /*Step 5. Check Status*/
					vsm_spi_receive_buffer[1] = 0;
					vsm_spi_send_buffer[0] = SPI_RS_CMD;
    				write_and_read.pu1InData = vsm_spi_receive_buffer;
    				write_and_read.u4DataLen = 2;
    				write_and_read.pu1OutData= vsm_spi_send_buffer;
    				write_and_read.uCount=1;
					if (STATUS_OK != DclSPI_Control(spi_handle, SPI_IOCTL_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_and_read)) {
                        LOGE("SPI master send_and_receive polling failed \r\n");
                    } else {
                        LOGD("after WD, status_receive[0] 0x%x status_receive[1] 0x%x\r\n", vsm_spi_receive_buffer[0], vsm_spi_receive_buffer[1]);
                        if (vsm_spi_receive_buffer[0] & SPI_ERR_MASK) {
                            /*Write Status to clear*/
                            vsm_spi_send_buffer[0] = SPI_WS_CMD;
                            vsm_spi_send_buffer[1] = 0;
							if (STATUS_OK != DclSPI_WriteData(spi_handle, vsm_spi_send_buffer, 2, &write_data_len, (DCL_OPTIONS)0)) {
                                LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
                            } else {
                                LOGD("[spi write]SPI master WS after WD ok\r\n");
                            }
                        } else {
                            write_success = 1;
							LOGD("[spi write]SPI send data ok write_success=%d\r\n", write_success);
                        }
                    }
                }
            } else {
                /*Write Status to clear*/
                vsm_spi_send_buffer[0] = SPI_WS_CMD;
                vsm_spi_send_buffer[1] = 0;
				if (STATUS_OK != DclSPI_WriteData(spi_handle, vsm_spi_send_buffer, 2, &write_data_len, (DCL_OPTIONS)0)) {
                    LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
                } else {
                    LOGD("[spi write]SPI master WS ok\r\n");
                    vsm_spi_receive_buffer[1] = 0;
					vsm_spi_send_buffer[0] = SPI_RS_CMD;
					write_and_read.pu1InData = vsm_spi_receive_buffer;
    				write_and_read.u4DataLen = 2;
    				write_and_read.pu1OutData= vsm_spi_send_buffer;
    				write_and_read.uCount=1; 
                    if (STATUS_OK != DclSPI_Control(spi_handle, SPI_IOCTL_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_and_read)) {
                        LOGE("SPI master send_and_receive polling failed \r\n");
                    } else {
                        LOGD("after write status status_receive[0] 0x%x, status_receive[1] 0x%x\r\n", vsm_spi_receive_buffer[0], vsm_spi_receive_buffer[1]);
                        LOGD("(status_receive[0] & SPI_ERR_MASK) 0x%x\r\n", (vsm_spi_receive_buffer[0] & SPI_ERR_MASK));
                    }
                }
            }
        }
    }

    if (write_success) {
        ret = VSM_STATUS_OK;
    }
    return ret;
}

#ifndef FAST_SPI
vsm_status_t vsm_spi_write_read(kal_uint8 addr, kal_uint8 reg, kal_uint8 *data, uint32_t size)
{
    int32_t ret, i = 0;
    int32_t read_success = 0;
    kal_uint8 read_status_cmd = SPI_RS_CMD;
    kal_uint8 read_data_cmd = SPI_RD_CMD;
    //kal_uint8 write_status_cmd = SPI_WS_CMD;
    kal_uint8 poweron_cmd = 0x0e;
    kal_uint8 status_receive[2] = {0};
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    hal_spi_master_chip_select_timing_t chip_select_timing;

    ret = hal_spi_master_init(g_vsm_spi_master_port, &g_vsm_spi_cfg);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_init failed (%d).\r\n", ret);
        return -1;
    }
    
    chip_select_timing.chip_select_hold_count = 25;
    chip_select_timing.chip_select_idle_count = 25;
    chip_select_timing.chip_select_setup_count = 25;
    hal_spi_master_set_chip_select_timing(g_vsm_spi_master_port, chip_select_timing);

    /*step 1. Power on 2523 spi*/
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, &poweron_cmd, 1)) {
        LOGE("[spi read]SPI master send polling failed \r\n");
    } else {
        LOGD("[spi read]SPI master is sending data \r\n");
        /*step 2. Config Read*/
        vsm_spi_send_buffer[0] = SPI_CR_CMD;
        vsm_spi_send_buffer[1] = reg;
        vsm_spi_send_buffer[2] = addr;
        vsm_spi_send_buffer[3] = (size & 0xFF) - 1;
        vsm_spi_send_buffer[4] = (size & 0xFF00)>>8;        
        //if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 5)) {
            LOGE("[spi read]SPI master send_and_receive polling failed \r\n");
        }

        /*Step 3. Read Status*/
        status_receive[1] = 0;
        spi_send_and_receive_config.receive_length = 2;
        spi_send_and_receive_config.send_length = 1;
        spi_send_and_receive_config.send_data = &read_status_cmd;
        spi_send_and_receive_config.receive_buffer = status_receive;
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
            LOGE("[spi read]SPI master send_and_receive polling failed \r\n");
        } else {
            LOGD("status after config read,status_receive[1] 0x%x\r\n", status_receive[1]);
            //if (status_receive[0] == ( SPISLV_STATUS_CFG_SUCCESS_MASK | SPISLV_STATUS_TXRX_FIFO_RDY_MASK)) {
            if (status_receive[0] & SPISLV_STATUS_CFG_SUCCESS_MASK) {
                /*step 4. Start Data transmission*/
                vsm_spi_send_buffer[0] = read_data_cmd;
                spi_send_and_receive_config.send_data = vsm_spi_send_buffer;
                //spi_send_and_receive_config.send_data = &read_data_cmd;
                spi_send_and_receive_config.send_length = 1;
                //spi_send_and_receive_config.receive_buffer = data;
                spi_send_and_receive_config.receive_buffer = vsm_spi_send_buffer;
                spi_send_and_receive_config.receive_length = size+1;    
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
#else
                ret = hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config);
#endif
                if (HAL_SPI_MASTER_STATUS_OK != ret) {
                    LOGE("[spi read]hal_spi_master_send_and_receive_polling failed (%d).\r\n", ret);
                } else {
                    for (i = 0; i < size; i ++) {
                        if (i < 4) {
                            data[i] = vsm_spi_send_buffer[1+i];
                            LOGD("data[%d]:0x%x\r\n", i, data[i]);
                        }
                    }
                    /*Step 5. Check Status*/
                    status_receive[1] = 0;
                    spi_send_and_receive_config.receive_length = 2;
                    spi_send_and_receive_config.send_length = 1;
                    spi_send_and_receive_config.send_data = &read_status_cmd;
                    spi_send_and_receive_config.receive_buffer = status_receive;
                    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
                        LOGE("SPI master send_and_receive polling failed \r\n");
                    } else {
                        LOGD("after read data status_receive[0] 0x%x status_receive[1] 0x%x\r\n", status_receive[0], status_receive[1]);
                        if (status_receive[0] & SPI_ERR_MASK) {
                            /*Write Status to clear*/
                            vsm_spi_send_buffer[0] = SPI_WS_CMD;
                            vsm_spi_send_buffer[1] = 0;
                            if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 2)) {
                                LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
                            } else {
                                LOGD("[spi write]SPI master WS ok\r\n");
                                status_receive[1] = 0;
                                spi_send_and_receive_config.receive_length = 2;
                                spi_send_and_receive_config.send_length = 1;
                                spi_send_and_receive_config.send_data = &read_status_cmd;
                                spi_send_and_receive_config.receive_buffer = status_receive;
                                if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
                                    LOGD("SPI master send_and_receive polling failed \r\n");
                                } else {
                                    LOGD("after write status status_receive[0] 0x%x, status_receive[1] 0x%x\r\n", status_receive[0], status_receive[1]);
                                    LOGD("(status_receive[0] & SPI_ERR_MASK) 0x%x\r\n", (status_receive[0] & SPI_ERR_MASK));
                                }
                            }
                        } else {
                            read_success = 1;
                        }
                    }
                }
            } else {
                /*Write Status to clear*/
                vsm_spi_send_buffer[0] = SPI_WS_CMD;
                vsm_spi_send_buffer[1] = 0;
                if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 2)) {
                    LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
                } else {
                    LOGD("[spi write]SPI master WS ok\r\n");
                    status_receive[1] = 0;
                    spi_send_and_receive_config.receive_length = 2;
                    spi_send_and_receive_config.send_length = 1;
                    spi_send_and_receive_config.send_data = &read_status_cmd;
                    spi_send_and_receive_config.receive_buffer = status_receive;
                    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
                        LOGD("SPI master send_and_receive polling failed \r\n");
                    } else {
                        LOGD("after write status status_receive[0] 0x%x, status_receive[1] 0x%x\r\n", status_receive[0], status_receive[1]);
                        LOGD("(status_receive[0] & SPI_ERR_MASK) 0x%x\r\n", (status_receive[0] & SPI_ERR_MASK));
                    }
                }
            }
        }
    }
    
    hal_spi_master_deinit(g_vsm_spi_master_port);

    if (!read_success) {
        ret = VSM_STATUS_OK;
    }
    return ret;
}
#else

vsm_status_t vsm_spi_write_read(kal_uint8 addr, kal_uint8 reg, kal_uint8 *data, kal_uint32 size)
{
	vsm_status_t ret = VSM_STATUS_OK;
	DCL_STATUS spi_status = STATUS_OK;
    kal_int32 i = 0;
	DCL_BUFF_LEN write_data_len = 1;
	SPI_CTRL_WRITE_AND_READE_T  write_and_read;

	if (!spi_handle) return VSM_STATUS_UNINITIALIZED;

	// power on
	vsm_spi_send_buffer[0] = SPI_PWRO_CMD;
	spi_status = DclSPI_WriteData(spi_handle, vsm_spi_send_buffer, 1, &write_data_len, (DCL_OPTIONS)0);
	if(STATUS_OK != spi_status){
		LOGE("[vsm_spi_write_read] send pwro cmd failed");
		return VSM_STATUS_ERROR;
	}	

    /*step 1. Config Read*/
    vsm_spi_send_buffer[0] = SPI_CR_CMD;
    vsm_spi_send_buffer[1] = reg;
    vsm_spi_send_buffer[2] = addr;
    vsm_spi_send_buffer[3] = (size & 0xFF) - 1;
    vsm_spi_send_buffer[4] = (size & 0xFF00)>>8;
	spi_status = DclSPI_WriteData(spi_handle, vsm_spi_send_buffer, 5, &write_data_len, (DCL_OPTIONS)0);
	if (STATUS_OK != spi_status) {
        LOGE("[vsm_spi_write_read]SPI master send_and_receive polling failed,port %d \r\n", spi_status);
		return VSM_STATUS_ERROR;
    }
	
    /*step 2. Start Data transmission*/
	vsm_spi_send_buffer[0] = SPI_RD_CMD;
    write_and_read.pu1InData = vsm_spi_receive_buffer;
    write_and_read.u4DataLen = size + 1;
    write_and_read.pu1OutData= vsm_spi_send_buffer;
    write_and_read.uCount=1; 
	spi_status = DclSPI_Control(spi_handle, SPI_IOCTL_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_and_read);
	if (STATUS_OK != spi_status) {
		LOGE("[vsm_spi_write_read]SPI master send_and_receive polling failed \r\n");
		return VSM_STATUS_ERROR;
	} 

    for (i = 0; i < size; i ++) {
        if (i < 4) {
            data[i] = vsm_spi_receive_buffer[1+i];
        }
    }
	
    /* Step 3. Read Status,if data == 0, make sure it's right status */
    if (*((uint32_t *)(data)) == 0) {
		vsm_spi_receive_buffer[1] = 0;
		vsm_spi_send_buffer[0] = SPI_RS_CMD;
    	write_and_read.u4DataLen = 2;
    	write_and_read.pu1OutData= vsm_spi_send_buffer;
		write_and_read.pu1InData = vsm_spi_receive_buffer;
		write_and_read.uCount=1;
		if (STATUS_OK != DclSPI_Control(spi_handle, SPI_IOCTL_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_and_read)) {
			LOGE("[spi read]SPI master send_and_receive polling failed \r\n");
			return VSM_STATUS_ERROR;
		} else {
			LOGE("[spi read]err status 0x%x \r\n", vsm_spi_receive_buffer[0]);                
        	if (vsm_spi_receive_buffer[0] & SPISLV_STATUS_CMD_ERR_MASK || vsm_spi_receive_buffer[0] & SPISLV_STATUS_TIMOUT_ERR_MASK ||
            		vsm_spi_receive_buffer[0] & SPISLV_STATUS_WR_ERR_MASK || vsm_spi_receive_buffer[0] & SPISLV_STATUS_RD_ERR_MASK) {
            	ret = VSM_STATUS_ERROR;
           	} else {
        ret = VSM_STATUS_OK;
    }
		
        }
    }
	
    return ret;
}

vsm_status_t vsm_spi_write_read_retry(uint8_t addr, uint8_t reg, uint8_t *data, uint32_t size)
{
	kal_int32 i = 0;
    DCL_BUFF_LEN write_data_len = 1;
	SPI_CTRL_WRITE_AND_READE_T  write_and_read;
	DCL_STATUS spi_status = STATUS_OK;

    /*Step 1. Read Status*/
	vsm_spi_receive_buffer[1] = 0;
	vsm_spi_send_buffer[0] = SPI_RS_CMD;
	write_and_read.u4DataLen = 2;
	write_and_read.pu1OutData= vsm_spi_send_buffer;
	write_and_read.pu1InData = vsm_spi_receive_buffer;
	write_and_read.uCount=1;
	if (STATUS_OK != DclSPI_Control(spi_handle, SPI_IOCTL_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_and_read)) {
		LOGE("[spi read]SPI master send_and_receive polling failed \r\n");
		return VSM_STATUS_ERROR;
	} else {
		/*Step 2. Clear Status*/
        //LOGE("[spi read]status_receive[0] 0x%x\r\n",status_receive[0]);    
        if (vsm_spi_receive_buffer[0] & SPISLV_STATUS_CMD_ERR_MASK || vsm_spi_receive_buffer[0] & SPISLV_STATUS_TIMOUT_ERR_MASK ||
            vsm_spi_receive_buffer[0] & SPISLV_STATUS_WR_ERR_MASK || vsm_spi_receive_buffer[0] & SPISLV_STATUS_RD_ERR_MASK) {
            LOGE("[spi write]status_receive[0] 0x%x,addr 0x%x, reg 0x%x\r\n", vsm_spi_receive_buffer[0], addr, reg);
            /*Write Status to clear*/
            vsm_spi_send_buffer[0] = SPI_WS_CMD;
            vsm_spi_send_buffer[1] = (vsm_spi_receive_buffer[0] & SPISLV_STATUS_CMD_ERR_MASK) | (vsm_spi_receive_buffer[0] & SPISLV_STATUS_TIMOUT_ERR_MASK) |
                                     (vsm_spi_receive_buffer[0] & SPISLV_STATUS_WR_ERR_MASK)  | (vsm_spi_receive_buffer[0] & SPISLV_STATUS_RD_ERR_MASK);
            LOGE("[spi write]WS data 0x%x \r\n",vsm_spi_send_buffer[1]);
            if (STATUS_OK != DclSPI_WriteData(spi_handle, vsm_spi_send_buffer, 2, &write_data_len, (DCL_OPTIONS)0)) {
                LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
				return VSM_STATUS_ERROR;
            }
        }

        /*step 3. Config Read*/
		vsm_spi_send_buffer[0] = SPI_CR_CMD;
		vsm_spi_send_buffer[1] = reg;
		vsm_spi_send_buffer[2] = addr;
		vsm_spi_send_buffer[3] = (size & 0xFF) - 1;
		vsm_spi_send_buffer[4] = (size & 0xFF00)>>8;
		spi_status = DclSPI_WriteData(spi_handle, vsm_spi_send_buffer, 5, &write_data_len, (DCL_OPTIONS)0);
		if (STATUS_OK != spi_status) {
			LOGE("[vsm_spi_write_read]SPI master send_and_receive polling failed, status %d \r\n", spi_status);
			return VSM_STATUS_ERROR;
		}

		/*step 4. Start Data transmission*/
		vsm_spi_send_buffer[0] = SPI_RD_CMD;
		write_and_read.pu1InData = vsm_spi_receive_buffer;
		write_and_read.u4DataLen = size + 1;
		write_and_read.pu1OutData= vsm_spi_send_buffer;
		write_and_read.uCount=1; 
		spi_status = DclSPI_Control(spi_handle, SPI_IOCTL_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_and_read);
		if (STATUS_OK != spi_status) {
			LOGE("[vsm_spi_write_read]SPI master send_and_receive polling failed \r\n");
			return VSM_STATUS_ERROR;
		}
		for (i = 0; i < size; i ++) {
            if (i < 4) {
                data[i] = vsm_spi_receive_buffer[1+i];
            }
        }
    }
    
    return VSM_STATUS_OK;
}

#endif

#endif /* MT2511_USE_SPI */