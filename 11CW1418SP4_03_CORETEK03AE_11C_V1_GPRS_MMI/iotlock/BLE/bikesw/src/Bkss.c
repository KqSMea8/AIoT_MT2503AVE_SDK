#ifdef __IOT_BLE_BKSS_SUPPORT__
#include "MMIDataType.h"
#include "MMITimer.h"
#include "BkssSrvGprot.h"
#include "aes.h"
#include "BkssSrv.h"
#include "Bkss_def.h"

#include "BtcmSrvGprot.h"
#include "kal_general_types.h"
#include "kal_public_api.h"
#include "GlobalResDef.h"
#include "GpioSrvGprot.h"
#include "ProfilesSrvGprot.h"
#include "mmi_rp_srv_prof_def.h"
#include "common_nvram_editor_data_item.h"
#include "BtcmSrvProt.h"
#include "app_datetime.h"
#include "ImeiSrvGprot.h"
#include "time.h"
#include "ps_public_struct.h"
#include "custom_mmi_default_value.h"
#include "DateTimeGprot.h"
#include "lock_data.h"
#include "lock_net.h"
#include "iotlock.h"

//#define BKSS_SRV_APP_TARGET_ALERT_TIME        (1000 * 60)
extern U32 bikesw_send_data(void* buf, U32 len);
extern void get_device_id_from_imei(BCD *device_id);
extern kal_char *release_verno(void);
extern kal_char* build_date_time(void); 
extern void bkss_update_adv_data(void);
extern void iot_get_battery(void);
extern void verno_date_time_init(void);
extern void save_order_num_to_nvram(const kal_uint8 *order_num) ;
extern void get_order_num_from_nvram(kal_uint8 *order_num);
static void bkss_srv_app_event_notify(U32 event, void* para);
static void bkss_srv_app_target_alert_disable(void);
void bkss_vib_status_callback();
void bkss_send_iccid(void);
void CloseLockStatus(void);


extern gprs_lock_cntx g_lock_cntx;
extern int motion_sensor_state;

//extern kal_uint8 current_percentage;
extern kal_uint8 verno_date_time[16];
char vib_status = 0;
kal_uint8 iccid_inited = 255;
static kal_uint8 bkss_iccid[10] = {0};

static U8 gtoken[4];  //令牌
nvram_ef_bel_bkss_struct bel_bkss = {0};  //存放密钥和密码
static U8 temp_key[BT_KEY_MAX_LEN + 1];
static kal_uint8 bkss_imei[10] = {0};
bkss_callback_t bkss_cb_cntx;
static MMI_BOOL is_alerting = MMI_FALSE;
kal_uint8 aes128_key[BT_KEY_MAX_LEN + 1] = {0};

/*****************************************************************************
 * FUNCTION
 *  fmpt_srv_app_target_alert_enable
 * DESCRIPTION
 *  to start alert
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bkss_srv_app_target_alert_enable(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	bkss_dbg_print(MOD_ENG, "bkss_srv_app_target_alert_enable");
	if (is_alerting == MMI_TRUE)
	{
	    return;
	}
	is_alerting = MMI_TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_fmpapp_target_alert_enable
 * DESCRIPTION
 *  to stop alert
 * PARAMETERS
 * void
 * RETURNS
 *  void
 *****************************************************************************/
void bkss_srv_app_target_alert_disable(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bkss_dbg_print(MOD_ENG, "bkss_srv_app_target_alert_disable");
    if (is_alerting == MMI_FALSE)
    {
        return;
    }
	is_alerting = MMI_FALSE;
}


void bkss_set_vib_status(char state)
{
	bkss_dbg_print(MOD_ENG, "enter bkss_set_vib_status state ==%d",state);
	StopTimer(BLE_BKSS_VIB_TIMER);
	vib_status = state;
	if(vib_status == 1)
		StartTimer(BLE_BKSS_VIB_TIMER, 5*1000, bkss_vib_status_callback);
}

void bkss_vib_status_callback()
{
	bkss_dbg_print(MOD_ENG, "enter bkss_set_vib_status close");
	bkss_set_vib_status(0);
}

char bkss_get_vib_status(void)
{
	return vib_status;
}
void bkss_dump_buffer(kal_uint8 *out, int len){
	char buffer[512] = {0};
	kal_uint8 *ptr = out;
	kal_uint8 *end = out+len;
	kal_uint16 count = 0;
	kal_uint16 i = 0;
	char *pre = "spconser:bkss out=";
	kal_uint8 llen = 127 - strlen(pre);

	for(; ptr < end && i < 512; ptr++) {
		kal_uint8 low = *ptr %16;
		kal_uint8 high = *ptr >> 4;
		
		if(high < 10) {
			buffer[i++] = high+'0';
		} else {
			buffer[i++] = 'A' + high - 10;
		}
		if(low < 10) {
			buffer[i++] = low+'0';
		} else {
			buffer[i++] = 'A' + low -10;
		}
	}
	
	count = (len<<1) / llen + 1;
	bkss_dbg_print(MOD_ENG, "spconser: len=%d, count=%d", len, count);
	i = 0;
	while(i < count) {
		bkss_dbg_print(MOD_ENG, "%s%s", pre, buffer + i * llen);
		i++;
	}
}
void bkss_get_iccid_rsp(void *info) {
	mmi_smu_read_sim_rsp_struct *iccid_data;
	kal_uint8 i,tmp;

	iccid_data = (mmi_smu_read_sim_rsp_struct*) info;
	mmi_frm_clear_protocol_event_handler(MSG_ID_MMI_SMU_READ_SIM_RSP, bkss_get_iccid_rsp);
	memset(bkss_iccid, 0, sizeof(bkss_iccid));
	if (iccid_data->result.flag == 0) {
		memcpy(bkss_iccid, iccid_data->data, iccid_data->length);
		//bkss_dump_buffer(bkss_iccid, sizeof(bkss_iccid));
		for(i=0;i<10;i++)
		{
			tmp=bkss_iccid[i];
			bkss_iccid[i] = (tmp%16) * 16 + tmp/16;
		}
		bkss_dbg_print(MOD_ENG, "bkss_get_iccid_rsp bkss_iccid===%s", bkss_iccid);
		if(iccid_inited != 255)
		{
			bkss_send_iccid();
		}
		iccid_inited = 1;
	} 
}

