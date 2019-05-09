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

#include "sensor_alg_interface.h"
#include "stdio.h"
#include "algo_adaptor/algo_adaptor.h"
#include "stdint.h"
#include "common_nvram_editor_data_item.h"
#include "nvram_common_defs.h"

#define BP_ENABLE 1
#define BP_GET_ECG_ERROR 1
#define BP_ECG_ERROR_INTERUPT 0 

#if BP_ENABLE
#include "bp_alg.h"
//#include "nvdm.h"
#include <stdlib.h> 
#endif

#if BP_GET_ECG_ERROR
static int32_t ecg_error_count;
static int32_t ecg_error_status;
#endif
#define MAUI_LIB

// Revision History:
// v.1.0.0.1: 2016/03/16 init (Shu-Yu Hsu)
// v.1.0.0.2: 2016/05/20 add ppg_control (Shu-Yu Hsu)
// v.1.0.0.6: 2016/06/24 modify scenario for data collection experiments (Shu-Yu Hsu)
// v.1.0.0.7: 2016/07/04 modify scenario for data collection experiments (Shu-Yu Hsu)
// v.1.1.0.1: 2016/07/15 add relization of bp library (Chih-Ming Fu)
// v.1.1.0.2: 2016/07/27 modify pwtt output in bp library (Chih-Ming Fu)

// PPG control config
#if defined(MTK_SENSOR_BIO_USE_MT2511)
#define BP_PPG_CTRL_ON
#endif

#if defined(BP_PPG_CTRL_ON)
#include "vsm_driver.h"
#include "ppg_control.h"
#endif

#if defined(__UPDATE_BINARY_FILE__) && defined(__UPDATE_PKG_SENSOR_ONLY__)
#include "ubin_symbol_wrapper.h"
#endif

#define EXPECT_ECG_LENGTH (512*1*25)
#define EXPECT_PPG_LENGTH (512*2*25)

#define BP_PPG_BUF_SIZE 10
#define BP_FS_PPG_HZ 512
#define BP_PPG_BITWIDTH 23

uint32_t bp_cnt_ecg;
uint32_t bp_cnt_ppg;
int32_t bp_switch;
int32_t timer_ppg;

#if BP_ENABLE
#define MAX_NUM_PWTT_ONCE 8
bp_alg_out_t bp_output;
int32_t num_pwtt_out;
int32_t pwtt_out[MAX_NUM_PWTT_ONCE];
int32_t bp_height;
int32_t bp_weight;
int32_t bp_gender;
int32_t bp_age;
int32_t bp_handlen;
#endif

#if defined(BP_PPG_CTRL_ON)
    int32_t bp_ppg_buf[BP_PPG_BUF_SIZE];
    int32_t bp_ppg_buf_cnt;
    uint32_t bp_agc_reg_tx;
    uint32_t bp_agc_reg_rx;
    uint32_t bp_init_agc_flag;    
    //uint32_t cnt_saturate_pos;
    //uint32_t cnt_saturate_neg;
    int32_t bp_ambdac;
    int32_t bp_led_current;
#endif

/* syslog */
//#define LOGI(fmt,arg...)   LOG_I(sensor, "[BP]: "fmt,##arg)
//#define LOGE(fmt,arg...)   LOG_E(sensor, "[BP]: "fmt,##arg)
/* printf*/
#define LOGE(fmt, args...)    printf("[BP] ERR: "fmt, ##args)
#define LOGD(fmt, args...)    printf("[BP] DBG: "fmt, ##args)

#define BLOOD_PRESSURE_MONITOR_PPG1_512HZ_POLLING_TIME 32
#define BLOOD_PRESSURE_MONITOR_ECG_POLLING_TIME 32

static sensor_input_list_t input_blood_pressure_monitor_comp_ppg1;
static sensor_input_list_t input_blood_pressure_monitor_comp_ecg;

//static uint32_t fusion_bp_prev_notify_time;

