
/*****************************************************************************
*  Copyright Statement: 
*  -------------------- 
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *    motion_sensor_custom.c
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This Module is for motion sensor driver.
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(MOTION_SENSOR_SUPPORT)
#include "kal_release.h"
#include "custom_config.h"
#include "gpio_sw.h"
#include "drv_comm.h"
#include "eint.h"
#include "motion_sensor.h"
#include "motion_sensor_custom.h"
#include "motion_sensor_I2C.h"
#include "motion_sensor_hw_define.h"
#include "kal_trace.h"
#include "stack_config.h"

kal_uint32 MS_DELAY_TIME	= 350; 

char MS_sensor_buffer[10];
char MS_sensor_Pcode;
char MS_sensor_CHIPID;

#define MS_DBG

#ifdef MS_DBG
kal_char ms_buff[200];
#endif

kal_bool MS_I2C_write_byte(kal_uint8 ucBufferIndex, kal_uint8 pucData)
{
	ms_i2c_configure(MS_SLAVE_ADDR, 300);
	return ms_i2c_send(MS_SLAVE_ADDR,ucBufferIndex,&pucData,1);
}


#define MS_I2C_read(a,b,c) ms_i2c_receive(MS_SLAVE_ADDR,a,b,c)


kal_bool mcube_i2c_read_buf(kal_uint8 reg, char* buf, kal_uint8 size)
{
	ms_i2c_configure(MS_SLAVE_ADDR, 300);
    return ms_i2c_receive(MS_SLAVE_ADDR, reg, buf, size);
}

/*
#define gsensor_filter(last_data, data)		((( 90* (last_data)) + (10 * (data))) / 100)
static void MC3XXX_HPF(kal_int16 x, kal_int16 y, kal_int16 z)
{

	s32 filter_data_x,filter_data_y,filter_data_z;
	static s32 pre_data_x,pre_data_y,pre_data_z;

	filter_data_x = x*1000;
	filter_data_y = y*1000;
	filter_data_z = z*1000;
	
	filter_data_x = gsensor_filter(pre_data_x, filter_data_x);
	filter_data_y = gsensor_filter(pre_data_y, filter_data_y);
	filter_data_z = gsensor_filter(pre_data_z, filter_data_z);
	
	pre_data_x = filter_data_x;
	pre_data_y = filter_data_y;
	pre_data_z = filter_data_z;
	
	x= filter_data_x/1000;
	y= filter_data_y/1000;
	z= filter_data_z/1000;

}
*/

void mc3xxx_read_xyz(kal_int16 *x,kal_int16 *y,kal_int16 *z)
{	 
	if(MS_sensor_CHIPID==0xA0 && 0x70==(MS_sensor_Pcode&0xF0) )	//3436
	{
	 	 MS_I2C_read(MS_REG_XOUT,MS_sensor_buffer,3);
		 *x=MS_sensor_buffer[0];
	     *y=MS_sensor_buffer[1];
	     *z=MS_sensor_buffer[2];
	}
	else if(0x10==(MS_sensor_Pcode&0xF1))		 //MC3413\3256
	{
	     MS_I2C_read(0x0d,MS_sensor_buffer,6);
		*x=(kal_int16)(MS_sensor_buffer[0] + (MS_sensor_buffer[1] << 8));
		*y= (kal_int16)(MS_sensor_buffer[2] + (MS_sensor_buffer[3] << 8));
		*z=(kal_int16)(MS_sensor_buffer[4] + (MS_sensor_buffer[5] << 8));
	}
	else if(0x60==(MS_sensor_Pcode&0xF1))
	{
	     MS_I2C_read(MS_REG_XOUT,MS_sensor_buffer,3);
		 *x=MS_sensor_buffer[0];
	     *y=MS_sensor_buffer[1];
	     *z=MS_sensor_buffer[2];
	}

	//MC3XXX_HPF(x,y,z);
}

/*It is for mCube time stamp*/
kal_uint32 mc3xxx_get_milli_time(void)
{
   kal_uint32 time;
   kal_get_time(&time);
   return kal_ticks_to_milli_secs(time);
}