void bkss_get_iccid_req(void)
{
	if (iccid_inited != 1) 
	{
		MYQUEUE Message;
		mmi_smu_read_sim_req_struct *dataPtr;

		mmi_frm_set_protocol_event_handler(MSG_ID_MMI_SMU_READ_SIM_RSP,bkss_get_iccid_rsp,MMI_TRUE);

		Message.oslSrcId = MOD_MMI;
		Message.oslDestId = MOD_L4C;
			
		Message.oslMsgId = MSG_ID_MMI_SMU_READ_SIM_REQ;
		dataPtr = (mmi_smu_read_sim_req_struct*) OslConstructDataPtr(sizeof(mmi_smu_read_sim_req_struct));
		dataPtr->file_idx = (U8) FILE_ICCID_IDX;
		dataPtr->para = 0;
		dataPtr->length = 0x0a;
		Message.oslDataPtr = (oslParaType*) dataPtr;
		Message.oslPeerBuffPtr = NULL;
		OslMsgSendExtQueue(&Message);
	}
}
void bkss_send_iccid(void)
{
	U8 tempbuf[17];
	U8 i;
	bkss_dbg_print(MOD_ENG,"ICC ID");
	if(iccid_inited == 1)
	{
		memset(tempbuf,0,sizeof(tempbuf));
		tempbuf[0]=0x05;
		tempbuf[1]=0x28;
		tempbuf[2]=0x0A;
		for(i=0;i<10;i++)
		{
			tempbuf[3+i]=bkss_iccid[i];
		}
		bikesw_send_data(tempbuf,16);	

	}
	else
	{
		iccid_inited = 0;
		bkss_get_iccid_req();
	}
}
/*****************************************************************************
 * FUNCTION
 *  bkss_srv_app_connection_state_cb
 * DESCRIPTION
 *  to notify reporter current connection state
 * PARAMETERS
 *  [bdaddr] Return remote bluetooth device addr
 *  [state] Profile current connection state
 * RETURNS
 *  void
 *****************************************************************************/
void bkss_srv_app_connection_state_cb(char* bdaddr, int connect_state)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
//    MMI_TRACE(MMI_CONN_TRC_G7_BT, TRC_BLE_BKSSAPP_CONNECT_CB,bdaddr,connect_state);
	switch (connect_state)
	{
		/*according bkss profile connect status to handle diff things*/
		case BKSS_STATUS_DISCONNECTED:
		{
			bkss_srv_app_target_alert_enable();
		}
			break;
		case BKSS_STATUS_CONNECTING:
			break;
		case BKSS_STATUS_CONNECTED:
		{
			CloseLockStatus();
		}
			break;	
		case BKSS_STATUS_DISCONNECTING:
			break;
		default:
			break;

	}    
}

MMI_BOOL getToken(char * revbuf,U8 offset)
{
	char * buf = revbuf + offset;
	if(*(buf)==gtoken[0] && *(buf+1)==gtoken[1] && *(buf+2)==gtoken[2] && *(buf+3)==gtoken[3])
	{
		bkss_dbg_print(MOD_ENG,"gtoken TRUE");
		return MMI_TRUE;
	}
	bkss_dbg_print(MOD_ENG,"gtoken FALSE");
	return MMI_FALSE;
}

void init_param(void)  //初始化密钥密码
{
	S16 error;
	U8 i = 0;
	memset(&bel_bkss,0,sizeof(nvram_ef_bel_bkss_struct));
	ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&bel_bkss, NVRAM_EF_BLE_BKSS_SIZE);
	memset(aes128_key,0,BT_KEY_MAX_LEN);
	for(i=0;i<BT_KEY_MAX_LEN;i++)
	{
		aes128_key[i] = bel_bkss.key[i];
	}
	bkss_dbg_print(MOD_ENG,"init_param%d",bel_bkss.psd[0] + bel_bkss.psd[1] + bel_bkss.psd[2] + bel_bkss.psd[3] + bel_bkss.psd[4] + bel_bkss.psd[5]);
}
S32 bkss_set_param(BKSS_NV_TYPE type)  //初始化密钥密码
{
	nvram_ef_bel_bkss_struct info = {0};  //存放密钥和密码
	U8 i = 0;
	memset(&info,0,sizeof(nvram_ef_bel_bkss_struct));
	ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info, NVRAM_EF_BLE_BKSS_SIZE);
	if(type == BKSS_NV_TYPE_KEY)
	{
		for(i=0;i<BT_KEY_MAX_LEN;i++)
		{
			info.key[i] = bel_bkss.key[i];
		}
	}
	else if(type == BKSS_NV_TYPE_PSD)
	{
		for(i=0;i<BT_PSD_MAX_LEN;i++)
		{
			info.psd[i] = bel_bkss.psd[i];
		}
	}
	else if(type == BKSS_NV_TYPE_TIME)
	{
		memcpy(&info.lock_time,&bel_bkss.lock_time,sizeof(applib_time_struct));
	}
	else
	{
		return NVRAM_WRITE_FAIL;
	}
	WriteRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info, NVRAM_EF_BLE_BKSS_SIZE);
	return NVRAM_WRITE_SUCCESS;
}

MMI_BOOL check_psw(char *password)
{
	if(password[0] == bel_bkss.psd[0]
		&&password[1] == bel_bkss.psd[1]
		&&password[2] == bel_bkss.psd[2]
		&&password[3] == bel_bkss.psd[3]
		&&password[4] == bel_bkss.psd[4]
		&&password[5] == bel_bkss.psd[5])
		{
			bkss_dbg_print(MOD_ENG,"check_psw TRUE");
			return MMI_TRUE;
		}
	bkss_dbg_print(MOD_ENG,"check_psw FALSE");
	return MMI_FALSE;
}

MMI_BOOL change_psw(char *password)  //修改密码
{
	nvram_ef_bel_bkss_struct info = {0};  //存放密钥和密码
	nvram_ef_bel_bkss_struct info1 = {0};  //存放密钥和密码
	kal_uint8 i;
	memset(&info,0,sizeof(nvram_ef_bel_bkss_struct));
	ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info, NVRAM_EF_BLE_BKSS_SIZE);
	for(i=0;i<3;i++)
	{
		memcpy(info.psd, password, BT_PSD_MAX_LEN);
		WriteRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info, NVRAM_EF_BLE_BKSS_SIZE);
		ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info1, NVRAM_EF_BLE_BKSS_SIZE);
		if(memcmp(info.psd, info1.psd, BT_PSD_MAX_LEN) == 0)
		{
			return KAL_TRUE;
		}
		else
		{
			continue;
		}
	}
	memcpy(info.psd, bel_bkss.psd, BT_PSD_MAX_LEN);
	WriteRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info, NVRAM_EF_BLE_BKSS_SIZE);
	return KAL_FALSE;
}


MMI_BOOL change_key(kal_uint8 *keycode) //修改密钥
{
	nvram_ef_bel_bkss_struct info = {0};  //存放密钥和密码
	nvram_ef_bel_bkss_struct info1 = {0};  //存放密钥和密码
	kal_uint8 i;
	memset(&info,0,sizeof(nvram_ef_bel_bkss_struct));
	ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info, NVRAM_EF_BLE_BKSS_SIZE);
	for(i=0;i<3;i++)
	{
		memcpy(info.key, keycode, BT_KEY_MAX_LEN);
		WriteRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info, NVRAM_EF_BLE_BKSS_SIZE);
		ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info1, NVRAM_EF_BLE_BKSS_SIZE);
		if(memcmp(info.key, info1.key, BT_KEY_MAX_LEN) == 0)
		{
			return KAL_TRUE;
		}
		else
		{
			continue;
		}
	}
	memcpy(info.key, bel_bkss.key, BT_KEY_MAX_LEN);
	WriteRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info, NVRAM_EF_BLE_BKSS_SIZE);
	return KAL_FALSE;
}

