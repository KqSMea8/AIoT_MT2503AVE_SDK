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

#include "string.h"
#include "stdio.h"
#include "kal_general_types.h"
#include "app_ltlcom.h"
#include "task_config.h"
#include "syscomp_config.h"
#include "custom_util.h"
#include "stack_ltlcom.h"
#include "stack_config.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
#include "kal_release.h"
#include "kal_internal_api.h"
#include "sensor_alg_interface.h"
#include "sensor_manager.h"
#include "dcl.h"
#include "drv_comm.h"
#include "MMI_features.h"
#include "TimerEvents.h"
#include "MMITimer.h"

#if defined(__UPDATE_BINARY_FILE__) && defined(__UPDATE_PKG_SENSOR_ONLY__)
#include "custom_ubin_irt.h"
extern kal_uint32 UBIN_mmi_symbol_DummyReference();
extern kal_uint32 UBIN_mmi_symbol_DummyReference();
#endif

#if defined(MTK_SENSOR_ACCELEROMETER_USE_BMA255)
#include "bma255_sensor_adaptor.h"
#endif

#ifdef MTK_SENSOR_BIO_USE_MT2511
#include "vsm_sensor_subsys_adaptor.h"
#endif

#if (defined(FUSION_PEDOMETER_USE) && (FUSION_PEDOMETER_USE==M_INHOUSE_PEDOMETER)) \
    || (defined(FUSION_HEART_RATE_MONITOR_USE) && (FUSION_HEART_RATE_MONITOR_USE==M_INHOUSE_HEART_RATE_MONITOR)) \
    || (defined(FUSION_SLEEP_TRACKER_USE) && (FUSION_SLEEP_TRACKER_USE==M_INHOUSE_SLEEP_TRACKER)) \
    || (defined(FUSION_HEART_RATE_VARIABILITY_USE) && (FUSION_HEART_RATE_VARIABILITY_USE==M_INHOUSE_HEART_RATE_VARIABILITY)) \
    || (defined(FUSION_BLOOD_PRESSURE_USE) && (FUSION_BLOOD_PRESSURE_USE==M_INHOUSE_BLOOD_PRESSURE))
/* fusion algorithm adaptor */
#include "algo_adaptor/algo_adaptor.h"
#endif

#if (defined(FUSION_HEART_RATE_MONITOR_USE) && (FUSION_HEART_RATE_MONITOR_USE==M_INHOUSE_HEART_RATE_MONITOR))
  #ifdef HEART_RATE_MONITOR_OUT_PATTERN
    int golden_idx = 0;
    int golden_total = 0;
    uint32_t hr_last_update_time;
const uint32_t hr_output[80] =  {
80,
80,
80,
80,
80,
80,
80,
80,
79,
79,
78,
77,
77,
77,
80,
79,
79,
79,
80,
82,
82,
82,
82,
77,
74,
72,
71,
70,
69,
69,
71,
72,
74,
76,
78,
76,
75,
78,
80,
82,
84,
85,
87,
88,
90,
91,
97,
98,
100,
101,
102,
101,
101,
100,
101,
98,
99,
99,
99,
99,
100,
101,
103,
104,
105,
106,
107,
108,
109,
110,
111,
112,
113,
114,
115,
116,
117,
118,
119,
120};

  #endif
#endif

#if (defined(FUSION_HEART_RATE_VARIABILITY_USE) && (FUSION_HEART_RATE_VARIABILITY_USE==M_INHOUSE_HEART_RATE_VARIABILITY))
  #ifdef HEART_RATE_VARIABILITY_OUT_PATTERN
    #include "output_hrv.h"
    uint32_t hrv_start_time;
    uint32_t hrv_reported;
  #endif
#endif

#if (defined(FUSION_BLOOD_PRESSURE_USE) && (FUSION_BLOOD_PRESSURE_USE==M_INHOUSE_HEART_RATE_VARIABILITY))
  #ifdef BLOOD_PRESSURE_OUT_PATTERN
    uint32_t bp_start_time;
    uint32_t bp_reported;
  #endif
#endif

#define SENSOR_SPP_DATA_MAGIC       54321
#define SENSOR_SPP_DATA_RESERVED    12345
#define SENSOR_TYPE_INTERNAL_PWTT    8001

static int32_t acc_buff_data_array[16] = {SENSOR_SPP_DATA_MAGIC,SENSOR_TYPE_ACCELEROMETER,0,0,0,0,0,0,0,0,0,0,0,0,0,SENSOR_SPP_DATA_RESERVED};
static uint32_t ppg1_buff_data_array[16] = {SENSOR_SPP_DATA_MAGIC,SENSOR_TYPE_BIOSENSOR_PPG1,0,0,0,0,0,0,0,0,0,0,0,0,0,SENSOR_SPP_DATA_RESERVED};
static uint32_t PPG1_512HZ_buff_data_array[16] = {SENSOR_SPP_DATA_MAGIC,SENSOR_TYPE_BIOSENSOR_PPG1_512HZ,0,0,0,0,0,0,0,0,0,0,0,0,0,SENSOR_SPP_DATA_RESERVED};
static int32_t hrm_buff_data_array[16] = {SENSOR_SPP_DATA_MAGIC,SENSOR_TYPE_HEART_RATE_MONITOR,0,0,0,0,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED};
static int32_t bpm_buff_data_array[16] = {SENSOR_SPP_DATA_MAGIC,SENSOR_TYPE_BLOOD_PRESSURE_MONITOR,0,0,0,0,0,0,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED};
static int32_t bpm_pwtt_buff_data_array[16] = {SENSOR_SPP_DATA_MAGIC,SENSOR_TYPE_INTERNAL_PWTT,0,0,0,0,0,0,0,0,0,0,0,0,0,SENSOR_SPP_DATA_RESERVED};
static uint32_t ekg_buff_data_array[16] = {SENSOR_SPP_DATA_MAGIC,SENSOR_TYPE_BIOSENSOR_EKG,0,0,0,0,0,0,0,0,0,0,0,0,0,SENSOR_SPP_DATA_RESERVED};
static int acc_buff_count = 0;
static int ppg1_buff_count = 0;
static int ekg_buff_count = 0;
static int32_t acc_seq = 1;
static int32_t ppg1_seq = 1;
static int32_t hrm_seq = 1;
static int32_t bpm_seq = 1;
static int32_t pwtt_seq = 1;
static int32_t ekg_seq = 1;

/*----------------------------------------------------------------*/
/* Macro                                                          */
/*----------------------------------------------------------------*/ 
/*printf*/
#define SM_ERR(fmt, args...)	kal_prompt_trace(MOD_MMI,"[SensorManager]ERR: "fmt, ##args)
#define SM_WARN(fmt, args...)	kal_prompt_trace(MOD_MMI,"[SensorManager]WARN: ",fmt, ##args)
/* syslog */
//#define SENSOR_MANAGER_LOG_DISABLE
#ifndef SENSOR_MANAGER_LOG_DISABLE
#define SM_INFO(fmt,arg...)		kal_prompt_trace(MOD_MMI,"[SensorManager]INFO: "fmt,##arg)
#else
#define SM_INFO(fmt,arg...)     
#endif
//#define SM_DEBUG
#ifdef SM_DEBUG
#define SM_DBG(fmt, args...)	kal_prompt_trace(MOD_MMI,"[SensorManager]DBG: "fmt,##args)
#else
#define SM_DBG(fmt, args...)    
#endif

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

