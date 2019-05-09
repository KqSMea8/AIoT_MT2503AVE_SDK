#include "MMIDataType.h"
#include "MMITimer.h"
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
#include "soc_api.h"
#include "iotlock.h"
#include "btostypes.h"
#include "SimCtrlSrvGprot.h"
#include "l4c_common_enum.h"
#include "resource_audio.h"
#include "ProfilesSrv.h"

extern kal_uint32 iot_get_battery_voltage(void);
extern kal_int16 iot_get_battery_temperature(void);
extern kal_int16 iot_get_battery_vcharger(void);
extern void iot_set_system_sleep_mode_enable(void);
extern void iot_set_system_sleep_mode_disable(void);
extern void get_device_id_from_imei(BCD *device_id);
extern kal_char *release_verno(void);
extern kal_char* build_date_time(void); 
extern void rmmi_write_to_uart(kal_uint8 * buffer, kal_uint16 length, kal_bool stuff);
extern void switch_trans_mode();
extern	void enter_aging_test_mode();
extern void srv_reboot_normal_start();
extern void aging_test_ata();
extern void bkss_readRssi_ata(char *bdaddr);
extern void Bma250_read_data_xyz(void);
extern int bma250_get_chip_id(unsigned char *chip_id);
extern void acc_sensor_init_with_interrupt(void);
extern void acc_sensor_get_xyz(kal_int16 *accl_x,kal_int16 *accl_y,kal_int16 *accl_z);
extern void iot_set_bkup_nv_second_byte(kal_uint8 type);
extern kal_uint8 iot_get_bkup_nv_second_byte(void);
extern void open_LED1_test(void);
extern void iot_led_on_off_ctrl(kal_uint8 led_no, kal_bool b_on);
extern void iot_motor_start(kal_bool b_dir);
extern void iot_motor_stop(void);
extern kal_uint8 iot_get_switch_state(kal_uint8 num);
extern kal_int16 iot_get_battery_isense(void);
extern void iot_bat_charger_en(kal_bool chr_en);
extern void iot_ata_gps_open(void);
extern void iot_ata_reset_gps_count(void);
extern void iot_ata_gps_sleep(void);
extern kal_bool iot_led_speak_warning_callback(void);
extern void open_lock_ata(void);

#define __NEW_AT_HL300_SUPPORT__

extern srv_bt_cm_cntx_struct g_srv_bt_cm_cntx;
extern nvram_ef_bel_bkss_struct bel_bkss;
extern kal_uint8 agingbuf[512];
extern gprs_lock_cntx g_lock_cntx;
kal_uint16 iot_tone_id = 0;
extern void WriteDomain(char * buffer);
extern void WritePort(kal_uint32 port);
char domian_buffer[IOT_LOCK_DOMAIN_MAX_LEN];
kal_uint32	port;
kal_bool is_ata_port_flag =KAL_FALSE;
static BCD g_get_iccid_ext[10] = {0};

void iot_ata_play_speaker_start(void)
{
	kal_uint32 ret = 0;
	iot_tone_id = TONE_3000HZ_SOUND;
	ret = srv_prof_play_tone_with_id(SRV_PROF_TONE_ALARM,
							iot_tone_id,
							SRV_PROF_RING_TYPE_REPEAT,
							NULL);
}

void iot_ata_play_speaker_stop(void)
{
	iot_tone_id = 0;
	mdi_audio_suspend_background_play();
	srv_prof_stop_tone(SRV_PROF_TONE_ALARM);
	mdi_audio_stop_all();
}

void dump_buffer_ext(Byte *out, int len){
	char buffer[512] = {0};
	kal_uint8 *ptr = out;
	kal_uint8 *end = out+len;
	kal_uint16 count = 0;
	kal_uint16 i = 0;
	char *pre = "spconser:out=";
	kal_uint8 llen = 127 - strlen(pre);
	char sendbuffer[128+1];

	kal_prompt_trace(MOD_MMI, "spconser: enter %s", __FUNCTION__);
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
	sprintf(sendbuffer, "ICCID=%s", buffer);
	rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);

}

void get_iccid_rsp_ext(void *info) {
	kal_uint8 i = 0;
	mmi_smu_read_sim_rsp_struct *iccid_data;

	kal_prompt_trace(MOD_MMI, "spconser: %s enter begin", __FUNCTION__);
	iccid_data = (mmi_smu_read_sim_rsp_struct*) info;
	mmi_frm_clear_protocol_event_handler(MSG_ID_MMI_SMU_READ_SIM_RSP, get_iccid_rsp_ext);
	memset(g_get_iccid_ext, 0, sizeof(g_get_iccid_ext));
	if (iccid_data->result.flag == L4C_SUCCESS) {
		kal_uint8 a;
		memcpy(g_get_iccid_ext, iccid_data->data, iccid_data->length);
		for(i= 0; i < 10; i++) {
			a = g_get_iccid_ext[i];
			g_get_iccid_ext[i] = ((a&0x0f)<<4) + (a>>4);
		}
		dump_buffer_ext(g_get_iccid_ext, sizeof(g_get_iccid_ext));
	} else {
		kal_prompt_trace(MOD_MMI, "spconser: %s read iccid fail!", __FUNCTION__);
	}
}