void bkss_get_battery(void)
{
	iot_get_battery();
	bkss_update_adv_data();
}
U8 bkss_get_battery_percentage(void)
{
	iot_get_battery_percentage();
}
//手动关锁，给app发送状态消息
void SendLockState(applib_time_struct curr_time)
{
	U8 tempbuf[17];
	U32 len = 16;

	LOCK_STATE state = read_lock_state();
	memset(tempbuf,0,sizeof(tempbuf));
	tempbuf[0] = 0x05;
	tempbuf[1] = 0x08;
	tempbuf[2] = 0x01;
	if(state == LOCKED)
		tempbuf[3] = 0x00;  //关锁成功
	else
		tempbuf[3] = 0x01;
	tempbuf[4] = 0x07;
	tempbuf[5] = curr_time.nYear>>8;
	tempbuf[6] = curr_time.nYear;
	tempbuf[7] = curr_time.nMonth;
	tempbuf[8] = curr_time.nDay;
	tempbuf[9] = curr_time.nHour;
	tempbuf[10] = curr_time.nMin;
	tempbuf[11] = curr_time.nSec;
	bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);
	bikesw_send_data(tempbuf,len);	
}
void CloseLockTimer(void)
{
	StopTimer(BLE_BKSS_CLOSE_TIMER);
	SendLockState(bel_bkss.lock_time);	
}
void CloseLockStatus(void)
{
	LOCK_STATE state = read_lock_state();
	if(state == LOCKED)
	{
		StopTimer(BLE_BKSS_CLOSE_TIMER);
		StartTimer(BLE_BKSS_CLOSE_TIMER, 50, CloseLockTimer);
	}
}

void BkssSaveLocktime(applib_time_struct curr_time)
{
	S16 error;
	bel_bkss.lock_time.nYear = curr_time.nYear;
	bel_bkss.lock_time.nMonth = curr_time.nMonth;
	bel_bkss.lock_time.nDay = curr_time.nDay;
	bel_bkss.lock_time.nHour = curr_time.nHour;
	bel_bkss.lock_time.nMin = curr_time.nMin;
	bel_bkss.lock_time.nSec = curr_time.nSec;
	bel_bkss.lock_time.DayIndex = curr_time.DayIndex;
	bkss_dbg_print(MOD_ENG,"lock_time %4d%2d%2d%2d%2d%2d%2d",curr_time.nYear,curr_time.nMonth,curr_time.nDay,curr_time.nHour,curr_time.nMin,curr_time.nSec,curr_time.nDay);
	error = bkss_set_param(BKSS_NV_TYPE_TIME);
	SendLockState(curr_time);
}


kal_uint8 bt_order_num[BT_LOCK_ORDER_NUM_MAX_LEN] = {0}; 
kal_bool bt_order = KAL_FALSE;
kal_uint8 bt_check_code(kal_uint8* order_num,kal_uint8* len)
{
	kal_uint8 *ptr = order_num;
	kal_uint8 *end = ptr + *len;
	
	Byte check_code = *ptr;
	for(ptr++; ptr < end; ptr++) 
	{
		check_code ^= *ptr;
	}
	return check_code;
}
void bt_switch_lock_mode_callback (kal_bool result)
{
	U8 tempbuf[17];
	U32 len = 16;

	memset(tempbuf,0,sizeof(tempbuf));
	tempbuf[0]=0x05;
	tempbuf[1]=0x21;
	tempbuf[2]=0x01;
	if (result == KAL_FALSE)
	{
		tempbuf[3]=0x01;
	}
	else
	{
		tempbuf[3]=0x00;
	}
	bikesw_send_data(tempbuf,len);	
}
kal_uint8 open_lock_buf[17];
void bt_open_lock_callback (kal_bool result)
{
	if (result == KAL_FALSE)
	{
		open_lock_buf[3] = 0x01;
	}
	else
	{
		open_lock_buf[3] = 0x00;
	}
	bikesw_send_data(open_lock_buf,16);
}
void bt_open_lock_ex(void)
{
	bt_open_lock(BT_UNLOCK,bt_open_lock_callback);
}
kal_bool changing_psd_key = KAL_FALSE;
void changing_stop(void)
{
	changing_psd_key = KAL_FALSE;
	StopTimer(BOOTUP_LED_BEEP_TIMER);
}
void bkss_volume_test (kal_bool result)
{
	U8 tempbuf[17];
	U32 len = 16;

	memset(tempbuf,0,sizeof(tempbuf));
	tempbuf[0] = 0x05;
	tempbuf[1] = 0x44;
	tempbuf[2] = 0x01;
	tempbuf[3] = 0x00;
	bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);		
	bikesw_send_data(tempbuf,len);	
}
/*****************************************************************************
 * FUNCTION
 *  mmi_bkssapp_data_to_read_notify_cb
 * DESCRIPTION
 *  to notify reporter mmi task start/stop path loss alert(IAS)
 * PARAMETERS
 *  [bdaddr] Return remote bluetooth device addr
 *  [level] alert level of IAS
 * RETURNS
 *  void
 *****************************************************************************/