//#define SDK_PESUDO_PEDOMETER
#define ACC_MAX_BUF_CNT 32
#define GYRO_MAX_BUF_CNT 8
#define ACTIVITY_MAX_BUF_CNT 10
#define GESTURE_MAX_BUF_CNT 10
#define BIO_MAX_BUF_CNT 128

typedef struct {
    uint32_t start_time;
    sensor_data_unit_t mgr_acc_data[ACC_MAX_BUF_CNT];
    uint32_t mgr_acc_delay;
    sensor_data_unit_t ppg1_data[BIO_MAX_BUF_CNT];
    sensor_data_unit_t ppg2_data[BIO_MAX_BUF_CNT];
    sensor_data_unit_t ekg_data[BIO_MAX_BUF_CNT];
    sensor_data_unit_t bisi_data[BIO_MAX_BUF_CNT];
    uint32_t data_updated; // bitmap for which physical sensor updated

    sensor_descriptor_t heart_rate_monitor_descriptor;
    sensor_descriptor_t heart_rate_variability_descriptor;
    sensor_descriptor_t blood_pressure_descriptor;

    sensor_data_unit_t heart_rate_monitor_data;
    sensor_data_unit_t heart_rate_variability_data;
    sensor_data_unit_t blood_pressure_data;
    uint32_t polling_period;
} sensor_manager_t;


/*----------------------------------------------------------------*/
/* Global Variable                                                */
/*----------------------------------------------------------------*/ 
int32_t sensor_send_digest_callback(sensor_data_t *const output);
#ifdef FUSION_HEART_RATE_MONITOR_USE
sensor_subscriber_t heart_rate_monitor_subscriber = {
    "ap21", 0, SENSOR_TYPE_HEART_RATE_MONITOR, 0, sensor_send_digest_callback
};
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
sensor_subscriber_t blood_pressure_subscriber = {
    "ap23", 0, SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 0, sensor_send_digest_callback
};
#endif

#define MMI_TIMER
#if defined(EVEN_TIMER)
	event_scheduler *sensor_timer;
	eventid sensor_event_scheduler_id;
#elif defined(GPT_TIMER)
	DCL_HANDLE sensor_gpt_handle = 0xFF;
#elif defined(MMI_TIMER)
	kal_timerid sensor_timer;
#endif

static sensor_manager_t smt;

kal_timerid test_timer;
static sensor_data_t this_acc_data;
static sensor_data_t this_bio_ppg1_data;
static sensor_data_t this_bio_ppg2_data;
static sensor_data_t this_bio_ecg_data;
static sensor_data_t this_bio_bisi_data;

uint32_t xSMLastExecutionTime;

static sensor_data_t this_acc_data;
static sensor_data_t this_bio_ppg1_data;
static sensor_data_t this_bio_ecg_data;
static sensor_data_t this_bio_bisi_data;

static int is_timer_stop = 1;

//+++ sensor throttling
#define SENSOR_DRIVER_MAX_DELAY 0x7FFFFFFF

struct sensor_control_data {
    uint32_t enable;
    uint32_t delay;
};
static struct sensor_control_data all_sensor_ctrl_data[SENSOR_TYPE_ALL];
sensor_subscriber_t subscriber_list[SENSOR_TYPE_ALL];//TODO change to multi-app sub 1 sensor

/*----------------------------------------------------------------*/
/* Function                                                       */
/*----------------------------------------------------------------*/ 
#ifdef __MMI_BLOOD_PRESSURE_SUPPORT__
extern void mmi_blood_pressure_send_data(U8 *data, U32 data_len);
extern void mmi_blood_pressure_update_data(U8 *data, U32 data_len);
extern void mmi_heart_rate_update_data(U8 *data, U32 data_len);
extern void mmi_heart_rate_send_data(U8 *data, U32 data_len);
#else
void mmi_blood_pressure_send_data(U8 *data, U32 data_len){}
void mmi_blood_pressure_update_data(U8 *data, U32 data_len){}
void mmi_heart_rate_send_data(U8 *data, U32 data_len){}
void mmi_heart_rate_update_data(U8 *data, U32 data_len){}
#endif

static int32_t 	sensor_manager_init(void);
static int32_t 	sensor_throttling_control(uint32_t sensor_type, uint32_t enable, uint32_t delay);
static uint32_t is_sensor_enabled(uint32_t sensor_type);
static void 	sensor_TimerCallback(void *arg);
static uint32_t get_defined_sensor();
/*----------------------------------------------------------------*/ 

static void sensor_timer_init(void)
{
	//SM_DBG("sensor_timer_init");
#if defined(EVEN_TIMER)
    sensor_timer = evshed_create("Sensor_timer", MOD_SNRSUBSYS, 0, 255);
    evshed_set_index(sensor_timer, 1); 
#elif defined(GPT_TIMER)
	sensor_gpt_handle = DclSGPT_Open(DCL_GPT_CB, 0);
#elif defined(MMI_TIMER)
    
#endif
}

void sensor_set_timer(kal_uint32 ms)
{
	//SM_DBG("sensor_set_timer");
#if defined(EVEN_TIMER)
	sensor_event_scheduler_id = evshed_set_event(sensor_timer, 
												(kal_timer_func_ptr) sensor_TimerCallback, 
												NULL, 
												(ms/KAL_MILLISECS_PER_TICK+1));
#elif defined(GPT_TIMER)
	DCL_STATUS status;
	SGPT_CTRL_START_T start;

	start.u2Tick=ms/gptimer_interval+1;
	start.pfCallback = sensor_TimerCallback;
	start.vPara=NULL;
	status = DclSGPT_Control(sensor_gpt_handle, SGPT_CMD_START,(DCL_CTRL_DATA_T*)&start);
        
    if(status != STATUS_OK)
    {
    	SM_DBG("sensor_set_timer:failed");
    	ASSERT(0);  		
    }	
#elif defined(MMI_TIMER)
    StartTimer(SENSOR_SUBSYS_TIMER, ms, sensor_TimerCallback);
#endif
}


void sensor_stop_timer(void)
{
	//SM_DBG("sensor_stop_timer");
#if defined(EVEN_TIMER)
	evshed_cancel_event(sensor_timer, &sensor_event_scheduler_id);
#elif defined(GPT_TIMER)
	DclSGPT_Control(sensor_gpt_handle, SGPT_CMD_STOP, 0);
#elif defined(MMI_TIMER)
	StopTimer(SENSOR_SUBSYS_TIMER);
#endif
}

void send_sensor_msg_to_mmi(uint32_t sensor_type, uint8_t *data, uint32_t data_len)
{
#if 0 /*dump buffer*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif

	switch(sensor_type)	{
		case SENSOR_TYPE_ACCELEROMETER:
		case SENSOR_TYPE_BIOSENSOR_PPG1:
			mmi_heart_rate_send_data(data, data_len);
			break;
		case SENSOR_TYPE_BIOSENSOR_PPG1_512HZ:
		case SENSOR_TYPE_BIOSENSOR_EKG:
			mmi_blood_pressure_send_data(data, data_len);
			break;
		case SENSOR_TYPE_HEART_RATE_MONITOR:
			mmi_heart_rate_update_data(data, data_len);
			break;
		case SENSOR_TYPE_BLOOD_PRESSURE_MONITOR:
		case SENSOR_TYPE_INTERNAL_PWTT:
			mmi_blood_pressure_update_data(data, data_len);
			break;
		default:
			break;
	}

}


