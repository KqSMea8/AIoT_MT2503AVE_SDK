#include "adups_typedef.h"
#include "adups_debug.h"
#include "adups_define.h"
#include "MMI_include.h"
#include "ImeiSrvGprot.h"
#include "med_utility.h"
#include "verno.h"
#ifdef ADUPS_AUTO_TEST
#include "nvram_struct.h"
#endif

#include "adups_device.h"


#ifdef __ADUPS_FOTA_GPS__
#define ADUPS_FOTA_SERVICE_GPS_MODEL         "YS_GPS"
#endif

adups_extern void adups_get_device_time(Adups_time_struct* time);

adups_extern void ADUPS_CK_confirm_lsk(void);
adups_int32 AdupsGetUpState(adups_int32 adupsUpState)
{
	return adupsUpState;
}

adups_int32 AdupsGetFotaState(adups_int32 adupsFotaState)
{
	return adupsFotaState;
}
void AdupsDownData(void)
{
	ADUPS_CK_confirm_lsk();
}

 adups_int32 adups_sim_ready(void)
{
	if(!srv_sim_ctrl_is_available(MMI_SIM1))
		return E_ADUPS_FAILURE;

	else
		return E_ADUPS_SUCCESS;

}


adups_int32 adups_get_device_mid(adups_char* mid)
{
#if 0//def ADUPS_AUTO_TEST
	if(mid)
		adups_memcpy(mid, adups_imei_test, adups_strlen(adups_imei_test));
	
	return E_ADUPS_SUCCESS;
	
#else
	if(!mid)
		return E_ADUPS_FAILURE;
	
	if(adups_em_mode() == ADUPS_TRUE)
	{
		adups_em_get_mid(mid);
		return E_ADUPS_SUCCESS;
	}
	
	if(!srv_imei_get_imei((mmi_sim_enum)MMI_SIM1,mid,(SRV_IMEI_MAX_LEN + 1)))
		return E_ADUPS_FAILURE;
	else
		return E_ADUPS_SUCCESS;
#endif	
}



 extern kal_char *release_verno(void);
adups_char* adups_get_device_version(void)
{
#if 1
	static adups_char verno_str[100];
	adups_uint8 verno_size;
	verno_size=adups_strlen(release_verno())+adups_strlen(build_date_time());
	adups_memset(verno_str,0,100);
	adups_snprintf(verno_str, 100, "%s_%s",release_verno(),build_date_time());
	return verno_str;      
#else
       return "MAUI.11C.W13.52.SP3.V2.F6";
#endif
      
}


extern kal_char* gps_p_get_fireware_version(void);
adups_char* adups_get_gps_version(void)
{
#ifdef __ADUPS_FOTA_GPS__
	return gps_p_get_fireware_version();
#endif
}

adups_char* adups_get_device_gps_model(void)
{
#ifdef __ADUPS_FOTA_GPS__
	return ADUPS_FOTA_SERVICE_GPS_MODEL;
#endif
}

adups_char* adups_get_device_oem(void)
{
	return ADUPS_FOTA_SERVICE_OEM;	
}

adups_char* adups_get_device_model(void)
{
	return ADUPS_FOTA_SERVICE_MODEL;		
}

adups_char* adups_get_device_product_id(void)
{
	return ADUPS_FOTA_SERVICE_PRODUCT_ID;		
}

adups_char* adups_get_device_product_sec(void)
{
	return ADUPS_FOTA_SERVICE_PRODUCT_SEC;		
}

adups_char* adups_get_device_type(void)
{
	return ADUPS_FOTA_SERVICE_DEVICE_TYPE;		
}

adups_char* adups_get_device_platform(void)
{
	return ADUPS_FOTA_SERVICE_PLATFORM;		
}

adups_char* adups_get_device_network_type(void)
{
	return ADUPS_DEVICE_NETWORKTYPE;		
}

void adups_get_device_time(Adups_time_struct* time)
{
	applib_dt_get_rtc_time((applib_time_struct*)time);
}

adups_uint32 adups_get_utc_time_sec(Adups_time_struct* time)
{
	return applib_dt_mytime_2_utc_sec((applib_time_struct*)time,KAL_FALSE);
}

adups_uint8 adups_get_device_battery_level(void)
{
	return srv_charbat_get_battery_level();
}
extern void srv_reboot_normal_start();

extern void srv_reboot_direct_pwron_req(void);
void adups_device_start_reboot(void)
{	
	adups_DebugPrint_Ext("[ADUPS_COMMON]:","adups: reboot, please waiting...");
	//srv_alm_pwr_reset(1,3);
	srv_reboot_normal_start();
	//srv_reboot_direct_pwron_req();
	//ASSERT(0);
	//srv_alm_normal_reset(KAL_TRUE, 1);
}