void mc3xxx_sensor_get_data(kal_uint16 *x_adc, kal_uint16 *y_adc, kal_uint16 *z_adc)
{
	kal_int16 x_temp,y_temp,z_temp;
	kal_int16 x,y,z;
	kal_uint32 time_ms;
	kal_uint16 acc_0g;
	
	mc3xxx_read_xyz(&x_temp,&y_temp,&z_temp);
	//time_ms = mc3xxx_get_milli_time(); 
	if(0xA0==MS_sensor_CHIPID && 0x70==(MS_sensor_Pcode&0xF0))		//MC3436
	{
		 //kal_prompt_trace(MOD_ENG,"Motion Sensor Raw value is raw_x : %d  ,raw_y : %d   ,raw_z : %d ,time_stamp = %d ",(signed char)x_temp,(signed char)y_temp,(signed char)z_temp,time_ms);
	 
		 x_temp&=0xFF;                              
		 y_temp&=0xFF;
		 z_temp&=0xFF;
		 
		 //kal_prompt_trace(MOD_ENG,"Motion Sensor Raw value is raw_x1 : %d  ,raw_y1 : %d   ,raw_z1 : %d  ",x_temp,y_temp,z_temp);

		 if(x_temp<128)     
		     x=x_temp+127;
		 else
		 {
			 x=x_temp-128;
		 }
		 
		 if(y_temp<128)
		     y=y_temp+127;
		 else
		 {
			 y=y_temp-128;
		 }
		 
		 if(z_temp<128)
		     z=z_temp+127;
		 else
		 {
			 z=z_temp-128;
		 }

		 acc_0g = ACC_MC3436_0G_X;
		 
	}
	else if(0x10==(MS_sensor_Pcode&0xF1))		//MC3413\3256
	{
		kal_prompt_trace(MOD_ENG,"Motion Sensor Raw value is raw_x : %d  ,raw_y : %d   ,raw_z : %d ,time_stamp = %d ",x_temp,y_temp,z_temp);
		x_temp=x_temp&0x3FFF;
		y_temp=y_temp&0x3FFF;
		z_temp=z_temp&0x3FFF;	 

		if(x_temp <8096)
		{
			x=x_temp + 8095;
		}
		else
		{
			x=x_temp - 8096;
		}
		if(y_temp <8096)
		{
			y=y_temp + 8095;
		}
		else
		{
			y=y_temp - 8096;
		}
		if(z_temp <8096)
		{
			z=z_temp + 8095;
		}
		else
		{
			z=z_temp - 8096;
		}

		acc_0g = ACC_MC3413_0G_X;
			
	}	
	else if(0x60==(MS_sensor_Pcode&0xF1))		//MC3433
	{
		 //kal_prompt_trace(MOD_ENG,"Motion Sensor Raw value is raw_x : %d  ,raw_y : %d   ,raw_z : %d ,time_stamp = %d ",(signed char)x_temp,(signed char)y_temp,(signed char)z_temp,time_ms);
	 
		 x_temp&=0xFF;                              
		 y_temp&=0xFF;
		 z_temp&=0xFF;
		 
		 //kal_prompt_trace(MOD_ENG,"Motion Sensor Raw value is raw_x1 : %d  ,raw_y1 : %d   ,raw_z1 : %d  ",x_temp,y_temp,z_temp);

		 if(x_temp<128)     
		     x=x_temp+127;
		 else
		 {
			 x=x_temp-128;
		 }
		 
		 if(y_temp<128)
		     y=y_temp+127;
		 else
		 {
			 y=y_temp-128;
		 }
		 
		 if(z_temp<128)
		     z=z_temp+127;
		 else
		 {
			 z=z_temp-128;
		 }

		 acc_0g = ACC_MC3433_0G_X;
		 
	}

	kal_prompt_trace(MOD_ENG,"Motion Sensor Raw value is raw_x2 : %d  ,raw_y2 : %d   ,raw_z2 : %d  ",x,y,z);

#if defined(MOTION_SENSOR_BACK_0)	
	*x_adc= x;
	*y_adc= y;
	*z_adc= z;
#elif defined(MOTION_SENSOR_BACK_90)
	*x_adc= y;
	*y_adc= 2*acc_0g-x;
	*z_adc= z;
#elif defined(MOTION_SENSOR_BACK_180)
	*x_adc= 2*acc_0g-x;
	*y_adc= 2*acc_0g-y;
	*z_adc= z;
#elif defined(MOTION_SENSOR_BACK_270)
	*x_adc= 2*acc_0g-y;
	*y_adc= x;
	*z_adc= z;
#elif defined(MOTION_SENSOR_FRONT_0)
	*x_adc= 2*acc_0g-x;
	*y_adc= y;
	*z_adc= 2*acc_0g-z;
#elif defined(MOTION_SENSOR_FRONT_90)
	*x_adc= 2*acc_0g-y;
	*y_adc= 2*acc_0g-x;
	*z_adc= 2*acc_0g-z;
#elif defined(MOTION_SENSOR_FRONT_180)
	*x_adc= x;
	*y_adc= 2*acc_0g-y;
	*z_adc= 2*acc_0g-z;
#elif defined(MOTION_SENSOR_FRONT_270)
	*x_adc= y;
	*y_adc= x;
	*z_adc= 2*acc_0g-z;
#endif

#ifdef MS_DBG
	 kal_prompt_trace(MOD_ENG,"Motion Sensor Readed and value is x:%d  ,y:%d   ,z:%d  ",*x_adc,*y_adc,*z_adc);
#endif
}