int32_t sensor_send_digest_callback(sensor_data_t *const output)
{
    uint32_t i = 0;
    uint32_t bunch_count = 0;
	int32_t ppg1_amb_flag =1;

	SM_DBG("sensor_send_digest_callback:sensor_type=%d", output->data[0].sensor_type);

    switch (output->data[0].sensor_type) {

        case SENSOR_TYPE_ACCELEROMETER:
			SM_INFO("sensor_send_digest_callback:SENSOR_TYPE_ACCELEROMETER, cnt=%d",output->data_exist_count);
			for (i=0; i<output->data_exist_count; i++) {		
				if (acc_buff_count < 12) {
					acc_buff_data_array[acc_buff_count+3] = output->data[i].accelerometer_t.x;
                	acc_buff_data_array[acc_buff_count+3+1] = output->data[i].accelerometer_t.y;
                	acc_buff_data_array[acc_buff_count+3+2] = output->data[i].accelerometer_t.z;
                	acc_buff_data_array[acc_buff_count+3+3] = output->data[i].time_stamp;
                	acc_buff_count += 4;
				}
			    if (acc_buff_count == 12) {
					acc_buff_data_array[2] = acc_seq;
					send_sensor_msg_to_mmi(SENSOR_TYPE_ACCELEROMETER, (uint8_t *)acc_buff_data_array, sizeof(acc_buff_data_array));
					acc_seq++;
                	acc_buff_count = 0;
            	}
			}
            break;

        case SENSOR_TYPE_BIOSENSOR_PPG1_512HZ:
			SM_INFO("sensor_send_digest_callback:SENSOR_TYPE_BIOSENSOR_PPG1_512HZ, cnt=%d",output->data_exist_count);
            for (i=0; i<output->data_exist_count; i++) {
				// SENSOR_TYPE_BIOSENSOR_PPG1_512HZ only sample ppg , no amb
	            if(ppg1_amb_flag==1){     
	                if (ppg1_buff_count < 12) {
	                    PPG1_512HZ_buff_data_array[ppg1_buff_count+3] = output->data[i].bio_data.data;
	                    ppg1_buff_count++;
	                }
	                if (ppg1_buff_count == 12) {
						PPG1_512HZ_buff_data_array[2] = ppg1_seq;
						send_sensor_msg_to_mmi(SENSOR_TYPE_BIOSENSOR_PPG1_512HZ, (uint8_t *)PPG1_512HZ_buff_data_array, sizeof(PPG1_512HZ_buff_data_array));
	                    ppg1_seq++;
	                    ppg1_buff_count = 0;
	                }
	                ppg1_amb_flag = 0;
	            } else {
	                ppg1_amb_flag = 1;
	            }
            }
            break;

        case SENSOR_TYPE_BIOSENSOR_PPG1:
			SM_INFO("sensor_send_digest_callback:SENSOR_TYPE_BIOSENSOR_PPG1, cnt=%d",output->data_exist_count);
			for (i=0; i<output->data_exist_count; i++) {
				if (ppg1_buff_count < 12) {
					ppg1_buff_data_array[ppg1_buff_count+3] = output->data[i].bio_data.data;
					ppg1_buff_count++;
				}
				if (ppg1_buff_count == 12) {
					ppg1_buff_data_array[2] = ppg1_seq;
					send_sensor_msg_to_mmi(SENSOR_TYPE_BIOSENSOR_PPG1, (uint8_t *)ppg1_buff_data_array, sizeof(ppg1_buff_data_array));
					ppg1_seq++;
					ppg1_buff_count = 0;
				}			
			}
            break;
        case SENSOR_TYPE_BIOSENSOR_PPG2:
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
#endif
            break;
        case SENSOR_TYPE_BIOSENSOR_BISI:
#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
            break;
        case SENSOR_TYPE_BIOSENSOR_EKG:
			SM_INFO("sensor_send_digest_callback:SENSOR_TYPE_BIOSENSOR_EKG, cnt=%d",output->data_exist_count);
			for (i=0; i<output->data_exist_count; i++) {
				if (ekg_buff_count < 12) {
					ekg_buff_data_array[ekg_buff_count+3] = output->data[i].bio_data.data;
					ekg_buff_count++;
				}
				if (ekg_buff_count == 12) {
					ekg_buff_data_array[2] = ekg_seq;
					send_sensor_msg_to_mmi(SENSOR_TYPE_BIOSENSOR_EKG, (uint8_t *)ekg_buff_data_array, sizeof(ekg_buff_data_array));
					ekg_seq++;
					ekg_buff_count = 0;
				}			
			}
            break;      

        case SENSOR_TYPE_HEART_RATE_MONITOR:

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
#ifdef DTP_ENABLE
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif /*DTP_ENABLE*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#else
            SM_INFO("sensor_send_digest_callback:SENSOR_TYPE_HEART_RATE_MONITOR, cnt=%d", output->data_exist_count);
			for (i=0; i<output->data_exist_count; i++) {
				hrm_buff_data_array[2] = hrm_seq;
				hrm_buff_data_array[3] = output->data[i].heart_rate_t.bpm;
				hrm_buff_data_array[4] = output->data[i].heart_rate_t.status;
				hrm_buff_data_array[5] = output->data[i].time_stamp;;
				send_sensor_msg_to_mmi(SENSOR_TYPE_HEART_RATE_MONITOR, (uint8_t *)hrm_buff_data_array, sizeof(hrm_buff_data_array));
				hrm_seq++;
				SM_INFO("*****************[HRM]bpm:%d*********************",output->data[0].heart_rate_t.bpm);
			}
#endif
            break;

        case SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR: {
#if 0
/* under construction !*/
#ifdef GT_PROJECT_ENABLE
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
#ifdef DTP_ENABLE
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif /*DTP_ENABLE*/
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
#ifdef FUSION_HEART_RATE_VARIABILITY_USE
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
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
            break;
        }

        case SENSOR_TYPE_BLOOD_PRESSURE_MONITOR: {
            if (output->data[0].blood_pressure_t.pwtt == NULL) {
#ifdef GT_PROJECT_ENABLE
                if (queAppMgr != NULL) {
                    outEvent.event = EVENT_UPDATE_BP;
                    outEvent.userdata = &output->data[0].blood_pressure_t;
                    xQueueSend(queAppMgr, &outEvent, 0);
                }
#endif

                SM_INFO("Blood pressure sbp(%d), dbp(%d), status(%d) \r\n", output->data[0].blood_pressure_t.sbp,
                                 output->data[0].blood_pressure_t.dbp, output->data[0].blood_pressure_t.status);
				SM_INFO("sensor_send_digest_callback:SENSOR_TYPE_BLOOD_PRESSURE_MONITOR");
				for (i=0; i<output->data_exist_count; i++) {
					bpm_buff_data_array[2] = bpm_seq;
					bpm_buff_data_array[3] = output->data[i].blood_pressure_t.sbp;
					bpm_buff_data_array[4] = output->data[i].blood_pressure_t.dbp;
					bpm_buff_data_array[5] = output->data[i].blood_pressure_t.bpm;
					bpm_buff_data_array[6] = output->data[i].blood_pressure_t.status;
					bpm_buff_data_array[7] = output->data[i].time_stamp;
					send_sensor_msg_to_mmi(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, (uint8_t *)bpm_buff_data_array, sizeof(bpm_buff_data_array));
					bpm_seq++;
					SM_INFO("*****************[BPM]bpm:%d*********************",output->data[i].blood_pressure_t.bpm);
				}

#ifdef FUSION_BLOOD_PRESSURE_USE
				if (sensor_unsubscribe_sensor(blood_pressure_subscriber.handle) < 0) {
					SM_ERR("B.P unsubscription fail\r\n");
				} else {
					SM_INFO("B.P. unsubscribed\r\n");
				}
#endif
            } else { /* pwtt data*/
            	for (i=0; i<output->data_exist_count; i++) {
					kal_int32 j = 0;
            		bpm_pwtt_buff_data_array[2] = pwtt_seq;
                	//bpm_pwtt_buff_data_array[3] = 0; /* feature type*/
                	bpm_pwtt_buff_data_array[4] = output->data[i].blood_pressure_t.status; /* status*/
                	bpm_pwtt_buff_data_array[5] = output->data[i].time_stamp; /* timestamp */
                	bunch_count = output->data[i].blood_pressure_t.numPwtt;
                	if (bunch_count > 9) {
                    	bunch_count = 9; /* maximum data  to transmit is 9 */
	                }
                	while (j < bunch_count) {
                    	bpm_pwtt_buff_data_array[6 + j] = output->data[i].blood_pressure_t.pwtt[j];
                    	j++;
                	}
                	/*SM_INFO("pwtt (%d) in B.P. %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", output->data[i].blood_pressure_t.numPwtt, bpm_pwtt_buff_data_array[0],
                                 		bpm_pwtt_buff_data_array[1], bpm_pwtt_buff_data_array[2], bpm_pwtt_buff_data_array[3], bpm_pwtt_buff_data_array[4], bpm_pwtt_buff_data_array[5], bpm_pwtt_buff_data_array[6],
                                 		bpm_pwtt_buff_data_array[7], bpm_pwtt_buff_data_array[8], bpm_pwtt_buff_data_array[9], bpm_pwtt_buff_data_array[10], bpm_pwtt_buff_data_array[11]);*/
					send_sensor_msg_to_mmi(SENSOR_TYPE_INTERNAL_PWTT, (uint8_t *)bpm_pwtt_buff_data_array, sizeof(bpm_pwtt_buff_data_array));
                	pwtt_seq++;
            	}
			}
            break;
		}

		default:
            //SM_INFO("type = %lu,  timestamp = %lu \r\n",
            //                 output->data[0].sensor_type,
            //                 output->data[0].time_stamp);
            break;
    }

    return 0;
}