S32 bkss_srv_app_data_to_read_notify_cb(char *bd_addr, void* buf, U32 len)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	U8 buffer[16];
	char * revbuf = buf;
	U8 tempbuf[17];
	MMI_BOOL bResp;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	if(len < 16)
	{
		///return;
	}
	init_param();
	
	memset(buffer,0,sizeof(buffer));
	bkss_dbg_print(MOD_ENG, "%s() %d revbuf =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__
		,revbuf[0],revbuf[1],revbuf[2],revbuf[3],revbuf[4],revbuf[5],revbuf[6],revbuf[7],revbuf[8],revbuf[9],revbuf[10],revbuf[11],revbuf[12],revbuf[13],revbuf[14],revbuf[15]);
	bkss_dbg_print(MOD_ENG, "%s() %d aes128_key =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__
		,aes128_key[0],aes128_key[1],aes128_key[2],aes128_key[3],aes128_key[4],aes128_key[5],aes128_key[6],aes128_key[7],
		aes128_key[8],aes128_key[9],aes128_key[10],aes128_key[11],aes128_key[12],aes128_key[13],aes128_key[14],aes128_key[15]);
	AES128_ECB_decrypt(buf,aes128_key,buffer);
	revbuf = buffer;

	
	for(i=0;i<len;i++)
	{
		bkss_dbg_print(MOD_ENG,"notify_cb buf[%d]=%x\r\n",i,*(revbuf+i));
	}
	len = 16;
	bResp = MMI_TRUE;
	memset(tempbuf,0,sizeof(tempbuf));
	//命令解析
	if(*(revbuf)==0x06 && *(revbuf+1)==0x01 && *(revbuf+2)==0x01 && *(revbuf+3)==0x01 ) //秘钥
	{
		memset(gtoken,0,sizeof(gtoken));
		tempbuf[0]=0x06;
		tempbuf[1]=0x02;
		tempbuf[2]=0x07;
		tempbuf[3]=rand()%255;
		tempbuf[4]=rand()%255;
		tempbuf[5]=rand()%255;
		tempbuf[6]=rand()%255;
		tempbuf[7]=0x01;  //芯片类型
		tempbuf[8]=0x01;  //版本整数位
		tempbuf[9]=0x01;  //版本小数位
		tempbuf[10]=0x05;  //IDL
		tempbuf[11]=0x0B;//IDH
		gtoken[0]= tempbuf[3];
		gtoken[1]= tempbuf[4];
		gtoken[2]= tempbuf[5];
		gtoken[3]= tempbuf[6];
		for(i=0;i<4;i++)
		{
			bkss_dbg_print(MOD_ENG,"bkss_srv_app_data_to_read_notify_cb gtoken[%d]=%x\r\n",i,*(gtoken+i));
		}
	}

	
	else if(*(revbuf)==0x06 && *(revbuf+1)==0x03 && *(revbuf+2)==0x04 && getToken(revbuf,7)) //同步时间
	{
		//起始年月需要确认
		//mktime localtime
		struct tm tmtime;
		MYTIME set_t = {0};
		long time_sec;

		
		tempbuf[0]=0x06;
		tempbuf[1]=0x04;
		tempbuf[2]=0x01;
		time_sec = *(revbuf+3) + *(revbuf+4)*16*16 + *(revbuf+5)*16*16*16*16 + *(revbuf+6)*16*16*16*16*16*16 + 8*60*60;
		localtime_r(&time_sec,&tmtime);

		set_t.nDay= tmtime.tm_mday;
		set_t.nHour= tmtime.tm_hour;
		set_t.nMin= tmtime.tm_min;
		set_t.nMonth= tmtime.tm_mon + 1;
		set_t.nSec= tmtime.tm_sec;
		set_t.DayIndex= tmtime.tm_wday;
		set_t.nYear= tmtime.tm_year + 1900;

		mmi_dt_set_rtc_dt(&set_t);
		bkss_dbg_print(MOD_ENG,"UPDATE TIME %4d.%2d.%2d %2d:%2d:%2d week index := %2d",set_t.nYear,set_t.nMonth,set_t.nDay,set_t.nHour,set_t.nMin,set_t.nSec,set_t.DayIndex);
	}

	else if(*(revbuf)==0x06 && *(revbuf+1)==0x05 && *(revbuf+2)==0x01 && *(revbuf+3)==0x01 && getToken(revbuf,4)) //查询关锁时间
	{
		struct tm tmtime = {0};
		long t_time = 0;

		tmtime.tm_year= bel_bkss.lock_time.nYear-1900;
		tmtime.tm_mon= bel_bkss.lock_time.nMonth - 1;
		tmtime.tm_mday= bel_bkss.lock_time.nDay;
		tmtime.tm_hour = bel_bkss.lock_time.nHour;
		tmtime.tm_min= bel_bkss.lock_time.nMin;
		tmtime.tm_sec= bel_bkss.lock_time.nSec;
		tmtime.tm_isdst= 0;

		
		t_time= mktime(&tmtime);

		t_time=t_time-8*60*60;
		//bkss_dbg_print(MOD_ENG,"%x, %d",t_time,t_time);
		tempbuf[0]=0x06;
		tempbuf[1]=0x06;
		tempbuf[2]=0x06;
		
		tempbuf[5]=t_time%256;
		t_time = t_time/256;
		tempbuf[6]=t_time%256;
		t_time = t_time/256;
		tempbuf[7]=t_time%256;
		t_time = t_time/256;
		tempbuf[8]=t_time%256;
		
		
		

		//bkss_dbg_print(MOD_ENG,"%x %x %x %x",tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6]);
		
		bkss_dbg_print(MOD_ENG,"LOCK TIME");
	}

	else if(*(revbuf)==0x02 && *(revbuf+1)==0x01 && *(revbuf+2)==0x01 && *(revbuf+3)==0x01&& getToken(revbuf,4)) //获取电量
	{
		kal_int16 vbat = iot_get_battery_voltage();
		kal_int16 vbat_temp = iot_get_battery_temperature();
		tempbuf[0]=0x02;
		tempbuf[1]=0x02;
		tempbuf[2]=0x01;
		tempbuf[3]=2;
		tempbuf[4]=vbat>>8;
		tempbuf[5]=vbat;
		tempbuf[6]=2;
		tempbuf[7]=vbat_temp>>8;
		tempbuf[8]=vbat_temp;
		bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);
		bkss_dbg_print(MOD_ENG,"CHARGE1");
	}


	else if(*(revbuf)==0x02 && *(revbuf+1)==0x01 && *(revbuf+2)==0x01 && *(revbuf+3)==0x02&& getToken(revbuf,4)) //获取内外部电量
	{
		kal_int16 vbat = iot_get_battery_voltage();
		kal_int16 vbat_temp = iot_get_battery_temperature();
		kal_int16 vbus_v = iot_get_battery_vcharger();
		tempbuf[0]=0x02;
		tempbuf[1]=0x02;
		tempbuf[2]=0x02;
		tempbuf[3]=2;
		tempbuf[4]=vbat>>8;
		tempbuf[5]=vbat;
		tempbuf[6]=2;
		tempbuf[7]=vbat_temp>>8;
		tempbuf[8]=vbat_temp;
		tempbuf[9]=2;
		tempbuf[10]=vbus_v>>8;
		tempbuf[11]=vbus_v;
		bkss_dbg_print(MOD_ENG,"%s() %d tempbuf=%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);
		bkss_dbg_print(MOD_ENG,"CHARGE2");
	}


	else if(*(revbuf)==0x05 && *(revbuf+1)==0x01 && *(revbuf+2)==0x06 && getToken(revbuf,9)&&check_psw(revbuf+3)) //开锁
	{
		applib_time_struct curr_time;
		applib_dt_get_rtc_time(&curr_time);
		bkss_dbg_print(MOD_ENG,"open_lock0");
		bkss_dbg_print(MOD_ENG,"sizeof(bel_bkss) = %d, sizeof(nvram_ef_bel_bkss_struct) = %d",sizeof(bel_bkss),sizeof(nvram_ef_bel_bkss_struct));
		bt_open_lock_ex();
		g_lock_cntx.is_has_order_num = KAL_FALSE;
		save_is_has_order_num(g_lock_cntx.is_has_order_num);
		memset(open_lock_buf,0,sizeof(open_lock_buf));
		open_lock_buf[0] = 0x05;
		open_lock_buf[1] = 0x02;
		open_lock_buf[2] = 0x01;
		open_lock_buf[3] = 0x00;  //00 开锁成功 01 开锁失败
		open_lock_buf[4] = 0x07;
		open_lock_buf[5] = curr_time.nYear>>8;
		open_lock_buf[6] = curr_time.nYear;
		open_lock_buf[7] = curr_time.nMonth;
		open_lock_buf[8] = curr_time.nDay;
		open_lock_buf[9] = curr_time.nHour;
		open_lock_buf[10] = curr_time.nMin;
		open_lock_buf[11] = curr_time.nSec;
		bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);
		changing_psd_key = MMI_FALSE;
		return 0;	
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x11 && *(revbuf+2)==0x06 && getToken(revbuf,9)&&check_psw(revbuf+3)) //订单号开锁
	{
		memset(bt_order_num,0,BT_LOCK_ORDER_NUM_MAX_LEN);
		bkss_dbg_print(MOD_ENG,"%s() %d",__func__,__LINE__);
		bt_order = KAL_TRUE;
		changing_psd_key = MMI_FALSE;
		return 0;	
	}
	else if((*(revbuf)==0x05 && *(revbuf+1)==0x12)&&bt_order) //订单号开锁
	{
		kal_int8 t = *(revbuf+2);
		kal_int8 ll =  t - 1;
		kal_int8 current = *(revbuf+3);
		kal_uint8 order = (t%12 == 0)?(t/12):(t/12 + 1);
		kal_uint8 check = 0;
		
		if(t >= BT_LOCK_ORDER_NUM_MAX_LEN)
		{
			changing_psd_key = MMI_FALSE;
			return 0;	
		}
		bkss_dbg_print(MOD_ENG,"%s() %d ll=%d t=%d",__func__,__LINE__,ll,t);
		if((t%12 != 0)&&(order == current +1))
			memcpy(bt_order_num + current*12,revbuf + 4,t%12);
		else
			memcpy(bt_order_num + current*12,revbuf + 4,12);
		if(order == current +1)
		{
			applib_time_struct curr_time;
			kal_uint8 temp_order_buff[BT_LOCK_ORDER_NUM_MAX_LEN] = {0}; 
			applib_dt_get_rtc_time(&curr_time);
			bkss_dbg_print(MOD_ENG,"%s() %d bt_order_num=%s",__func__,__LINE__,bt_order_num,strlen(bt_order_num));	
			if(bt_check_code(bt_order_num,&ll) == bt_order_num[ll])
			{
				if(ll>0)
				{
					memcpy(temp_order_buff, bt_order_num, ll);
				}
				if(check_order_num(temp_order_buff, ll,BT_UNLOCK, 0) == 0)
				{
					nvram_ef_order_num order = {0};
					tempbuf[0] = 0x05;
					tempbuf[1] = 0x13;
					tempbuf[2] = 0x01;
					tempbuf[3] = 0x01;  //01 开锁失败
					tempbuf[4] = 0x07;
					tempbuf[5] = curr_time.nYear>>8;
					tempbuf[6] = curr_time.nYear;
					tempbuf[7] = curr_time.nMonth;
					tempbuf[8] = curr_time.nDay;
					tempbuf[9] = curr_time.nHour;
					tempbuf[10] = curr_time.nMin;
					tempbuf[11] = curr_time.nSec;
					changing_psd_key = MMI_FALSE;
				}
				else
				{
					bt_open_lock_ex();
					memset(open_lock_buf,0,sizeof(open_lock_buf));
					open_lock_buf[0] = 0x05;
					open_lock_buf[1] = 0x13;
					open_lock_buf[2] = 0x01;
					open_lock_buf[3] = 0x00;  //00 开锁成功 01 开锁失败
					open_lock_buf[4] = 0x07;
					open_lock_buf[5] = curr_time.nYear>>8;
					open_lock_buf[6] = curr_time.nYear;
					open_lock_buf[7] = curr_time.nMonth;
					open_lock_buf[8] = curr_time.nDay;
					open_lock_buf[9] = curr_time.nHour;
					open_lock_buf[10] = curr_time.nMin;
					open_lock_buf[11] = curr_time.nSec;
					bt_order = KAL_FALSE;
					bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);
					changing_psd_key = MMI_FALSE;
					return 0;	
				}
			}
			else
			{
				tempbuf[0] = 0x05;
				tempbuf[1] = 0x13;
				tempbuf[2] = 0x01;
				tempbuf[3] = 0x02;  //订单号校验失败
				tempbuf[4] = 0x07;
				tempbuf[5] = curr_time.nYear>>8;
				tempbuf[6] = curr_time.nYear;
				tempbuf[7] = curr_time.nMonth;
				tempbuf[8] = curr_time.nDay;
				tempbuf[9] = curr_time.nHour;
				tempbuf[10] = curr_time.nMin;
				tempbuf[11] = curr_time.nSec;
				bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);
				bt_order = KAL_FALSE;
			}
		}
		else
		{
			changing_psd_key = MMI_FALSE;
			return 0;	
		}
	}


	else if(*(revbuf)==0x05 && *(revbuf+1)==0x31 && *(revbuf+2)==0x08 && getToken(revbuf,11)&&check_psw(revbuf+3)) //流水号开锁
	{
		applib_time_struct curr_time;
		applib_dt_get_rtc_time(&curr_time);
		bkss_dbg_print(MOD_ENG,"open_lock2");
		bt_open_lock_ex();
		g_lock_cntx.is_has_order_num = KAL_FALSE;
		save_is_has_order_num(g_lock_cntx.is_has_order_num);
		memset(open_lock_buf,0,sizeof(open_lock_buf));
		open_lock_buf[0] = 0x05;
		open_lock_buf[1] = 0x32;
		open_lock_buf[2] = 0x01;
		open_lock_buf[3] = 0x00;  //00 开锁成功 
		open_lock_buf[4] = 0x07;
		open_lock_buf[5] = curr_time.nYear>>8;
		open_lock_buf[6] = curr_time.nYear;
		open_lock_buf[7] = curr_time.nMonth;
		open_lock_buf[8] = curr_time.nDay;
		open_lock_buf[9] = curr_time.nHour;
		open_lock_buf[10] = curr_time.nMin;
		open_lock_buf[11] = curr_time.nSec;
		bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);
		changing_psd_key = MMI_FALSE;
		return 0;	
	}

	else if(*(revbuf)==0x05 && *(revbuf+1)==0x0C && *(revbuf+2)==0x01 && *(revbuf+3)==0x01 && getToken(revbuf,4)) //关锁
	{
		bkss_dbg_print(MOD_ENG,"close_lock");
		reset_lock();
		bResp=MMI_FALSE;
		StartTimer(BLE_BKSS_CLOSE_TIMER, 200, CloseLockTimer);
	}


	else if(*(revbuf)==0x05 && *(revbuf+1)==0x0e && *(revbuf+2)==0x01 && *(revbuf+3)==0x01 && getToken(revbuf,4)) //查询锁状态
	{
		
		LOCK_STATE state = read_lock_state();
		bkss_dbg_print(MOD_ENG,"state");
		tempbuf[0]=0x05;
		tempbuf[1]=0x0F;
		tempbuf[2]=0x01;
		if(state == UNLOCKED)
			tempbuf[3]=0x00;	  //open
		else if(state == LOCKED)
			tempbuf[3]=0x01;	 //close
	}

	else if(*(revbuf)==0x05 && *(revbuf+1)==0x03 && *(revbuf+2)==0x06 && getToken(revbuf,9)&&check_psw(revbuf+3)) //修改密码 old password
	{
		kal_prompt_trace(MOD_ENG,"OLD PASSWORD");
		changing_psd_key = MMI_TRUE;
		StartTimer(BOOTUP_LED_BEEP_TIMER, 2*1000, changing_stop);
		return 0;	
	}
	else if(changing_psd_key&&(*(revbuf)==0x05 && *(revbuf+1)==0x04 && *(revbuf+2)==0x06 && getToken(revbuf,9)))
	{
		MMI_BOOL res;
	
		kal_prompt_trace(MOD_ENG,"NEW PASSWORD");
		res = change_psw(revbuf+3);
		tempbuf[0]=0x05;
		tempbuf[1]=0x05;
		tempbuf[2]=0x01;
		if(res == MMI_TRUE)
		{
			tempbuf[3]=0x00;
		}
		else
		{
			tempbuf[3]=0x01;
		}
		kal_prompt_trace(MOD_ENG,"NEW PASSWORD tempbuf[3] = %d psd=%x,%x,%x,%x,%x,%x",tempbuf[3],*(revbuf+3),*(revbuf+4),*(revbuf+5)
			,*(revbuf+6),*(revbuf+7),*(revbuf+8));
	}
	else if(*(revbuf)==0x07 && *(revbuf+1)==0x01 && *(revbuf+2)==0x08) //修改秘钥 
	{
	//保存秘钥
		int i;
		kal_prompt_trace(MOD_ENG,"NEW KEY1");
		memset(temp_key,0,sizeof(temp_key));
		for(i=0;i<8;i++)
		{
			temp_key[i]=revbuf[i+3];
		}
		changing_psd_key = MMI_TRUE;
		StartTimer(BOOTUP_LED_BEEP_TIMER, 3*1000, changing_stop);
		return 0;	
	}

	else if(changing_psd_key&&(*(revbuf)==0x07 && *(revbuf+1)==0x02 && *(revbuf+2)==0x08)) //修改秘钥 
	{
	//保存秘钥
		int i;
		MMI_BOOL res;
		kal_prompt_trace(MOD_ENG,"NEW KEY2");
		for(i=0;i<8;i++)
		{
			temp_key[i+8]=revbuf[i+3];
		}
		res = change_key(temp_key);
		tempbuf[0]=0x07;
		tempbuf[1]=0x03;
		tempbuf[2]=0x01;
		if(res == MMI_TRUE)
		{
			tempbuf[3]=0x00;
		}
		else
		{
			tempbuf[3]=0x01;
		}
		kal_prompt_trace(MOD_ENG,"NEW KEY2 tempbuf[3] = %d temp_key=%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",tempbuf[3],temp_key[0],temp_key[1],temp_key[2],temp_key[3]
			,temp_key[4],temp_key[5],temp_key[6],temp_key[7],temp_key[8],temp_key[9],temp_key[10],temp_key[11],temp_key[12],temp_key[13],temp_key[14],temp_key[15]);

	}

	else if(*(revbuf)==0x05 && *(revbuf+1)==0x14 && *(revbuf+2)==0x01 && *(revbuf+3)==0x01 && getToken(revbuf,4)) //设置还车状态
	{
		bkss_dbg_print(MOD_ENG,"SET BACK");
		tempbuf[0]=0x05;
		tempbuf[1]=0x15;
		tempbuf[2]=0x01;
	}
	
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x20 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //查询锁的工作模式
	{
		kal_uint32 mode = get_lock_curr_mode(); 
		bkss_dbg_print(MOD_ENG,"QUERY LOCK MODE");
		tempbuf[0]=0x05;
		tempbuf[1]=0x20;
		tempbuf[2]=0x01;
		if(mode == 0)
		{
			tempbuf[3]=0x00; //正常模式
		}
		else
		{
			tempbuf[3]=0x01; //运输模式
		}
		
	}
	
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x21 && *(revbuf+2)==0x01 && getToken(revbuf,4))  //设置锁的工作模式
	{
		kal_uint8 mode = *(revbuf+3);
		if(mode == 0x02)
		{
			tempbuf[0]=0x05;
			tempbuf[1]=0x21;
			tempbuf[2]=0x01;
			tempbuf[3]=0x00;
			srv_reboot_normal_start();
		}
		else
		{
			bt_switch_lock_mode(mode,bt_switch_lock_mode_callback);
			changing_psd_key = MMI_FALSE;
			return 0;	
		}
	}
	
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x22 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //查询锁的工作状态
	{
		LOCK_STATE state = read_lock_state();
		kal_int16 gsm_strength = get_gsm_strength();
		kal_int16 vbat = iot_get_battery_voltage();
		kal_int16 vbat_temp = iot_get_battery_temperature();
		bkss_dbg_print(MOD_ENG,"QUERY LOCK STATE");
		tempbuf[0]=0x05;
		tempbuf[1]=0x22;
		tempbuf[2]=0x08;
		if(state == UNLOCKED)  //锁震动状态
		{
			tempbuf[3]=0x00;	  //open
		}	
		else if(state == LOCKED)
		{
			tempbuf[3]=0x01;	 //close
		}	
		if(motion_sensor_state == 0)  //g-senser 异常
		{
			tempbuf[3] |=0x02;	  //open
		}	
		if(bkss_get_vib_status() == 1)  //锁震动状态
		{
			tempbuf[3] |=0x04;	  //open
		}	
		if(bmt_read_chr_status() == CHARGER_PLUG_IN)  //充电
		{
			tempbuf[3] |=0x08;	  //open
		}	
		bkss_dbg_print(MOD_ENG,"enter bkss_set_vib_status tempbuf[3] ===%x",tempbuf[3]);
		tempbuf[4] = iot_get_battery_percentage();

		tempbuf[4] = get_gsm_state();

		tempbuf[5] = get_gprs_duration_time();

		tempbuf[6] = gsm_strength > 0 ? gsm_strength : -gsm_strength;

		tempbuf[7] = get_gps_state();

		tempbuf[8] = get_gps_duration_time();

		tempbuf[9] = get_gps_count();
		
		tempbuf[10]=2;
		
		tempbuf[11]=vbat>>8;
		
		tempbuf[12]=vbat;
		
		tempbuf[13]=2;
		
		tempbuf[14]=vbat_temp>>8;
		
		tempbuf[15]=vbat_temp;
		
	}
	
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x23 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4))   //查询锁的gsm
	{
		kal_uint8 i = 0;
		kal_char imei[SRV_IMEI_MAX_LEN + 1];
		kal_uint8 code[10];
		
		memset(imei,0,sizeof(imei));
		get_device_id_from_imei(code);

		bkss_dbg_print(MOD_ENG,"QUERY GSM");
		tempbuf[0]=0x05;
		tempbuf[1]=0x23;
		tempbuf[2]=0x06;
		for(i=0;i<8;i++)
		{
			tempbuf[3+i]=code[i];
		}
		

	}
	
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x24 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4))  //查询锁的gsm版本号
	{
		if(verno_date_time[0] == 0)
		{
			verno_date_time_init();
			bkss_dbg_print(MOD_ENG,"QUERY GSM VERSION");
		}
	
		tempbuf[0]=0x05;
		tempbuf[1]=0x24;
		tempbuf[2]=strlen(verno_date_time);
		for(i=0;i<tempbuf[2];i++)
		{
			tempbuf[3+i]=verno_date_time[i];
		}
		bkss_dbg_print(MOD_ENG,"%d tempbuf=%s",tempbuf[2],verno_date_time);
	}
	else if(*(revbuf)==0x03 && *(revbuf+1)==0x01 && *(revbuf+2)==0x01 && *(revbuf+3)==0x01 && getToken(revbuf,4)) //进入空中升级模式
	{
		bkss_dbg_print(MOD_ENG,"UPDATE VERSION");
		tempbuf[0]=0x03;
		tempbuf[1]=0x02;
		tempbuf[2]=0x01;
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x28 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //查询ICC ID
	{
		bkss_send_iccid();
		changing_psd_key = MMI_FALSE;
		return 0;	
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x29 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //查询IMEI
	{
		kal_uint8 i = 0;
		kal_char imei[SRV_IMEI_MAX_LEN + 1];
		kal_uint8 code[10];
		
		memset(imei,0,sizeof(imei));
		//get IMEI
		if(srv_imei_get_imei(MMI_SIM1, imei, SRV_IMEI_MAX_LEN+1)) {
			for(i = 0; i < SRV_IMEI_MAX_LEN; i += 2 ) {
				if(i == SRV_IMEI_MAX_LEN - 1)
					bkss_imei[i/2] = (imei[i] - '0') * 16;
				else
					bkss_imei[i/2] = (imei[i] - '0') * 16 + (imei[i+1] - '0');
				bkss_dbg_print(MOD_ENG,"GET IMEI bkss_imei==%x,bkss_imei[i+1]==%x",imei[i],imei[i+1]);
				bkss_dbg_print(MOD_ENG,"GET IMEI bkss_imei==%x",bkss_imei[i/2]);
			}
		}

		tempbuf[0]=0x05;
		tempbuf[1]=0x29;
		tempbuf[2]=0x08;
		for(i=0;i<8;i++)
		{
			tempbuf[3+i]=bkss_imei[i];
		}	
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x30 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //查询域名
	{
		kal_uint8 count,l;
		kal_uint8 j = 0;
		l = strlen(g_lock_cntx.domain.domain_buffer);
		count = l/BKSS_DOMAIN_MAX_LEN;
		count += (l%BKSS_DOMAIN_MAX_LEN)?1:0;
	 	bkss_dbg_print(MOD_ENG,"GET DOMAIN");
		for(i=0;i<count;i++)
		{
			memset(tempbuf, 0, sizeof(tempbuf));
			tempbuf[0]=0x05;
			tempbuf[1]=0x30;
			tempbuf[2]=l;
			tempbuf[3]=i;
			for(j=0;j<BKSS_DOMAIN_MAX_LEN;j++)
			{
				if((BKSS_DOMAIN_MAX_LEN*i+j) < l)
				{
					tempbuf[4+j]=g_lock_cntx.domain.domain_buffer[BKSS_DOMAIN_MAX_LEN*i+j];
					continue;
				}
				else
				{
					break;
				}
			}
			bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);
			bikesw_send_data(tempbuf,len);
		}	
		changing_psd_key = MMI_FALSE;
		return 0;	
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x32 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //查询IP
	{
		bkss_dbg_print(MOD_ENG,"GET IP");
		tempbuf[0]=0x05;
		tempbuf[1]=0x32;
		tempbuf[2]=0x04;
		for(i=0;i<4;i++)
		{
			tempbuf[3+i] = g_lock_cntx.server_ip.addr[i + 1];
		}
		bkss_dbg_print(MOD_ENG,"GET IMEI g_lock_cntx.server_ip addr:%d.%d.%d.%d.%d",
			g_lock_cntx.server_ip.addr[0],g_lock_cntx.server_ip.addr[1], g_lock_cntx.server_ip.addr[2],g_lock_cntx.server_ip.addr[3],g_lock_cntx.server_ip.addr[4]);
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x33 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //查询预留nv第一位
	{
		tempbuf[0] = 0x05;
		tempbuf[1] = 0x34;
		tempbuf[2] = 0x01;
		tempbuf[3] = iot_get_bkup_nv_first_byte();
		bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);		
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x35 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //查询串口状态
	{
		tempbuf[0] = 0x05;
		tempbuf[1] = 0x36;
		tempbuf[2] = 0x01;
		tempbuf[3] = iot_get_bkup_nv_third_byte();
		bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);		
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x37 && *(revbuf+2)==0x01 && (*(revbuf+3)==0x00||*(revbuf+3)==0x01 )&& getToken(revbuf,4)) //设置串口状态
	{
		iot_uart1_power_ctrl(*(revbuf+3));
		iot_set_bkup_nv_third_byte(*(revbuf+3));
		tempbuf[0] = 0x05;
		tempbuf[1] = 0x38;
		tempbuf[2] = 0x01;
		tempbuf[3] = 0x00;
		bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);		
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x39 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //找车
	{
		iot_led_speak_warning_callback();
		bkss_dbg_print(MOD_ENG,"GET buf[0]");
		tempbuf[0] = 0x05;
		tempbuf[1] = 0x40;
		tempbuf[2] = 0x01;
		tempbuf[3] = 0x00;
		bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);		
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x41 && *(revbuf+2)==0x01 && *(revbuf+3)==0x00 && getToken(revbuf,4)) //SN
	{
		kal_uint8 sn_buf[64] = {0};	
		memset(sn_buf,0,sizeof(sn_buf));
		ReadRecordIot(NVRAM_EF_BARCODE_NUM_LID, 1, sn_buf, 64);
		tempbuf[0] = 0x05;
		tempbuf[1] = 0x42;
		tempbuf[2] = 0x0D;
		for (i = 0; i < 13; ++i) 
		{			
			tempbuf[3+i] = sn_buf[i];
		}		
		bkss_dbg_print(MOD_ENG, "%s() %d tempbuf =%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",__func__,__LINE__,tempbuf[0],tempbuf[1],tempbuf[2],tempbuf[3],tempbuf[4],tempbuf[5],tempbuf[6],tempbuf[7],tempbuf[8],tempbuf[9],tempbuf[10],tempbuf[11],tempbuf[12],tempbuf[13],tempbuf[14],tempbuf[15]);		
	}
	else if(*(revbuf)==0x05 && *(revbuf+1)==0x43 && *(revbuf+2)==0x01 && (*(revbuf+3)==0x00||*(revbuf+3)==0x01 )&& getToken(revbuf,4)) //白天黑夜铃声测试
	{
		bkss_set_volume_test(*(revbuf+3));
		iot_play_tone_with_filepath_ext(IOT_TONE_UNLOCK,SRV_PROF_RING_TYPE_ONCE,bkss_volume_test);
		return 0;
	}
	else
	{
		bkss_dbg_print(MOD_ENG,"bResp = MMI_FALSE");
		bResp = MMI_FALSE;
	}
	changing_psd_key = MMI_FALSE;



	if(bResp)
	{
		//for(i=0;i<len;i++)
		//{
		//	bkss_dbg_print(MOD_ENG,"bkss_srv_app_data_to_read_notify_cb tempbuf[%d]=%x\r\n",i,*(tempbuf+i));
		//}


		bkss_dbg_print(MOD_ENG,"bResp = MMI_TRUE");
		//bkss_dbg_print(MOD_ENG,"bkss_srv_app_data_to_read_notify_cb bikesw_send_data= %s",tempbuf);
		bikesw_send_data(tempbuf,len);	
		return 0;	
	}	
	else
	{
		return -1;	
	}	
}
void bkss_srv_app_wechatdata_to_read_notify_cb(char *bd_addr, void* buf, U32 len)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	char *testbuf;
	//char* rbd_addr;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	bkss_srv_app_data_to_read_notify_cb(bd_addr,buf,len);
	
}