/* Get tilt status*/

void mc3xxx_sensor_get_status(kal_uint32 *status)
{
	kal_uint8 new_tilt_status;
	kal_uint8 bafr_status;
	kal_uint8 pola_status;
	kal_uint32 time_ms;
	
	MS_I2C_read(MS_REG_TILT,MS_sensor_buffer,1);
	*status = MS_sensor_buffer[0];
	new_tilt_status = *status & (TILT_STATUS_TAP_EVENT | TILT_STATUS_DROP_EVENT | TILT_STATUS_SHAKE_EVENT);
	bafr_status = *status & TILT_STATUS_BAFR_MASK;
	pola_status = *status & TILT_STATUS_POLA_MASK;
	
#if defined(MOTION_SENSOR_BACK_0) //MCUBE_PLACEMENT_TOP_LEFT_DOWN	

		switch (bafr_status)
		{
		case TILT_STATUS_BAFR_FRONT:
			new_tilt_status |= TILT_STATUS_BAFR_FRONT;
			break;

		case TILT_STATUS_BAFR_BACK:
			new_tilt_status |= TILT_STATUS_BAFR_BACK;
			break;
		}
		switch (pola_status)
		{
		case TILT_STATUS_POLA_LEFT:
			new_tilt_status |= TILT_STATUS_POLA_UP;
			break;

		case TILT_STATUS_POLA_RIGHT:
			new_tilt_status |= TILT_STATUS_POLA_DOWN;
			break;

		case TILT_STATUS_POLA_DOWN:
			new_tilt_status |= TILT_STATUS_POLA_LEFT;
			break;

		case TILT_STATUS_POLA_UP:
			new_tilt_status |= TILT_STATUS_POLA_RIGHT;
			break;
		}
		
#elif defined(MOTION_SENSOR_BACK_90) //MCUBE_PLACEMENT_TOP_LEFT_UP

	switch (bafr_status)
		{
		case TILT_STATUS_BAFR_FRONT:
			new_tilt_status |= TILT_STATUS_BAFR_FRONT;
			break;

		case TILT_STATUS_BAFR_BACK:
			new_tilt_status |= TILT_STATUS_BAFR_BACK;
			break;
		}
	switch (pola_status)
		{
		case TILT_STATUS_POLA_LEFT:
			new_tilt_status |= TILT_STATUS_POLA_LEFT;
			break;

		case TILT_STATUS_POLA_RIGHT:
			new_tilt_status |= TILT_STATUS_POLA_RIGHT;
			break;

		case TILT_STATUS_POLA_DOWN:
			new_tilt_status |= TILT_STATUS_POLA_DOWN;
			break;

		case TILT_STATUS_POLA_UP:
			new_tilt_status |= TILT_STATUS_POLA_UP;
			break;
		}
		
#elif defined(MOTION_SENSOR_BACK_180)  //MCUBE_PLACEMENT_TOP_RIGHT_UP
	
	switch (bafr_status)
		{
		case TILT_STATUS_BAFR_FRONT:
			new_tilt_status |= TILT_STATUS_BAFR_FRONT;
			break;

		case TILT_STATUS_BAFR_BACK:
			new_tilt_status |= TILT_STATUS_BAFR_BACK;
			break;
		}
		switch (pola_status)
		{
		case TILT_STATUS_POLA_LEFT:
			new_tilt_status |= TILT_STATUS_POLA_DOWN;
			break;

		case TILT_STATUS_POLA_RIGHT:
			new_tilt_status |= TILT_STATUS_POLA_UP;
			break;

		case TILT_STATUS_POLA_DOWN:
			new_tilt_status |= TILT_STATUS_POLA_RIGHT;
			break;

		case TILT_STATUS_POLA_UP:
			new_tilt_status |= TILT_STATUS_POLA_LEFT;
			break;
		}
		
#elif defined(MOTION_SENSOR_BACK_270) //MCUBE_PLACEMENT_TOP_RIGH_DOWN
	
	switch (bafr_status)
		{
		case TILT_STATUS_BAFR_FRONT:
			new_tilt_status |= TILT_STATUS_BAFR_FRONT;
			break;

		case TILT_STATUS_BAFR_BACK:
			new_tilt_status |= TILT_STATUS_BAFR_BACK;
			break;
		}
	switch (pola_status)
		{
		case TILT_STATUS_POLA_LEFT:
			new_tilt_status |= TILT_STATUS_POLA_RIGHT;
			break;

		case TILT_STATUS_POLA_RIGHT:
			new_tilt_status |= TILT_STATUS_POLA_LEFT;
			break;

		case TILT_STATUS_POLA_DOWN:
			new_tilt_status |= TILT_STATUS_POLA_UP;
			break;

		case TILT_STATUS_POLA_UP:
			new_tilt_status |= TILT_STATUS_POLA_DOWN;
			break;
		}
		
#elif defined(MOTION_SENSOR_FRONT_0) //MCUBE_PLACEMENT_BOTTOM_RIGHT_DOWN
	
	switch (bafr_status)
		{
		case TILT_STATUS_BAFR_FRONT:
			new_tilt_status |= TILT_STATUS_BAFR_BACK;
			break;

		case TILT_STATUS_BAFR_BACK:
			new_tilt_status |= TILT_STATUS_BAFR_FRONT;
			break;
		}
	switch (pola_status) {
		case TILT_STATUS_POLA_LEFT:
			new_tilt_status |= TILT_STATUS_POLA_UP;
			break;

		case TILT_STATUS_POLA_RIGHT:
			new_tilt_status |= TILT_STATUS_POLA_DOWN;
			break;

		case TILT_STATUS_POLA_DOWN:
			new_tilt_status |= TILT_STATUS_POLA_RIGHT;
			break;

		case TILT_STATUS_POLA_UP:
			new_tilt_status |= TILT_STATUS_POLA_LEFT;
			break;
		}
		
#elif defined(MOTION_SENSOR_FRONT_90)  //MCUBE_PLACEMENT_BOTTOM_RIGHT_UP
	
	switch (bafr_status)
		{
		case TILT_STATUS_BAFR_FRONT:
			new_tilt_status |= TILT_STATUS_BAFR_BACK;
			break;

		case TILT_STATUS_BAFR_BACK:
			new_tilt_status |= TILT_STATUS_BAFR_FRONT;
			break;
		}
	switch (pola_status)
		{
		case TILT_STATUS_POLA_LEFT:
			new_tilt_status |= TILT_STATUS_POLA_RIGHT;
			break;

		case TILT_STATUS_POLA_RIGHT:
			new_tilt_status |= TILT_STATUS_POLA_LEFT;
			break;

		case TILT_STATUS_POLA_DOWN:
			new_tilt_status |= TILT_STATUS_POLA_DOWN;
			break;

		case TILT_STATUS_POLA_UP:
			new_tilt_status |= TILT_STATUS_POLA_UP;
			break;
		}
		
#elif defined(MOTION_SENSOR_FRONT_180) //MCUBE_PLACEMENT_BOTTOM_LEFT_UP
	
	switch (bafr_status)
		{
		case TILT_STATUS_BAFR_FRONT:
			new_tilt_status |= TILT_STATUS_BAFR_BACK;
			break;

		case TILT_STATUS_BAFR_BACK:
			new_tilt_status |= TILT_STATUS_BAFR_FRONT;
			break;
		}
	switch (pola_status)
		{
		case TILT_STATUS_POLA_LEFT:
			new_tilt_status |= TILT_STATUS_POLA_DOWN;
			break;

		case TILT_STATUS_POLA_RIGHT:
			new_tilt_status |= TILT_STATUS_POLA_UP;
			break;

		case TILT_STATUS_POLA_DOWN:
			new_tilt_status |= TILT_STATUS_POLA_LEFT;
			break;

		case TILT_STATUS_POLA_UP:
			new_tilt_status |= TILT_STATUS_POLA_RIGHT;
			break;
		}
		
#elif defined(MOTION_SENSOR_FRONT_270) //MCUBE_PLACEMENT_BOTTOM_LEFT_DOWN
	
	switch (bafr_status)
		{
		case TILT_STATUS_BAFR_FRONT:
			new_tilt_status |= TILT_STATUS_BAFR_BACK;
			break;

		case TILT_STATUS_BAFR_BACK:
			new_tilt_status |= TILT_STATUS_BAFR_FRONT;
			break;
		}
		switch (pola_status)
		{
		case TILT_STATUS_POLA_LEFT:
			new_tilt_status |= TILT_STATUS_POLA_LEFT;
			break;

		case TILT_STATUS_POLA_RIGHT:
			new_tilt_status |= TILT_STATUS_POLA_RIGHT;
			break;

		case TILT_STATUS_POLA_DOWN:
			new_tilt_status |= TILT_STATUS_POLA_UP;
			break;

		case TILT_STATUS_POLA_UP:
			new_tilt_status |= TILT_STATUS_POLA_DOWN;
			break;
		}
		
#endif

	*status = new_tilt_status;
	
#ifdef MS_DBG	
	time_ms = mc3xxx_get_milli_time(); /*get time stamp*/
	kal_prompt_trace(MOD_ENG,"Motion Sensor Status = : %x, time_stamp = %d   ",new_tilt_status, time_ms);
#endif

}