static void sensor_TimerCallback(void *arg)
{
    uint32_t sensor_type = 0;
    int32_t buf_out = 0;
	int32_t err;
    //uint32_t xSMLastExecutionTim = sensor_driver_get_ms_tick();
    //uint32_t xSMLastExecutionTim = ust_get_current_time();
    //uint32_t xSMLastExecutionTim = INT_GetCurrentTime();
    //kal_sleep_task(ms/KAL_MILLISECS_PER_TICK+1);
    xSMLastExecutionTime = sensor_driver_get_ms_tick();

    if ( sensor_driver_get_enable(SENSOR_TYPE_ACCELEROMETER) == 1) {
        SM_DBG("get g sensor data");
        buf_out = 0;
        /*Accelerometer*/
        sensor_type = SENSOR_TYPE_ACCELEROMETER;
        err = sensor_driver_get_data(sensor_type, smt.mgr_acc_data, \
                               ACC_MAX_BUF_CNT * sizeof(sensor_data_unit_t), &buf_out);	
        if (buf_out > 0 && err >= 0) {
            smt.data_updated |= (1U << SENSOR_TYPE_ACCELEROMETER);
            this_acc_data.data_exist_count = buf_out;
            this_acc_data.data = smt.mgr_acc_data;
			sensor_send_digest_callback(&this_acc_data);
        }
    }

    /* bio sensor */
    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_PPG1) == 1 ) {
        SM_DBG("get ppg1 data");
        buf_out = 0;
        sensor_type = SENSOR_TYPE_BIOSENSOR_PPG1;
        err = sensor_driver_get_data(sensor_type, &(smt.ppg1_data), \
                               BIO_MAX_BUF_CNT, &buf_out);
        if (buf_out > 0 && err >= 0) {
            //smt.ppg1_data[ppg1_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1);
            this_bio_ppg1_data.data_exist_count = buf_out;
            this_bio_ppg1_data.data = smt.ppg1_data;
			sensor_send_digest_callback(&this_bio_ppg1_data);
        }
    }

    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_PPG2) == 1 ) {
        buf_out = 0;
        sensor_type = SENSOR_TYPE_BIOSENSOR_PPG2;
        sensor_driver_get_data(sensor_type, &(smt.ppg2_data), \
                               BIO_MAX_BUF_CNT, &buf_out);
        if (buf_out > 0) {
            //smt.ppg2_data[ppg2_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_PPG2);
            this_bio_ppg2_data.data_exist_count = buf_out;
            this_bio_ppg2_data.data = smt.ppg2_data;
        }
    }

    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_BISI) == 1 ) {
        buf_out = 0;
        sensor_type = SENSOR_TYPE_BIOSENSOR_BISI;
        sensor_driver_get_data(sensor_type, &(smt.bisi_data), \
                               BIO_MAX_BUF_CNT, &buf_out);
        if (buf_out > 0) {
           // smt.bisi_data[bisi_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_BISI);
            this_bio_bisi_data.data_exist_count = buf_out;
            this_bio_bisi_data.data = smt.bisi_data;
        }
    }

    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_EKG) == 1 ) {
		SM_DBG("get ekg data");
        buf_out = 0;
        sensor_type = SENSOR_TYPE_BIOSENSOR_EKG;
        sensor_driver_get_data(sensor_type, &(smt.ekg_data), \
                               BIO_MAX_BUF_CNT, &buf_out);
        if (buf_out > 0) {
            //smt.ekg_data[ekg_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_EKG);
            this_bio_ecg_data.data_exist_count = buf_out;
            this_bio_ecg_data.data = smt.ekg_data;
			sensor_send_digest_callback(&this_bio_ecg_data);
        }
    }

    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_PPG1_512HZ) == 1 ) {
		SM_DBG("get ppg1_512hz data");
        buf_out = 0;
        sensor_type = SENSOR_TYPE_BIOSENSOR_PPG1_512HZ;
        sensor_driver_get_data(sensor_type, &(smt.ppg1_data), \
                            BIO_MAX_BUF_CNT, &buf_out);
        if (buf_out > 0) {
            //smt.ppg1_data[ppg1_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1_512HZ);
            this_bio_ppg1_data.data_exist_count = buf_out;
            this_bio_ppg1_data.data = smt.ppg1_data;
			sensor_send_digest_callback(&this_bio_ppg1_data);
        }
    }
    /* Feed data to Fusion algorithm */
    /* HR */
#if defined(FUSION_HEART_RATE_MONITOR_USE)
    if (is_sensor_enabled(SENSOR_TYPE_HEART_RATE_MONITOR)) {
        SM_DBG("heart rate algo");
      #ifndef HEART_RATE_MONITOR_OUT_PATTERN
        if (smt.data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                smt.heart_rate_monitor_descriptor.process_data(&this_acc_data, NULL);
        }
        if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)){
            /* process biosensor data */
          smt.heart_rate_monitor_descriptor.process_data(&this_bio_ppg1_data, NULL);
        }
      #else
            SM_INFO("xSMLastExecutionTime %u, hr_last_update_time %u", xSMLastExecutionTime, hr_last_update_time);

        if ((xSMLastExecutionTime - hr_last_update_time) > 1000UL) {
            SM_INFO("heart rate notify");
            /* update every second*/
            sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_MONITOR);
            hr_last_update_time = xSMLastExecutionTime;
        }
      #endif
    }