#if BP_GET_ECG_ERROR
static int32_t get_ecg_error(int32_t data_curr)
{
  static int32_t data_pre;
  if (data_curr == data_pre || data_curr-data_pre> 25000 || data_pre - data_curr> 25000)
  {
    ecg_error_count ++;
    
  }
  data_pre = data_curr;
  if(ecg_error_count>1536)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
#endif

static int32_t get_blood_pressure_monitor_result(sensor_data_t *const output)
{
    sensor_data_unit_t *bp_data = output->data;    
    if(output == NULL || output->data == NULL){
      return -1;
    }

   
    bp_data->sensor_type = SENSOR_TYPE_BLOOD_PRESSURE_MONITOR;
    bp_data->time_stamp = 0;
    bp_data->blood_pressure_t.dbp = -2;
    bp_data->blood_pressure_t.sbp = -2;
    bp_data->blood_pressure_t.status = 2;   
#if BP_ENABLE                
#if BP_GET_ECG_ERROR
    if (ecg_error_status)
    {
      bp_data->blood_pressure_t.dbp = -1;
      bp_data->blood_pressure_t.sbp = -1;
      bp_data->blood_pressure_t.bpm = -1;
      bp_data->blood_pressure_t.pwtt = NULL;
      bp_data->blood_pressure_t.numPwtt = 0;            
      bp_data->blood_pressure_t.status = ecg_error_status;
	  num_pwtt_out = 0;
    }
    else
    {
#endif    
    if (num_pwtt_out!=0)
    {
        bp_data->blood_pressure_t.dbp = 0;
        bp_data->blood_pressure_t.sbp = 0;
        bp_data->blood_pressure_t.bpm = 0;
        bp_data->blood_pressure_t.pwtt = pwtt_out;
        bp_data->blood_pressure_t.numPwtt = num_pwtt_out;            
          bp_data->blood_pressure_t.status = ecg_error_status;   
        num_pwtt_out = 0;
    }
    else if(num_pwtt_out==0)
    {
          bp_data->blood_pressure_t.dbp = bp_output.dbp;
          bp_data->blood_pressure_t.sbp = bp_output.sbp;
          //bp_data->blood_pressure_t.sbp = bp_output.num_pwtt; 
          //bp_data->blood_pressure_t.dbp = bp_output.pwtt[0]; 
          //bp_data->blood_pressure_t.sbp = bp_gender; 
          //bp_data->blood_pressure_t.dbp = bp_handlen;
        bp_data->blood_pressure_t.bpm = bp_output.bpm;
        bp_data->blood_pressure_t.pwtt = NULL;
        bp_data->blood_pressure_t.numPwtt = 0;
          bp_data->blood_pressure_t.status = ecg_error_status; 
        num_pwtt_out = 0;  
      }
#if BP_GET_ECG_ERROR
  }
#endif
#endif  
    bp_data->blood_pressure_t.status = ecg_error_status;    

    return 1;
}

static int32_t blood_pressure_monitor_process_data(const sensor_data_t *input_list, void *reserve)
{
    int32_t ret = 1;
    int32_t count = input_list->data_exist_count;
    sensor_data_unit_t *data_start = input_list->data;
    uint32_t input_sensor_type = input_list->data->sensor_type;
//    uint32_t input_time_stamp = input_list->data->time_stamp;
    int32_t i;
    
#if defined(BP_PPG_CTRL_ON)
    bus_data_t ppg_reg_info;
//    uint32_t ppg_reg_value;
    ppg_control_t ppg1_ctrl_input;
    int32_t ppg_control_output[2];
    int32_t ppg_adjust_flag;    
#endif //#if defined(BP_PPG_CTRL_ON)
    
    // stop BP application when data is already 45-sec
    if (bp_cnt_ecg >= EXPECT_ECG_LENGTH && bp_cnt_ppg >= EXPECT_PPG_LENGTH) {        
        bp_switch=0;
#if BP_ENABLE    
    #ifdef MAUI_LIB
        bp_alg_get_bp(&bp_output);
    #endif
        for(i=0;i<bp_output.num_pwtt;i++)
        {
            pwtt_out[num_pwtt_out] = bp_output.pwtt[i];
            num_pwtt_out++;
            if ( (num_pwtt_out==MAX_NUM_PWTT_ONCE) || (i==(bp_output.num_pwtt-1)) )
            {
                sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
            }
        }
#endif
        sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
    }

    if(input_sensor_type == SENSOR_TYPE_BIOSENSOR_PPG1_512HZ) {
        //bp_cnt_ppg+=count;
        timer_ppg+=count;
        
#if defined(BP_PPG_CTRL_ON)
        if(bp_cnt_ppg >= 10240) {
            bp_init_agc_flag = 1;
        }
#endif
        while (count != 0) {
#if BP_ENABLE           
            if (bp_cnt_ppg%2==0)
            {
              #ifdef MAUI_LIB
              bp_alg_process_data( data_start->bio_data.data, 1);
              #endif
            }
            bp_cnt_ppg++;
#endif             
#if defined(BP_PPG_CTRL_ON)
            if(bp_init_agc_flag == 0) {
                bp_ppg_buf[bp_ppg_buf_cnt] = data_start->bio_data.data;
                bp_ppg_buf_cnt++;

                if(bp_ppg_buf_cnt >= BP_PPG_BUF_SIZE) {
                    ppg1_ctrl_input.input = bp_ppg_buf;
                    ppg1_ctrl_input.input_fs = BP_FS_PPG_HZ;
                    ppg1_ctrl_input.input_length = bp_ppg_buf_cnt;
                    ppg1_ctrl_input.input_bit_width = BP_PPG_BITWIDTH;
                    ppg1_ctrl_input.input_config = 1;
                    ppg1_ctrl_input.input_source = 1;
                    ppg_adjust_flag = ppg_control_process(&ppg1_ctrl_input, PPG_CONTROL_MODE_DUAL1, ppg_control_output);
    
                    if(ppg_adjust_flag == 1) {
                        ppg_reg_info.reg  = (0x332C & 0x00FF);
                        ppg_reg_info.addr = (0x332C & 0xFF00) >> 8;
                        ppg_reg_info.data_buf = (uint8_t *)&bp_agc_reg_tx;
                        ppg_reg_info.length = sizeof(bp_agc_reg_tx);
                        vsm_driver_read_register(&ppg_reg_info);
                        ppg_reg_info.reg  = (0x3318 & 0x00FF);
                        ppg_reg_info.addr = (0x3318 & 0xFF00) >> 8;
                        ppg_reg_info.data_buf = (uint8_t *)&bp_agc_reg_rx;
                        ppg_reg_info.length = sizeof(bp_agc_reg_rx);
                        vsm_driver_read_register(&ppg_reg_info);
                        bp_led_current = bp_agc_reg_tx & 0x000000FF;
                        bp_ambdac = (bp_agc_reg_rx >> 22) & 0x00000007;
                    }
                    bp_ppg_buf_cnt -= BP_PPG_BUF_SIZE;
                }
            }
/*             
            else if(bp_cnt_ppg & 0x00000001 == 1) {
                // per 15-sec DC adjustment
                int32_t value = (data_start->bio_data.data);
                int32_t max_value = 1<<23;
                int32_t max_value_div2 = 1<<(22);
                value = (value<max_value_div2)?value:(value-max_value);
                
                if(value > 18000){
                    cnt_saturate_pos++;
                } else if(value < -24000){
                    cnt_saturate_neg++;
                }
                
                if((bp_cnt_ppg >= 14848 && bp_cnt_ppg < 15000) || (bp_cnt_ppg >= 30208 && bp_cnt_ppg < 30300) ){ //14.5s || 29.5s
                    if(cnt_saturate_pos > 1024){
                        if(bp_ambdac < 6){
                            bp_ambdac++;
                            if(bp_led_current > 8) {
                                bp_led_current -= 8;
                            }
                        }else if(bp_led_current > (1+16)){
                            bp_led_current -= 16;
                        }
                    }else if(cnt_saturate_neg > 1024){
                        if(bp_ambdac > 0){
                            bp_ambdac--;
                            if(bp_led_current < 255-8){
                                bp_led_current += 8;
                            }
                        }else if(bp_led_current < (255-16)){
                            bp_led_current += 16;
                        }
                    }
                    
                    if((cnt_saturate_pos > 1024)||(cnt_saturate_neg > 1024)){
                        cnt_saturate_pos = 0;
                        cnt_saturate_neg = 0;
                        bp_led_current = bp_led_current & 0x000000FF;
                        bp_agc_reg_tx = (bp_led_current<<8) + bp_led_current;
                        bp_agc_reg_rx = (bp_agc_reg_rx & 0xFC3FFFFF) + (bp_ambdac<<22);                        
                        ppg_reg_info.reg = (0x332C & 0x00FF);
                        ppg_reg_info.addr = (0x332C & 0xFF00) >> 8;
                        ppg_reg_info.data_buf = (uint8_t *)&bp_agc_reg_tx;
                        ppg_reg_info.length = sizeof(bp_agc_reg_tx);
                        vsm_driver_write_register(&ppg_reg_info);
                        ppg_reg_info.reg = (0x3318 & 0x00FF);
                        ppg_reg_info.addr = (0x3318 & 0xFF00) >> 8;
                        ppg_reg_info.data_buf = (uint8_t *)&bp_agc_reg_rx;
                        ppg_reg_info.length = sizeof(bp_agc_reg_rx);
                        vsm_driver_write_register(&ppg_reg_info);
                        vsm_driver_update_register();
                    }
                } else if( (bp_cnt_ppg == 15000) || (bp_cnt_ppg == 30300)){
                    cnt_saturate_pos = 0;
                    cnt_saturate_neg = 0;
                }
            }
*/            
#endif //#if defined(BP_PPG_CTRL_ON)
            data_start++;
            count--;

        }//while count
        
    } else if(input_sensor_type == SENSOR_TYPE_BIOSENSOR_EKG) {
        bp_cnt_ecg+=count;
        while (count != 0) {
#if BP_ENABLE             
#if BP_GET_ECG_ERROR
            ecg_error_status = get_ecg_error(data_start->bio_data.data);
#if BP_ECG_ERROR_INTERUPT            
            if (ecg_error_status==1)
            {
              sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
            }
#endif
#endif            
            #ifdef MAUI_LIB
            bp_alg_process_data( data_start->bio_data.data, 0);
            #endif
#endif
            data_start++;
            count--;
        }
    }

    return ret;
}


static int32_t blood_pressure_monitor_operate(int32_t command, void *buffer_out, int32_t size_out, \
                                 void *buffer_in, int32_t size_in)
{
    /* To handle user input of age, sex, height, weight information. */
    return 0;
}

const sensor_descriptor_t blood_pressure_monitor_desp = {
    SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, //output_type
    1, /* version */
    SENSOR_REPORT_MODE_ON_CHANGE, /* report_mode */
    {0, 0}, /*sensor_capability_t */ 
    &input_blood_pressure_monitor_comp_ppg1, /* sensor_input_list_t */
    blood_pressure_monitor_operate,
    get_blood_pressure_monitor_result,
    blood_pressure_monitor_process_data,
    0 /* accumulate */
};

int blood_pressure_monitor_register(void)
{
    int ret; /*return: fail=-1, pass>=0, which means the count of current register algorithm */

    input_blood_pressure_monitor_comp_ppg1.input_type = SENSOR_TYPE_BIOSENSOR_PPG1_512HZ;
    input_blood_pressure_monitor_comp_ppg1.sampling_delay = BLOOD_PRESSURE_MONITOR_PPG1_512HZ_POLLING_TIME; // ms

    input_blood_pressure_monitor_comp_ecg.input_type = SENSOR_TYPE_BIOSENSOR_EKG;
    input_blood_pressure_monitor_comp_ecg.sampling_delay = BLOOD_PRESSURE_MONITOR_ECG_POLLING_TIME; // ms

    input_blood_pressure_monitor_comp_ppg1.next_input = &input_blood_pressure_monitor_comp_ecg; // build as a signal linked list
    input_blood_pressure_monitor_comp_ecg.next_input = NULL;

    ret = sensor_fusion_algorithm_register_type(&blood_pressure_monitor_desp); //[SY]: ???
    if (ret < 0) {
        LOGE("fail to register blood pressure monitor \r\n");
    }
    ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 1);
    if (ret < 0) {
        LOGE("fail to register buffer \r\n");
    }
    return ret;
}