/*return sensor support gesture*/
kal_bool mc3xxx_query_gesture(kal_uint16 ms_gest_type)
{
	switch(ms_gest_type)
	{
		case MS_TAP:
			return KAL_TRUE;
			
		case MS_STEP:
			return KAL_TRUE;
			
		case MS_DROP:
			return KAL_TRUE;
			
		//only 3-axis sensor can support Flip gesture, 2-axis not support, return false.
		case MS_FLIP:
			return KAL_TRUE;
			
		default:
			return KAL_FALSE;
	}
}

kal_bool mc3xxx_get_sensor_params(kal_uint16 ms_params_type, MotionSensorQueryStruct *ms_params)
{
	kal_uint32 tilt_status;
	
	switch(ms_params_type)
	{
		case MS_DEV_STATUS:				
			mc3xxx_sensor_get_status(&tilt_status);
			ms_params->curr_val = tilt_status;
			#ifdef MS_DBG
			kal_prompt_trace(MOD_ENG,"MS_DEV_STATUS: curr_val = %x, max_val = %x, min_val = %x",ms_params->curr_val, ms_params->max_val, ms_params->min_val);
			#endif
			return KAL_TRUE;
			
		case MS_TAP_PULSE:
			MS_I2C_read(MS_REG_TAPP,MS_sensor_buffer,1);
			ms_params->curr_val = MS_sensor_buffer[0];
			ms_params->max_val = 0xF;
			ms_params->min_val = 0x0;	
			#ifdef MS_DBG			
			kal_prompt_trace(MOD_ENG,"MS_TAP_PULSE: curr_val = %x, max_val = %x, min_val = %x",ms_params->curr_val, ms_params->max_val, ms_params->min_val);
			#endif
			return KAL_TRUE;
			
		case MS_TAP_THRESHOLD:
			MS_I2C_read(MS_REG_TAP_TH,MS_sensor_buffer,1);
			ms_params->curr_val = MS_sensor_buffer[0];
			ms_params->max_val = 0xFF;
			ms_params->min_val = 0x0;				
			#ifdef MS_DBG
			kal_prompt_trace(MOD_ENG,"MS_TAP_THRESHOLD: curr_val = %x, max_val = %x, min_val = %x",ms_params->curr_val, ms_params->max_val, ms_params->min_val);
			#endif
			return KAL_TRUE;

		case MS_ACC_DATA_RESOLUTION:	
			if(0xA0==MS_sensor_CHIPID&& 0x70==(MS_sensor_Pcode&0xF0))
				ms_params->curr_val = (MC3436_RANGE * 1000)/(1<<MC3436_RESOLUTION);
			else if(0x10==(MS_sensor_Pcode&0xF1))
				ms_params->curr_val = (MC3413_RANGE * 1000)/(1<<MC3413_RESOLUTION);
			else if(0x60==(MS_sensor_Pcode&0xF1))
				ms_params->curr_val = (MC3433_RANGE * 1000)/(1<<MC3433_RESOLUTION);
			#ifdef MS_DBG
			kal_prompt_trace(MOD_ENG,"MS_ACC_DATA_RESOLUTION: curr_val = %x, max_val = %x, min_val = %x",ms_params->curr_val, ms_params->max_val, ms_params->min_val);
			#endif
			return KAL_TRUE;
			
		default:
			return KAL_FALSE;
	}
}