#endif

    /* HRV */
#if defined(FUSION_HEART_RATE_VARIABILITY_USE)
    if (is_sensor_enabled(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR)) {
        #ifndef HEART_RATE_VARIABILITY_OUT_PATTERN
        if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
            smt.heart_rate_variability_descriptor.process_data(&this_bio_ecg_data, NULL);
        }
        #else
        if (((xSMLastExecutionTime-hrv_start_time) > 10000UL) && (hrv_reported == 0)) {
            /* send HRV output pattern after subscription 10 seconds */
            sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR);
            hrv_reported = 1; /* one-shot notification */
        }
        #endif
    }
#endif

    /* BP */
#if defined(FUSION_BLOOD_PRESSURE_USE)
    if (is_sensor_enabled(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR)) {
		SM_DBG("blood pressure algo");
    #ifndef BLOOD_PRESSURE_OUT_PATTERN
        if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1_512HZ)) {
            smt.blood_pressure_descriptor.process_data(&this_bio_ppg1_data, NULL);
        }

        if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
            smt.blood_pressure_descriptor.process_data(&this_bio_ecg_data, NULL);
        }
    #else
        if (((xSMLastExecutionTime-bp_start_time) > 10000UL) && (bp_reported == 0)) {
            /* send BP output pattern after subscription 10 seconds */
            sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
            bp_reported = 1; /* one-shot notification */
        } else if (((xSMLastExecutionTime-bp_start_time) > 20000UL) && (bp_reported == 1)) {
            /* send pwtt output pattern after subscription 20 seconds */
            sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
            bp_reported = 2; /* one-shot notification */
        }
    #endif
    }
#endif

	if (!is_timer_stop)
		sensor_set_timer(smt.polling_period);

	SM_INFO("send data to fusion over!");
}

#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
    #if defined(MTK_SENSOR_ACCELEROMETER_USE_BMA255)
/* under construction !*/
/* under construction !*/
    #endif
/* under construction !*/
    #ifdef MTK_SENSOR_BIO_USE_MT2511
/* under construction !*/
/* under construction !*/
    #endif
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#if 1//def HEART_RATE_MONITOR_OUT_PATTERN
/* under construction !*/
/* under construction !*/
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
                #ifdef EVEN_TIMER
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

int32_t sensor_manager_init(void)
{
    uint32_t i = 0;

    memset(&smt, 0, sizeof(sensor_manager_t));
    for (i = 0; i < ACC_MAX_BUF_CNT; i++) {
        smt.mgr_acc_data[i].sensor_type = SENSOR_TYPE_ACCELEROMETER;
    }

    for (i = 0; i < BIO_MAX_BUF_CNT; i++) {
        smt.ppg1_data[i].sensor_type = SENSOR_TYPE_BIOSENSOR_PPG1;
        smt.ppg2_data[i].sensor_type = SENSOR_TYPE_BIOSENSOR_PPG2;
        smt.ekg_data[i].sensor_type = SENSOR_TYPE_BIOSENSOR_EKG;
        smt.bisi_data[i].sensor_type = SENSOR_TYPE_BIOSENSOR_BISI;
    }
    smt.polling_period = 20; //ms

#if defined(FUSION_HEART_RATE_MONITOR_USE)
    heart_rate_monitor_register();
    #ifdef HEART_RATE_MONITOR_OUT_PATTERN
    golden_total = sizeof(hr_output) / sizeof(hr_output[0]);
    #endif
#endif

#if defined(FUSION_HEART_RATE_VARIABILITY_USE)
    heart_rate_variability_monitor_register();
#endif

#if defined(FUSION_BLOOD_PRESSURE_USE)
    blood_pressure_monitor_register();
#endif

    return 1;
}


int32_t sensor_fusion_algorithm_register_type(const sensor_descriptor_t *desp)
{
    SM_INFO("sensor_fusion_algorithm_register_type(%d) \r\n", desp->sensor_type);
    if (SENSOR_TYPE_HEART_RATE_MONITOR == desp->sensor_type) {
        memcpy(&(smt.heart_rate_monitor_descriptor), desp, sizeof(sensor_descriptor_t));
    } else if (SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR == desp->sensor_type) {
        memcpy(&(smt.heart_rate_variability_descriptor), desp, sizeof(sensor_descriptor_t));
    } else if (SENSOR_TYPE_BLOOD_PRESSURE_MONITOR == desp->sensor_type) {
        memcpy(&(smt.blood_pressure_descriptor), desp, sizeof(sensor_descriptor_t));
    } else {
        return -1;
    }
    return 1;
}

int32_t sensor_fusion_algorithm_register_data_buffer(uint32_t sensor_type, uint32_t exist_data_count)
{
    SM_INFO("sensor_fusion_algorithm_register_data_buffer(%d), Count(%d) \r\n", sensor_type, exist_data_count);

    switch (sensor_type) {
        //case SENSOR_TYPE_SLEEP:
        //    smt.sleep_data_buffer_number = exist_data_count;
        //    break;

        default:
            if (exist_data_count > 1) {
                SM_ERR("sensor_fusion_algorithm_register_data_buffer for more than 1 NOT support... \r\n");
                return -1;
            }
            break;
    }

    return 1;
}