/*****************************************************************************
 * FUNCTION
 *  bkss_srv_app_config_level_alert
 * DESCRIPTION
 *  to notify reporter mmi task link loss alert level config result
 * PARAMETERS
 *  [bdaddr] Return remote bluetooth device addr
 *  [level] alert level of LLS 
 * RETURNS
 *  void
 *****************************************************************************/
void bkss_srv_app_read_data_request_cb(char *bdaddr, void* buf, U32 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	//char* rbd_addr = NULL;
    U8 temp_alert_level;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
   
	int i;
	char *testbuf;
	testbuf=(char *)buf;
	for(i=0;i<len;i++)
		{
		bkss_dbg_print(MOD_ENG,"bkss_srv_app_data_to_read_notify_cb buf[%d]=%x\r\n",i,*(testbuf+i));
		}
	
    // save the temp_alert_level to NVRAM
}

void bkss_srv_app_read_wechatindicate_request_cb(char *bdaddr, void* buf, U32 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
}

void bkss_srv_app_read_wechatdata_request_cb(char *bdaddr, void* buf, U32 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
}
/*****************************************************************************
 * FUNCTION
 *  bkss_srv_app_power_on_to_init
 * DESCRIPTION
 *  This function is to rigister pbkss profile CBs
 * PARAMETERS
 *  void
 *
 *
 * RETURNS
 *  void
 *****************************************************************************/