void* adups_memalloc(adups_int32 size)
{
	void *ptr=NULL;
	
	ptr=mmi_malloc(size);
	//ptr=med_alloc_ext_mem(size);

	return ptr;
}

void adups_memfree(void *p)
{
			if(p != NULL)
        mmi_mfree(p);
        //med_free_ext_mem(p);
}

void* adups_memalloc_ext(adups_int32 size)
{
	void *ptr=NULL;
	ptr=med_alloc_ext_mem(size);
	return ptr;
}
void adups_memfree_ext(void *p)
{
	if(p != NULL)
        med_free_ext_mem((void **)&p);
}
adups_uint32 adups_entry_critical_timeout(void)
{
	return 80*1000;
}

void adups_entry_save_context(void)
{
	adups_DebugPrint_Ext("[ADUPS_COMMON]:","adups_entry_save_context");

}
extern adups_uint8 adups_get_dl_state(void);
extern adups_uint8 adups_get_fota_state(void);

void adups_exit_restore_context(void)
{
	
	adups_DebugPrint_Ext("[ADUPS_COMMON]:","adups_exit_restore_context adups_get_dl_state() is %d",adups_get_dl_state());
	if(adups_get_dl_state()==DL_SUCCESS)
	{
		adups_device_start_reboot();
	}
	adups_DebugPrint("adups_exit_restore_context");
}


void adups_get_device_data_point1(adups_char** name,adups_char** value)
{
	static adups_char areacode[10]={'\0'};
	//
	//name="XXX",value="XXXXX";
	//get_area_code((adups_uint8 *)areacode);
	*name=NULL;//"areaCode";
	*value=NULL;//areacode;
}
/*****************************
**up_result 输入，标识升级结果
**:1   升级成功
**:其他  升级失败
******************************/
adups_int32 adhups_get_upgrade_result(adups_int32 up_result)
{
		adups_DebugPrint("adhups_get_upgrade_result");

}
#if 0
void adups_get_device_data_point2(adups_char* name,adups_char* value)
{
	//??è?return null,
	//name="XXX",value="XXXXX";
	name="cellid";
	value="306254";	
}

void adups_get_device_data_point3(adups_char* name,adups_char* value)
{
	//??è?return null,
	//name="XXX",value="XXXXX";
	name="zuobing";
	value="wonderful";	
}

void adups_get_device_data_point4(adups_char* name,adups_char* value)
{
	//??è?return null,
	//name="XXX",value="XXXXX";
	name="zuobing";
	value="goood";	
}

void adups_get_device_data_point5(adups_char* name,adups_char* value)
{
	//??è?return null,
	//name="XXX",value="XXXXX";
	name="abc";
	value="52ahs";		
}
#endif

adups_uint32 adups_str2hex(const adups_char* pszStr, adups_uint32 u4Len)
{
	adups_uint32 u4Idx;
	adups_uint32 u4ReturnValue = 0;

	if ((pszStr == NULL) || (u4Len == 0))
	{
		return 0;
	}

	u4Len = (u4Len > 8) ? 8 : u4Len;

	for (u4Idx = 0;
		u4Idx < u4Len;
		u4Idx++)
	{
		if ((pszStr[u4Idx] >= '0') && (pszStr[u4Idx] <= '9'))
		{
			u4ReturnValue = u4ReturnValue << 4;
			u4ReturnValue += (adups_uint32)(adups_uint8)(pszStr[u4Idx] - '0');
		}
		else
		if ((pszStr[u4Idx] >= 'A') && (pszStr[u4Idx] <= 'F'))
		{
			u4ReturnValue = u4ReturnValue << 4;
			u4ReturnValue += (adups_uint32)(adups_uint8)(pszStr[u4Idx] - 'A' ) + 10;
		}
		else
		if ((pszStr[u4Idx] >= 'a') && (pszStr[u4Idx] <= 'f'))
		{
			u4ReturnValue = u4ReturnValue << 4;
			u4ReturnValue += (adups_uint32)(adups_uint8)(pszStr[u4Idx] - 'a') + 10;
		}
		else
		{
			return 0;
		}
	}

	return u4ReturnValue;
}




#ifdef ADUPS_AUTO_TEST
adups_uint8 adups_int2imei(adups_uint8 in, ADUPS_BOOL revert)
{
	adups_uint8 val=0, len=0;
	adups_char str[10]={0};
	
	adups_memset(str, 0, 10);
	//itoa(in, str, 10);
	adups_itoa(in, str, 10);
	len = adups_strlen(str);
	val = adups_str2hex(str, len);

	if(revert)
		return (adups_uint8)(val<<4 | ((val>>4)&0x0F));
	else
		return val;
}