int32_t sensor_fusion_algorithm_notify(uint32_t sensor_type)
{
    sensor_data_t algorithm_result;
    int32_t ret = 1;
    int i;
	sensor_data_t * output;  //removed const? MT2511
  #ifdef BLOOD_PRESSURE_OUT_PATTERN
    int32_t pwtt_out_pattern[9] = {11, 22, 33, 44, 55, 66, 77, 88, 99};
  #endif
  #ifdef HEART_RATE_VARIABILITY_OUT_PATTERN
    int32_t beatpos_out_pattern[8] = {7, 14, 21, 28, 35, 42, 49, 56};
  #endif

    /* Not consider one result containing multiple data */
    algorithm_result.data_exist_count = 1;
    algorithm_result.fifo_max_size = 0;

    SM_DBG("sensor_fusion_algorithm_notify(%d)\r\n", sensor_type);
    if (SENSOR_TYPE_HEART_RATE_MONITOR== sensor_type) {
        if (smt.heart_rate_monitor_descriptor.get_result != NULL) {
            algorithm_result.data = &(smt.heart_rate_monitor_data);

        #ifndef HEART_RATE_MONITOR_OUT_PATTERN
            smt.heart_rate_monitor_descriptor.get_result(&algorithm_result);
        #else
            smt.heart_rate_monitor_data.sensor_type = SENSOR_TYPE_HEART_RATE_MONITOR;
            smt.heart_rate_monitor_data.time_stamp = xSMLastExecutionTime;
            smt.heart_rate_monitor_data.heart_rate_t.bpm = hr_output[golden_idx];
            golden_idx = (golden_idx+1)%golden_total;
        #endif
            SM_INFO("Heart rate monitor on-change: %d, %d\r\n", smt.heart_rate_monitor_data.time_stamp, smt.heart_rate_monitor_data.heart_rate_t.bpm);
        }
    }  else if (SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR == sensor_type) {
        if (smt.heart_rate_variability_descriptor.get_result != NULL) {
            algorithm_result.data = &(smt.heart_rate_variability_data);
            #ifndef HEART_RATE_VARIABILITY_OUT_PATTERN
            smt.heart_rate_variability_descriptor.get_result(&algorithm_result);
            SM_INFO("Heart rate variability on-change: %d, %d, %d, %d, %d\r\n", smt.heart_rate_variability_data.time_stamp, smt.heart_rate_variability_data.heart_rate_variability_t.SDNN,
                smt.heart_rate_variability_data.heart_rate_variability_t.LF, smt.heart_rate_variability_data.heart_rate_variability_t.HF, smt.heart_rate_variability_data.heart_rate_variability_t.LF_HF);
            #else
              if (hrv_reported == 0) {
                  smt.heart_rate_variability_data.sensor_type = SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR;
                  smt.heart_rate_variability_data.time_stamp = xSMLastExecutionTime;
                  smt.heart_rate_variability_data.heart_rate_variability_t.SDNN = 0;
                  smt.heart_rate_variability_data.heart_rate_variability_t.LF = 0;
                  smt.heart_rate_variability_data.heart_rate_variability_t.HF = 0;
                  smt.heart_rate_variability_data.heart_rate_variability_t.LF_HF= 0;
                  smt.heart_rate_variability_data.heart_rate_variability_t.beatPos = beatpos_out_pattern;
                  smt.heart_rate_variability_data.heart_rate_variability_t.numBeatPos = 8;
                  hrv_reported = 1;
              } else if (hrv_reported == 1) {
                  smt.heart_rate_variability_data.sensor_type = SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR;
                  smt.heart_rate_variability_data.time_stamp = xSMLastExecutionTime;
                  smt.heart_rate_variability_data.heart_rate_variability_t.SDNN = hrv_output[0];
                  smt.heart_rate_variability_data.heart_rate_variability_t.LF = hrv_output[1];
                  smt.heart_rate_variability_data.heart_rate_variability_t.HF = hrv_output[2];
                  smt.heart_rate_variability_data.heart_rate_variability_t.LF_HF= hrv_output[3];
                  smt.heart_rate_variability_data.heart_rate_variability_t.beatPos = NULL;
                  smt.heart_rate_variability_data.heart_rate_variability_t.numBeatPos = 0;
                  hrv_reported = 2;
            }
            #endif
        }
    } else if (SENSOR_TYPE_BLOOD_PRESSURE_MONITOR == sensor_type) {
        if (smt.blood_pressure_descriptor.get_result != NULL) {
            algorithm_result.data = &(smt.blood_pressure_data);
            #ifndef BLOOD_PRESSURE_OUT_PATTERN
            smt.blood_pressure_descriptor.get_result(&algorithm_result);
            SM_INFO("Blood pressure sbp(%d), dbp(%d), status(%d) \r\n", smt.blood_pressure_data.blood_pressure_t.sbp,
                smt.blood_pressure_data.blood_pressure_t.dbp, smt.blood_pressure_data.blood_pressure_t.status);
            #else
              if (bp_reported == 0) {
                  smt.blood_pressure_data.sensor_type = SENSOR_TYPE_BLOOD_PRESSURE_MONITOR;
                  smt.blood_pressure_data.time_stamp = xSMLastExecutionTime;
                  smt.blood_pressure_data.blood_pressure_t.sbp = 0;
                  smt.blood_pressure_data.blood_pressure_t.dbp = 0;
                  smt.blood_pressure_data.blood_pressure_t.status = 3;
                  smt.blood_pressure_data.blood_pressure_t.pwtt = pwtt_out_pattern;
                  smt.blood_pressure_data.blood_pressure_t.numPwtt = 9;
                  bp_reported = 1;
              } else if (bp_reported == 1) {
                  smt.blood_pressure_data.sensor_type = SENSOR_TYPE_BLOOD_PRESSURE_MONITOR;
                  smt.blood_pressure_data.time_stamp = xSMLastExecutionTime;
                  smt.blood_pressure_data.blood_pressure_t.sbp = 130;
                  smt.blood_pressure_data.blood_pressure_t.dbp = 80;
                  smt.blood_pressure_data.blood_pressure_t.status = -1;
                  smt.blood_pressure_data.blood_pressure_t.pwtt = NULL;
                  smt.blood_pressure_data.blood_pressure_t.numPwtt = 0;
                  bp_reported = 2;
              }
            #endif
        }
    } else {
        ret = -1;
        return ret;
    }

    output = &algorithm_result;
    for (i = 0; i < SENSOR_TYPE_ALL; i++) {
        if ( subscriber_list[i].send_digest != NULL && sensor_type == subscriber_list[i].type ) {
            SM_DBG("%d %d %d %d report output\r\n", output->data[0].value[0], output->data[0].value[1], output->data[0].value[2], output->data[0].value[3]);
            subscriber_list[subscriber_list[i].type].send_digest(output);
        }
    }

    return ret;
}

int32_t sensor_subscribe_sensor(sensor_subscriber_t *subscriber)
{
	int index;
	uint32_t supported_algo;
	
    //TODO change to multi-app subscribe
    SM_INFO("sensor_subscribe_sensor (%u) \r\n", subscriber->type);

    supported_algo = get_defined_sensor();
    if ( (supported_algo & (1U << subscriber->type) ) == 0 ) {
        SM_INFO("sensor type %d undefined \r\n", subscriber->type);
        return -1;
    }

    //TODO get unique subscriber_list index
    index = subscriber->type;

    //store app subscriber in sensor manager

    strcpy(subscriber_list[index].name , subscriber->name);//TODO subscribe index generate
    subscriber->handle =  subscriber->type;//TODO assign by sensor manager when multi-app subscribe
    subscriber_list[index].handle = subscriber->type;//TODO assign by sensor manager when multi-app subscribe
    subscriber_list[index].type = subscriber->type;
    subscriber_list[index].delay = subscriber->delay;
    subscriber_list[index].send_digest = subscriber->send_digest;

    //TODO if multi-app subscribe , need check duplicate subscribe

    sensor_throttling_control(subscriber->type, 1, subscriber->delay);
    return 1;
}

int32_t sensor_unsubscribe_sensor(int32_t handle)
{
    //TODO when change multi-app subscribe , need modification

    sensor_subscriber_t subscriber = subscriber_list[handle];//TODO handle to subscriber_t will different in multi-app subscribe

    //TODO check handle to subscribe_t exist
    sensor_throttling_control(subscriber.type, 0, subscriber.delay);
    memset(&subscriber_list[handle], 0, sizeof(sensor_subscriber_t));
    return 1;
}

static uint32_t get_defined_sensor()
{
    uint32_t supported_algo = 0;
#ifdef MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE
    supported_algo |= (1U << SENSOR_TYPE_ACCELEROMETER);
    supported_algo |= (1U << SENSOR_TYPE_GYROSCOPE);
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_USE
    supported_algo |= (1U << SENSOR_TYPE_ACCELEROMETER);
#endif

#ifdef MTK_SENSOR_GYROSCOPE_USE
    supported_algo |= (1U << SENSOR_TYPE_GYROSCOPE);
#endif

#ifdef MTK_SENSOR_MAGNETIC_USE
    supported_algo |= (1U << SENSOR_TYPE_MAGNETIC_FIELD);
#endif

#ifdef MTK_SENSOR_BAROMETER_USE
    supported_algo |= (1U << SENSOR_TYPE_PRESSURE);
#endif

#ifdef MTK_SENSOR_PROXIMITY_USE
    supported_algo |= (1U << SENSOR_TYPE_PROXIMITY);
#endif

#if defined(FUSION_HEART_RATE_MONITOR_USE)
        supported_algo |= (1U << SENSOR_TYPE_HEART_RATE_MONITOR);
#endif

#if defined(FUSION_HEART_RATE_VARIABILITY_USE)
    supported_algo |= (1U << SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR);
#endif

#if defined(FUSION_BLOOD_PRESSURE_USE)
    supported_algo |= (1U << SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
#endif

#ifdef MTK_SENSOR_BIO_USE_MT2511
    supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1);
    supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_PPG2);
    supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_BISI);
    supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_EKG);
    supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_EEG);
    supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_EMG);
    supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_GSR);
    supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1_512HZ);