#if BP_ENABLE  
void read_nvram_data()
{
#ifdef __MMI_BLOOD_PRESSURE_SUPPORT__
	int ret_val;
	nvram_ef_bp_info_struct bp_info;

	memset(&bp_info, 0, NVRAM_EF_BP_INFO_SIZE);
	ret_val = nvram_external_read_data(NVRAM_EF_BP_INFO_LID, 1, (kal_uint8 *)&bp_info, NVRAM_EF_BP_INFO_SIZE);

	if (ret_val)
	{
		bp_height = bp_info.height;
		bp_weight = bp_info.weight;
		bp_gender = bp_info.gender;
		bp_age = bp_info.age;
		bp_handlen = bp_info.armlen;
	}
#endif /*__MMI_BLOOD_PRESSURE_SUPPORT__*/
}
#endif

int blood_pressure_monitor_init(void)
{
    bp_cnt_ecg = 0;
    bp_cnt_ppg = 0;
#if defined(BP_PPG_CTRL_ON)
    bp_init_agc_flag = 0;
    timer_ppg = 0;
    bp_ppg_buf_cnt = 0;
    //cnt_saturate_pos = 0;
    //cnt_saturate_neg = 0;
    
    // Call PPG control lib init
    ppg_control_init();
    ppg_control_set_app(PPG_CONTROL_APP_BP);
    
    bp_agc_reg_tx = 0;
    bp_agc_reg_rx = 0;
    bp_led_current = bp_agc_reg_tx & 0x000000FF;
    bp_ambdac = (bp_agc_reg_rx >> 22) & 0x00000007;
#endif //#if defined(BP_PPG_CTRL_ON)
#if BP_ENABLE                  
    bp_output.sbp = 0;
    bp_output.dbp = 0;
    bp_output.bpm = 0;
    num_pwtt_out = 0;
#if BP_GET_ECG_ERROR
    ecg_error_count = 0;
#endif   
    //read_nvram_data();
    #ifdef MAUI_LIB
    bp_alg_init();
    bp_alg_set_user_info(bp_age, bp_gender, bp_height, bp_weight, bp_handlen);
    #endif
#endif
    return 1;
}