void get_iccid_req_ext(void)
{
    MYQUEUE Message;
    mmi_smu_read_sim_req_struct *dataPtr;

    mmi_frm_set_protocol_event_handler(MSG_ID_MMI_SMU_READ_SIM_RSP,get_iccid_rsp_ext,MMI_TRUE);

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

kal_int16 iot_get_battery_temp_volt(void)
{
	kal_uint32 vbat_temp = 0;
	bmt_get_avg_adc_channel_voltage(DCL_VBATTMP_ADC_CHANNEL, &vbat_temp, 20);
	vbat_temp = vbat_temp/1000;
}

void WriteDomain_ata()
{
	WriteDomain(domian_buffer);
}
void WritePort_ata()
{
	WritePort(port);
}

void bt_get_bd_addr(BD_ADDR *dst,const srv_bt_cm_bt_addr *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memcpy((U8 *)&dst->addr[0], (U8 *)&src->lap, 3);
    dst->addr[3] = src->uap;
    memcpy((U8 *)&dst->addr[4], (U8 *)&src->nap, 2);
}

signed int ata_srv_app_notify_cb( char* buf, unsigned int len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 i;
    U8 buffer[16];
    char * revbuf = buf;
	U8 tempbuf[17];
	MMI_BOOL bResp;
	static MMI_BOOL changepassword = MMI_FALSE;
	static kal_uint8 HL300_ATA_test_OK_Flag = KAL_FALSE;
	char sendbuffer[128+1]; //MAUI_02377056
	//applib_time_struct ttime = {0};
	//char* rbd_addr;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	kal_prompt_trace(MOD_ENG,"notify_cb buf=%s\r\n",revbuf);

	memset(buffer,0,sizeof(buffer));

	len = 16;
	bResp = MMI_TRUE;
	is_ata_port_flag =KAL_FALSE;
	memset(tempbuf,0,sizeof(tempbuf));
	//命令解析
	if(*(revbuf)==0x06 && *(revbuf+1)==0x01 && *(revbuf+2)==0x01 && *(revbuf+3)==0x01 ) //秘钥
	{
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
		
	}

	else if(strncmp(revbuf,"AT+CHARGE",strlen("AT+CHARGE"))==0)//获取电量
	{
		kal_uint8 bat_level = 0;
		bat_level=iot_get_battery_percentage();
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "CHARGE=%d", bat_level);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		kal_prompt_trace(MOD_ENG,"CHARGE");
	}

	else if(strncmp(revbuf,"AT+ESLEEP",strlen("AT+ESLEEP"))==0) //enable sleep mode
	{
		kal_prompt_trace(MOD_ENG,"ESLEEP");
		iot_set_system_sleep_mode_enable();
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "SLEEP Mode:Enable");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+DSLEEP",strlen("AT+DSLEEP"))==0) //disable sleep mode
	{
		kal_prompt_trace(MOD_ENG,"DSLEEP");
		iot_set_system_sleep_mode_disable();
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "SLEEP Mode:Disable");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+RDGSMID",strlen("AT+RDGSMID"))==0) //AT+RDGSMID
	{
		kal_uint8 imei[SRV_IMEI_MAX_LEN + 1];
		memset(imei,0,sizeof(imei));
		//get IMEI
		if(!srv_imei_get_imei(MMI_SIM1, imei, SRV_IMEI_MAX_LEN+1))
		{
			//get IMEI failed
			kal_prompt_trace(MOD_MMI, "%s GET IMEI ERROR", __FUNCTION__);
		}
		kal_prompt_trace(MOD_ENG,"AT+RDGSMID");
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "GSMID:%s",(imei+3));
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+FINDBIKE",strlen("AT+FINDBIKE"))==0) //AT+FINDBIKE
	{
		kal_prompt_trace(MOD_ENG,"AT+FINDBIKE");
		is_ata_port_flag = KAL_TRUE;
		iot_led_speak_warning_callback();
		sprintf(sendbuffer, "OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

#ifdef __NEW_AT_HL300_SUPPORT__

	else if(strncmp(revbuf,"AT+HRDY",strlen("AT+HRDY"))==0)  //AT+HRDY
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HRDY");
		sprintf(sendbuffer, "READY");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HRDSN",strlen("AT+HRDSN"))==0) //AT+HRDSN
	{
		kal_int8 ret = 0;
		kal_uint8 sn_buf[64];
		kal_uint8 i =0;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HRDSN");
		for(i=0;i<64;i++)
		{
			sn_buf[i]=0x00;
		}
		ReadRecordIot(NVRAM_EF_BARCODE_NUM_LID, 1, sn_buf, 64);
		sprintf(sendbuffer, "SN : ");
		memcpy(sendbuffer+5, sn_buf, 13);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, (5+13), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HACCID",strlen("AT+HACCID"))==0)  // AT+HACCID
	{
		kal_uint8 bma253_id = 0x00;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HACCID");
		acc_sensor_init_with_interrupt();
		bma250_get_chip_id(&bma253_id);
		if(bma253_id == 0xFA)
		{
			sprintf(sendbuffer, "OK");
		}
		else
		{
			sprintf(sendbuffer, "ERROR");
		}
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HACCDATA",strlen("AT+HACCDATA"))==0)  // AT+HACCDATA
	{
		kal_int16 accl_x = 0, accl_y = 0, accl_z=0;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		acc_sensor_init_with_interrupt();
		kal_prompt_trace(MOD_ENG,"AT+HACCID");
		sprintf(sendbuffer, "3D : ");
		acc_sensor_get_xyz(&accl_x, &accl_y, &accl_z);
		sprintf(sendbuffer+5, "%d,%d,%d", accl_x, accl_y, accl_z);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HGPSON",strlen("AT+HGPSON"))==0) // AT+HGPSON
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HGPSON");
		StartTimer(LOCK_ATA_AT_TIMER1,20,iot_ata_gps_open);	//must StartTimer
		sprintf(sendbuffer, "OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HGPSOFF",strlen("AT+HGPSOFF"))==0) // AT+HGPSOFF
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HGPSOFF");
		StartTimer(LOCK_ATA_AT_TIMER2,20,iot_ata_gps_sleep); //must StartTimer
		//iot_ata_reset_gps_count();
		sprintf(sendbuffer, "OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HGPSINFO",strlen("AT+HGPSINFO"))==0) // AT+HGPSINFO
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HGPSINFO");
		sprintf(sendbuffer, "GPSINFO : %d,%d", get_gps_strength_count(),get_gps_strength_max());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HIMSI",strlen("AT+HIMSI"))==0) // AT+HIMSI
	{
		kal_int8 imsi[SRV_MAX_IMSI_LEN + 1];
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HIMSI");
		sprintf(sendbuffer, "IMSI : ");
		srv_sim_ctrl_get_imsi(MMI_SIM1, imsi, SRV_MAX_IMSI_LEN + 1);
		sprintf(sendbuffer+7, "%s", imsi+1);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HFLASH",strlen("AT+HFLASH"))==0) // AT+HFLASH
	{
		kal_uint8 temp_val = 0;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HFLASH");
		iot_set_bkup_nv_second_byte(0xBB);
		temp_val = iot_get_bkup_nv_second_byte();
		kal_prompt_trace(MOD_MMI, "%s-AT+HFLASH-%x", __FUNCTION__, temp_val);
		if(temp_val == 0xBB)
			sprintf(sendbuffer, "OK");
		else
			sprintf(sendbuffer, "ERROR");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HVER",strlen("AT+HVER"))==0) // AT+HVER
	{
		kal_char* verno = NULL;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		verno = release_verno();
		kal_prompt_trace(MOD_ENG,"AT+HVER");
		sprintf(sendbuffer, "VER : %s", verno);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HLEDON",strlen("AT+HLEDON"))==0) // AT+HLEDON
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		iot_led_on_off_ctrl(1,1);
		iot_led_on_off_ctrl(2,1);
		kal_prompt_trace(MOD_ENG,"AT+HLEDON");
	}

	else if(strncmp(revbuf,"AT+HLEDOFF",strlen("AT+HLEDOFF"))==0) // AT+HLEDOFF
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		iot_led_on_off_ctrl(1,0);
		iot_led_on_off_ctrl(2,0);
		kal_prompt_trace(MOD_ENG,"AT+HLEDOFF");
	}

	else if(strncmp(revbuf,"AT+HMOTOR",strlen("AT+HMOTOR"))==0) // AT+HMOTOR
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HMOTOR");
		StopTimer(LOCK_MOTOR_TIMER1);
		iot_motor_stop();	//stop
		iot_motor_start(1);
		StartTimer(LOCK_MOTOR_TIMER1,2*1000,iot_motor_stop);
	}

	else if(strncmp(revbuf,"AT+HMOTOL",strlen("AT+HMOTOL"))==0) // AT+HMOTOL
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HMOTOL");
		StopTimer(LOCK_MOTOR_TIMER1);
		iot_motor_stop();	//stop
		iot_motor_start(0);
		StartTimer(LOCK_MOTOR_TIMER1,2*1000,iot_motor_stop);
	}

	else if(strncmp(revbuf,"AT+HOPENSW",strlen("AT+HOPENSW"))==0) // AT+HOPENSW
	{
		kal_uint8 gpio_state = 2;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HOPENSW");
		gpio_state = iot_get_switch_state(1);
		if(gpio_state == 1)
			sprintf(sendbuffer, "H");
		else if(gpio_state == 0)
			sprintf(sendbuffer, "L");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HCLOSESW",strlen("AT+HCLOSESW"))==0) // AT+HCLOSESW
	{
		kal_uint8 gpio_state = 2;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HCLOSESW");
		gpio_state = iot_get_switch_state(2);
		if(gpio_state == 1)
			sprintf(sendbuffer, "H");
		else if(gpio_state == 0)
			sprintf(sendbuffer, "L");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HBTMAC",strlen("AT+HBTMAC"))==0) // AT+HBTMAC
	{
		kal_int32 temp=0;
		BD_ADDR bd_addr = {0};
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		memset(sendbuffer,0,sizeof(sendbuffer));
		bt_get_bd_addr(&bd_addr,&g_srv_bt_cm_cntx.host_dev.le_bd_addr);
		sprintf(sendbuffer,"BTMAC : ");
		sendbuffer[8] = bd_addr.addr[0];
		sendbuffer[9] = bd_addr.addr[1];
		sendbuffer[10] = bd_addr.addr[2];
		sendbuffer[11] = bd_addr.addr[3];
		sendbuffer[12] = bd_addr.addr[4];
		sendbuffer[13] = bd_addr.addr[5];
		sendbuffer[14] = 0x00;
		kal_prompt_trace(MOD_ENG,"AT+HBTMAC");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HCHARGE",strlen("AT+HCHARGE"))==0) // AT+HCHARGE
	{
		kal_uint32 time1 = 0x00;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		time1= drv_get_current_time();
		kal_prompt_trace(MOD_ENG,"AT+HCHARGE");
		iot_bat_charger_en(1);
		while(drv_get_duration_ms(time1) <= 200);	//200ms
		sprintf(sendbuffer, "CHARGE : %dmA", iot_get_battery_isense());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HVBUS",strlen("AT+HVBUS"))==0) // AT+HVBUS
	{
		kal_uint32 time1 = 0x00;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		time1= drv_get_current_time();
		iot_bat_charger_en(0);
		kal_prompt_trace(MOD_ENG,"AT+HVBUS");
		while(drv_get_duration_ms(time1) <= 200);	//delay 200ms
		sprintf(sendbuffer, "VBUS : %dmV",iot_get_battery_vcharger());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		//iot_bat_charger_en(1);
	}

	else if(strncmp(revbuf,"AT+HVBAT",strlen("AT+HVBAT"))==0) // AT+HVBAT
	{
		kal_uint32 time1 = 0x00;
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		time1 = drv_get_current_time();
		iot_bat_charger_en(0);
		kal_prompt_trace(MOD_ENG,"AT+HVBAT");
		while(drv_get_duration_ms(time1) <= 200);	//200ms
		sprintf(sendbuffer, "VBAT : %dmV",iot_get_battery_voltage());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		//iot_bat_charger_en(1);
	}

	else if(strncmp(revbuf,"AT+HBUZ",strlen("AT+HBUZ"))==0) // AT+HBUZ
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HBUZ");
		iot_ata_play_speaker_start();
		StartTimer(LOCK_BEEP_TIMER1,5*1000,iot_ata_play_speaker_stop);
	}

	else if(strncmp(revbuf,"AT+HVTEMP",strlen("AT+HVTEMP"))==0) // AT+HVBAT_TEMP
	{
		is_ata_port_flag = KAL_TRUE;
		if(HL300_ATA_test_OK_Flag)
			return;
		kal_prompt_trace(MOD_ENG,"AT+HVTEMP");
		sprintf(sendbuffer, "VBAT_TEMP : %dmV",iot_get_battery_temp_volt());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HTESTFINISH",strlen("AT+HTESTFINISH"))==0)  //AT+HEND
	{
		kal_prompt_trace(MOD_ENG,"AT+HTESTFINISH");
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "OK");
		HL300_ATA_test_OK_Flag = KAL_TRUE;
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HTESTSTART",strlen("AT+HTESTSTART"))==0)  //AT+HEND
	{
		kal_prompt_trace(MOD_ENG,"AT+HTESTSTART");
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "OK");
		HL300_ATA_test_OK_Flag = KAL_FALSE;
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

#endif

	else if(strncmp(revbuf,"AT+OPENLOCK",strlen("AT+OPENLOCK"))==0)//开锁
	{
		kal_prompt_trace(MOD_ENG,"open_lock_ata");
		open_lock_ata();
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "OPENLOCK=OPEN");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		kal_prompt_trace(MOD_ENG,"OPENLOCK");
	}

	else if(strncmp(revbuf,"AT+RESETLOCK",strlen("AT+RESETLOCK"))==0)//复位
	{
		kal_prompt_trace(MOD_ENG,"reset_lock");
		reset_lock();
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "RESETLOCK=RESET");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+LOCKSTATE",strlen("AT+LOCKSTATE"))==0)//查询锁状态
	{
		LOCK_STATE state = read_lock_state();
		kal_prompt_trace(MOD_ENG,"state");
		is_ata_port_flag = KAL_TRUE;

		if(state == UNLOCKED)
		{
			//tempbuf[3]=0x00;	  //open
			sprintf(sendbuffer, "LOCKSTATE=OPEN");
		}
		else if(state == LOCKED)
		{
			//tempbuf[3]=0x01;	 //close
			sprintf(sendbuffer, "LOCKSTATE=CLOSE");
		}
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+MODE",strlen("AT+MODE"))==0)//查询锁的工作模式
	{
		kal_uint32 mode = get_lock_curr_mode(); 
		kal_prompt_trace(MOD_ENG,"QUERY LOCK MODE");
		is_ata_port_flag = KAL_TRUE;

		if(mode == IOT_LOCK_MODE_NORMAL)
		{
			//tempbuf[3]=0x00; //正常模式
			sprintf(sendbuffer, "MODE=NORMAL");
		}
		else if(mode == IOT_LOCK_MODE_LOW_ENERGY)
		{
			sprintf(sendbuffer, "MODE=STOP_USE"); //低电量模式
	    }
		else
		{
			//tempbuf[3]=0x01; //运输模式		
			sprintf(sendbuffer, "MODE=TRANSPORT");
		}
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}
	
	else if(strncmp(revbuf,"AT+NORMALMODE",strlen("AT+NORMALMODE"))==0)//设置正常模式
	{
		kal_uint32 mode_old = get_lock_curr_mode();
		kal_prompt_trace(MOD_ENG,"SET LOCK NORMALMODE");
		if(mode_old != IOT_LOCK_MODE_NORMAL)
		{
		switch_lock_mode(IOT_LOCK_MODE_NORMAL);
		}
		else
		{
			open_beep_ms(200, 1);
		}
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "NORMALMODE=NORMAL");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+TRANSMODE",strlen("AT+TRANSMODE"))==0)//设置运输模式
	{
		//kal_uint8 mode = *(revbuf+3);
		kal_uint32 mode_old = get_lock_curr_mode();
		kal_prompt_trace(MOD_ENG,"SET LOCK TRANSMODE");
		if(mode_old != IOT_LOCK_MODE_TRANS)
		{
			switch_lock_mode(IOT_LOCK_MODE_TRANS);
		}
		else
		{
			open_beep_ms(200, 1);
		}
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "TRANSMODE=TRANS");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+GSMSTATE",strlen("AT+GSMSTATE"))==0)//GSM状态
	{
		kal_uint8 state = get_gsm_state(); 
		kal_prompt_trace(MOD_ENG,"QUERY GSMSTATE");
		is_ata_port_flag = KAL_TRUE;
		switch(state)
		{
			case GSM_CLOSED:
				sprintf(sendbuffer, "GSMSTATE=GSM_CLOSED");
				break;
			case GSM_CHECK_SIM:
				sprintf(sendbuffer, "GSMSTATE=GSM_CHECK_SIM");
				break;
			case GSM_REG:
				sprintf(sendbuffer, "GSMSTATE=GSM_REG");
				break;
			case GSM_INIT_SMS:
				sprintf(sendbuffer, "GSMSTATE=GSM_INIT_SMS");
				break;
			case GSM_CHECK_GPRS:
				sprintf(sendbuffer, "GSMSTATE=GSM_CHECK_GPRS");
				break;
			case GSM_CONN_PPP:
				sprintf(sendbuffer, "GSMSTATE=GSM_CONN_PPP");
				break;
			case GSM_CONN_TCP:
				sprintf(sendbuffer, "GSMSTATE=GSM_CONN_TCP");
				break;
			case GSM_SUCCESS:
				sprintf(sendbuffer, "GSMSTATE=GSM_SUCCESS");
				break;
			case GSM_OPENED:
				sprintf(sendbuffer, "GSMSTATE=GSM_OPENED");
				break;
			default: // GSM_ERROR
				sprintf(sendbuffer, "GSMSTATE=GSM_ERROR");
				break;
		} 
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+GPRSTIME",strlen("AT+GPRSTIME"))==0)//上一次GPRS上线时间
	{
		kal_uint8 time = get_gprs_duration_time();
		kal_prompt_trace(MOD_ENG,"QUERY GPRSTIME");
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "GPRSTIME=%d", time);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}	

	else if(strncmp(revbuf,"AT+STRENGTH",strlen("AT+STRENGTH"))==0)//上一次GSM信号强度
	{
		kal_int16 gsm_strengthbuf=0;
		kal_int16 gsm_strength = get_gsm_strength();
		gsm_strengthbuf = gsm_strength > 0 ? gsm_strength : -gsm_strength;
		is_ata_port_flag = KAL_TRUE;
		kal_prompt_trace(MOD_ENG,"QUERY STRENGTH");
		get_gps_state();
		sprintf(sendbuffer, "STRENGTH=%d", gsm_strengthbuf);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+GPSSTATE",strlen("AT+GPSSTATE"))==0)//GPS状态
	{
		kal_prompt_trace(MOD_ENG,"QUERY GPSSTATE");	
		kal_prompt_trace(MOD_ENG,"GPSSTATE=%d",get_gps_state());	
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "GPSSTATE=%d", get_gps_state());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+GPSTIME",strlen("AT+GPSTIME"))==0)//上一次GPS定位时间
	{
		kal_prompt_trace(MOD_ENG,"QUERY GPSTIME");	
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "GPSTIME=%d", get_gps_duration_time());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+GPSCOUNT",strlen("AT+GPSCOUNT"))==0)//上一次GPS收星数量
	{
		kal_prompt_trace(MOD_ENG,"QUERY GPSCOUNT");	
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "GPSCOUNT=%d", get_gps_count());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+GPSSTRENGTH",strlen("AT+GPSSTRENGTH"))==0)//GPS信号强度
	{
		kal_prompt_trace(MOD_ENG,"QUERY GPSSTRENGTH");
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "GPSSTRENGTH:COUNT=%d", get_gps_strength_count());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+IMEI",strlen("AT+IMEI"))==0)//获取GSM ID
	{
		kal_uint8 i = 0;
		kal_char imei[SRV_IMEI_MAX_LEN + 1];
		is_ata_port_flag = KAL_TRUE;
		memset(imei,0,sizeof(imei));
		//get IMEI
		if(!srv_imei_get_imei(MMI_SIM1, imei, SRV_IMEI_MAX_LEN+1)) {
			//get IMEI failed
			kal_prompt_trace(MOD_MMI, "BKss: %s GET IMEI ERROR", __FUNCTION__);
		}
		sprintf(sendbuffer, "IMEI=%s", imei);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		kal_prompt_trace(MOD_ENG,"GSM IMEI");
	}

	else if(strncmp(revbuf,"AT+ICCID",strlen("AT+ICCID"))==0)//获取iccid
	{
		is_ata_port_flag = KAL_TRUE;
		get_iccid_req_ext();
		kal_prompt_trace(MOD_ENG,"ICCID");
	}

	else if(strncmp(revbuf,"AT+GSMVER",strlen("AT+GSMVER"))==0)//获取GSM 版本号
	{
		kal_char* verno = release_verno();
		kal_char* date_time = build_date_time();
		kal_char verno_date_time[20]={0};
		is_ata_port_flag = KAL_TRUE;
		//memcpy(verno_date_time,verno,5);
		strncpy (verno_date_time, verno, 5);
		strncat(verno_date_time,date_time+2,6);
		sprintf(sendbuffer, "GSMVER=%s", verno_date_time);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+HMVER",strlen("AT+HMVER"))==0)//获取固件版本号
	{
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "HMVER=1.1");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		kal_prompt_trace(MOD_ENG,"sendbuffer=%s",sendbuffer);
	}

	else if(strncmp(revbuf,"AT+DOMAIN",strlen("AT+DOMAIN"))==0)//查询域名
	{
	 	kal_prompt_trace(MOD_ENG,"GET DOMAIN");
		is_ata_port_flag = KAL_TRUE;
		kal_prompt_trace(MOD_ENG,"DOMAIN=%s",g_lock_cntx.domain.domain_buffer);
		sprintf(sendbuffer, "DOMAIN=%s",g_lock_cntx.domain.domain_buffer);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+IP",strlen("AT+IP"))==0)//查询IP
	{
		kal_prompt_trace(MOD_ENG,"GET IP");
		//char buffer[128+1];
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "IP=%d.%d.%d.%d", g_lock_cntx.server_ip.addr[0],g_lock_cntx.server_ip.addr[1], g_lock_cntx.server_ip.addr[2],g_lock_cntx.server_ip.addr[3]);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

   else if(strncmp(revbuf,"AT+WRITEIP",strlen("AT+WRITEIP"))==0)//修改IP 
	{
	    unsigned char ip1,ip2,ip3,ip4;
		is_ata_port_flag = KAL_TRUE;
		sscanf(revbuf+strlen("AT+WRITEIP"), "%d.%d.%d.%d", &ip1,&ip2,&ip3,&ip4); 
		g_lock_cntx.server_ip.addr[0]=ip1;
		g_lock_cntx.server_ip.addr[1]=ip2;
		g_lock_cntx.server_ip.addr[2]=ip3;
		g_lock_cntx.server_ip.addr[3]=ip4;
		kal_prompt_trace(MOD_ENG,"WRITEIP revbuf=%s",revbuf+strlen("AT+WRITEIP"));
		kal_prompt_trace(MOD_ENG,"WRITEIP g_server_ip.addr=%d,%d,%d,%d",g_lock_cntx.server_ip.addr[0],g_lock_cntx.server_ip.addr[1], g_lock_cntx.server_ip.addr[2],g_lock_cntx.server_ip.addr[3]);
		sprintf(sendbuffer, "WRITEIP=OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		kal_prompt_trace(MOD_ENG,"sendbuffer=%s",sendbuffer);
	}

   	else if(strncmp(revbuf,"AT+WRITEDOMAIN",strlen("AT+WRITEDOMAIN"))==0)//修改域名
	{
		memset(domian_buffer, 0, sizeof(domian_buffer));	
	 	kal_prompt_trace(MOD_ENG,"GET WRITEDOMAIN");
		is_ata_port_flag = KAL_TRUE;
		sscanf(revbuf+strlen("AT+WRITEDOMAIN"), "%s", &domian_buffer);
		kal_prompt_trace(MOD_ENG,"WRITEDOMAIN=%s",domian_buffer);
		//WriteDomain(buffer);
		StartTimer(ATA_WRITEDOMAIN_TIMER, 5*1000, WriteDomain_ata);
		kal_prompt_trace(MOD_ENG,"WRITEDOMAIN=OK");
		sprintf(sendbuffer, "WRITEDOMAIN=OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+WRITEPORT",strlen("AT+WRITEPORT"))==0)//修改PORT
	{
	    StopTimer(ATA_WRITEDOMAIN_TIMER);
		port =0;
	 	kal_prompt_trace(MOD_ENG,"GET WRITEPORT");
		is_ata_port_flag = KAL_TRUE;
		sscanf(revbuf+strlen("AT+WRITEPORT"), "%d", &port);
		kal_prompt_trace(MOD_ENG,"WRITEPORT=%d",port);
		StartTimer(ATA_WRITEDOMAIN_TIMER, 5*1000, WritePort_ata);
		kal_prompt_trace(MOD_ENG,"WRITEPORT=OK");
		sprintf(sendbuffer, "WRITEPORT=OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+REBOOT",strlen("AT+REBOOT"))==0)//REBOOT
	{
		is_ata_port_flag = KAL_TRUE;
	 	kal_prompt_trace(MOD_ENG,"GET REBOOT");
		sprintf(sendbuffer, "REBOOT=OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		srv_reboot_normal_start();
	}

   	else if(strncmp(revbuf,"AT+AGINGTEST",strlen("AT+AGINGTEST"))==0)//老化测试
	{
	    is_ata_port_flag = KAL_TRUE;
	 	kal_prompt_trace(MOD_ENG,"GET AGINGTEST");
		sprintf(sendbuffer, "AGINGTEST=OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		aging_test_ata();
	}

	else if(strncmp(revbuf,"AT+AGINGSTATE",strlen("AT+AGINGSTATE"))==0)//老化测试
	{
	 	kal_prompt_trace(MOD_ENG,"GET AGINGSTATE");
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "AGINGSTATE=%d",agingbuf[0]);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
		kal_prompt_trace(MOD_ENG,"AGINGSTATE=%d",agingbuf[0]);
	}

	else if(strncmp(revbuf,"AT+LADC",strlen("AT+LADC"))==0)//查询LADC
	{
		kal_int32 ladc=0;
		is_ata_port_flag = KAL_TRUE;
		ladc=iot_get_battery_voltage();
		kal_prompt_trace(MOD_MMI, "ladc=%d",ladc);
		sprintf(sendbuffer, "LADC=%d",ladc);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+TADC",strlen("AT+TADC"))==0)//查询TADC
	{
		kal_int32 tadc=0;
		tadc=iot_get_battery_vcharger();
		is_ata_port_flag = KAL_TRUE;
		kal_prompt_trace(MOD_MMI, "tadc=%d",tadc);
		sprintf(sendbuffer, "TADC=%d",tadc);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+TEMP",strlen("AT+TEMP"))==0)//查询TEMP
	{
		kal_int32 temp=0;
		is_ata_port_flag = KAL_TRUE;			
		temp=iot_get_battery_temp_volt();
	    kal_prompt_trace(MOD_MMI, "temp=%d",temp);
		sprintf(sendbuffer, "TEMP=%d",temp);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+GSENSOR",strlen("AT+GSENSOR"))==0)//查询GSENSOR
	{
		kal_int32 temp=0;
		is_ata_port_flag = KAL_TRUE;
		Bma250_read_data_xyz();
	}

	else if(strncmp(revbuf,"AT+BTMAC",strlen("AT+BTMAC"))==0)//查询蓝牙MAC
	{
		kal_int32 temp=0;
		BD_ADDR bd_addr = {0};
		is_ata_port_flag = KAL_TRUE;
		bt_get_bd_addr(&bd_addr,&g_srv_bt_cm_cntx.host_dev.le_bd_addr);
	    kal_prompt_trace(MOD_MMI, "BTMAC==%02x,%02x,%02x,%02x,%02x,%02x",
				bd_addr.addr[5],bd_addr.addr[4],bd_addr.addr[3],bd_addr.addr[2],bd_addr.addr[1],bd_addr.addr[0]);
		sprintf(sendbuffer, "BTMAC=%02x,%02x,%02x,%02x,%02x,%02x",bd_addr.addr[5],bd_addr.addr[4],bd_addr.addr[3],bd_addr.addr[2],bd_addr.addr[1],bd_addr.addr[0]);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+REDLED",strlen("AT+REDLED"))==0)//设置红灯闪烁
	{
		is_ata_port_flag = KAL_TRUE;
		spronser_led_ctrl(1,500,5);
		sprintf(sendbuffer, "RED LED NORMAL");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+BLUELED",strlen("AT+BLUELED"))==0)//设置蓝灯闪烁
	{
		is_ata_port_flag = KAL_TRUE;
		spronser_led_ctrl(2,500,5);
		sprintf(sendbuffer, "BLUE LED NORMAL");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+CURRENT",strlen("AT+CURRENT"))==0)//读取电流
	{
		is_ata_port_flag = KAL_TRUE;
		sprintf(sendbuffer, "CURRENT=%d ",iot_get_battery_isense());
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+IMSI",strlen("AT+IMSI"))==0)//查询imsi
	{
		CHAR imsi[SRV_MAX_IMSI_LEN + 1];
		kal_prompt_trace(MOD_ENG,"GET IMSI");
		is_ata_port_flag = KAL_TRUE;
		srv_sim_ctrl_get_imsi(MMI_SIM1, imsi, SRV_MAX_IMSI_LEN + 1);
		sprintf(sendbuffer, "IMSI=%s", imsi+1);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+GPSON",strlen("AT+GPSON"))==0)//gps打开
	{
		is_ata_port_flag = KAL_TRUE;
		StartTimer(LOCK_ATA_AT_TIMER1,20,iot_ata_gps_open);	//must StartTimer
		sprintf(sendbuffer, "GPS ON NORMAL");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+GPSOFF",strlen("AT+GPSOFF"))==0)//gps关闭
	{
		is_ata_port_flag = KAL_TRUE;
		StartTimer(LOCK_ATA_AT_TIMER2,20,iot_ata_gps_sleep); //must StartTimer
		sprintf(sendbuffer, "GPS OFF NORMAL");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+SN",strlen("AT+SN"))==0)//查询sn
	{
		kal_int8 ret = 0;
		kal_uint8 sn_buf[64];
		kal_uint8 i =0;
		is_ata_port_flag = KAL_TRUE;
		for(i=0;i<64;i++)
		{
			sn_buf[i]=0x00;
		}
		ReadRecordIot(NVRAM_EF_BARCODE_NUM_LID, 1, sn_buf, 64);
		//for(i=0;i<64;i++)
		//{
		//	kal_prompt_trace(MOD_MMI, "SN[%d] = %d", i,sn_buf[i]);
		//}
		sprintf(sendbuffer, "SN=%s", sn_buf);
		rmmi_write_to_uart((kal_uint8*)sendbuffer, 16, KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+WRITEBTKEYT",strlen("AT+WRITEBTKEYT"))==0)//写蓝牙秘钥
	{
		nvram_ef_bel_bkss_struct bel = {0};  //存放密钥和密码
		kal_uint8 cout=0,i=0;
		kal_uint8 tmp[5] = "0x00";
		kal_uint8 len = strlen("AT+WRITEBTKEYT");
		ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, &bel, sizeof(nvram_ef_bel_bkss_struct));
		kal_prompt_trace(MOD_ENG,"WRITE BT KEYT");
		is_ata_port_flag = KAL_TRUE;
		for(i=0;i<16;i++)
		{
			tmp[0]='0';
			tmp[1]='x';
			tmp[2]=revbuf[len + 2*i];
			tmp[3]=revbuf[len + 2*i + 1];
			bel.key[i] = strtol(tmp,NULL,16);
		}
		WriteRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, &bel, sizeof(nvram_ef_bel_bkss_struct));
		sprintf(sendbuffer, "WRITE BTKEYT OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+READBTKEYT",strlen("AT+READBTKEYT"))==0)//读蓝牙秘钥
	{
		char temp_buf[32];
		nvram_ef_bel_bkss_struct bel = {0};  //存放密钥和密码
		is_ata_port_flag = KAL_TRUE;
		ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, &bel, sizeof(nvram_ef_bel_bkss_struct));
		memset(temp_buf,32,0);
		for(i=0;i<16;i++)
		{
		    sprintf(temp_buf+i*2, "%x", bel.key[i]/16);
		    sprintf(temp_buf+i*2+1, "%x", bel.key[i]%16);
		}	
		sprintf(sendbuffer, "READBTKEYT =%s", temp_buf);
		rmmi_write_to_uart((kal_uint8*)sendbuffer,44, KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+WRITEBTPASSWORD",strlen("AT+WRITEBTPASSWORD"))==0)//写蓝牙密码
	{
		nvram_ef_bel_bkss_struct bel = {0};  //存放密钥和密码
		kal_uint8 cout=0,i=0;
		kal_uint8 tmp[5] = "0x00";
		kal_uint8 len = strlen("AT+WRITEBTPASSWORD");
		ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, &bel, sizeof(nvram_ef_bel_bkss_struct));
		kal_prompt_trace(MOD_ENG,"WRITE BT PASSWORD");       
		is_ata_port_flag = KAL_TRUE;
		for(i=0;i<6;i++)
		{
			tmp[0]='0';
			tmp[1]='x';
			tmp[2]=revbuf[len + 2*i];
			tmp[3]=revbuf[len + 2*i + 1];
			bel.psd[i] = strtol(tmp,NULL,16);
		}
		WriteRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, &bel, sizeof(nvram_ef_bel_bkss_struct));
		sprintf(sendbuffer, "WRITE BT PASSWORD OK");
		rmmi_write_to_uart((kal_uint8*)sendbuffer, strlen(sendbuffer), KAL_TRUE);
	}

	else if(strncmp(revbuf,"AT+READBTPASSWORD",strlen("AT+READBTPASSWORD"))==0)//读蓝牙密码
	{
		char temp_buf[12];
		nvram_ef_bel_bkss_struct bel = {0};  //存放密钥和密码
		is_ata_port_flag = KAL_TRUE;
		ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, &bel, sizeof(nvram_ef_bel_bkss_struct));
		memset(temp_buf,12,0);
		for(i=0;i<6;i++)
		{
		    sprintf(temp_buf+i*2, "%x", bel.psd[i]/16);
		    sprintf(temp_buf+i*2+1, "%x", bel.psd[i]%16);
		}
		sprintf(sendbuffer, "READBTPASSWORD =%s", temp_buf);
		rmmi_write_to_uart((kal_uint8*)sendbuffer,28, KAL_TRUE);
	}

	else
	{
		kal_prompt_trace(MOD_ENG,"bResp = MMI_FALSE");
		bResp = MMI_FALSE;
	}

	if(bResp)
	{
		return 0;	
	}	
	else
	{
		return -1;	
	}	
}

