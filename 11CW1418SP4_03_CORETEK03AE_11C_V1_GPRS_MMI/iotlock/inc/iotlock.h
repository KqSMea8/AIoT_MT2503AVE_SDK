#ifndef __IOT_SPC__
#define __IOT_SPC__
#include <stdlib.h>
#include "MMI_features.h"
#include "MMIDataType.h"
#include "kal_general_types.h"
#include "lock_data.h"
#include "dcl.h"
#include "Mmi_rp_srv_iot_lock_def.h"
#include "ProfilesSrv.h"
#include "resource_audio.h"
#include "mdi_datatype.h"
#include "mmi_rp_srv_prof_def.h"
#include "bmt_utility.h"
#include "linked_list.h"

#define ReadRecordIot(nLID, nRecordId,pBuffer,nBufferSize) \
    ASSERT(nvram_external_read_data(nLID, nRecordId, (kal_uint8*)pBuffer, nBufferSize))
#define WriteRecordIot(nLID, nRecordId,pBuffer,nBufferSize) \
    ASSERT(nvram_external_write_data(nLID, nRecordId, (kal_uint8*)pBuffer, nBufferSize))
typedef enum
{
    IOT_TONE_NONE,
    IOT_TONE_LOCK,
    IOT_TONE_UNLOCK,
    IOT_TONE_WARN,
    IOT_TONE_END_OF_ENUM           /* End of enum */
}iot_tone_enum;

extern LINKEDLIST g_report_list;
extern kal_bool 	g_normal_open_lock_state;
extern mdi_result srv_prof_play_audio_id(srv_prof_play_audio_struct play_info);
extern void StartTimer(U16 timerid, U32 delay, FuncPtr funcPtr);
extern mdi_result iot_play_tone_with_filepath(iot_tone_enum tone_type);
kal_uint8 iot_get_bkup_nv_first_byte(void);
void iot_set_bkup_nv_first_byte(kal_uint8 type);
void iot_set_bkup_nv_second_byte(kal_uint8 type);
kal_uint8 iot_get_bkup_nv_second_byte(void);
void iot_set_bkup_nv_third_byte(kal_uint8 type);
kal_uint8 iot_get_bkup_nv_second_byte(void);
void iot_set_bkup_nv_gps_info(GPS_INFO *info);
void iot_get_bkup_nv_gps_info(GPS_INFO *info);
kal_bool iot_get_bkup_nv_exception_reboot(void);
void iot_set_bkup_nv_exception_reboot(kal_uint8 *ex_reboot);

void iot_get_battery(void);
kal_uint8 iot_get_battery_percentage(void);

srv_prof_play_tone_callback iot_open_lock_play_tone_callback(void);
#endif