#endif

    return supported_algo;
}

//+++ sensor throttling start
static uint32_t is_sensor_enabled(uint32_t sensor_type)
{
    return all_sensor_ctrl_data[sensor_type].enable;
}

sensor_input_list_t *get_algorithm_input_list(uint32_t sensor_type)
{
    sensor_input_list_t *tmp_input_list = NULL;
    switch (sensor_type) {

        case SENSOR_TYPE_HEART_RATE_MONITOR:
            tmp_input_list = smt.heart_rate_monitor_descriptor.input_list;
            break;
        case SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR:
            tmp_input_list = smt.heart_rate_variability_descriptor.input_list;
            break;
        case SENSOR_TYPE_BLOOD_PRESSURE_MONITOR:
            tmp_input_list = smt.blood_pressure_descriptor.input_list;
            break;
        default:
            tmp_input_list = NULL;
            SM_ERR("get_algorithm_input_list: illegal sensor type %d \r\n", sensor_type);
            break;
    }
    return tmp_input_list;
}
uint32_t get_algorithm_used_hw_sensor_delay(int algorithm_type, int hw_sensor_type)
{
    uint32_t hw_sensor_delay = SENSOR_DRIVER_MAX_DELAY;
    sensor_input_list_t *tmp_input_list;
    tmp_input_list = get_algorithm_input_list(algorithm_type);

    while (tmp_input_list != NULL) {
        if (hw_sensor_type == tmp_input_list->input_type) {
            hw_sensor_delay = tmp_input_list->sampling_delay;
            break;
        } else {
            tmp_input_list = tmp_input_list->next_input;
        }
    }

    return hw_sensor_delay;
}

uint32_t recalcuate_new_hw_sensor_delay(uint32_t hw_sensor_type)
{
    int i;
    uint32_t optimized_hw_sensor_delay = SENSOR_DRIVER_MAX_DELAY;
    uint32_t tmp_hw_sensor_delay = 0;
    // check hw sensor
    if ( all_sensor_ctrl_data[hw_sensor_type].enable == 1) {
        optimized_hw_sensor_delay = all_sensor_ctrl_data[hw_sensor_type].delay;
    }
    // check algorithm
    for (i = SENSOR_VIRTUAL_TYPE_START; i < SENSOR_TYPE_ALL; i++) {
        if ( all_sensor_ctrl_data[i].enable == 1 ) {
            tmp_hw_sensor_delay = get_algorithm_used_hw_sensor_delay(i, hw_sensor_type);
            if ( tmp_hw_sensor_delay > 0 && tmp_hw_sensor_delay < optimized_hw_sensor_delay ) {
                optimized_hw_sensor_delay = tmp_hw_sensor_delay;
            }
        }
    }
    return optimized_hw_sensor_delay;
}

uint32_t is_algorithm_with_hw_sensor(int algorithm_type, int hw_sensor_type)
{
    uint32_t hw_sensor_enable = 0;
    sensor_input_list_t *tmp_input_list;
    tmp_input_list = get_algorithm_input_list(algorithm_type);

    while (tmp_input_list != NULL) {
        if (hw_sensor_type == tmp_input_list->input_type) {
            hw_sensor_enable = 1;
            break;
        } else {
            tmp_input_list = tmp_input_list->next_input;
        }
    }

    return hw_sensor_enable;
}

uint32_t recalcuate_new_hw_sensor_enable(uint32_t hw_sensor_type)
{
    int i;
    uint32_t optimized_hw_sensor_enable = 0;
    // check hw sensor
    if ( all_sensor_ctrl_data[hw_sensor_type].enable == 1) {
        optimized_hw_sensor_enable = 1;
    }
    // check algorithm
    for (i = SENSOR_VIRTUAL_TYPE_START; i < SENSOR_TYPE_ALL; i++) {
        if ( all_sensor_ctrl_data[i].enable == 1 ) {
            if ( is_algorithm_with_hw_sensor(i, hw_sensor_type) == 1 ) {
                optimized_hw_sensor_enable = 1;
            }
        }
    }
    return optimized_hw_sensor_enable;
}

void dump_all_sensor_ctrl_data()
{
    int i = 0;
    for (i = 0; i < SENSOR_TYPE_ALL; i++) {
        SM_DBG("type=%d enable=%d delay=%d\r\n", i, all_sensor_ctrl_data[i].enable, all_sensor_ctrl_data[i].delay);
    }
}