void bkss_srv_app_power_on_to_init(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	//bkss_callback_t bkss_cb_cntx;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	//MMI_TRACE(MMI_CONN_TRC_G7_BT, TRC_BLE_BKSSAPP_SWITCH_INIT);
	/*rigister callback to enable reporter role*/
	bkss_cb_cntx.connection_state_cb = bkss_srv_app_connection_state_cb;
	bkss_cb_cntx.data_to_read_notify_cb = bkss_srv_app_data_to_read_notify_cb;
	bkss_cb_cntx.read_data_request_cb = bkss_srv_app_read_data_request_cb;
	bkss_cb_cntx.read_wechatindicate_request_cb = bkss_srv_app_read_wechatindicate_request_cb;
	bkss_cb_cntx.wechatdata_to_read_notify_cb = bkss_srv_app_wechatdata_to_read_notify_cb;
	bkss_cb_cntx.read_wechatdata_request_cb = bkss_srv_app_read_wechatdata_request_cb;
	bkss_init(&bkss_cb_cntx);
}


/*****************************************************************************
 * FUNCTION
 *  mmi_bt_power_on_to_deinit
 * DESCRIPTION
 *  This function is to received service event
 * PARAMETERS
 *  void
 *
 *
 * RETURNS
 *  void
 *****************************************************************************/