void adups_set_nvram_imei(void)
{
    nvram_write_imei_req_struct* req;
    adups_uint32 val=0;
	Adups_time_struct dt;
	adups_uint8 tmp=0;
		
    req = (nvram_write_imei_req_struct*)construct_local_para(sizeof(nvram_write_imei_req_struct), TD_CTRL);
    req->access_id = MOD_ADUPS;
	req->record_index = 1;
	req->svn = 1;
	//adups_memcpy(req->imei, "820161129103018", sizeof("820161129103018"));

	adups_get_device_time(&dt);

	val = dt.nYear;
	req->imei[0] = adups_int2imei(val/100, 1);
	req->imei[1] = adups_int2imei(val%100, 1);
	req->imei[2] = adups_int2imei(dt.nMonth, 1);
	req->imei[3] = adups_int2imei(dt.nDay, 1);
	req->imei[4] = adups_int2imei(dt.nHour, 1);
	req->imei[5] = adups_int2imei(dt.nMin, 1);
	req->imei[6] = adups_int2imei(dt.nSec, 1);
	req->imei[7] = adups_int2imei(dt.DayIndex, 0);

	adups_task_send_msg(MOD_NVRAM, PS_NVRAM_SAP, MSG_ID_NVRAM_WRITE_IMEI_REQ,req, 0);	
}
#endif
/****************************
*download_status  网络状态 DL_NETWORK_ERROR 下载出错  DL_NO_SPACE 空间不足  DL_SUCCESS 下载成功
*adupsstatus CK_NEW_VERSION表示有新的版本   UP_SUCCESS 表示上报成功
*adupsfileSize  差分包的大小
* adupsstate fota state -->STATE_CV  表示检测版本 STATE_DL 下载差分包版本  STATE_RD  上报下载差分包下载结果  STATE_RU 上报升级结果
*adupsDlResult 下载差分包结果 DL_SUCCESS 表示下载成功
*version_name 最新版本的版本名字
**************************/
void GetMainStates(adups_char *version_name,adups_int32 download_status,adups_int32 adupsstatus,adups_int32 adupsfileSize,adups_int32 adupsstate,adups_uint8 adupsDlResult)
{
	if((adupsstatus == STATE_DL) && adupsDlResult != DL_SUCCESS) {
		//download failed
#if defined(__IOT_LOCK__)
		set_fota_download_status(0x02);
#endif
	}
}

void adups_process_status_cb(adups_int32 status,adups_uint8 fota_status)
{
	adups_DebugPrint("adups_process_status_cb: %d,%d", status,fota_status);

	switch(status)
	{
		case FOTA_SUCCESS:
		#ifdef ADUPS_AUTO_TEST
		if(fota_status==STATE_RU)
		{
			adups_Remove_File(adups_get_login_file_path());
			adups_set_nvram_imei();
		}
		#endif			
			break;

		default:
			break;
	}
}

void adups_fota_atcmd_custom(adups_char *cmd, adups_char *para0, adups_char *para1,adups_char *para2, adups_write func)
{
}



ADUPS_BOOL s_adups_em_mode = ADUPS_FALSE;
#ifdef ADUPS_ENGINE_MODE_ON
#define ADUPS_EM_MID_MAXLEN		20
adups_uint8 s_adups_em_imei[ADUPS_EM_MID_MAXLEN+1] = {0};
#endif

void adups_em_set_mode(ADUPS_BOOL mode)
{
	s_adups_em_mode = mode; 
}

ADUPS_BOOL adups_em_mode(void)
{
	return s_adups_em_mode;
}

void adups_em_set_mid(adups_char* mid)
{
#ifdef ADUPS_ENGINE_MODE_ON
	adups_uint8 len = adups_strlen(mid);
	
	if(!mid)
		return;
	
	if(len > SRV_IMEI_MAX_LEN)
		len  = SRV_IMEI_MAX_LEN;

	adups_memset(s_adups_em_imei, 0, ADUPS_EM_MID_MAXLEN+1);
	if(mid[0] != ' ')
	{
		adups_memcpy(s_adups_em_imei, mid, len);
		adups_DebugPrint("adups set my mid(only RAM valid):%s", s_adups_em_imei);
	}		
#endif		
}

void adups_em_get_mid(adups_char* mid)
{
#ifdef ADUPS_ENGINE_MODE_ON
	if(!mid)
		return;

	adups_memcpy(mid, s_adups_em_imei, adups_strlen(s_adups_em_imei));
#endif	
}