kal_bool mc3xxx_set_sensor_params(kal_uint16 ms_params_type, kal_uint32 ms_params)
{
	switch(ms_params_type)
	{
		case MS_TAP_PULSE:
			if((ms_params>=0)&&(ms_params<=0xF))
			{
				MS_I2C_write_byte(MS_REG_MODE, MS_STANDBY_MODE); //standby mode
				MS_I2C_write_byte(MS_REG_TAPP,ms_params);
				MS_I2C_write_byte(MS_REG_MODE, MS_WAKE_MODE); //wake up mode
				return KAL_TRUE;
			}
			else  // invalid value
			{
				return KAL_TRUE;
			}
							
		case MS_TAP_THRESHOLD:
			if((ms_params>=0)&&(ms_params<=0xFF))
			{
				MS_I2C_write_byte(MS_REG_MODE, MS_STANDBY_MODE); //standby mode
				MS_I2C_write_byte(MS_REG_TAP_TH,ms_params);
				MS_I2C_write_byte(MS_REG_MODE, MS_WAKE_MODE); //wake up mode
				return KAL_TRUE;
			}
			else  // invalid value
			{
				return KAL_TRUE;
			}
			
		default:
			return KAL_FALSE;
	}
}

void mc3xxx_sensor_pwr_up(void)
{
	kal_uint8 temp_buf;
	MS_I2C_write_byte(MS_REG_MODE, MS_WAKE_MODE);
}
void mc3xxx_sensor_pwr_down(void)
{
	MS_I2C_write_byte(MS_REG_MODE, MS_STANDBY_MODE);
}