void bkss_srv_power_on_to_deinit(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables												  */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body													  */
	/*----------------------------------------------------------------*/	
	//MMI_TRACE(MMI_CONN_TRC_G7_BT, TRC_BLE_BKSSAPP_SWITCH_DEINIT);
	/*derigister callback to unable reporter role*/
	bkss_deinit(&bkss_cb_cntx);
	bkss_cb_cntx.connection_state_cb = NULL;
	bkss_cb_cntx.data_to_read_notify_cb = NULL;
	bkss_cb_cntx.read_data_request_cb = NULL;
	bkss_cb_cntx.read_wechatindicate_request_cb = NULL;
	bkss_cb_cntx.wechatdata_to_read_notify_cb = NULL;
	bkss_cb_cntx.read_wechatdata_request_cb = NULL;
	return;
}


/*****************************************************************************
 * FUNCTION
 *  bkss_srv_app_init
 * DESCRIPTION
 *  to rigister callbacks and enable proximity reporter role
 * PARAMETERS
 *  void
 * RETURNS
 *  Status of profile enable result
 *****************************************************************************/
void bkss_srv_app_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	U32 event_mask;
	/*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    event_mask = SRV_BT_CM_EVENT_ACTIVATE |
                 SRV_BT_CM_EVENT_BEGIN_DEACTIVATE;
	srv_bt_cm_set_notify((srv_bt_cm_notifier)bkss_srv_app_event_notify, event_mask, NULL);
	return;	
}

void mmi_bkssapp_server_init(void)
{
	bkss_srv_app_init();
	bkss_get_battery();
}



/*****************************************************************************
 * FUNCTION
 *  bt_srv_app_event_notify
 * DESCRIPTION
 *  This function is to received service event
 * PARAMETERS
 *  void
 *
 *
 * RETURNS
 *  void
 *****************************************************************************/
