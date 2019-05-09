#ifndef __LOCK_DATA_H__
#define __LOCK_DATA_H__

#include "soc_api.h"
#include "common_nvram_editor_data_item.h"

//second
#define VIBRATE_REPORT_GAP		600
#define OPEN_G_SENSER_DELAY	5
#define GPS_LOCATION_TIME		90
#define LOCK_LOC_REPORT_DELAY  30

#define LOCK_ORDER_NUM_MAX_LEN 11
#define BASE_STATION_MAX 4

#define LOCK_CTR_BODY_MAX_LEN 16
#define LOCK_CTR_BODY_MIN_LEN 6

typedef kal_uint8  Byte;
typedef kal_uint16 Word;
typedef kal_uint32 DWord;
typedef kal_uint8 BCD;

#define SPCONSER_DEBUG
#ifdef SPCONSER_DEBUG
#define lock_debug_print	kal_prompt_trace
#else
#define lock_debug_print(...)
#endif

#define __LOCK_ADD_AES_ENCRYPT__

typedef enum{
	GSM_CLOSED, //GSM 下电
	GSM_CHECK_SIM, //没有检查到SIM 卡
	GSM_REG, //注册网络
	GSM_INIT_SMS,
	GSM_CHECK_GPRS, //注册网络失败
	GSM_CONN_PPP, //解析域名
	GSM_CONN_TCP, //连接TCP
	GSM_SUCCESS, //与平台通讯成功
	GSM_OPENED = 0xFE, //已上电
	GSM_ERROR = 0xFF, //GSM 故障
}GSM_STATE;

typedef enum{
	IOT_LOCK_MODE_NONE,
	IOT_LOCK_MODE_NORMAL = IOT_LOCK_MODE_NONE,
	IOT_LOCK_MODE_TRANS,
	IOT_LOCK_MODE_LOW_ENERGY,
	IOT_LOCK_MODE_SWITCH_RF,
	IOT_LOCK_MODE_MAX,
}IOT_LOCK_MODE;

typedef enum{
	UNLOCKED,
	LOCKED,
}LOCK_STATE;

typedef enum{
	TIMER_LOC_REPORT, //0000
	UNLOCK_LOC_REPORT, //0001
	LOCK_LOC_REPORT, //0010
	LOW_BATT_REPORT, //0011
	MOVE_LOC_REPORT, //0100
	VIB_LOC_REPORT, //0101
	GPRS_UNLOCK_DELAY_LOC_REPORT, //0110
	NT_UNLOCK_TIMER_LOC_REPORT, //0111
	BT_UNLOCK_LOC_REPORT, //1000
	BT_UNLOCK_TIMER_LOC_REPORT, //1001
	BT_LOCK_LOC_REPORT, //1010
	SMS_UNLOCK_LOC_REPORT, //1011
	SMS_UNLOCK_DELAY_LOC_REPORT, //1100
	SMS_UNLOCK_TIMER_LOCK_REPORT, //1101
	SMS_LOCK_LOC_REPORT, //1110
	LOCK_AMEND_LOC_REPORT, //1111
}LOC_REPORT_TYPE;

typedef enum{
	LOW_TEMP_LOC_REPORT,
	HIGH_TEMP_LOC_REPORT,
	LOW_POWER_LOC_REPORT,
	NORMAL_POWER_LOC_REPORT,
}LOC_WARN_REPORT_TYPE;



typedef enum{
	NT_UNLOCK,
	BT_UNLOCK,
	SMS_UNLOCK,
}UNLOCK_TYPE;


typedef enum{
	GPS_OFF,
	GPS_SLEEP,
	GPS_ON,
}GPS_STATE;

typedef enum {
	GET_LOCK_HOSTNAME = 1000,
	GET_NEW_LOCK_HOSTNAME,
	GET_LBS_HOSTNAME,
	GET_DOWNLOAD_VOICEPACKAGE,
}GET_HOSTNAME_TYPE;

typedef enum{
	ERR_REGISTER_APPID,
	ERR_SOCKET_CREATE,
	ERR_SOCKET_SET_NBIO,
	ERR_SOCKET_SET_SYNC,
	ERR_SOCKET_SET_NODELAY,
	ERR_SOCKET_CONNECT,
	ERR_SOCKET_RESET,
	ERR_DNS,
}gprs_lock_error;

typedef enum{
	TYPE_LOCK_SOC,
	TYPE_NEW_LOCK_SOC,
	TYPE_LBS_SOC,
	TYPE_DOWNLOAD_SOC,
}SOCKET_TYPE;


typedef enum{
	voicedownload_type_success=0,
	voicedownload_type_fail=1,
}voice_download_bool_type;


typedef enum{
	LOCK_MUSIC_TYPE,
	UNLOCK_MUSIC_TYPE,
	WARN_MUSIC_TYPE,
}download_music_type;

typedef struct __domain{
	kal_char domain_buffer[IOT_LOCK_DOMAIN_MAX_LEN];
	kal_uint8 len;
	kal_uint16 port;
}DOMAIN_T;

typedef enum{
	ONLY_UPDATE_ONCE_MODE = 1,
	EVERY_GAP_UPDATE_MODE,
}ecg_curr_update_mode;