#if 0
#include "SensorSrvGport.h"
//#include "dll_load_def.h"
 
extern	void DLLWriteLogFileEx(char* fmt,...);

extern SRV_SENSOR_HANDLE srv_sensor_start_listen(srv_sensor_type_enum type, void *option, 
										  SRV_SENSOR_EVENT_HDLR event_hdlr, void *user_data);

extern SRV_SENSOR_HANDLE g_test_handle;
SRV_SENSOR_HANDLE g_test_handle;

void mmi_test_motion_step_callback(
                                srv_sensor_type_enum sensor_type, 
                                void *sensor_data, 
                                void *user_data  )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    module_type module_id;
	static U32 count = 0;
	U32 * P_data;
    
    srv_sensor_motion_tap_struct *p_tap;
    
    U8 text_hight, text_line = 1;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //MMI_ASSERT(SRV_SENSOR_MOTION_TAP == sensor_type);
    
    P_data = (U32 *)user_data;
    count = *P_data;
#ifdef __MMI_PROTOCOL_FILE_TEST__
	DLLWriteLogFileEx("per meter count =%d",count);
#endif
    kal_prompt_trace(MOD_ENG,"callback step= %d",count);
    
}

void mc3xxx_sensor_test(void)
{
	kal_uint16 x_adc=0, y_adc=0, z_adc=0;
	kal_bool temp_return;
	char read_test;
	kal_prompt_trace(MOD_ENG,"///*****************************************************");

	mc3xxx_sensor_pwr_up();
	
	mc3xxx_sensor_get_data(&x_adc,&y_adc,&z_adc);

	kal_prompt_trace(MOD_ENG,"kyler--test--- x_adc=%d,y_adc=%d,z_adc=%d",x_adc,y_adc,z_adc);
	
	temp_return = MS_I2C_read(MS_REG_PCODE,&read_test,1);
	kal_prompt_trace(MOD_ENG,"MS_I2C_read read_test 2= %d,temp_return=%d",read_test,temp_return);

    g_test_handle = 
        srv_sensor_start_listen(SRV_SENSOR_MOTION_STEP , NULL ,
        mmi_test_motion_step_callback, NULL);
	kal_prompt_trace(MOD_ENG,"g_test_handle=%d", g_test_handle);

	kal_prompt_trace(MOD_ENG,"*****************************************************///");
}
#endif

int motion_sensor_state = -1;