void bkss_srv_app_event_notify(U32 event, void* para)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
   //MMI_TRACE(MMI_CONN_TRC_G7_BT, TRC_BLE_BKSSAPP_SWITCH_PROC, event, 0);
    switch (event)
    {
        case SRV_BT_CM_EVENT_ACTIVATE:
            bkss_srv_app_power_on_to_init();
            break;

        case SRV_BT_CM_EVENT_BEGIN_DEACTIVATE:
            bkss_srv_power_on_to_deinit();
            break;

		default :
			break;
	}

}


void bike_switch_bt_power_on_handle()
{
    srv_bt_cm_power_status_enum ret;

    bkss_dbg_print(MOD_ENG, "[BKSS] bt_switch_mode_power_on_handle");
    ret = srv_bt_cm_get_power_status();
    if (ret == SRV_BT_CM_POWER_OFF)
    {
        srv_bt_cm_switch_on();
    }
}

void bike_switch_bt_power_off_handle()
{
    srv_bt_cm_power_status_enum power_status;
    srv_bt_cm_result_codes_enum conn_status;

    bkss_dbg_print(MOD_ENG, "[BKSS] bt_switch_mode_power_off_handle");
    power_status = srv_bt_cm_get_power_status();
    if (power_status == SRV_BT_CM_POWER_ON)
    {
        conn_status = srv_bt_cm_release_all_conn();
        if (conn_status == SRV_BT_CM_RESULT_NO_CONNECTION)
        {
            srv_bt_cm_switch_off();
        }
    }
}

#endif

