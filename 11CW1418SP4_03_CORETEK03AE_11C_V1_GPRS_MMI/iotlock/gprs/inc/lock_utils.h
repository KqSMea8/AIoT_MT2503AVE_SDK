#ifndef __LOCK_UTILS_H__
#define __LOCK_UTILS_H__

#include "lock_data.h"

#define MAX_RESEND_ITEMS 60
#define REG_AUTH_TIMEOUT          60*1000
#define AUTH_CODE_MAX_LEN 16
typedef void (*switch_lock_mode_callback) (kal_bool result);

extern void srv_reboot_normal_start();
void dump_buffer(Byte *out, int len);
MSG_NODE *get_one_unused_node();
void debug_print_list();
void add_one_msg_to_list(Byte *msg_ptr, Byte msg_len, Word number, kal_uint8 state);
void delete_one_msg_node(MSG_NODE* node);
void remove_one_msg_from_list(kal_bool via_number, Word number);
MSG_NODE *get_one_msg_from_list(kal_bool with_number, Word number);
void update_all_msg_to_nvram();
void update_all_msg_from_nvram();
void convert_header_from_big_endian(MSG_HEADER *msg_header);
void convert_comm_req_from_big_endian(COMMON_REQ *req);
void free_msg(MESSAGE msg);
void write_dword_to_buffer_big_endian(Byte *buf, DWord v);
void split_buffer_to_msg(Byte *buf_out, kal_uint16 buf_len, MESSAGE *msg, Byte*checkcode);
void get_term_para(Byte *para_out, Word *len, Word recv_number);
DWord convert(char *buf);
DWord convert2(float v);
kal_uint16 message_escape_and_addflag(Byte*buffer_out, MESSAGE msg, Byte checkcode);
Byte compute_check_code(MESSAGE msg);
void dump_out_buffer(Byte *out, int len);
/* name: get_msg_header
 * para:
 *	buffer_out, msg header out para
 *	mgsid, message id
 * 	sec_typ, secret type
 *	msglen: message length
 * return message heard length
 */
Byte get_msg_hearder(Byte *buffer_out, Word mgsid, Byte sec_typ, Word msglen, Word *number);
void get_bt_mac_addr(Byte* bt_mac);
void get_iccid_rsp(void *info);
void get_iccid_req(void);
kal_bool check_check_code(MESSAGE msg, Byte code);
/* name: message_escape
 * para:
 *    buffer_out
 *    msg
 *    checkcode
 * return the length after escape
 */
//转义还原
kal_uint8 message_unescape(const Byte *buffer_in, Byte *buffer_out, kal_uint16 len);
Byte parse_reg_ans(Byte *buffer, kal_uint16 buffer_len, Byte *token, Byte *token_len);
kal_bool check_cmd(const Byte *buffer, kal_uint16 *len);
Byte parse_flag_and_cmd(Byte *buffer, kal_uint16 buffer_len,Word recv_number,Word _msg_id);
Word get_query_attr_msg(Byte *msg_out,Word recv_number);
void ans_query_attr(Word recv_number);
Byte set_device_paras(Byte*buffer, kal_uint16 len);
void register_lock();
Word get_term_reg_msg(Byte *msg_out);
void lock_authorized();
void send_term_com_ans(Word serial, Word _msg_id, Byte result);
void send_query_term_para_ans(Word recv_number);
kal_uint32 get_unlock_count();
void unlock_count_add_one();
void get_device_id_from_imei(BCD *device_id);
Word get_battery_level();
void send_heart_beat(kal_uint8 socket_fd);
kal_uint8 get_gprs_duration_time() ;
kal_int16 get_gsm_strength();
GSM_STATE get_gsm_state(void);
void set_gsm_state(GSM_STATE state);
kal_bool switch_lock_mode(IOT_LOCK_MODE mode);
void bt_switch_lock_mode(IOT_LOCK_MODE mode,switch_lock_mode_callback cb);
void switch_trans_mode();
void switch_normal_mode();
void switch_energy_mode();
void switch_reconnect_mode(void);
IOT_LOCK_MODE get_lock_curr_mode();
void resend_msg(MSG_NODE* node);
void resend_msg_timeout(Word number);
void save_token_to_nvram(const kal_uint8 *token, kal_uint8 len);
kal_bool get_fota_upgrate_status();
void get_token_from_nvram(kal_uint8 *token, kal_uint8 *len);
void ans_saved_gps_attr(kal_int8 socket_fd, Word recv_number);
void report_loc_gps_info(void);
void ans_query_charge_attr(kal_int8 socket_fd, Word recv_number);
Word get_lock_auth_msg(Byte *msg_out);
void send_download_result_soc(Word serial, Byte result);
void clear_token_from_nvram(void);
void ecg_msg_upload(kal_int8 socket_fd);
void open_lock_ecg_msg_upload(void);
Byte set_ecg_paras(Byte*buffer, kal_uint16 len);
void send_query_ecg_para_ans(Word recv_number);

Word get_serial_number();
order_info *get_one_order_info();
void save_order_num_info_to_nvram(nvram_ef_order_num *order);
void save_order_num_to_nvram(const kal_uint8 *order_num) ;
void get_order_num_from_nvram(kal_uint8 *order_num);
U32 iotlock_srv_brw_get_current_utc_time (void);
void enter_low_energy_mode();
void handle_sms_recv_cmd(Byte *buf,Byte len);
void play_tone(void);
void save_is_has_order_num(const kal_uint8 is_has_order_num);
void get_is_has_order_num(kal_uint8 *is_has_order_num);
Byte parse_set_device_state_cmd(Byte *buffer,Word serial,Word msg_id);
kal_bool check_order_num(kal_uint8 *order_num,kal_uint8 order_num_len,UNLOCK_TYPE type,Word ans_serial);
#ifdef __LOCK_ADD_AES_ENCRYPT__   
void init_key(void);
void sms_aes128_ecb_encrypt(kal_uint8* input, kal_uint8 len, kal_uint8* output);
void sms_aes128_ecb_decrypt(kal_uint8* input, kal_uint8 in_len, kal_uint8* output,kal_uint8 *out_len);
void message_add_aes_encrypt_ecb(Byte*buffer_in,kal_uint16 buff_in_len,Byte*buffer_out,kal_uint32 *buff_out_len);
void message_add_aes_decrypt_ecb(Byte*buffer_in,kal_uint16 buff_in_len,Byte*buffer_out,kal_uint32 *buff_out_len);
kal_bool check_message_is_aes_encrypt(MSG_HEADER *msg_header);
void split_aes_decrypt_buff_to_msg(Byte *buf_in, kal_uint16 buf_len, MESSAGE *msg);
void aes_decrypt_msg_body(Byte *buf_in, kal_uint16 in_buf_len, Byte *buf_out, kal_uint32* out_buf_len);
void lock_msg_body_add_aes_encrypt_to_buff(Byte *buf_in, kal_uint16 in_buf_len, Byte *buf_out, kal_uint32* out_buf_len);
#endif
#endif