void mc3xxx_sensor_init(void)
{
	kal_uint8 temp_buf[2] = {0x00,0x00};
#ifdef MS_DBG
	sprintf(ms_buff,"mc3xxx_sensor_init start");
	kal_bootup_print(ms_buff);
#endif

	 ms_i2c_configure(MS_SLAVE_ADDR, MS_DELAY_TIME); 
	  
	 mc3xxx_sensor_pwr_up(); // you can change default level the Int Pin  by this function . Now default level is low .
	 //get chipid
	 MS_I2C_read(MS_REG_CHIPID,MS_sensor_buffer,1);
	 MS_sensor_CHIPID =  MS_sensor_buffer[0];
	kal_prompt_trace(MOD_MMI,"%s--g-sensor id:%x",__FUNCTION__, MS_sensor_CHIPID);
#ifdef MS_DBG
	sprintf(ms_buff,"mc3xxx chip id = %d",MS_sensor_buffer[0]);//34x6 it should be 0xA0
	kal_bootup_print(ms_buff);
#endif

	//get Product code
	MS_I2C_read(MS_REG_PCODE,MS_sensor_buffer,1);

	MS_sensor_Pcode = MS_sensor_buffer[0];

#if defined MS_DBG
	sprintf(ms_buff,"mc3xxx Product code = %d",MS_sensor_buffer[0]);
	kal_bootup_print(ms_buff);
#endif
	kal_prompt_trace(MOD_MMI,"%s--MS_sensor_Pcode:%x",__FUNCTION__, MS_sensor_Pcode);	 
	mc3xxx_sensor_pwr_down();//standby mode

	/* Sample Rate Controls */
	 //if(MS_sensor_CHIPID==0xA0 && 0x70==(MS_sensor_Pcode&0xF0) ) // 3436 
     if(MS_sensor_CHIPID==0xA0)	//3416
	//if((MS_sensor_CHIPID==0xA0) &&(0x40==(MS_sensor_Pcode&0xF0))) //3416
	 {
		MS_I2C_write_byte(0x08,0x00); // 128hz
		MS_I2C_write_byte(0x20,0x11); // 4g    2^8/(2*2)=64  1G = 64
		
		//anymotion threshold
		MS_I2C_write_byte(0x43,0x55); // Lsb     eg. 0x555 it means 1G  . 0x555/2 means 1/2G
		MS_I2C_write_byte(0x44,0x05); // Msb
		//anymotion debounce 
		MS_I2C_write_byte(0x45,0x88); // bigger and  easier to be triggered..
		//set intterupt type
		MS_I2C_write_byte(0x06,0x44); //Set Interrupt type  .See Spec 04 means anymotion with auto clear
		//Enable anymotion detect
		MS_I2C_write_byte(0x09,0x04); 
		// latch time 
		MS_I2C_write_byte(0x4A,0xA0); 
	 }
	 else if(0x10==(MS_sensor_Pcode&0xF1))		//MC3413\3256
	 {
	 	MS_I2C_write_byte(0x08,0x00);
		MS_I2C_write_byte(0x20,0x25); 
		/////////set tap int///////////
		MS_I2C_write_byte(0x09,0x00);//enable tap detect on x ,y and z
		MS_I2C_write_byte(0x0A,0x0A);//
		MS_I2C_write_byte(0x0B,0x0A);//
		MS_I2C_write_byte(0x0C,0x0A);//

		MS_I2C_write_byte(0x09,0x40);//enable tap detect on x ,y and z
		MS_I2C_write_byte(0x0A,0x30);//threshold of tap x
		MS_I2C_write_byte(0x0B,0x30);//threshold of tap y
		MS_I2C_write_byte(0x0C,0x30);//threshold of tap z

		MS_I2C_write_byte(0x09,0xFF);

		MS_I2C_write_byte(0x06,0x3F);//set int type  'tap'
		////////set tap end/////////// 
	 }
	 else if(0x60==(MS_sensor_Pcode&0xF1))	//mc3433
	 {
		MS_I2C_write_byte(0x08,0x0A);//128hz
		MS_I2C_write_byte(0x20,0x02);//8bit 
		/////////set tap int///////////
		MS_I2C_write_byte(0x09,0xFF);//enable tap detect on x ,y and z
		MS_I2C_write_byte(0x0A,0x20);//threshold of tap x
		MS_I2C_write_byte(0x0B,0x20);//threshold of tap y
		MS_I2C_write_byte(0x0C,0x20);//threshold of tap z
		MS_I2C_write_byte(0x06,0x3F);//set int type  'tap'
		////////set tap end///////////
	 }
	 else 
	 {
		MS_I2C_write_byte(0x08,0x00);
		MS_I2C_write_byte(0x20,0x0F);
	 }

	 if(motion_sensor_state == -1)
	 {
		 if (0x10==(MS_sensor_Pcode&0xF1))
		 {
			 motion_sensor_state = 1;		 //G-sensor OK
		 }
		 else
		 {
			 motion_sensor_state = 0;		 //G-sensor Fail
		 }
	 }
	//mc3xxx_sensor_pwr_up();
}

