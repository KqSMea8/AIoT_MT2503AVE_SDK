#ifndef __LOCK_NET_H__
#define __LOCK_NET_H__

#include "lock_utils.h"
#include "lock_data.h"

#include "app_md5.h"

#define MAX_BUFFER_LEN 384
#define MAX_GPS_LOCATION_COUNTS 100

#define TERMINAL_COM_ANS_ID  		0x0001
#define PLATFORM_COM_ANS_ID  		0x8001
#define TERMINAL_HEART_ID  			0x0002
#define TERMINAL_REGISTER_ID  		0x0100
#define TERMINAL_REGISTER_ANS_ID  	0x8100
#define TERMINAL_AUTN_ID  			0x0120
#define SET_TERMINAL_PARAM_ID  	0x8103
#define QUERY_TERMINAL_PARAM_ID  	0x8104
#define QUERY_TERMINAL_PARAM_ANS_ID 0x0104
#define QUERY_TERMINAL_ATTR_ID  	0x8170
#define QUERY_TERMINAL_ATTR_ANS_ID  0x0170
#define TERMINAL_FOTA_ID  			0x8108
#define TERMINAL_LOC_COM_ID 		0x0200
#define TERMINAL_CTRL_ID 			0x8500
#define PLATFORM_CTRL_ANS_ID 		0x0500 
#define QUERY_ONCE_GPS_REQ              0x8109
#define QUERY_ONCE_GPS_RSP              0x0109
#define PLATFORM_SET_DEVICE_STATE             	0x8110
#define QUERY_TEMP_ELEC_REQ            0x8111
#define QUERY_TEMP_ELEC_RSP            0x0111
#define DOWN_VOICE_PACKAGE			0x8112
#define TERMINAL_DOWNLOAD_ID		0x0112
#define QUERY_TERMINAL_IS_WORKING_ID            0x8113

#define ECG_MSG_UPLOAD                    0x0114

#define SET_ECG_CTR_PARAM_ID                    0x8115
#define QUERY_SET_ECG_CTR_PARAM_ID                    0x8116
#define QUERY_SET_ECG_CTR_PARAM_ANS_ID                    0x0116


#define TIMER_LOC_REPORT_MASK   0x01
#define UNLOCK_LOC_REPORT_MASK	0x02
#define LOCK_LOC_REPORT_MASK	0x04
#define LOW_BATT_REPORT_MASK	0x08
#define MOVE_LOC_REPORT_MASK	0x10
#define VIB_LOC_REPORT_MASK		0x20
#define BEEP_LOC_REPORT_MASK   0x40
//second
#define SOCKET_RECONECT_DELAY   5

#define SECRET_NONE 0
#define SECRET_AES  1

#define BASE_STATION_MAX_COUNT  3
#define SOCKET_CONN_MAX_COUNT	8

#define START_END_FLAG 0x7E

#define STATE_WAIT_SEND        0
#define STATE_SENDING             1

#define FLAG_SUCCESS 0x00000001
#define FLAG_FINISH  0x00000002
#define FLAG_EXCUTE_ERR_HANDLER 0x00000004
#define FLAG_START_DOWNLOAD 0x00000008
#define FLAG_FIND_ETAG 0x00000010
#define FLAG_ACCESS_NVRAM_ERR 0x00000020
#define FLAG_FILE_OCCURED_ERR 0x00000040

#define DOWNLOAD_SUCCESS                 (0)
#define DOWNLOAD_ERROR                   (-1)


#define MMI_DATA_DEFAULT_FOLDER_DIR L"D:\\temp"
#define MMI_DATA_DEFAULT_FILE_DIR L"D:\\temp.mp3"
#define OPEN_TONE_FILE_DIR L"D:\\open.mp3"
#define CLOSE_TONE_FILE_DIR L"D:\\close.mp3"
#define WARN_TONE_FILE_DIR L"D:\\warn.mp3"

#define SMS_MAX_CONTENT_LEN            140
#define MAX_DEVICE_ID_LEN            12
#define SMS_MSG_HEADER_LEN            16

#define SMS_OPEN_LOCK_TYPE  				0x01
#define SMS_SET_DEVICE_PARAM_TYPE 		0x02
#define SMS_SET_DEVICE_STATE_TYPE 		0x03
#define SMS_SET_CTR_CMD_TYPE 			0x04
#define SMS_UPLOAD_FOTA_PACK_TYPE 			0x05

extern const kal_char *lbs_host;
extern const kal_char *lbs_path;

void LockTaskInit();

//for socket
void lock_socket_create(SOCKET_TYPE soc_type);
kal_bool lock_socket_notify (void* msg_ptr);
kal_bool get_host_by_name(void* inMsg);
kal_bool lock_socket_receive(kal_uint8 socket_id, Byte *buffer, kal_uint16 len, kal_uint16 *recvlen);
void lock_socket_close(SOCKET_TYPE soc_type);
void lock_socket_send(kal_int8 socket_id, Byte *buffer, kal_uint16 len, kal_bool resend, Word number);
void lock_submit_error(SOCKET_TYPE soc_type, gprs_lock_error err_code);
void save_new_server();
kal_bool domain_update();
void handle_command(const Byte *buff);
void heart_beat();
void lock_bike_callback();
void send_download_request(kal_int8 socket_id);
#ifdef __ADUPS_FOTA_SOC__
extern void adups_net_start_get_new_version(void);
void save_the_current_version();
void update_fota_upgrate_status();
#endif

static void my_download_socket_receive(void);
static voice_download_bool_type voicedownload_check_download_finish(void);
static S32 voicedownload_check (void);
static voice_download_bool_type voice_dn_deal_with_receiving_packet(U8 *buffer,U32 length);
static S32 voicedownload_open (void);
static U32  voice_download_epofile_property(char *buf,int length);
static S32 voicedownload_write (const void* buffer, U32 offset, U32 length,U32* p_nWritten);
static void VOICEDOWNLOAD_SET_FLAG(U32 flag);
static voice_download_bool_type VOICEDOWNLOAD_GET_FLAG(U32 flag);
void send_download_request(kal_int8 socket_id);
void check_play_tone_with_filepath_and_set_new(void);
void set_music_type(Byte type);
download_music_type get_music_type(void);
void check_play_tone_with_filepath_and_set_new(void);
void check_file_exist_and_delete_temp_file(void);
void my_lock_socket_create(void);

static S32 my_voice_download_check (void);
static int hash_string(unsigned char *hash,int length);
static kal_bool my_applib_md5_file_update(applib_md5_ctx *mdContext, const kal_wchar *filename);

void my_socket_reconnect(SOCKET_TYPE soc_type);
void lock_init_account(void);

void nwow_gsm_state_kpled_init(void);

void ecu_login_test(void);


#endif