typedef struct {
	DOMAIN_T domain;
	DOMAIN_T new_domain;
	DOMAIN_T lbs_domain;
	DOMAIN_T download_domian;
	char download_url[IOT_LOCK_DOMAIN_MAX_LEN];
	sockaddr_struct server_ip;
	sockaddr_struct new_server_ip;
	sockaddr_struct lbs_server_ip;
	sockaddr_struct download_server_ip;
	kal_uint32 account_id;
	kal_uint32 gps_update_lock_gap;
	kal_uint32 gps_update_run_gap;
	kal_uint32 heart_beat_time_gap;
	kal_int32 unlock_valid_time_gap;
	kal_uint32 gprs_time;
	kal_uint32 gprs_duration_time;
	kal_int8 socket_id;
	kal_int8 new_socket_id;
	kal_int8 lbs_socket;
	kal_int8 download_id;
	kal_int8 download_recv_buf[1024];
	kal_uint32 download_recv_buf_len;
	int download_sum;
	int DOWN_EPOFILE_SIZE;
	kal_uint32 download_flag;
	char check_sum[32+1];
	download_music_type music_type;
	Word	 download_serial;
	kal_bool is_downloading;
	kal_bool is_receiveing;
	kal_uint32 download_len;
	kal_uint8 app_id;
	kal_uint8  curr_mode;
	kal_uint8 gsm_state;
	kal_bool auth_result;
	Word reg_serial_no;
	kal_uint32 low_power;
	kal_uint32 unlock_time;
	kal_uint32 unlock_duration_time;
	kal_uint8  ecg_is_update;
	kal_uint32 ecg_samp_time_gap;
	kal_uint32 ecg_update_data_time_gap;
	kal_uint8  ecg_curr_update_mode;
	kal_uint8  current_unlock_mode;
	kal_bool gprs_is_need_reconnect;
	kal_bool gprs_is_connecting;
	kal_bool is_sim_ok;
	kal_uint16 speaker_play_counts;
	kal_uint16 led_flash_counts;
	kal_uint16 speaker_led_flash_counts;
	kal_bool is_has_order_num;
	kal_bool is_has_same_order_num;
	kal_uint8  uart1_state;
}gprs_lock_cntx;

#pragma pack(1)
typedef struct __message{
	Byte *header;
	Byte *body;
	Byte headerlen;
	Word bodylen;
}MESSAGE;

typedef struct __msg_header{
	Word id;
	Word attr;
	BCD dev_id[6];
	Word serial_number;
}MSG_HEADER;

typedef struct __comm_req{
	Word serial;
	Word id;
	Byte result;
}COMMON_REQ;

typedef struct __comm_loc{
	DWord alarm;
	DWord state;
	DWord latitude;
	DWord longitude;
	Word hight;
	Word speed;
	Word direction;
	BCD datetime[6];
}COMM_LOC;

typedef struct __base_station{
	Word lac;
	DWord ci;
	Byte strenth;
}BASE_STATION;

typedef struct __unlock_count{
	Byte id;
	Byte len;
	DWord count;
}UNLOCK_COUNT;

typedef struct __volt{
	Byte id;
	Byte len;
	Word elec;
	Word volt;
	Word charge_volt;
}VOLT;

typedef struct __order_num{
	Byte id;
	Byte len;
	Byte order_num[LOCK_ORDER_NUM_MAX_LEN];
}ORDER_NUM;

typedef struct __date_time{
	Byte id;
	Byte len;
	kal_uint32 open_lock_time;
}DATE_TIME;


typedef struct __batt_temp{
	Byte id;
	Byte len;
	kal_int16 vbat_temp;
}BATT_TEMP;

typedef struct __attach{
	Byte network_strenth[3];
	Byte gnss[3];
	UNLOCK_COUNT unlock_count;
	Byte lock_state[4];
	VOLT volt;
}ATTACH;

typedef struct __attach_loc {
	Byte stations;
	ATTACH attach;
	Word mcc;
	Word mnc;
	BASE_STATION base_station[BASE_STATION_MAX];
}ATTACH_LOC;

typedef struct __gps_data{
	Byte gps_counts;
	DWord gps_state;
	DWord latitude;
	DWord longitude;
	Word hight;
	Word direction;
	Word speed;
	BCD datetime[6];
}GPS_DATA;



typedef struct __gps_info{
	DWord alarm;
	DWord state;
	DWord latitude;
	DWord longitude;
	Word hight;
	Word speed;
	Word direction;
	Byte stations;
	Byte gnss;
	Word mcc;
	Word mnc;
	BASE_STATION base_station[BASE_STATION_MAX];
}GPS_INFO;

typedef struct __led_ctrl{
	kal_uint8 led_no;
	kal_uint8 count;
	kal_uint8 state;
	kal_uint16 time;
}LED_CTRL;

typedef struct __waring_para{
	kal_uint8 fd;
	Word msg_id;
	Word ans_number;
	kal_bool resend;
}WARING_PARA;

//one resend msg size: 128 Byte
typedef struct __msg_data{
	kal_uint8 sendCount;
	kal_uint8 length;
	Word number;
	Byte body[256];
}MSGDATA;

typedef struct __msg_node{
	kal_uint8 used:1; //0 unused, 1 used
	kal_uint8 state:1; //0 wait, 1 sending
	kal_uint8 :6; //no used
	MSGDATA msgdata;
	struct __msg_node* pre; //next node
	struct __msg_node* nxt; //next node
}MSG_NODE;

typedef struct __msg_list{
	MSG_NODE *header;
	MSG_NODE *tail;
	kal_uint8 count;
}MSG_LIST;

#define SEND_TIMEOUT     30000

IOT_LOCK_MODE get_curr_mode();
void set_curr_mode(IOT_LOCK_MODE mode);

#endif