/*customizaton function pointer*/
MotionSensor_customize_function_struct ms_custom_func=
{
     ms_get_data,
     mc3xxx_sensor_get_data,     
     mc3xxx_sensor_init,
     mc3xxx_sensor_pwr_up,
     mc3xxx_sensor_pwr_down,
     NULL,
	 NULL,
	 NULL,
	 NULL,
     mc3xxx_query_gesture,
     mc3xxx_get_sensor_params,
     mc3xxx_set_sensor_params
};   
/*customizaton data*/
MotionSensor_custom_data_struct  ms_custom_data_def_mc3433 = 
{          
   /*X axis*/
   ACC_MC3433_0G_X,   
   ACC_MC3433_1G_X,
   ACC_MC3433_MINUS1G_X,     
   /*Y axis*/
   ACC_MC3433_0G_Y,   
   ACC_MC3433_1G_Y,
   ACC_MC3433_MINUS1G_Y,     
   /*Z axis*/
   ACC_MC3433_0G_Z,   
   ACC_MC3433_1G_Z,
   ACC_MC3433_MINUS1G_Z,
   /*support interrupt or not*/
   KAL_FALSE,
   0,
   0/*channel*/    
};
MotionSensor_custom_data_struct  ms_custom_data_def_mc3413 = 
{          
   /*X axis*/
   ACC_MC3413_0G_X,   
   ACC_MC3413_1G_X,
   ACC_MC3413_MINUS1G_X,     
   /*Y axis*/
   ACC_MC3413_0G_Y,   
   ACC_MC3413_1G_Y,
   ACC_MC3413_MINUS1G_Y,     
   /*Z axis*/
   ACC_MC3413_0G_Z,   
   ACC_MC3413_1G_Z,
   ACC_MC3413_MINUS1G_Z,
   /*support interrupt or not*/
   KAL_FALSE,
   0,
   0/*channel*/    
};
MotionSensor_custom_data_struct  ms_custom_data_def_mc3436 = 
{          
   /*X axis*/
   ACC_MC3436_0G_X,   
   ACC_MC3436_1G_X,
   ACC_MC3436_MINUS1G_X,     
   /*Y axis*/
   ACC_MC3436_0G_Y,   
   ACC_MC3436_1G_Y,
   ACC_MC3436_MINUS1G_Y,     
   /*Z axis*/
   ACC_MC3436_0G_Z,   
   ACC_MC3436_1G_Z,
   ACC_MC3436_MINUS1G_Z,
   /*support interrupt or not*/
   KAL_FALSE,
   0,
   0/*channel*/    
};

MotionSensor_custom_data_struct * (ms_get_data)(void)
{
	if(MS_sensor_CHIPID==0xA0 && 0x70==(MS_sensor_Pcode&0xF0))  //here , pls make sure you have got the value of MS_sensor_Pcode 
	{
		return (&ms_custom_data_def_mc3436);
	}
	else if(0x10==(MS_sensor_Pcode&0xF1))		//MC3413\3256
	{
   		return (&ms_custom_data_def_mc3413);
	}
	else if(0x60==(MS_sensor_Pcode&0xF1))	//mc3433
	{
		return (&ms_custom_data_def_mc3433);
	}
	
} 

MotionSensor_customize_function_struct * (ms_GetFunc)(void)
{
     return &(ms_custom_func);
}

/*return value: 0: is ok    other: is failed*/

kal_bool MC34XX_read_data_test(void)
{
#if 0
	bma250acc_t accel;
	int i = 0,j=0;
	unsigned char dat[63];
	unsigned char *data;
	data=dat;
	//acc_sensor_init();
	//acc_sensor_init_with_interrupt();
	for(i=0;i<5;i++)
	{
		bma250_read_reg(0x00,data,0x3f);
		for(j=0;j<63;j++)
		{
			kal_prompt_trace(MOD_MMI,"reg data[%d]: %x",j,data[j]);
		}
		bma250_read_accel_xyz(&accel);
		kal_prompt_trace(MOD_MMI, "%s:acc_x: %d, acc_y: %d, acc_z: %d", __FUNCTION__, accel.x, accel.y, accel.z);
		System_Delay_1ms(10);
	}
#endif
	return KAL_TRUE;
}

#endif