static int32_t sensor_throttling_control(uint32_t sensor_type, uint32_t enable, uint32_t delay)
{
    int32_t ret = 0;
    uint32_t optimized_hw_sensor_enable[SENSOR_PHYSICAL_TYPE_ALL] = {0};
    uint32_t optimized_hw_sensor_delay[SENSOR_PHYSICAL_TYPE_ALL] = {0};
    int i;
    sensor_input_list_t *tmp_input_list = NULL;
    uint32_t finetuned_polling_sensors_delay = 0;
    char sensors_dump[80] = {0};
    uint32_t log_len = 0;
    int32_t fifowatermark = 40;

    SM_INFO("[sensor_throttling_control]type(%d), enable(%d), delay(%d)\r\n", sensor_type, enable, delay);

    // get current hw sensor enable,delay configuration
    for (i = 0; i < SENSOR_PHYSICAL_TYPE_ALL; i++) {
        optimized_hw_sensor_enable[i] = sensor_driver_get_enable(i);
        optimized_hw_sensor_delay[i] = sensor_driver_get_delay(i);
    }
    //log
    //dump_all_sensor_ctrl_data();

    // check cmd with new sensor config
    if ( enable == all_sensor_ctrl_data[sensor_type].enable && delay == all_sensor_ctrl_data[sensor_type].delay ) {
        SM_DBG("recv (enable,delay) is the same with current sensor ctrl data \r\n");
        return 0;
    }

    SM_DBG("original_sensor_ctrl enable=%d delay=%d\r\n", all_sensor_ctrl_data[sensor_type].enable, all_sensor_ctrl_data[sensor_type].delay);
    all_sensor_ctrl_data[sensor_type].enable = enable;
    all_sensor_ctrl_data[sensor_type].delay = delay;

    //log
    dump_all_sensor_ctrl_data();

    // optimize hw sensor delay & enable
    if (sensor_type < SENSOR_PHYSICAL_TYPE_ALL) {
        //hw sensor
        SM_DBG("hw sensor cmd\r\n");
        optimized_hw_sensor_delay[sensor_type] = recalcuate_new_hw_sensor_delay(sensor_type);
        optimized_hw_sensor_enable[sensor_type] = recalcuate_new_hw_sensor_enable(sensor_type);
    } else {
        //algorithm
        SM_DBG("algorithm cmd sensor_type = %u \r\n", sensor_type);
        switch (sensor_type) {



#if defined(FUSION_HEART_RATE_MONITOR_USE)
            case SENSOR_TYPE_HEART_RATE_MONITOR:
                if (enable != 0) {
                    heart_rate_monitor_init();
                }
                break;
#endif

#if defined(FUSION_HEART_RATE_VARIABILITY_USE)
            case SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR:
                if (enable != 0) {
                #ifdef HEART_RATE_VARIABILITY_OUT_PATTERN
                    hrv_start_time = xSMLastExecutionTime;
                #endif
                    heart_rate_variability_monitor_init();
                } else {
                #ifdef HEART_RATE_VARIABILITY_OUT_PATTERN
                    hrv_reported = 0;
                #endif
                }
                break;
#endif

#if defined(FUSION_BLOOD_PRESSURE_USE)
            case SENSOR_TYPE_BLOOD_PRESSURE_MONITOR:
                if (enable != 0) {
                #ifdef BLOOD_PRESSURE_OUT_PATTERN
                    bp_start_time = xSMLastExecutionTime;
                #endif
                    blood_pressure_monitor_init();
                } else {
                #ifdef BLOOD_PRESSURE_OUT_PATTERN
                    bp_reported = 0;
                #endif
                }
                break;
#endif

            default:
                break;
        }
        tmp_input_list =  get_algorithm_input_list(sensor_type);
        while (tmp_input_list != NULL) {
            SM_DBG("tmp_input_list type=%d,delay=%d\r\n", (uint32_t)tmp_input_list->input_type, tmp_input_list->sampling_delay);
            optimized_hw_sensor_delay[tmp_input_list->input_type] = recalcuate_new_hw_sensor_delay(tmp_input_list->input_type);
            optimized_hw_sensor_enable[tmp_input_list->input_type] = recalcuate_new_hw_sensor_enable(tmp_input_list->input_type);
            tmp_input_list = tmp_input_list->next_input;
        }
    }


    for (i = 0; i < SENSOR_PHYSICAL_TYPE_ALL; i++) {
        //SM_INFO("optimized type=%d enable=%d delay=%d \r\n", i, optimized_hw_sensor_enable[i], optimized_hw_sensor_delay[i]);
        snprintf(sensors_dump+log_len, 80-log_len, "%d,%d|", optimized_hw_sensor_enable[i], optimized_hw_sensor_delay[i]);
        log_len = strlen(sensors_dump);

        //TODO fifowatermark how to define ( what is max delay decision ? )
        if (i == SENSOR_TYPE_ACCELEROMETER && optimized_hw_sensor_enable[i] == 1 ) {
            smt.mgr_acc_delay = optimized_hw_sensor_delay[i];
            sensor_driver_set_cust(SENSOR_TYPE_ACCELEROMETER, NULL, 0, NULL, (void *)(&fifowatermark), sizeof(int32_t));
        }
        //TODO Need to turn on ACC, then GYRO
        sensor_driver_enable(i, optimized_hw_sensor_enable[i]);

        if (optimized_hw_sensor_enable[i] != 0) {
            sensor_driver_set_delay(i, optimized_hw_sensor_delay[i]);
        }

    }
    SM_INFO("%s\n", sensors_dump);
#if 1
    // handle polling sensor
    finetuned_polling_sensors_delay = 0;
    for( i=0; i < SENSOR_PHYSICAL_TYPE_ALL ;i++) {
        if( sensor_driver_get_event_type(i) == 1){
            //polling sensor
            if( sensor_driver_get_enable(i) > 0){
                if( finetuned_polling_sensors_delay == 0 ) {
                    finetuned_polling_sensors_delay = sensor_driver_get_delay(i);
                }else {
                    if( finetuned_polling_sensors_delay >  sensor_driver_get_delay(i) ) {
                        finetuned_polling_sensors_delay = sensor_driver_get_delay(i);
                    }
                }
            }
        }
    }
#endif
    SM_INFO("finetuned_polling_sensors_delay = %d\r\n", finetuned_polling_sensors_delay);
    if( finetuned_polling_sensors_delay == 0 ){
        //stop timeout timer
        SM_INFO("stop timer\r\n");
        sensor_stop_timer();
		is_timer_stop= 1;
    } else {
    	smt.polling_period = finetuned_polling_sensors_delay;
		sensor_stop_timer();
		sensor_set_timer(smt.polling_period);
		if (is_timer_stop == 1) {
            SM_INFO("reset timer\r\n");
            is_timer_stop = 0;
        }
    }

#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
    #if !defined(HEART_RATE_MONITOR_OUT_PATTERN) && !defined(HEART_RATE_VARIABILITY_PATTERN) /* for test pattern to have task trigger */
        #if !defined(DEVICE_BAND) && !defined(DEVICE_HDK)
/* under construction !*/
/* under construction !*/
        #endif
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

    return ret;
}

#if defined(__MTK_TARGET__) && defined(__UPDATE_BINARY_FILE__) && defined(__UPDATE_PKG_SENSOR_ONLY__)
static void sensor_ubin_init()
{
    //UBIN_DummyReference();
    //UBIN_mmi_symbol_DummyReference();
    unsigned int *p_mmi_symbol_addr = NULL;
    p_mmi_symbol_addr = (unsigned int*)(custom_get_UPDATE_ROM_entry() + UBIN_SYMBOL_ENTRY_SENSOR * sizeof(void *));
    kal_prompt_trace(MOD_MMI, "[FOTA UBIN]Call %s, jump_addr = 0x%x\r\n", __FUNCTION__, p_mmi_symbol_addr);
    ((void (*)(void))(*p_mmi_symbol_addr))();
}
#endif

void sensor_subsys_initialize(void)
{
#if defined(__MTK_TARGET__) && defined(__UPDATE_BINARY_FILE__) && defined(__UPDATE_PKG_SENSOR_ONLY__)   
    sensor_ubin_init();
#endif

    sensor_manager_init();

    #if defined(MTK_SENSOR_ACCELEROMETER_USE_BMA255)
    /* acc sensor */
    bma255_sensor_subsys_init();
    #endif

    #ifdef MTK_SENSOR_BIO_USE_MT2511
    /* Bio sensor */
    vsm_driver_sensor_subsys_init();
    #endif

    /* create timer to process driver data and algo */
    sensor_timer_init();

#ifdef HEART_RATE_MONITOR_OUT_PATTERN
	kal_sleep_task(1000); // FOR Catcher
    //if (sensor_subscribe_sensor(&heart_rate_monitor_subscriber) < 0) {
	if (sensor_subscribe_sensor(&blood_pressure_subscriber) < 0) {	
        SM_ERR("Heart rate monitor subscription fail");
    } else {
        sensor_set_timer(200);
    }
#endif

	//SetProtocolEventHandler((PsFuncPtr) sensor_timeout_callback, MSG_ID_TIMER_EXPIRY);

}

void sensor_subsys_enable(uint8_t type, uint8_t enabled)
{
	if (type == SENSOR_TYPE_BLOOD_PRESSURE_MONITOR){
		if (enabled) {
			sensor_subscribe_sensor(&blood_pressure_subscriber);
		} else {
			sensor_unsubscribe_sensor(blood_pressure_subscriber.handle);
		}

	} else if (type == SENSOR_TYPE_HEART_RATE_MONITOR) {
		if (enabled) {
			sensor_subscribe_sensor(&heart_rate_monitor_subscriber);
		} else {
			sensor_unsubscribe_sensor(heart_rate_monitor_subscriber.handle);
		}
	}
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
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif

