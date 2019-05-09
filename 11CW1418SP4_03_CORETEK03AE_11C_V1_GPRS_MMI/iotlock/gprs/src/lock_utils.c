#include <stdlib.h>

#include "mmi_features.h"
#include "kal_release.h"
#include "kal_trace.h"
#include "MMIDataType.h"
#include "BtcmSrvProt.h"
#include "nvram_common_defs.h"
#include "mmi_frm_nvram_gprot.h"
#include "common_nvram_editor_data_item.h"
#include "Mmi_rp_srv_iot_lock_def.h"
#include "MMITimer_Int.h"
#include "l4c_common_enum.h"
#include "motion_sensor_custom.h"
#include "ModeSwitchSrvGprot.h"
#include "NwInfoSrvGprot.h"
#include "ImeiSrvGprot.h" 
#include "mmi_msg_struct.h"
#include "NwInfoSrv.h"
#include "med_utility.h"
#include "lock_net.h"
#include "lock_utils.h"
#include "lock_gps.h"
#include "lock_drv.h"
#include "iotlock.h"

#include "SmsSrvGprot.h"
#include "SmsAppType.h"
#include "aes_tiny.h"
#include "base64.h"

MSG_NODE g_resend_msg_pool[MAX_RESEND_ITEMS] = {0};
MSG_LIST g_msg_list = {NULL, NULL, 0};
Word reg_serial_no;
kal_char g_device_id[13] = {0};
BCD g_get_iccid[10] = {0};
IOT_LOCK_MODE curr_mode = IOT_LOCK_MODE_NORMAL; //0 normal 1 transport

extern kal_char *release_verno(void);
extern kal_char* release_hw_ver(void) ;
extern kal_char* build_date_time(void); 
extern MMI_BOOL srv_bootup_sim_is_inserted(mmi_sim_enum sim);
extern void verno_date_time_init(void);

extern kal_uint8 verno_date_time[16];
extern gprs_lock_cntx g_lock_cntx;
extern Word g_serial_number;
extern kal_bool g_beep_beep;
extern srv_bt_cm_cntx_struct g_srv_bt_cm_cntx;
extern kal_uint32 auth_counts;
extern kal_uint32 reg_counts;
extern kal_bool g_switch_server_flag;
extern Word g_gps_serial_number;
extern COMM_LOC g_comm_loc;

extern kal_bool g_need_open_gps;

kal_uint8	 g_ecg_result_buff[2048];
kal_uint32 g_ecg_upload_count = 0;

kal_bool g_is_update_heart_gap = KAL_FALSE;

kal_uint8 lock_aes128_key[BT_KEY_MAX_LEN + 1] = {0};

Word g_sms_serial_num = 0;


IOT_LOCK_MODE get_curr_mode(void)
{
	static MMI_BOOL is_init = MMI_FALSE;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	if (!is_init)
	{
		nvram_ef_iot_lock_struct lock_info;
		is_init =  MMI_TRUE;
		ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &lock_info, NVRAM_EF_IOT_LOCK_SIZE);
		set_curr_mode(lock_info.mode);
	}
	return g_lock_cntx.curr_mode;
}
void set_curr_mode(IOT_LOCK_MODE mode)
{
	g_lock_cntx.curr_mode = mode;
}
GSM_STATE get_gsm_state(void)
{
	return g_lock_cntx.gsm_state;
}
void set_gsm_state(GSM_STATE state)
{
	g_lock_cntx.gsm_state = state;

	if (state ){
		
	}else{
		
	}
}

void switch_energy_mode(Byte flag)
{
	nvram_ef_iot_lock_struct lock_info;

	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &lock_info, NVRAM_EF_IOT_LOCK_SIZE);
	if(lock_info.low_power != flag) {
		g_lock_cntx.low_power = lock_info.low_power = flag;
		WriteRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &lock_info, NVRAM_EF_IOT_LOCK_SIZE);
		if(flag == 0){
			if(read_lock_state()==LOCKED) {
				open_gps_and_get_loc(g_lock_cntx.gps_update_lock_gap, TIMER_LOC_REPORT);
			} else {
				open_gps_and_get_loc(g_lock_cntx.gps_update_run_gap, TIMER_LOC_REPORT);
			}
		}
	}
}

Byte* escape_one_byte(Byte orig, Byte *optr, kal_uint16 *count) {
	if(orig == 0x7e) {
		*optr = 0x7d;
		optr +=1;
		*optr = 0x02;
		*count +=2;
	} else if(orig == 0x7d) {
		*optr = 0x7d;
		optr +=1;
		*optr = 0x01;
		*count +=2;
	} else {
		*optr = orig;
		*count +=1;
	}

	return optr;
}

kal_uint16 message_escape_and_addflag(Byte*buffer_out, MESSAGE msg, Byte checkcode){
	const Byte *ptr = msg.header;
	Byte *optr = buffer_out;
	const Byte *end = ptr + msg.headerlen;
	kal_uint16 count = 0;

	*optr = START_END_FLAG; // start flag
	count +=1;
	optr +=1;

	for(; ptr < end; ptr++, optr++) {
		optr = escape_one_byte(*ptr, optr, &count);
	}

	if(msg.body != NULL) {
		ptr = msg.body;
		end = ptr + msg.bodylen;
		for(; ptr < end; ptr++, optr++) {
			optr = escape_one_byte(*ptr, optr, &count);
		}
	}

	optr = escape_one_byte(checkcode, optr, &count);

	optr++;
	*optr = START_END_FLAG;// end flag
	count +=1;

	return count;
}

//abcd.xxxx->ab.xxxxxx * 1000000
DWord convert(char *buf) {
	float tmp = strtof(buf, NULL);
	kal_uint32 a = (kal_uint32)(tmp / 100);
	kal_uint32 b = (kal_uint32)((tmp - a * 100) * 1000000 / 60 + 0.5);

	return (a * 1000000 + b);
}

DWord convert2(float v) {
	kal_uint32 a = (kal_uint32)(v / 100);
	kal_uint32 b = (kal_uint32)((v - a * 100) * 1000000 / 60 + 0.5);

	return (a * 1000000 + b);
}

void dump_out_buffer(Byte *out, int len){
	#ifdef SPCONSER_DEBUG
	char buffer[512] = {0};
	kal_uint8 *ptr = out;
	kal_uint8 *end = out+len;
	kal_uint16 count = 0;
	kal_uint16 i = 0;
	char *pre = "iotlock:out=";
	kal_uint8 llen = 127 - strlen(pre);

	kal_prompt_trace(MOD_MMI, "iotlock: enter %s", __FUNCTION__);
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
	kal_prompt_trace(MOD_MMI, "iotlock: len=%d, count=%d", len, count);
	i = 0;
	while(i < count) {
		kal_prompt_trace(MOD_MMI, "%s%s", pre, buffer + i * llen);
		i++;
	}
	#endif
}

Byte compute_check_code(MESSAGE msg) {
	const Byte *ptr = msg.header;
	const Byte *end = ptr + msg.headerlen;
	
	Byte check_code = *ptr;
	for(ptr++; ptr < end; ptr++) {
		check_code ^= *ptr;
	}

	if(msg.body != NULL) {
		ptr = msg.body;
		end = ptr + msg.bodylen;
		for(; ptr < end; ptr++) {
			check_code ^= *ptr;
		}
	}
	
	return check_code;
}

void write_dword_to_buffer_big_endian(Byte *buf, DWord v) {
	Byte *ptr = buf;

	Byte high1 = v % 256; //low
	Byte high2 = (v>>8) % 256; 
	Byte high3 = (v>>16) % 256;
	Byte high4 = (v>>24) % 256; //high
	
	*ptr = high4;
	*(ptr + 1) = high3;
	*(ptr + 2) = high2;
	*(ptr + 3) = high1;
}

void write_word_to_buffer_big_endian(Byte *buf, Word v) {
	Byte *ptr = buf;

	Byte low, high;

	low = v % 256;
	high = v >> 8;
	
	*ptr = high;
	*(ptr + 1) = low;
}


void get_term_para(Byte *para_out, Word *len, Word recv_number) {
	Byte *ptr = para_out;
	
	Byte serial[2] = {0};
	Byte para_count = 0x08;

	nvram_ef_iot_lock_struct para;
	Byte uart1_state = iot_get_bkup_nv_third_byte();
	
	serial[0] = recv_number>>8;
	serial[1] = recv_number%256;
	
	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &para, NVRAM_EF_IOT_LOCK_SIZE);

	memcpy(ptr, serial, 2);
	*(ptr+2) = para_count;
	ptr += sizeof(Word) + sizeof(Byte);
	*len = sizeof(Word) + sizeof(Byte);

	//para list
	if(strlen(para.apn) > 0) {
		//DWord id = 0x0010;
		Byte length = strlen(para.apn);

		write_dword_to_buffer_big_endian(ptr, 0x0010);
		*(ptr + 4) = length;
		ptr += sizeof(DWord) + sizeof(Byte);
		memcpy(ptr, para.apn, length);
		ptr += length;
		*len += (sizeof(DWord) + sizeof(Byte) + length);
		lock_debug_print(MOD_MMI, "iotlock: %s get apn: %s", __FUNCTION__, para.apn);
	}

	if(strlen(para.ip_domain) > 0) {
		//DWord id = 0x0013;
		Byte length = strlen(para.ip_domain);
		
		write_dword_to_buffer_big_endian(ptr, 0x0013);
		*(ptr + 4) = length;
		ptr += sizeof(DWord) + sizeof(Byte);
		memcpy(ptr, para.ip_domain, length);
		ptr += length;
		*len += (sizeof(DWord) + sizeof(Byte) + length);
		lock_debug_print(MOD_MMI, "iotlock: %s get ip_domain: %s", __FUNCTION__, para.ip_domain);
	}

	if(para.port > 0) {
		//DWord id = 0x0018;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0018);
		*(ptr + 4) = sizeof(DWord);
		ptr += sizeof(DWord) + sizeof(Byte);
		write_dword_to_buffer_big_endian(ptr, para.port);
		ptr += sizeof(DWord);
		*len += (2*sizeof(DWord) + sizeof(Byte));
		lock_debug_print(MOD_MMI, "iotlock: %s get port: %d", __FUNCTION__, para.port);
	}

	if(para.lock_second > 0) {
		//DWord id = 0x0027;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0027);
		*(ptr + 4) = sizeof(DWord);
		ptr += sizeof(DWord) + sizeof(Byte);
		write_dword_to_buffer_big_endian(ptr, para.lock_second);
		ptr += sizeof(DWord);
		*len += (2*sizeof(DWord) + sizeof(Byte));
		lock_debug_print(MOD_MMI, "iotlock: %s get lock_second: %d", __FUNCTION__, para.lock_second);
	}

	if(para.run_second > 0) {
		//DWord id = 0x0029;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0029);
		*(ptr + 4) = sizeof(DWord);
		ptr += sizeof(DWord) + sizeof(Byte);
		write_dword_to_buffer_big_endian(ptr, para.run_second);
		ptr += sizeof(DWord);
		*len += (2*sizeof(DWord) + sizeof(Byte));
		lock_debug_print(MOD_MMI, "iotlock: %s get run_second: %d", __FUNCTION__, para.run_second);
	}

	if(para.heart_beat_time_gap > 0) {
		//DWord id = 0x0030;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0030);
		*(ptr + 4) = sizeof(DWord);
		ptr += sizeof(DWord) + sizeof(Byte);
		write_dword_to_buffer_big_endian(ptr, para.heart_beat_time_gap);
		ptr += sizeof(DWord);
		*len += (2*sizeof(DWord) + sizeof(Byte));
		lock_debug_print(MOD_MMI, "iotlock: %s get heart_beat_time_gap: %d", __FUNCTION__, para.heart_beat_time_gap);
	}

	if(para.unlock_valid_time_gap > 0) {
		//DWord id = 0x0031;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0031);
		*(ptr + 4) = sizeof(DWord);
		ptr += sizeof(DWord) + sizeof(Byte);
		write_dword_to_buffer_big_endian(ptr, para.unlock_valid_time_gap);
		ptr += sizeof(DWord);
		*len += (2*sizeof(DWord) + sizeof(Byte));
		lock_debug_print(MOD_MMI, "iotlock: %s get unlock_valid_time_gap: %d", __FUNCTION__, para.unlock_valid_time_gap);
	}
	
	if(uart1_state ==0||uart1_state ==1) {
		//DWord id = 0x0032;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0032);
		*(ptr + 4) = sizeof(Byte);
		ptr += sizeof(DWord) + sizeof(Byte);
		*ptr = uart1_state;
		ptr += sizeof(Byte);
		*len += (sizeof(DWord) + 2*sizeof(Byte));
		lock_debug_print(MOD_MMI, "iotlock: %s get unlock_valid_time_gap: %d", __FUNCTION__, uart1_state);
	}
}

void split_buffer_to_msg(Byte *buf_out, kal_uint16 buf_len, MESSAGE *msg, Byte *checkcode){
	Byte *ptr = buf_out + 1; //remove start flag
	if((*ptr & 0x20) == 1) {
		//split pack, not support
		msg->headerlen = 16;
	} else {
		msg->headerlen = 12;
	}

	lock_debug_print(MOD_MMI, "iotlock: %s headerlen=%d", __FUNCTION__, msg->headerlen);
	msg->bodylen = buf_len - (3 + msg->headerlen);
	if(msg->bodylen > 0) {
		msg->body = (Byte*)med_alloc_ext_mem(msg->bodylen);
		memcpy(msg->body, (ptr + msg->headerlen), msg->bodylen);
	} else {
		msg->body = NULL;
	}
	
	msg->header = (Byte*)med_alloc_ext_mem(msg->headerlen);
	memcpy(msg->header, ptr, msg->headerlen);
	
	*checkcode = buf_out[buf_len - 2];
}

void free_msg(MESSAGE msg) {
	if(msg.body != NULL) {
		med_free_ext_mem((void **)&(msg.body));
		msg.body = NULL;
		msg.bodylen = 0;
	}

	if(msg.header != NULL) {
		med_free_ext_mem((void **)&(msg.header));
		msg.header = NULL;
		msg.headerlen = 0;
	}
}

void convert_header_from_big_endian(MSG_HEADER *msg_header){
	Word tmp = msg_header->id;
	msg_header->id = ((tmp%256)<<8) + (tmp>>8);

	tmp = msg_header->serial_number;
	msg_header->serial_number = ((tmp%256)<<8) + (tmp>>8);

	tmp = msg_header->attr;
	msg_header->attr = ((tmp%256)<<8) + (tmp>>8);
}

void convert_comm_req_from_big_endian(COMMON_REQ *req){
	Word tmp = req->serial;
	req->serial = ((tmp%256)<<8) + (tmp>>8);

	tmp = req->id;
	req->id = ((tmp%256)<<8) + (tmp>>8);
}


void dump_buffer(Byte *out, int len){
	char buffer[512] = {0};
	kal_uint8 *ptr = out;
	kal_uint8 *end = out+len;
	kal_uint16 count = 0;
	kal_uint16 i = 0;
	char *pre = "iotlock:out=";
	kal_uint8 llen = 127 - strlen(pre);

	kal_prompt_trace(MOD_MMI, "iotlock: enter %s", __FUNCTION__);
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
	kal_prompt_trace(MOD_MMI, "iotlock: len=%d, count=%d", len, count);
	i = 0;
	while(i < count) {
		kal_prompt_trace(MOD_MMI, "%s%s", pre, buffer + i * llen);
		i++;
	}
}

MSG_NODE *get_one_unused_node(){
	kal_uint8 i;
	MSG_NODE *ret = NULL;

	for(i = 0; i < MAX_RESEND_ITEMS; i++) {
		if(g_resend_msg_pool[i].used != 1) {
			ret = &g_resend_msg_pool[i];
			memset(ret, 0, sizeof(MSG_NODE));
			break;
		}
	}
	kal_prompt_trace(MOD_MMI, "iotlock: %s ret=%x, g_resend_msg_pool[%d]", __FUNCTION__, ret, i);

	return ret;
}

void debug_print_list(){
#ifdef SPCONSER_DEBUG
	MSG_NODE *node_ptr = g_msg_list.header;

	while(node_ptr != NULL) {
		kal_prompt_trace(MOD_MMI, "iotlock: ptr: %x", node_ptr);
		node_ptr = node_ptr->nxt;
	}
#endif
}

void add_one_msg_to_list(Byte *msg_ptr, Byte msg_len, Word number, kal_uint8 state){
	MSG_NODE *node_ptr = get_one_unused_node();

	kal_prompt_trace(MOD_MMI, "iotlock: start LINE: %d call print_list: ", __LINE__);
	debug_print_list();
	if(node_ptr == NULL) {
		remove_one_msg_from_list(KAL_FALSE, 0);
		update_all_msg_to_nvram();
		node_ptr = get_one_unused_node();
	}

	node_ptr->used= 1;
	node_ptr->msgdata.length = msg_len;
	node_ptr->state = state;
	node_ptr->msgdata.sendCount = 1;
	node_ptr->msgdata.number = number;
	memcpy(node_ptr->msgdata.body, msg_ptr, msg_len);

	// add to tail
	if(g_msg_list.header == NULL){
		g_msg_list.header = node_ptr;
	}

	if(g_msg_list.tail != NULL) {
		node_ptr->pre = g_msg_list.tail;
		g_msg_list.tail->nxt = node_ptr;
	}
	g_msg_list.tail = node_ptr;

	kal_prompt_trace(MOD_MMI, "iotlock: end LINE: %d call print_list: ", __LINE__);
	debug_print_list();
	update_all_msg_to_nvram();

	kal_prompt_trace(MOD_MMI, "iotlock: end %s", __FUNCTION__);
}

void delete_one_msg_node(MSG_NODE* node){
	if(node == g_msg_list.header && node == g_msg_list.tail) { //only one node
		g_msg_list.header = NULL;
		g_msg_list.tail = NULL;
	} else if(node == g_msg_list.header) {//delete the first node
		g_msg_list.header = node->nxt;
	} else if(node == g_msg_list.tail) {//delete the last node
		g_msg_list.tail = node->pre;
		g_msg_list.tail->nxt = NULL;
	} else {
		kal_prompt_trace(MOD_MMI, "iotlock: %d, msg_ptr->nxt %x", __LINE__, node->nxt);
		node->pre->nxt = node->nxt;
		node->nxt->pre = node->pre;
		kal_prompt_trace(MOD_MMI, "iotlock: %d, msg_ptr->pre%x", __LINE__, node->pre);
	}
	memset(node, 0, sizeof(MSG_NODE));
}

void remove_one_msg_from_list(kal_bool via_number, Word number){
	kal_prompt_trace(MOD_MMI, "iotlock: %s,  start  via=%d, number_type=%d", __FUNCTION__, via_number, number);
	debug_print_list();
	if(via_number) { //send success, and remove it from linklist
		MSG_NODE *msg_ptr = g_msg_list.header;
		while(msg_ptr != NULL) {
			if(msg_ptr->msgdata.number == number) {
				kal_prompt_trace(MOD_MMI, "iotlock: %s, line:%d, remove %x", __FUNCTION__, __LINE__, msg_ptr);
				delete_one_msg_node(msg_ptr);
				break;
			} else {
				msg_ptr = msg_ptr->nxt;
			}
		}
	} 
	else { //send failed, but the memory pool full, so remove the early data
		MSG_NODE *msg_ptr = g_msg_list.header;
		while(msg_ptr != NULL) {
			kal_prompt_trace(MOD_MMI, "iotlock: %s, line:%d, remove OTHER : %x", __FUNCTION__, __LINE__, msg_ptr);
			delete_one_msg_node(msg_ptr);
			break;
		}
	}
	kal_prompt_trace(MOD_MMI, "iotlock: %s,  end  via=%d, number_type=%d", __FUNCTION__, via_number, number);
	debug_print_list();
	update_all_msg_to_nvram();
}

MSG_NODE *get_one_msg_from_list(kal_bool with_number, Word number ){
	MSG_NODE *ret = NULL;
	MSG_NODE *msg_ptr = g_msg_list.header;

	if(with_number){
		while(msg_ptr != NULL) {
			if(msg_ptr->msgdata.number == number ) {
				ret = msg_ptr;
				break;
			}
			msg_ptr = msg_ptr->nxt;
		}
	} else {
		while(msg_ptr != NULL) {
			if(msg_ptr->state == STATE_WAIT_SEND) {
				ret = msg_ptr;
				break;
			}
			msg_ptr = msg_ptr->nxt;
		}
	}

	kal_prompt_trace(MOD_MMI, "iotlock: %s  ret=%x", __FUNCTION__, ret);
	return ret;
}

void update_all_msg_to_nvram(){
	kal_uint8 index;
	kal_uint8 count = 0;
	Byte buf[NVRAM_EF_IOT_LOCK_MSG_SIZE] = {0};
	MSG_NODE *ptr = g_msg_list.header;
	kal_uint16 size = sizeof(MSGDATA);

	kal_prompt_trace(MOD_MMI, "iotlock: %s start", __FUNCTION__);
	for(index = 1; index <= NVRAM_EF_IOT_LOCK_MSG_TOTAL; index++ ) {
		if(ptr != NULL) {
			if(ptr->used == 1){
				count +=1;
				memcpy(buf,  &ptr->msgdata, size);
				WriteRecordIot(NVRAM_EF_IOT_LOCK_MSG_LID, index, buf, NVRAM_EF_IOT_LOCK_MSG_SIZE);
			}
			ptr = ptr->nxt;
		} else {
			memset(buf, 0, size);
			WriteRecordIot(NVRAM_EF_IOT_LOCK_MSG_LID, index, buf, NVRAM_EF_IOT_LOCK_MSG_SIZE);
			break;
		}
	}
	kal_prompt_trace(MOD_MMI, "iotlock: %s count=%d", __FUNCTION__, count);
}

void update_all_msg_from_nvram(){
	kal_uint8 index;
	kal_uint8 count = 0;
	Byte buf[NVRAM_EF_IOT_LOCK_MSG_SIZE] = {0};

	for(index = 1; index <= NVRAM_EF_IOT_LOCK_MSG_TOTAL; index++ ) {
		kal_prompt_trace(MOD_MMI, "iotlock: %s read", __FUNCTION__);
		ReadRecordIot(NVRAM_EF_IOT_LOCK_MSG_LID, index, buf, NVRAM_EF_IOT_LOCK_MSG_SIZE);

		if(buf[1] == 0) {
			break;
		} else {
			MSG_NODE *node = get_one_unused_node();
			memcpy(&node->msgdata, buf, NVRAM_EF_IOT_LOCK_MSG_SIZE);
			if(node->msgdata.length > 0) {
				count +=1;
				node->used= 1;
				node->state = STATE_WAIT_SEND;

				// add to tail
				if(g_msg_list.header == NULL){
					g_msg_list.header = node;
				}

				if(g_msg_list.tail != NULL) {
					node->pre = g_msg_list.tail;
					g_msg_list.tail->nxt = node;
				}
				g_msg_list.tail = node;
			}
		}
	}

	kal_prompt_trace(MOD_MMI, "iotlock: %s get one, all:%d", __FUNCTION__, count);
}

Byte get_msg_hearder(Byte *buffer_out, Word mgsid, Byte sec_typ, Word msglen, Word*number_ptr){
	Byte len = 0;
	Word number = 0;
	Byte *ptr = buffer_out;
	BCD dev_id[6] = {0}; //{0x23,0x45,0x34,0x89,0x73,0x52};//BCD
	Word tmp;
	
	//convert to Big Endian 
	*(ptr+1) = mgsid % (1<<8);
	*ptr = mgsid>>8;
	ptr += sizeof(mgsid);
	len += sizeof(mgsid);

	if(msglen >= 1024){ //split pack
		//not support, FIXME
		tmp = (sec_typ << 10) + 1023;
	} 

	tmp = (sec_typ << 10) + msglen;
	*(ptr+1) = tmp % (1<<8);
	*ptr = tmp>>8;
	ptr += sizeof(tmp);
	len += sizeof(tmp);

	get_device_id_from_imei(dev_id);
	memcpy(ptr, dev_id, 6);
	ptr += sizeof(dev_id);
	len += sizeof(dev_id);

	if(*number_ptr > 0){
		number = *number_ptr;
	} else {
		number = get_serial_number();
		*number_ptr = number;
	}

	*(ptr+1) = number % (1<<8);
	*ptr = number>>8;
	ptr += sizeof(number);
	len += sizeof(number);

	return len;
}

kal_bool check_check_code(MESSAGE msg, Byte code){
	Byte org_code = compute_check_code(msg);
	lock_debug_print(MOD_MMI, "iotlock: %s org_code = %x, code=%x", __FUNCTION__, org_code, code);
	if(code == org_code){
		return KAL_TRUE;
	} else {
		return KAL_FALSE;
	}
}

kal_uint8 message_unescape(const Byte *buffer_in, Byte *buffer_out, kal_uint16 len){
	const Byte *ptr = buffer_in;
	const Byte *end = ptr + len-1;
	kal_uint8 length = len;

	*buffer_out = *ptr;
	buffer_out++;
	
	for(ptr++; ptr < end; ptr++, buffer_out++ ) {
		if(*ptr == 0x7d && *(ptr+1) == 0x02){
			*buffer_out = 0x7e;
			ptr++;
			length -= 1;
		} else if(*ptr == 0x7d && *(ptr+1) == 0x01) {
			*buffer_out = 0x7d;
			ptr++;
			length -= 1;
		} else {
			*buffer_out = *ptr;
		}
	}

	*buffer_out = *ptr;

	return length;

}

Byte set_device_paras(Byte*buffer, kal_uint16 len)
{
	Byte* ptr = buffer;
	Byte *start = NULL;
	Byte paras_count = *ptr;
	kal_uint8 index;
	Byte result = 0;
	nvram_ef_iot_lock_struct info;

	ptr +=1; //skip para count
	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);
	for(index = 0; index < paras_count; index++) {
		DWord para_type = (*(ptr)<<24) + (*(ptr+1)<<16) + (*(ptr+2)<<8) + *(ptr+3);
		Byte paralen = *(ptr+4);
		Byte para[64] = {0};

		start = ptr + sizeof(DWord) + sizeof(Byte);
		memcpy(para, start, paralen);
		
		switch(para_type) {
			case 0x0010: //main server apn
				lock_debug_print(MOD_MMI, "iotlock: %s apn=%s", __FUNCTION__, *para);
				memset(info.apn, 0, sizeof(info.apn));
				memcpy(info.apn, para, paralen);
				break;
			case 0x0013: //main server ip or domain
				lock_debug_print(MOD_MMI, "iotlock: %s server addr=%s", __FUNCTION__, *para);
				g_lock_cntx.new_domain.len = (paralen>IOT_LOCK_DOMAIN_MAX_LEN)?IOT_LOCK_DOMAIN_MAX_LEN:paralen;
				memcpy(g_lock_cntx.new_domain.domain_buffer, para, g_lock_cntx.new_domain.len);
				break;
			case 0x0018: //server tcp port
				{
					Byte *p = para;
					DWord tcpport = (*(p)<<24) + (*(p+1)<<16) + (*(p+2)<<8) + *(p+3);
					g_lock_cntx.new_domain.port = tcpport;
					g_lock_cntx.new_server_ip.port = tcpport;
					lock_debug_print(MOD_MMI, "iotlock: %s server port=%d", __FUNCTION__, tcpport);
				}
				break;
			case 0x0027: //when locked, send gps data time gap
				{
					Byte *p = para;
					DWord gap = (*(p)<<24) + (*(p+1)<<16) + (*(p+2)<<8) + *(p+3);
					if(gap >= 30) { 
						info.lock_second = gap;
						lock_debug_print(MOD_MMI, "iotlock: %s time gap =%d", __FUNCTION__, gap);
						g_lock_cntx.gps_update_lock_gap = gap;
						if(gap <= 120) {
								g_need_open_gps = KAL_TRUE;
						}
						open_gps_and_get_loc(gap, TIMER_LOC_REPORT);
					}else {
						result = 1;
					}
				}
				break;
			case 0x0029: //unlocked, send gps data time gap
				{
					Byte *p = para;
					DWord value = (*(p)<<24) + (*(p+1)<<16) + (*(p+2)<<8) + *(p+3);
					lock_debug_print(MOD_MMI, "iotlock: %s value =%d", __FUNCTION__, value);
					if(value == 0xFEFE) { 
						if(read_lock_state() == UNLOCKED) { //unlocked state
							info.mode = IOT_LOCK_MODE_TRANS;
							set_curr_mode(info.mode);
						}
					} else if(value >= 30){
						info.run_second = value;
						g_lock_cntx.gps_update_run_gap = value;
						if(value <= 120) {
							g_need_open_gps = KAL_TRUE;
						}
						open_gps_and_get_loc(value, TIMER_LOC_REPORT);
					}else {
						result = 1;
					}
				}
				break;
			 case 0x0030: //set heart gap
				{
					Byte *p = para;
					DWord value = (*(p)<<24) + (*(p+1)<<16) + (*(p+2)<<8) + *(p+3);
					lock_debug_print(MOD_MMI, "iotlock: %s value =%d", __FUNCTION__, value);
						info.heart_beat_time_gap = value;
						g_lock_cntx.heart_beat_time_gap = value;
						//StopTimer(LOCK_HEART_BEAT_TIMER);
						//heart_beat();
						g_is_update_heart_gap = KAL_TRUE;
				}
				break;

			case 0x0031: //set open lock valid time
				{
					Byte *p = para;
					DWord value = (*(p)<<24) + (*(p+1)<<16) + (*(p+2)<<8) + *(p+3);
					lock_debug_print(MOD_MMI, "iotlock: %s value =%d", __FUNCTION__, value);
					info.unlock_valid_time_gap = value;
					g_lock_cntx.unlock_valid_time_gap = value;
					lock_debug_print(MOD_MMI, "iotlock: %s() line:%d unlock_valid_time_gap = %d", __FUNCTION__,__LINE__,
						g_lock_cntx.unlock_valid_time_gap);
				}
				break;
			case 0x0032: //set uart1 state
				{
					Byte *p = para;
					Byte value = *p;
					lock_debug_print(MOD_MMI, "iotlock: %s value =%d", __FUNCTION__, value);
					iot_set_bkup_nv_third_byte(value);
					g_lock_cntx.uart1_state = value;
					if(value==0)
					{
						iot_uart1_power_ctrl(0);
						kal_prompt_trace(MOD_ENG, "iotlock: %s() line:%d: close uart1", __FUNCTION__,__LINE__);

					}
					else if(value==1)
					{
						iot_uart1_power_ctrl(1);
						kal_prompt_trace(MOD_ENG, "iotlock: %s() line:%d: open uart1", __FUNCTION__,__LINE__);
					}
					else
					{
						kal_prompt_trace(MOD_ENG, "iotlock: %s() line:%d: uart1 error state", __FUNCTION__,__LINE__);
					}
					lock_debug_print(MOD_MMI, "iotlock: %s() line:%d uart1_state = %d", __FUNCTION__,__LINE__,
						g_lock_cntx.uart1_state);
				}
				break;
			default:
				result = 3;
				break;
		}
		ptr =ptr + 5 + paralen;
	}
	WriteRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);

	return result;
}


void parse_lock_timegap_and_order_num_unlock(Byte *buffer,Byte order_num_len,Word recv_number,Word _msg_id,UNLOCK_TYPE type)
{
	Byte *ptr = buffer;
	DWord value = 0;	
	kal_uint32 current_time = 0;
	kal_int32 valid_time_gap = 0;
	Byte ret = 0x0;
	Byte order_num[LOCK_ORDER_NUM_MAX_LEN] = {0};
	
	value = (*(ptr)<<24) + (*(ptr+1)<<16) + (*(ptr+2)<<8) + *(ptr+3);
	g_lock_cntx.unlock_time = value;
	lock_debug_print(MOD_MMI, "iotlock: %s, unlock_time=%d",__func__, g_lock_cntx.unlock_time);
	current_time = iotlock_srv_brw_get_current_utc_time();
	lock_debug_print(MOD_MMI, "iotlock: %s line:%d, duration_time = %d, unlock_time=%d", __FUNCTION__, __LINE__,current_time, g_lock_cntx.unlock_time);
	valid_time_gap = current_time - g_lock_cntx.unlock_time;
	valid_time_gap = (valid_time_gap > 0)?valid_time_gap:(0 - valid_time_gap);
	lock_debug_print(MOD_MMI, "iotlock: %s line:%d, current_time = %d, valid_time_gap=%d", __FUNCTION__, __LINE__,current_time, valid_time_gap);

	
	if(order_num_len == 0)
	{
		g_lock_cntx.is_has_order_num = KAL_FALSE;
		save_is_has_order_num(g_lock_cntx.is_has_order_num);
		if(valid_time_gap<=g_lock_cntx.unlock_valid_time_gap)
		{
			ret = 0;
			lock_debug_print(MOD_MMI, "iotlock: %s line:%d, valid_time_gap = %d", __FUNCTION__, __LINE__,valid_time_gap);
			send_term_com_ans(recv_number, _msg_id, ret); 
			open_lock_first_play_tone_then_unlock(type);
		}
		else
		{
			ret = 1;
			send_term_com_ans(recv_number, _msg_id, ret); 
			unlock_time_over_valid_time_gap_report_loc(type, recv_number);
			lock_debug_print(MOD_MMI, "iotlock: %s line:%d,valid_time_gap error ", __FUNCTION__, __LINE__);

		}

	}
	else
	{
			ptr +=4;
			memcpy(order_num, ptr, order_num_len);
			lock_debug_print(MOD_MMI, "iotlock: %s line:%d, order_num_len = %d order_num = %s", __FUNCTION__, __LINE__,order_num_len,order_num);
			if(check_order_num(order_num, order_num_len,type, recv_number) == 0)
			{
				ret = 1;
				send_term_com_ans(recv_number, _msg_id, ret); 
			}
			else
			{
				if(valid_time_gap<=g_lock_cntx.unlock_valid_time_gap)
				{
					ret = 0;
					send_term_com_ans(recv_number, _msg_id, ret); 
					lock_debug_print(MOD_MMI, "iotlock: %s line:%d, valid_time_gap = %d", __FUNCTION__, __LINE__,valid_time_gap);
					open_lock_first_play_tone_then_unlock(type);
				}
				else
				{
					ret = 1;
					send_term_com_ans(recv_number, _msg_id, ret); 
					lock_debug_print(MOD_MMI, "iotlock: %s line:%d,valid_time_gap error ", __FUNCTION__, __LINE__);
					unlock_time_over_valid_time_gap_report_loc(type, recv_number);
				}
			}

	}

}




Byte parse_flag_and_cmd(Byte *buffer, kal_uint16 buffer_len,Word recv_number,Word _msg_id)
{
	Byte *ptr = buffer;
	Byte ret = 0x0;
	
	Byte flag = *ptr;
	Byte cmd = *(ptr+1);
	kal_uint32 current_time = 0;
	kal_int32 valid_time_gap = 0;
	DWord value = 0;	
	Byte order_num[LOCK_ORDER_NUM_MAX_LEN] = {0};
	Byte order_num_len = 0;
	lock_debug_print(MOD_MMI, "iotlock: %s, flag=%d, cmd=%d", __FUNCTION__, flag, cmd);
	g_serial_number = recv_number;
	
	if(flag == 0x1) 
	{
		if(cmd == 0) {
			if(buffer_len>LOCK_CTR_BODY_MAX_LEN||buffer_len<LOCK_CTR_BODY_MIN_LEN)
			{
				ret = 2;
				send_term_com_ans(recv_number, _msg_id, ret); 
				lock_debug_print(MOD_MMI, "iotlock: %s, invalid data ERROR", __FUNCTION__);
			}
			else
			{
				if(buffer_len==LOCK_CTR_BODY_MIN_LEN)
				{
					lock_debug_print(MOD_MMI, "iotlock: %s, no order num", __FUNCTION__);
					parse_lock_timegap_and_order_num_unlock(buffer+2,0, recv_number, _msg_id,NT_UNLOCK);

				}
				else
				{
					order_num_len = buffer_len - LOCK_CTR_BODY_MIN_LEN;
					lock_debug_print(MOD_MMI, "iotlock: %s, has order num order_num_len =%d ", __FUNCTION__,order_num_len);
					parse_lock_timegap_and_order_num_unlock(buffer+2,order_num_len, recv_number, _msg_id,NT_UNLOCK);

				}
			}
				
			
		} else if(cmd == 1) {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
		} else {
			ret = 2;
			send_term_com_ans(recv_number, _msg_id, ret); 
		}
	} 
	else if(flag == 0x2)
	{
		if(cmd == 0) {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
		} else if(cmd == 1) {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
			#ifdef __IOT_LOCK_BEEPSPK__
			open_beep_ms(150, 6);
			#else
			iot_play_tone_with_filepath(IOT_TONE_WARN);
			lock_debug_print(MOD_MMI, "iotlock: %s line:%d,gprs open speaker ", __FUNCTION__, __LINE__);
			#endif
		} else {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
			g_lock_cntx.speaker_play_counts = cmd;
			play_tone();
		}
	} 
	else if(flag == 0x4) {
		if(cmd == 0) {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
		} else if(cmd == 1) {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
			iot_led_ctrl(2,200,3);
		} else {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
			g_lock_cntx.led_flash_counts = cmd;
			gprs_and_sms_find_bike_led_flash();
		}
		lock_debug_print(MOD_MMI, "iotlock: %s line:%d,gprs open flash ", __FUNCTION__, __LINE__);
	} 
	else if(flag == 0x6) {
		if(cmd == 0) {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
		} else if(cmd == 1) {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
			iot_led_speak_warning_callback();
			lock_debug_print(MOD_MMI, "iotlock: %s line:%d,gprs open and speaker flash ", __FUNCTION__, __LINE__);
		} else {
			ret = 0;
			send_term_com_ans(recv_number, _msg_id, ret); 
			g_lock_cntx.speaker_led_flash_counts = cmd;
			gprs_and_sms_find_bike_speaker_led_flash();
		}
	} else {
		ret = 3;
		send_term_com_ans(recv_number, _msg_id, ret); 
	}
	return ret;
}

U32 iotlock_srv_brw_get_current_utc_time (void)
{
	applib_time_struct curr_time;
	kal_uint32 curr_time2sec = 0;
	applib_dt_get_rtc_time(&curr_time);
	kal_prompt_trace(MOD_MMI,"iotlock %s:%4d/%2d/%2d %2d:%2d:%2d:%2d",__FUNCTION__,curr_time.nYear,curr_time.nMonth,curr_time.nDay,curr_time.nHour,curr_time.nMin,curr_time.nSec,curr_time.nDay);
	curr_time2sec = applib_dt_mytime_2_utc_sec(&curr_time, MMI_FALSE);
	curr_time2sec = curr_time2sec - 8*60*60;;
	return curr_time2sec;
}


kal_bool check_cmd(const Byte *buffer, kal_uint16 *len){
	kal_uint16 index = 0;
	const Byte *ptr = (Byte*)buffer;

	if(*ptr == 0x7E) {
		kal_prompt_trace(MOD_MMI, "iotlock: %s index = %d, ptr=%x", __FUNCTION__, index++, *ptr);
		ptr++;
		while(ptr < buffer+MAX_BUFFER_LEN) {
			if(*ptr == 0x7E){
				*len = ptr - buffer + 1;
				return KAL_TRUE;
			}
			ptr++;
		}
	}
	
	return KAL_FALSE;
}

Byte parse_reg_ans(Byte *buffer, kal_uint16 buffer_len, Byte *token, Byte *token_len)
{
	Word serial_number;
	Byte ret;
	Byte *ptr = buffer;
	
	serial_number = ((*ptr) << 8) + *(ptr+1);
	kal_prompt_trace(MOD_MMI, "iotlock: %s serial number = %d", __FUNCTION__, serial_number);
	ptr += sizeof(Word);
	ret = *ptr;
	if(ret == 0 && reg_serial_no == serial_number) {
		ptr += sizeof(Byte);
		memcpy(token, ptr, buffer_len-3);
		*token_len = buffer_len - 3;
	} else {
		kal_prompt_trace(MOD_MMI, "iotlock: %s ret = %d", __FUNCTION__, ret);
	}
	
	return ret;
}



Word get_query_attr_msg(Byte *msg_out,Word recv_number){
	Byte *ptr = msg_out;
	Word count = 0;
	Byte dev_type[2] = {0x00, 0x00};
	Byte manuf_id[5] = {0x41,0x33,0x35,0x31,0x39};//{'A','3','5','1','9'};
	Byte dev_type_id[20] = {0x41,0x32,0x33,0x34,0x35,0x36,0x37,0x38
						   ,0,0,0,0,0,0,0,0,0,0,0,0};//{'A','2','3','4','5','6','7','8',0,0,0,0,0,0,0,0,0,0,0,0};
	Byte bt_mac[7] = {0x0};
	
	Byte hw_ver_len = 0;
	kal_char *ver_ptr = NULL;
	Byte sw_ver_len = 0;

	Byte serial[2] = {0};

	serial[0] = recv_number>>8;
	serial[1] = recv_number%256;

	memcpy(ptr, serial, 2);
	ptr +=2;
	count += 2;

	
	memcpy(ptr, dev_type, 2);
	ptr +=2;
	count += 2;
	memcpy(ptr, manuf_id, 5);
	ptr +=5;
	count += 5;
	memcpy(ptr, dev_type_id, 20);
	ptr +=20;
	count += 20;
	get_bt_mac_addr(bt_mac);
	bt_mac[6] = 0x01; //0x01(gps+gprs+bt+changre), 0x02(gps+pgrs+bt), other(gps+gprs)
	memcpy(ptr, bt_mac, 7);
	ptr +=7;
	count += 7;
	memcpy(ptr, g_get_iccid, 10);
	ptr +=10;
	count += 10;

	ver_ptr = release_hw_ver();
	hw_ver_len = strlen(ver_ptr);
	*ptr = hw_ver_len;
	ptr++;
	memcpy(ptr, ver_ptr, hw_ver_len);
	ptr +=hw_ver_len;

	sw_ver_len = strlen(verno_date_time);;
	*ptr = sw_ver_len;
	ptr++;
	memcpy(ptr, verno_date_time, sw_ver_len);
	ptr +=sw_ver_len;
	count += (2 + hw_ver_len + sw_ver_len);

	return count;

}

void ans_query_attr(Word recv_number){
	
	Byte header[16] = {0};
	Byte body[384] = {0};
	Byte buffer[512] = {0};
	
	kal_uint16 len;
	MESSAGE msg;
	Byte check_code;
	Word number = 0;

	msg.bodylen = get_query_attr_msg(body,recv_number);
	msg.body = body;
	msg.headerlen = get_msg_hearder(header, QUERY_TERMINAL_ATTR_ANS_ID, SECRET_NONE, msg.bodylen, &number);
	msg.header = header;
	check_code = compute_check_code(msg);
	
	
	len = message_escape_and_addflag(buffer, msg, check_code);
	lock_debug_print(MOD_MMI, "iotlock:  %s begin len=%d", __FUNCTION__, len);
	if(g_lock_cntx.auth_result) {
		lock_socket_send(g_lock_cntx.socket_id, buffer, len, KAL_FALSE, number);
	}
}

void register_lock() {
	Byte token[128] = {0};
	Byte tokenlen = 0;

	g_lock_cntx.auth_result = KAL_FALSE;
	get_token_from_nvram(token, &tokenlen);
	if(tokenlen > 3 && !g_switch_server_flag && (auth_counts == 0 || auth_counts%3 != 0)) {
		lock_authorized();
	} else {
		Byte header[16] = {0};
		Byte body[256] = {0};
		Byte buffer[384] = {0};

		kal_uint16 len;
		MESSAGE msg;
		Byte check_code;
		Word number = 0;
		StartTimer(LOCK_REG_TIMEOUT_TIMER, REG_AUTH_TIMEOUT, my_lock_socket_create);
		msg.bodylen = get_term_reg_msg(body);
		msg.body = body;
		msg.headerlen = get_msg_hearder(header, TERMINAL_REGISTER_ID, SECRET_NONE, msg.bodylen, &number);
		msg.header = header;
		check_code = compute_check_code(msg);

		reg_serial_no = number;
		reg_counts +=1;

		len = message_escape_and_addflag(buffer, msg, check_code);
		lock_debug_print(MOD_MMI, "iotlock:  %s begin len=%d, reg_counts=%d", __FUNCTION__, len, reg_counts);
		lock_socket_send(g_lock_cntx.socket_id, buffer, len, KAL_FALSE, number);
	}
}

Word get_term_reg_msg(Byte *msg_out) {
	Word count = 0;
	Byte *ptr = msg_out;
	
	Byte prov_id[2] = {0x00, 0x0B};
	Byte county_id[2] = {0x00, 0x01};
	Byte manuf_id[5] = {0x41,0x33,0x35,0x31,0x39};//{'A','3','5','1','9'};
	Byte dev_type_id[20] = {0x41,0x32,0x33,0x34,0x35,0x36,0x37,0x38
						   ,0,0,0,0,0,0,0,0,0,0,0,0};//{'A','2','3','4','5','6','7','8',0,0,0,0,0,0,0,0,0,0,0,0};
	Byte bt_mac[7] = {0};
	Byte dev_type = 0x02;

	get_bt_mac_addr(bt_mac);

	memcpy(ptr, prov_id, 2);
	ptr +=2;
	count +=2;
	memcpy(ptr, county_id, 2);
	ptr +=2;
	count +=2;
	memcpy(ptr, manuf_id, 5);
	ptr +=5;
	count +=5;
	memcpy(ptr, dev_type_id, 20);
	ptr +=20;
	count +=20;
	memcpy(ptr, bt_mac, 7);
	ptr +=7;
	count +=7;
	memcpy(ptr, &dev_type, 1);
	ptr +=1;
	count +=1;
	memcpy(ptr, g_get_iccid, 10);
	ptr +=10;
	count +=10;
	
	return count;
}

void lock_authorized(){
	
	Byte header[16] = {0};
	Byte token[128] = {0};
	Byte token_len = 0;
	Byte buffer[256] = {0};
	Byte body[256] = {0};
	kal_uint16 len;
	MESSAGE msg;
	Byte check_code;
	Word number = 0;

	StartTimer(LOCK_AUTH_TIMEOUT_TIMER, REG_AUTH_TIMEOUT, my_lock_socket_create);
	msg.bodylen = get_lock_auth_msg(body);
	msg.body = body;
	msg.headerlen = get_msg_hearder(header, TERMINAL_AUTN_ID, SECRET_NONE, msg.bodylen, &number);
	msg.header = header;
	check_code = compute_check_code(msg);

	len = message_escape_and_addflag(buffer, msg, check_code);
	lock_debug_print(MOD_MMI, "iotlock:  %s begin len=%d", __FUNCTION__, len);
	lock_socket_send(g_lock_cntx.socket_id, buffer, len, KAL_FALSE, number);
	auth_counts += 1;
}


Word get_lock_auth_msg(Byte *msg_out){
	Byte *ptr = msg_out;
	Word count = 0;
	Byte token[128] = {0};
	Byte token_len = 0;

	Byte dev_type = 0x02;
	Byte paranum = 0x02;
	Byte auth_id[2] = {0x00, 0x32};
	Byte sw_ver_id[2] = {0x00, 0x33};

	Byte sw_ver_len = 0;


	
	get_token_from_nvram(token, &token_len);

	memcpy(ptr, &paranum, 1);
	ptr +=1;
	count +=1;

	memcpy(ptr, auth_id, 2);
	ptr +=2;
	count +=2;
	
	*ptr = token_len;
	ptr++;
	memcpy(ptr, token, token_len);
	ptr +=token_len;
	count +=1+token_len;

	memcpy(ptr, sw_ver_id, 2);
	ptr +=2;
	count +=2;

	sw_ver_len = strlen(verno_date_time);
	*ptr = sw_ver_len;
	ptr++;
	memcpy(ptr, verno_date_time, sw_ver_len);
	ptr +=sw_ver_len;
	count +=  1 + sw_ver_len;

	return count;

}


void send_term_com_ans(Word serial, Word _msg_id, Byte result){
	
	Byte buffer[256] = {0};
	Byte header[16] = {0};
	Byte body[8] = {0};
	
	kal_uint16 len;
	MESSAGE msg;
	Byte check_code;
	Word number = 0;

	lock_debug_print(MOD_MMI, "iotlock:  %s serial=%x, _msg_id=%x", __FUNCTION__, serial, _msg_id);
	body[0] = serial>>8;
	body[1] = serial%256;
	body[2] = _msg_id>>8;
	body[3] = _msg_id%256;
	body[4] = result;
	msg.bodylen = 0x5;
	msg.body = body;
	msg.headerlen = get_msg_hearder(header, TERMINAL_COM_ANS_ID, SECRET_NONE, msg.bodylen, &number);
	msg.header = header;
	check_code = compute_check_code(msg);

	len = message_escape_and_addflag(buffer, msg, check_code);
	lock_debug_print(MOD_MMI, "iotlock:  %s  begin len=%d", __FUNCTION__, len);
	if(g_lock_cntx.auth_result) {
		lock_socket_send(g_lock_cntx.socket_id, buffer, len, KAL_FALSE, number);
	}
}
void send_download_result_soc(Word serial, Byte result){
	
	Byte buffer[256] = {0};
	Byte header[16] = {0};
	Byte body[8] = {0};
	
	kal_uint16 len;
	MESSAGE msg;
	Byte check_code;
	Word number = 0;

	lock_debug_print(MOD_MMI, "iotlock:  %s serial=%x,", __FUNCTION__, serial);
	body[0] = serial>>8;
	body[1] = serial%256;

	body[2] = result;
	msg.bodylen = 0x3;
	msg.body = body;
	msg.headerlen = get_msg_hearder(header, TERMINAL_DOWNLOAD_ID, SECRET_NONE, msg.bodylen, &number);
	msg.header = header;
	check_code = compute_check_code(msg);

	len = message_escape_and_addflag(buffer, msg, check_code);
	lock_debug_print(MOD_MMI, "iotlock:  %s  begin len=%d", __FUNCTION__, len);
	if(g_lock_cntx.auth_result) {
		lock_socket_send(g_lock_cntx.socket_id, buffer, len, KAL_FALSE, number);
	}
}
void send_query_term_para_ans(Word recv_number){
	
	Byte buffer[384] = {0};
	Byte header[16] = {0};
	Byte body[256] = {0};
	
	kal_uint16 len;
	Word bodylen = 0;
	Word number = 0;
	MESSAGE msg;

	Byte check_code;

	get_term_para(body, &bodylen, recv_number);
	msg.bodylen = bodylen;
	msg.body = body;
	msg.headerlen = get_msg_hearder(header, QUERY_TERMINAL_PARAM_ANS_ID, SECRET_NONE, msg.bodylen, &number);
	msg.header = header;
	check_code = compute_check_code(msg);

	len = message_escape_and_addflag(buffer, msg, check_code);
	lock_debug_print(MOD_MMI, "iotlock:  %s begin len=%d", __FUNCTION__, len);
	if(g_lock_cntx.auth_result) {
		lock_socket_send(g_lock_cntx.socket_id, buffer, len, KAL_FALSE, number);
	}
}


void ans_saved_gps_attr(kal_int8 socket_fd, Word recv_number){
	Byte buffer[256] = {0};
	Byte header[16] = {0};
	Byte body[50] = {0};
	Byte *ptr = body;
	
	kal_uint16 len;
	kal_int32 write_len;
	MESSAGE msg;
	Byte check_code;
	Word number = 0;
	COMM_LOC comm_loc;
	kal_uint32 utc_sec = 0;
	
	kal_prompt_trace(MOD_MMI, "iotlock:  %s serial=%x, _msg_id=%x", __FUNCTION__, recv_number, QUERY_ONCE_GPS_RSP);
	body[0] = recv_number>>8;
	body[1] = recv_number%256;
	ptr += 2;
	
	memset(&comm_loc,0,sizeof(comm_loc));
	memcpy(&comm_loc, &g_comm_loc, sizeof(COMM_LOC));
	
	comm_loc.state |= 0x1; //acc always open
	comm_loc.alarm = soc_htonl(comm_loc.alarm);
	comm_loc.state = soc_htonl(comm_loc.state);
	comm_loc.latitude = soc_htonl(comm_loc.latitude);
	comm_loc.longitude = soc_htonl(comm_loc.longitude);
	comm_loc.hight = soc_htons(comm_loc.hight);
	comm_loc.speed = soc_htons(comm_loc.speed);
	comm_loc.direction = soc_htons(comm_loc.direction);
	utc_sec = iotlock_srv_brw_get_current_utc_time();
	get_curr_time(comm_loc.datetime, utc_sec);
	memcpy(ptr, &comm_loc, sizeof(COMM_LOC));
	
	msg.bodylen = 2 + sizeof(COMM_LOC);
	msg.body = body;
	msg.headerlen = get_msg_hearder(header, QUERY_ONCE_GPS_RSP, SECRET_NONE, msg.bodylen, &number);
	msg.header = header;
	check_code = compute_check_code(msg);

	len = message_escape_and_addflag(buffer, msg, check_code);
	kal_prompt_trace(MOD_MMI, "iotlock:  %s  begin len=%d", __FUNCTION__, len);
	lock_socket_send(socket_fd, buffer, len, KAL_FALSE, number);
}


void report_loc_gps_info(void)
{
	ans_saved_gps_attr(g_lock_cntx.socket_id, g_gps_serial_number);
}


kal_uint32 get_unlock_count(){
	kal_uint32 unlock_counts;
	ReadRecordIot(NVRAM_EF_LOCK_COUNT_LID, 1, &unlock_counts, NVRAM_EF_LOCK_COUNT_SIZE);
	return unlock_counts;
}

void unlock_count_add_one(){
	kal_uint32 unlock_counts;
	ReadRecordIot(NVRAM_EF_LOCK_COUNT_LID, 1, (void*)&unlock_counts, NVRAM_EF_LOCK_COUNT_SIZE);
	unlock_counts += 1;
	WriteRecordIot(NVRAM_EF_LOCK_COUNT_LID, 1, (void*)&unlock_counts, NVRAM_EF_LOCK_COUNT_SIZE);
}

void get_device_id_from_imei(BCD *device_id){
	kal_uint8 i = 0;
	kal_char imei[SRV_IMEI_MAX_LEN + 1];
		
	memset(imei,0,sizeof(imei));

	//get IMEI
	if(!srv_imei_get_imei(MMI_SIM1, imei, SRV_IMEI_MAX_LEN+1)) {
		//get IMEI failed
		lock_debug_print(MOD_MMI, "iotlock: %s GET IMEI ERROR", __FUNCTION__);
	}

	for(i = 3; i < SRV_IMEI_MAX_LEN; i += 2 ) {
		device_id[i/2 - 1] = (imei[i] - '0') * 16 + (imei[i+1] - '0');
	}
	memcpy(g_device_id, imei+3, 12);
}

void save_token_to_nvram(const kal_uint8 *token, kal_uint8 len) {
	nvram_ef_iot_lock_struct info;

	memset(&info, 0, NVRAM_EF_IOT_LOCK_SIZE);
	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, (void*)&info, NVRAM_EF_IOT_LOCK_SIZE);
	memset(&info.auth_code, 0, 128);
	info.auth_len = (len>AUTH_CODE_MAX_LEN)?AUTH_CODE_MAX_LEN:len;
	memcpy(info.auth_code, token, info.auth_len);
	lock_debug_print(MOD_MMI, "iotlock:  %s() %d,auth_len=%d,", __FUNCTION__, __LINE__,info.auth_len);
	WriteRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, (void*)&info, NVRAM_EF_IOT_LOCK_SIZE);
}

void get_token_from_nvram(kal_uint8 *token, kal_uint8 *len){
	nvram_ef_iot_lock_struct info;

	memset(&info, 0, NVRAM_EF_IOT_LOCK_SIZE);
	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, (void*)&info, NVRAM_EF_IOT_LOCK_SIZE);
	*len = (info.auth_len>AUTH_CODE_MAX_LEN)?AUTH_CODE_MAX_LEN:(info.auth_len);
	if(info.auth_len > 3) {
		memcpy(token, info.auth_code, *len);
	}
}

void clear_token_from_nvram(void)
{
	nvram_ef_iot_lock_struct info;

	memset(&info, 0, NVRAM_EF_IOT_LOCK_SIZE);
	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, (void*)&info, NVRAM_EF_IOT_LOCK_SIZE);
	memset(&info.auth_code, 0, 128);
	info.auth_len = 0;
	WriteRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, (void*)&info, NVRAM_EF_IOT_LOCK_SIZE);
}

extern U8 bkss_get_battery_percentage();
Word get_battery_level()
{
	Word elec;
	elec = (Word)iot_get_battery_percentage();
	return elec;
}

Word get_heart_beat_msg(Byte *msg_out){
	Byte *ptr = msg_out;
	Word count = 0;

	Byte paranum = 0x05;
	Byte batt_vol_id[2] = {0x00, 0x30};
	Byte vbat_temp_id[2] = {0x00, 0x31};
	Byte batt_percent_id[2] = {0x00, 0x32};
	Byte solar_energy_vol_id[2] = {0x00, 0x33};
	Byte gsensor_id[2] = {0x00, 0x34};
	kal_int16 accl_x = 0;
	kal_int16 accl_y = 0;
	kal_int16 accl_z = 0;

	Byte sensor_xyz_bff[128] = {0};
	kal_int16 vbat = 0;
	Byte vbat_buf[2] = {0};
	kal_int16 vbat_temp = 0;
	Byte vbat_temp_buf[2] = {0};
	kal_int16 solar_energy_vol = 0;
	Byte solar_energy_vol_buf[2] = {0};
	Word battery_level = 0;
	Byte battery_level_buf[2] = {0};
	
	
	
	

	memcpy(ptr, &paranum, 1);
	ptr +=1;
	count +=1;
	
	vbat = iot_get_battery_voltage();
	vbat_buf[0] = vbat>>8;
	vbat_buf[1] = vbat%256;
	memcpy(ptr, batt_vol_id, 2);
	ptr +=2;
	count +=2;	
	*ptr = 2;
	ptr++;
	count ++;
	memcpy(ptr, vbat_buf, 2);
	ptr += 2;
	count +=2;

	vbat_temp = iotlock_get_battery_temperature();
	vbat_temp_buf[0] = vbat_temp>>8;
	vbat_temp_buf[1] = vbat_temp%256;
	
	memcpy(ptr, vbat_temp_id, 2);
	ptr +=2;
	count +=2;	
	*ptr = 2;
	ptr++;
	count ++;
	memcpy(ptr, vbat_temp_buf, 2);
	ptr += 2;
	count +=2;

	battery_level = get_battery_level();
	battery_level_buf[0] = battery_level>>8;
	battery_level_buf[1] = battery_level%256;
	memcpy(ptr, batt_percent_id, 2);
	ptr +=2;
	count +=2;	
	*ptr = 2;
	ptr++;
	count ++;
	memcpy(ptr, battery_level_buf, 2);
	ptr += 2;
	count +=2;

	solar_energy_vol = iot_get_battery_vcharger();
	solar_energy_vol_buf[0] = solar_energy_vol>>8;
	solar_energy_vol_buf[1] = solar_energy_vol%256;
	
	memcpy(ptr, solar_energy_vol_id, 2);
	ptr +=2;
	count +=2;	
	*ptr = 2;
	ptr++;
	count ++;
	memcpy(ptr, solar_energy_vol_buf, 2);
	ptr += 2;
	count +=2;

	lock_debug_print(MOD_MMI, "iotlock:  %s line: %d vbat=%d vbat_temp = %d battery_level = %d solar_energy_vol = %d", __FUNCTION__, __LINE__
		,vbat,vbat_temp,battery_level,solar_energy_vol);

	iot_acc_sensor_get_xyz_mg(&accl_x, &accl_y, &accl_z);

	memcpy(ptr, gsensor_id, 2);
	ptr +=2;
	count +=2;	
	
	sprintf(sensor_xyz_bff, "%d,%d,%d", accl_x,accl_y,accl_z);
	lock_debug_print(MOD_MMI, "iotlock:  %s line: %d sensor_xyz_bff=%s strlen(sensor_xyz_bff) = %d", __FUNCTION__, __LINE__,sensor_xyz_bff,strlen(sensor_xyz_bff));
	*ptr = strlen(sensor_xyz_bff);
	ptr++;
	count ++;
	memcpy(ptr, sensor_xyz_bff, strlen(sensor_xyz_bff));
	ptr +=strlen(sensor_xyz_bff);
	count +=strlen(sensor_xyz_bff);
	
	return count;

}


void send_heart_beat(kal_uint8 socket_fd){
	Byte buffer[512] = {0};
	Byte header[16] = {0};
	Byte body[512] = {0};

	kal_uint16 len;
	kal_int32 write_len;
	MESSAGE msg;
	Byte check_code;
	Word number = 0;

#ifdef __LOCK_ADD_AES_ENCRYPT__
		{

			kal_uint16 msg_body_len = 0;
			Byte aes_msg[1024] = 0;
			kal_uint16 aes_out_len = 0;
			msg_body_len = get_heart_beat_msg(body);
			aes_out_len = sizeof(aes_msg);
			dump_out_buffer(body, msg_body_len);
			lock_debug_print(MOD_MMI, "iotlock:  %s line: %d msg_body_len=%d", __FUNCTION__, __LINE__,msg_body_len);
			message_add_aes_encrypt_ecb(body, msg_body_len, aes_msg, (kal_uint32*)&aes_out_len);
			lock_debug_print(MOD_MMI, "iotlock:  %s line: %d out_len=%d", __FUNCTION__, __LINE__,aes_out_len);
			msg.bodylen = aes_out_len;
			msg.body = aes_msg;
			lock_debug_print(MOD_MMI, "iotlock:  %s line: %d msg.bodylen=%d", __FUNCTION__, __LINE__,msg.bodylen);
			dump_out_buffer(msg.body, msg.bodylen);
			msg.headerlen = get_msg_hearder(header, TERMINAL_HEART_ID, SECRET_AES, msg.bodylen, &number);
		}
#else
		msg.bodylen = get_heart_beat_msg(body);
		msg.body = body;
		msg.headerlen = get_msg_hearder(header, TERMINAL_HEART_ID, SECRET_NONE, msg.bodylen, &number);
#endif

	msg.header = header;
	check_code = compute_check_code(msg);

	len = message_escape_and_addflag(buffer, msg, check_code);
	lock_debug_print(MOD_MMI, "iotlock:  %s, begin len=%d", __FUNCTION__, len);
	lock_debug_print(MOD_MMI, "iotlock:  %s, socket_fd=%d", __FUNCTION__, socket_fd);
	write_len = soc_send(socket_fd, buffer, len, 0);
	dump_out_buffer(buffer, len);
	lock_debug_print(MOD_MMI, "iotlock:  %s, write_len=%d", __FUNCTION__, write_len);

	if(write_len < 0) {
		if(write_len != SOC_WOULDBLOCK) {
			my_lock_socket_create();
		}
	}
}

void ans_query_charge_attr(kal_int8 socket_fd, Word recv_number){
	Byte buffer[256] = {0};
	Byte header[16] = {0};
	Byte body[20] = {0};
	Byte *ptr = body;

	kal_uint16 len;
	kal_int32 write_len;
	MESSAGE msg;
	Byte check_code;
	Word number = 0;
	kal_int16 vbat = iot_get_battery_voltage();
	kal_int16 vbat_temp = iotlock_get_battery_temperature();

	Byte paranum = 0x02;

	body[0] = recv_number >> 8;
	body[1] = recv_number % 256;


	body[2] =paranum;

	body[3] =0x00;
	body[4] =0x34;
	body[5] =0x02;
	body[6] = vbat >> 8;
	body[7] = vbat % 256;

	body[8] =0x00;
	body[9] =0x35;
	body[10] =0x02;
	body[11] = vbat_temp >> 8;
	body[12] = vbat_temp % 256;


	msg.bodylen = 0x0D;
	msg.body = body;
	msg.headerlen = get_msg_hearder(header, QUERY_TEMP_ELEC_RSP, SECRET_NONE, msg.bodylen, &number);
	msg.header = header;
	

	check_code = compute_check_code(msg);
	len = message_escape_and_addflag(buffer, msg, check_code);

	lock_socket_send(socket_fd, buffer, len, KAL_FALSE, number);
}

kal_uint8 get_gprs_duration_time() 
{
	kal_uint8 tmp = (g_lock_cntx.gprs_duration_time + 500)/1000;
	if(tmp >= 255){
		tmp = 255;
	}
	return tmp;
}


kal_int16 get_gsm_strength()
{
	kal_int16 qdBm;
	kal_int16 asu;
	srv_nw_info_cntx_struct *cntx;

	cntx = srv_nw_info_get_cntx(MMI_SIM1);

	qdBm = cntx->signal.gsm_RSSI_in_qdBm;
	if(qdBm >= 0) {
		asu = 0;
	} else {
		asu = (113 + (qdBm >> 2) + 1) >> 1;
	}

	return asu;
}


switch_lock_mode_callback switch_lock_mode_cb;
void rf_switchmode_cb(mmi_event_struct *evt)
{
	srv_mode_switch_rsp_event_struct *rsp = (srv_mode_switch_rsp_event_struct*)evt;

	lock_debug_print(MOD_MMI, "iotlock: %s lock mode = %d", __FUNCTION__, rsp->user_data);
	lock_debug_print(MOD_MMI, "iotlock: %s result = %d", __FUNCTION__, rsp->result);
	if (rsp->result)
	{
		nvram_ef_iot_lock_struct info;
		srv_mode_switch_type_enum current_mode = srv_mode_switch_get_current_mode();
		g_lock_cntx.gprs_is_connecting = KAL_FALSE;
		ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);
		if(current_mode == SRV_MODE_SWITCH_SIM_ALL)
		{
			info.mode = IOT_LOCK_MODE_NORMAL;
			if(get_curr_mode()  != IOT_LOCK_MODE_NORMAL) 
			{
				set_curr_mode(info.mode);
				set_gsm_state(GSM_OPENED);
			}
			motion_sensor_pwr_on_off_switch();
		} 
		else if(current_mode == SRV_MODE_SWITCH_ALL_OFF)
		{
			IOT_LOCK_MODE lock_mode = (IOT_LOCK_MODE)rsp->user_data;
			if(lock_mode == IOT_LOCK_MODE_TRANS){
				info.mode = lock_mode;
				set_curr_mode(info.mode);
				set_gsm_state(GSM_CLOSED); //power off
				StopTimer(LOCK_RECONNECT_TIMER);
				StopTimer(LOCK_CONN_TIMER);
				gps_sleep();
				lock_socket_close(TYPE_LOCK_SOC);

				info.mode = IOT_LOCK_MODE_TRANS;
				motion_sensor_pwr_on_off_switch();
			}
			else if(lock_mode == IOT_LOCK_MODE_SWITCH_RF){
				lock_debug_print(MOD_MMI, "iotlock: %s line:%d reconnect switch normal mode", __FUNCTION__,__LINE__);
				switch_normal_mode();
			}
		}
		WriteRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);
		if(switch_lock_mode_cb != NULL)
		{
			switch_lock_mode_cb(KAL_TRUE);
			switch_lock_mode_cb = NULL;
		}
	}
	else { //switch failed, try again
		lock_debug_print(MOD_MMI, "iotlock: %s switch failed, try again", __FUNCTION__);
		switch_lock_mode((IOT_LOCK_MODE)rsp->user_data);
	}
}

kal_bool switch_lock_mode(IOT_LOCK_MODE mode)
{
	kal_bool result =KAL_FALSE;
	switch_lock_mode_cb = NULL;
	lock_debug_print(MOD_MMI, "iotlock: %s() mode=%d ", __FUNCTION__, mode);
	if(mode == IOT_LOCK_MODE_NORMAL) //normal
	{
		if(g_lock_cntx.low_power == 1) 
			switch_energy_mode(0x00);//exit energy mode
		result = srv_mode_switch_switch_rf_mode(SRV_MODE_SWITCH_SIM_ALL, (mmi_proc_func)rf_switchmode_cb, (void*)mode);
	}
	else if(mode == IOT_LOCK_MODE_TRANS)
	{
		StopTimer(LOCK_GPS_ON_TIMER);
		StopTimer(LOCK_GPS_UPDATE_TIMER);
		result = srv_mode_switch_switch_rf_mode(SRV_MODE_SWITCH_ALL_OFF, (mmi_proc_func)rf_switchmode_cb, (void*)mode);
	}
	else if(mode == IOT_LOCK_MODE_LOW_ENERGY)
	{
		StopTimer(LOCK_GPS_ON_TIMER);
		StopTimer(LOCK_GPS_UPDATE_TIMER);
		switch_energy_mode(0x01);
		result = KAL_TRUE;
	}
	else if(mode == IOT_LOCK_MODE_SWITCH_RF)
	{
		StopTimer(LOCK_RECONNECT_TIMER);
		result = srv_mode_switch_switch_rf_mode(SRV_MODE_SWITCH_ALL_OFF, (mmi_proc_func)rf_switchmode_cb, (void*)mode);
	}

	return result;
}
void bt_switch_lock_mode(IOT_LOCK_MODE mode,switch_lock_mode_callback cb)
{
	kal_bool result =KAL_FALSE;
	if(get_curr_mode() != mode)
	{
		result = switch_lock_mode(mode);
		switch_lock_mode_cb = cb;
		if((switch_lock_mode_cb != NULL)&&!result)
		{
			switch_lock_mode_cb(KAL_FALSE);
			switch_lock_mode_cb = NULL;
		}
	}
	else
	{
		if(switch_lock_mode_cb != NULL)
		{
			switch_lock_mode_cb(KAL_TRUE);
			switch_lock_mode_cb = NULL;
		}
	}
}

void switch_trans_mode()
{
	switch_lock_mode(IOT_LOCK_MODE_TRANS);
}

void switch_normal_mode(void)
{
	switch_lock_mode(IOT_LOCK_MODE_NORMAL);
}

void switch_low_energy_mode(void)
{
	switch_lock_mode(IOT_LOCK_MODE_LOW_ENERGY);
}


void enter_low_energy_mode() 
{
	switch_energy_mode(0x01); //enter low energy mode
}

void switch_reconnect_mode(void) 
{
	switch_lock_mode(IOT_LOCK_MODE_SWITCH_RF);
}

IOT_LOCK_MODE get_lock_curr_mode()
{
	return get_curr_mode();
}


void resend_msg(MSG_NODE* node)
{
	MSG_NODE* node_ptr = node;
	
	if(node_ptr == NULL){
		node_ptr = get_one_msg_from_list(KAL_FALSE, 0);
	}
	
	if(node_ptr != NULL){
		kal_int32 ret = soc_send(g_lock_cntx.socket_id, node_ptr->msgdata.body, node_ptr->msgdata.length, 0);
		kal_prompt_trace(MOD_MMI, "iotlock: %s, ret=%d", __FUNCTION__, ret);
		dump_buffer((Byte*)&node_ptr->msgdata, sizeof(MSGDATA));
		if(ret > 0 || ret == SOC_WOULDBLOCK){
			Word  number = node_ptr->msgdata.number;
			kal_uint8 offset =  number % 60;
			node_ptr->msgdata.sendCount += 1;
			node_ptr->state = STATE_SENDING;
			StartTimerEx(LOCK_RESEND_TIMEOUT_TIMER0 + offset, 
					SEND_TIMEOUT, (oslTimerFuncPtr)resend_msg_timeout, (void*)number);
		} else {
			kal_prompt_trace(MOD_MMI, "iotlock: %s, send error", __FUNCTION__);
		}
	}
}

void resend_msg_timeout(Word number) {
	MSG_NODE* node_ptr = get_one_msg_from_list(KAL_TRUE, number);
	kal_prompt_trace(MOD_MMI, "iotlock: %s, number:%d, addr:%x", __FUNCTION__, number, node_ptr);
	if(node_ptr != NULL) {
		if(node_ptr->msgdata.sendCount >= 4){
			delete_one_msg_node(node_ptr);
		} else {
			node_ptr->state = STATE_WAIT_SEND;
			resend_msg(node_ptr);
		}
	}
}


//»ñÈ¡À¶ÑÀµÄMac µØÖ·
void get_bt_mac_addr(Byte* bt_mac){
	U8 addr[7] = {0};

	memcpy(addr, (U8 *)&g_srv_bt_cm_cntx.host_dev.le_bd_addr.lap, 3);
	*( addr+3) = g_srv_bt_cm_cntx.host_dev.le_bd_addr.uap;
	memcpy((addr+4), (U8 *)&g_srv_bt_cm_cntx.host_dev.le_bd_addr.nap, 2);
	bt_mac[0] = addr[5];
	bt_mac[1] = addr[4];
	bt_mac[2] = addr[3];
	bt_mac[3] = addr[2];
	bt_mac[4] = addr[1];
	bt_mac[5] = addr[0];
}

void get_iccid_rsp(void *info) {
	kal_uint8 i=0;
	mmi_smu_read_sim_rsp_struct *iccid_data;

	kal_prompt_trace(MOD_MMI, "iotlock: %s enter begin", __FUNCTION__);
	iccid_data = (mmi_smu_read_sim_rsp_struct*) info;
	mmi_frm_clear_protocol_event_handler(MSG_ID_MMI_SMU_READ_SIM_RSP, (PsIntFuncPtr)get_iccid_rsp);
	memset(g_get_iccid, 0, sizeof(g_get_iccid));
	if (iccid_data->result.flag == L4C_SUCCESS) {
		kal_uint8 a;
		memcpy(g_get_iccid, iccid_data->data, iccid_data->length);
		for(i = 0; i < 10; i++){
			a = g_get_iccid[i];
			g_get_iccid[i] = ((a&0x0f)<<4) + (a>>4);
		}
		dump_buffer(g_get_iccid, sizeof(g_get_iccid));
	} else {
		lock_debug_print(MOD_MMI, "iotlock: %s read iccid fail!", __FUNCTION__);
	}
}

void get_iccid_req(void)
{
    MYQUEUE Message;
    mmi_smu_read_sim_req_struct *dataPtr;

    mmi_frm_set_protocol_event_handler(MSG_ID_MMI_SMU_READ_SIM_RSP, (PsIntFuncPtr)get_iccid_rsp,MMI_TRUE);

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

#ifdef __ADUPS_FOTA_SOC__

kal_uint32 accid_get_by_iot()
{
	lock_init_account();
	return g_lock_cntx.account_id;
}

void reset_fota_status()
{
	nvram_ef_fota_struct fota_info = {0};
	
	kal_prompt_trace(MOD_MMI, "iotlock: %s, ", __FUNCTION__);
	ReadRecordIot(NVRAM_EF_FOTA_LID, 1, &fota_info, NVRAM_EF_FOTA_SIZE);
	fota_info.flag = 0;
	WriteRecordIot(NVRAM_EF_FOTA_LID, 1, &fota_info, NVRAM_EF_FOTA_SIZE);
}

void set_fota_download_status(kal_uint8 status){
	nvram_ef_fota_struct fota_info = {0};

	kal_prompt_trace(MOD_MMI, "iotlock: %s, fota status:%d", __FUNCTION__, status);
	ReadRecordIot(NVRAM_EF_FOTA_LID, 1, &fota_info, NVRAM_EF_FOTA_SIZE);
	fota_info.flag |= status; //0x02
	WriteRecordIot(NVRAM_EF_FOTA_LID, 1, &fota_info, NVRAM_EF_FOTA_SIZE);
}

void save_the_current_version()
{
	kal_char *build_time_ptr = NULL;
	nvram_ef_fota_struct fota_info = {0};

	fota_info.flag |= 0x01;
	build_time_ptr = build_date_time();
	kal_prompt_trace(MOD_MMI, "iotlock: %s, fota curr ver:%s", __FUNCTION__, release_verno());
	kal_prompt_trace(MOD_MMI, "iotlock: %s, fota build time:%s", __FUNCTION__, build_time_ptr);
	memcpy(fota_info.sw_ver, build_time_ptr, strlen(build_time_ptr));
	WriteRecordIot(NVRAM_EF_FOTA_LID, 1, &fota_info, NVRAM_EF_FOTA_SIZE);
}

kal_bool get_fota_upgrate_status()
{
	nvram_ef_fota_struct fota_info = {0};

	ReadRecordIot(NVRAM_EF_FOTA_LID, 1, &fota_info, NVRAM_EF_FOTA_SIZE);

	if(fota_info.flag & 0x01) {
		return KAL_TRUE;
	} else {
		return KAL_FALSE;
	}
}

void update_fota_upgrate_status()
{
	kal_char *build_time_ptr = NULL;
	nvram_ef_fota_struct fota_info;

	build_time_ptr = build_date_time();
	ReadRecordIot(NVRAM_EF_FOTA_LID, 1, &fota_info, NVRAM_EF_FOTA_SIZE);

	if(fota_info.flag & 0x01) {
		//ÓÐÏÂ·¢fota Éý¼¶Ö¸Áî
		if(strncmp(fota_info.sw_ver, build_time_ptr, strlen(build_time_ptr)) != 0) {
			//fota ³É¹¦
			kal_prompt_trace(MOD_MMI, "iotlock: %s, fota success", __FUNCTION__);
			kal_prompt_trace(MOD_MMI, "iotlock: %s, fota old ver:%s, new ver:%s", __FUNCTION__, fota_info.sw_ver, build_time_ptr);
			memset(&fota_info, 0, NVRAM_EF_FOTA_SIZE);
			WriteRecordIot(NVRAM_EF_FOTA_LID, 1, &fota_info, NVRAM_EF_FOTA_SIZE);
		} else {
			kal_prompt_trace(MOD_MMI, "iotlock: %s need to fota", __FUNCTION__);
		}
	} else {
		kal_prompt_trace(MOD_MMI, "iotlock: %s no need to fota", __FUNCTION__);
	}
}

kal_bool srv_bootup_sim_is_inserted_test()
{
	MMI_BOOL ret = MMI_FALSE;
	ret=srv_sim_ctrl_is_inserted(MMI_SIM1);
	return ret;
}

#endif

void open_lock_ecg_msg_upload(void)
{
	kal_prompt_trace(MOD_MMI, "iotlock: %s()enter begin,%d ", __FUNCTION__,__LINE__);
	if(g_lock_cntx.ecg_curr_update_mode==ONLY_UPDATE_ONCE_MODE)
	{
#ifdef __NSK_ECG__
		ECG_PWR_OFF();
#endif
	}		
	ecg_msg_upload(g_lock_cntx.socket_id);

}

void ecg_msg_upload(kal_int8 socket_fd){
	Byte buffer[2100] = {0};
	Byte header[16] = {0};
	Byte body[2100] = {0};
	Byte *ptr = body;
	Byte order_num[LOCK_ORDER_NUM_MAX_LEN] = {0};
	Byte order_num_len;
	
	Byte temp_buff[408] = {0};
	Byte *temp_ptr = temp_buff;
	
	kal_uint16 len;
	MESSAGE msg;
	Byte check_code;
	Word number;
	
	Byte array_group_num = (g_lock_cntx.ecg_update_data_time_gap/g_lock_cntx.ecg_samp_time_gap);
	Byte paranum = 0x07;//(g_lock_cntx.ecg_update_data_time_gap/g_lock_cntx.ecg_samp_time_gap)*7;
	get_order_num_from_nvram(order_num);	
	lock_debug_print(MOD_MMI, "iotlock: %s(),%d order_num_len=%d", __FUNCTION__, __LINE__,strlen(order_num));
	
	*ptr = LOCK_ORDER_NUM_MAX_LEN-1;
	ptr++;
	memcpy(ptr, order_num, LOCK_ORDER_NUM_MAX_LEN-1);
	ptr +=LOCK_ORDER_NUM_MAX_LEN-1;

	memcpy(ptr, &array_group_num, 1);
	ptr +=1;
	memcpy(ptr, &paranum, 1);
	ptr +=1;

	memcpy(ptr, g_ecg_result_buff, array_group_num*ECG_PARA_NUM);
	ptr+=array_group_num*ECG_PARA_NUM;
	lock_debug_print(MOD_MMI, "iotlock: %s(),%d g_ecg_result_buff=%s  strlen(g_ecg_result_buff) = %d", __FUNCTION__, __LINE__,g_ecg_result_buff,array_group_num*ECG_PARA_NUM);

	msg.bodylen = 1+order_num_len+1+1+array_group_num*ECG_PARA_NUM;
	msg.body = body;
	msg.headerlen = get_msg_hearder(header, ECG_MSG_UPLOAD, SECRET_NONE, msg.bodylen, &number);
	msg.header = header;

	check_code = compute_check_code(msg);
	len = message_escape_and_addflag(buffer, msg, check_code);
	kal_prompt_trace(MOD_MMI, "iotlock: %s(),%d len = %d", __FUNCTION__,__LINE__,len);
	lock_socket_send(socket_fd, buffer, len, KAL_FALSE, number);

}

Byte set_ecg_paras(Byte*buffer, kal_uint16 len)
{
	Byte* ptr = buffer;
	Byte *start = NULL;
	Byte paras_count = *ptr;
	kal_uint8 index;
	Byte result = 0;
	nvram_ef_iot_lock_struct info;

	ptr +=1; //skip para count
	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);
	for(index = 0; index < paras_count; index++) {
		DWord para_type = (*(ptr)<<24) + (*(ptr+1)<<16) + (*(ptr+2)<<8) + *(ptr+3);
		Byte paralen = *(ptr+4);
		Byte para[64] = {0};

		start = ptr + sizeof(DWord) + sizeof(Byte);
		memcpy(para, start, paralen);
		
		switch(para_type) {
			case 0x0040: //ecg_is_update
				{
					Byte *p = para;
					info.ecg_is_update = *p;
					g_lock_cntx.ecg_is_update = *p;
					#ifdef __NSK_ECG__
					if(g_lock_cntx.ecg_is_update == 0)
					{
						ECG_PWR_OFF();
					}else if((read_lock_state() == UNLOCKED)&&(g_lock_cntx.ecg_is_update == 1)){
						ECG_PWR_OFF();
						ECG_PWR_ON();
					}else if((read_lock_state() == LOCKED)&&(g_lock_cntx.ecg_is_update == 1)){
						g_lock_cntx.ecg_is_update = 0;
						info.ecg_is_update = 0;
						result = 1;
					}
					#endif
					lock_debug_print(MOD_MMI, "iotlock: %s ecg_is_update=%d", __FUNCTION__, *p);
				}
				break;
			case 0x0041: //samp_time_gap
				{
					Byte *p = para;
					DWord samp_time_gap = (*(p)<<24) + (*(p+1)<<16) + (*(p+2)<<8) + *(p+3);
					info.ecg_samp_time_gap = samp_time_gap;
					g_lock_cntx.ecg_samp_time_gap = samp_time_gap;
					lock_debug_print(MOD_MMI, "iotlock: %s samp_time_gap=%d", __FUNCTION__, samp_time_gap);
				}
				break;
			case 0x0042: //ecg_curr_update_mode
				{
					Byte *p = para;
					info.ecg_update_data_mode = *p;
					g_lock_cntx.ecg_curr_update_mode = *p;
					lock_debug_print(MOD_MMI, "iotlock: %s ecg_curr_update_mode=%d", __FUNCTION__, *p);
				}
				break;
			case 0x0043: //ecg_update_data_time
				{
					Byte *p = para;
					DWord value = (*(p)<<24) + (*(p+1)<<16) + (*(p+2)<<8) + *(p+3);
					info.ecg_update_data_time = value;
					g_lock_cntx.ecg_update_data_time_gap = value;
					lock_debug_print(MOD_MMI, "iotlock: %s value =%d", __FUNCTION__, value);

				}
				break;
			default:
				result = 3;
				break;
		}
		ptr =ptr + 5 + paralen;
	}
	WriteRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);

	return result;
}

void get_ecg_para(Byte *para_out, Word *len, Word recv_number) 
{
	Byte *ptr = para_out;
	
	Byte serial[2] = {0};
	Byte para_count = 0x04;

	nvram_ef_iot_lock_struct para;

	serial[0] = recv_number>>8;
	serial[1] = recv_number%256;
	
	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &para, NVRAM_EF_IOT_LOCK_SIZE);

	memcpy(ptr, serial, 2);
	*(ptr+2) = para_count;
	ptr += sizeof(Word) + sizeof(Byte);
	*len = sizeof(Word) + sizeof(Byte);

	if(para.ecg_is_update== 0||para.ecg_is_update==1) {
		//DWord id = 0x0040;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0040);
		*(ptr + 4) = sizeof(Byte);
		ptr += sizeof(DWord) + sizeof(Byte);
		*ptr = para.ecg_is_update;
		ptr++;
		*len += (sizeof(DWord) + 2*sizeof(Byte));
		lock_debug_print(MOD_MMI, "iotlock: %s ecg_is_update: %d", __FUNCTION__, para.ecg_is_update);
	}

	if(para.ecg_samp_time_gap > 0) {
		//DWord id = 0x0041;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0041);
		*(ptr + 4) = sizeof(DWord);
		ptr += sizeof(DWord) + sizeof(Byte);
		write_dword_to_buffer_big_endian(ptr, para.ecg_samp_time_gap);
		ptr += sizeof(DWord);
		*len += (sizeof(DWord) + sizeof(Byte)+sizeof(DWord));
		lock_debug_print(MOD_MMI, "iotlock: %s get ecg_samp_time_gap: %d", __FUNCTION__, para.ecg_samp_time_gap);
	}

	if(para.ecg_update_data_mode == 1||para.ecg_update_data_mode == 2) {
		//DWord id = 0x0042;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0042);
		*(ptr + 4) = sizeof(Byte);
		ptr += sizeof(DWord) + sizeof(Byte);
		*ptr = para.ecg_update_data_mode;
		ptr++;
		*len += (sizeof(DWord) + 2*sizeof(Byte));
		lock_debug_print(MOD_MMI, "iotlock: %s get ecg_update_data_mode: %d", __FUNCTION__, para.ecg_update_data_mode);
	}	

	if(para.ecg_update_data_time > 0) {
		//DWord id = 0x0043;
		//Byte len = sizeof(DWord);
		
		write_dword_to_buffer_big_endian(ptr, 0x0043);
		*(ptr + 4) = sizeof(DWord);
		ptr += sizeof(DWord) + sizeof(Byte);
		write_dword_to_buffer_big_endian(ptr, para.ecg_update_data_time);
		ptr += sizeof(DWord);
		*len += (sizeof(DWord) + sizeof(Byte)+sizeof(DWord));
		lock_debug_print(MOD_MMI, "iotlock: %s get ecg_update_data_time: %d", __FUNCTION__, para.ecg_update_data_time);
	}
}

void send_query_ecg_para_ans(Word recv_number)
{
	
	Byte buffer[384] = {0};
	Byte header[16] = {0};
	Byte body[256] = {0};
	
	kal_uint16 len;
	Word bodylen = 0;
	Word number = 0;
	MESSAGE msg;

	Byte check_code;

	get_ecg_para(body, &bodylen, recv_number);
	msg.bodylen = bodylen;
	msg.body = body;
	msg.headerlen = get_msg_hearder(header, QUERY_SET_ECG_CTR_PARAM_ANS_ID, SECRET_NONE, msg.bodylen, &number);
	msg.header = header;
	check_code = compute_check_code(msg);

	len = message_escape_and_addflag(buffer, msg, check_code);
	lock_debug_print(MOD_MMI, "iotlock:  %s begin len=%d", __FUNCTION__, len);
	if(g_lock_cntx.auth_result) {
		//lock_socket_send(g_lock_cntx.socket_id, buffer, len, KAL_FALSE, number);
	}
}

order_info *get_one_order_info()
{
	order_info *info = (order_info*)get_ctrl_buffer(sizeof(order_info));
	if(info != NULL) {
		memset(info, 0, sizeof(order_info));
	}

	return info;
}

void save_order_num_info_to_nvram(nvram_ef_order_num *order)
{
	WriteRecordIot(NVRAM_EF_ORDER_NUM_LID, 1, order, NVRAM_EF_ORDER_NUM_SIZE);
}

void save_order_num_to_nvram(const kal_uint8 *order_num) 
{
	nvram_ef_order_num order = {0};
	
	ReadRecordIot(NVRAM_EF_ORDER_NUM_LID, 1, &order, NVRAM_EF_ORDER_NUM_SIZE);
	memcpy(order.order_num, order_num, LOCK_ORDER_NUM_MAX_LEN-1);
	WriteRecordIot(NVRAM_EF_ORDER_NUM_LID, 1, &order, NVRAM_EF_ORDER_NUM_SIZE);
}

void get_order_num_from_nvram(kal_uint8 *order_num)
{
	nvram_ef_order_num order = {0};

	ReadRecordIot(NVRAM_EF_ORDER_NUM_LID, 1, &order, NVRAM_EF_ORDER_NUM_SIZE);
	memcpy(order_num, order.order_num, LOCK_ORDER_NUM_MAX_LEN-1);
}

kal_bool check_order_num(kal_uint8 *order_num,kal_uint8 order_num_len,UNLOCK_TYPE type,Word ans_serial)
{

	lock_debug_print(MOD_MMI, "iotlock: %s order_num_len =%d", __FUNCTION__,order_num_len);
	if(order_num_len < LOCK_ORDER_NUM_MAX_LEN)
	{
		kal_uint8 temp_order_num[LOCK_ORDER_NUM_MAX_LEN] = {0}; 
		get_order_num_from_nvram(temp_order_num);
		lock_debug_print(MOD_MMI, "iotlock: %s temp_order_num =%s,order_num = %s", __FUNCTION__,temp_order_num,order_num);
		g_lock_cntx.is_has_order_num = KAL_TRUE;
		save_order_num_to_nvram(order_num);
		save_is_has_order_num(g_lock_cntx.is_has_order_num);
		if(strcmp(order_num, temp_order_num) == 0)
		{
			g_lock_cntx.is_has_same_order_num =  KAL_TRUE;
			if(type == SMS_UNLOCK)
				send_term_com_ans(g_sms_serial_num, SMS_OPEN_LOCK_TYPE, 1);
			has_same_order_num_report_loc(type, ans_serial);
			lock_debug_print(MOD_MMI, "iotlock: %s has_same_order_num", __FUNCTION__);
			return KAL_FALSE;
		}
		else
		{
			g_lock_cntx.is_has_same_order_num =  KAL_FALSE;
			lock_debug_print(MOD_MMI, "iotlock: %s no_same_order_num", __FUNCTION__);
			return KAL_TRUE;
		}
	}
	return KAL_FALSE;
}

void save_is_has_order_num(const kal_uint8 is_has_order_num) 
{
	nvram_ef_order_num order = {0};
	
	ReadRecordIot(NVRAM_EF_ORDER_NUM_LID, 1, &order, NVRAM_EF_ORDER_INFO_SIZE);
	order.is_has_order_num = is_has_order_num;
	WriteRecordIot(NVRAM_EF_ORDER_NUM_LID, 1, &order, NVRAM_EF_ORDER_INFO_SIZE);
}

void get_is_has_order_num(kal_uint8 *is_has_order_num)
{
	nvram_ef_order_num order = {0};

	ReadRecordIot(NVRAM_EF_ORDER_NUM_LID, 1, &order, NVRAM_EF_ORDER_INFO_SIZE);
	*is_has_order_num = order.is_has_order_num;
}


Word get_serial_number()
{
	static Word number = 0;
	Word result;

	result = number;
	number += 1;

	return result;
}



Byte parse_set_device_state_cmd(Byte *buffer,Word serial,Word msg_id)
{
	Byte *ptr = buffer;
	Byte *start = NULL;
	Byte paras_count = *ptr;
	kal_uint8 index;
	kal_bool s_need_reboot = KAL_FALSE;
	nvram_ef_iot_lock_struct info;
	Byte ret = 0;
	ptr +=1; //skip para count
	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);
	lock_debug_print(MOD_MMI, "iotlock: %s paras_count =%d", __FUNCTION__,paras_count);

	for(index = 0; index < paras_count; index++) {
		Byte para_type = *ptr;
		Byte paralen = *(ptr+1);
		Byte para[64] = {0};

		start = ptr +2*sizeof(Byte);
		memcpy(para, start, paralen);
		lock_debug_print(MOD_MMI, "iotlock: %s para_type =%d", __FUNCTION__,para_type);
		switch(para_type) {
			case 0x01: //reboot
				{
					Byte *p = para;
					Byte value = *p;
					if(value==0x01)
					{
						ret = 0;
						s_need_reboot = KAL_TRUE;
						kal_prompt_trace(MOD_ENG, "iotlock: %s() line:%d: srv_reboot_normal_start", __FUNCTION__,__LINE__);
					}
					else
					{
						s_need_reboot = KAL_FALSE;
						ret = 3;
					}
					send_term_com_ans(serial, msg_id, ret);
					
				}
				break;
			case 0x02: //change mode
				{
					Byte *p = para;
					Word value = (*(p)<<8) + *(p+1);
					lock_debug_print(MOD_MMI, "iotlock: %s value =%d", __FUNCTION__, value);
					if(value==0x0001)
					{
						ret = 0;
						info.mode = IOT_LOCK_MODE_NORMAL;
						StartTimer(LOCK_SMS_CHANGE_TIMER, 1000, switch_normal_mode);
						kal_prompt_trace(MOD_ENG, "iotlock: %s() line:%d: IOT_LOCK_MODE_NORMAL", __FUNCTION__,__LINE__);

					}
					else if(value==0x0002)
					{
						ret = 0;
						info.mode = IOT_LOCK_MODE_LOW_ENERGY;
						StartTimer(LOCK_SMS_CHANGE_TIMER, 1000, switch_low_energy_mode);
						kal_prompt_trace(MOD_ENG, "iotlock: %s() line:%d: IOT_LOCK_MODE_LOW_ENERGY", __FUNCTION__,__LINE__);
					}
					else if(value==0x0003)
					{
						
						if(read_lock_state() == UNLOCKED)
						{
							info.mode = IOT_LOCK_MODE_TRANS;
							StartTimer(LOCK_SMS_CHANGE_TIMER, 1000, switch_trans_mode);
							ret = 0;
						}
						else
						{
							ret = 1;
						}
						kal_prompt_trace(MOD_ENG, "iotlock: %s() line:%d: IOT_LOCK_MODE_TRANS", __FUNCTION__,__LINE__);
					}
					else
					{
						ret = 3;
					}
				}
				break;
			default:
				lock_debug_print(MOD_ENG, "iotlock: %s NO THIS COMMAND", __FUNCTION__);
				break;
		}
		ptr =ptr + 2 + paralen;
	}
	WriteRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);

	if(s_need_reboot){
		s_need_reboot = KAL_FALSE;
		StartTimer(LOCK_SMS_RESET_TIMER, 5*100, srv_reboot_normal_start);
	}
	return ret;
}


/**********************************sms begin*********************/
#ifdef __IOT_LOCK__
#if 1
#define lock_sms_dbg_print		kal_prompt_trace
#else
#define lock_sms_dbg_print(...)
#endif

kal_uint16 lock_number[SRV_SMS_MAX_ADDR_LEN];
kal_uint16 bkss_content[SMS_MAX_CONTENT_LEN];
kal_uint16 sms_count = 0;


kal_bool check_device_id(const char *device_id) {
	kal_bool ret = KAL_FALSE;
	kal_char src_imei[SRV_IMEI_MAX_LEN + 1];
	
	ret = srv_imei_get_imei(MMI_SIM1, src_imei, SRV_IMEI_MAX_LEN+1);
	if(ret) {
		if(strncmp(g_device_id, device_id, MAX_DEVICE_ID_LEN) == 0) { //equals
			kal_prompt_trace(MOD_MMI, "iotlock_sms: device id is OK");
			return KAL_TRUE;
		} else {
			kal_prompt_trace(MOD_MMI, "iotlock_sms: NOT GOOD device id is");
			return KAL_FALSE;
		}
	} else {
		kal_prompt_trace(MOD_MMI, "iotlock_sms: GET IMEI ERROR");
		return KAL_FALSE;
	}
}


Byte sms_check_code(const char *buffer,kal_uint8 check_len) {
	const Byte *ptr = buffer;
	const Byte *end = ptr + check_len;
	
	Byte check_code = *ptr;

	if(buffer != NULL)
	{
		for(ptr++; ptr < end; ptr++) {
			check_code ^= *ptr;
		}
	}
	
	return check_code;
}


kal_bool sms_check_check_code(const char *buffer, kal_uint8 check_len,Byte code){
	Byte org_code = sms_check_code(buffer,check_len);
	lock_debug_print(MOD_MMI, "iotlock_sms: %s org_code = %x, code=%x", __FUNCTION__, org_code, code);
	if(code == org_code){
		return KAL_TRUE;
	} else {
		return KAL_FALSE;
	}
}

//¼ì²é½ÓÊÕµ½µÄÖÊÁ¿ÊÇ·ñÕýÈ·
//return KAL_FALSE±íÊ¾ÖÊÁ¿²»ºÏ¸ñ£¬KAL_TRUE ±íÊ¾ÖÊÁ¿ÕýÈ·
kal_bool check_sms_cmd(const char *buffer){
	
	kal_uint32 cmd;
	const char *ptr = buffer;
	kal_uint16 body_len = 0;
	kal_uint16 check_len = 0;
	Byte check_code = 0;
	if((*ptr == 0xA5)&&(*(ptr+1) == 0x00)) {
		//Æ«ÒÆ1¸ö×Ö½Ú
		ptr +=2;

		//»ñÈ¡ÏûÏ¢Ìå³¤¶È
		body_len = *ptr;
		ptr += 12;
		lock_sms_dbg_print(MOD_MMI, "iotlock_sms: %s line= %d,body_len = %d", __FUNCTION__,__LINE__,body_len);
		g_sms_serial_num = (*(ptr)<<8) + *(ptr+1);
		lock_sms_dbg_print(MOD_MMI, "iotlock_sms: %s line= %d,g_sms_serial_num = %d", __FUNCTION__,__LINE__,g_sms_serial_num);
		check_code = *(buffer+body_len-1);
		lock_sms_dbg_print(MOD_MMI, "iotlock_sms: %s line= %d,check_code = %x", __FUNCTION__,__LINE__,check_code);
		check_len = body_len -2;
		if(sms_check_check_code(buffer,check_len,check_code)) {
			lock_sms_dbg_print(MOD_MMI, "iotlock_sms: %s line:%d parse OK", __FUNCTION__,__LINE__);
			return KAL_TRUE;
			
		} else {
			lock_sms_dbg_print(MOD_MMI, "iotlock_sms: %s line:%d parse error", __FUNCTION__,__LINE__);
			return KAL_FALSE;
		}
	}
	else {
		lock_sms_dbg_print(MOD_MMI, "iotlock_sms: %s line:%d ERROR CMD", __FUNCTION__,__LINE__);
		return KAL_FALSE;
	}
}

void parse_open_lock_cmd(Byte *buffer)
{
	Byte *ptr = buffer+SMS_MSG_HEADER_LEN;

	Byte order_num[LOCK_ORDER_NUM_MAX_LEN] = {0};
	Byte order_num_len = 0;

	kal_uint32 current_time = 0;
	kal_int32 valid_time_gap = 0;
	DWord value = 0;
	kal_int32 body_len = 0;
	body_len = *(buffer+2);
	lock_sms_dbg_print(MOD_MMI, "iotlock_sms: %s line:%d body_len = %d", __FUNCTION__,__LINE__,body_len);

	value = (*(ptr)<<24) + (*(ptr+1)<<16) + (*(ptr+2)<<8) + *(ptr+3);
	g_lock_cntx.unlock_time = value;
	kal_prompt_trace(MOD_MMI, "iotlock_sms: %s unlock_time = %d", __FUNCTION__, g_lock_cntx.unlock_time);
	current_time = iotlock_srv_brw_get_current_utc_time();
	lock_debug_print(MOD_MMI, "iotlock_sms: %s line:%d, duration_time = %d, unlock_time=%d", __FUNCTION__, __LINE__,current_time, g_lock_cntx.unlock_time);
	valid_time_gap = current_time - g_lock_cntx.unlock_time;
	valid_time_gap = (valid_time_gap > 0)?valid_time_gap:(0 - valid_time_gap);
	lock_debug_print(MOD_MMI, "iotlock_sms: %s line:%d, duration_time = %d, valid_time_gap=%d", __FUNCTION__, __LINE__,current_time, valid_time_gap);
	
	if(body_len == SMS_MSG_HEADER_LEN + 6)
	{
		parse_lock_timegap_and_order_num_unlock(buffer+SMS_MSG_HEADER_LEN,0, g_sms_serial_num, SMS_OPEN_LOCK_TYPE,SMS_UNLOCK);
	}
	else
	{
		ptr += 4;
		order_num_len = body_len - SMS_MSG_HEADER_LEN -6;
		parse_lock_timegap_and_order_num_unlock(buffer+SMS_MSG_HEADER_LEN,order_num_len, g_sms_serial_num, SMS_OPEN_LOCK_TYPE,SMS_UNLOCK);
	}
}

void parse_set_device_param_cmd(Byte *buffer)
{
	Byte *ptr = buffer+SMS_MSG_HEADER_LEN;
	Byte *start = NULL;
	Byte mode_type;
	Byte domain_buffer_len;
	Byte port[10];
	Word tcpport = 0;
	Byte paras_count = *ptr;
	kal_uint8 index;
	
	nvram_ef_iot_lock_struct info;
	
	ptr +=1; //skip para count
	ReadRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);
	lock_debug_print(MOD_MMI, "iotlock_sms: %s paras_count =%d", __FUNCTION__,paras_count);

	for(index = 0; index < paras_count; index++) {
		Byte para_type = *ptr;
		Byte paralen = *(ptr+1);
		Byte para[64] = {0};

		start = ptr +2*sizeof(Byte);
		memcpy(para, start, paralen);
		lock_debug_print(MOD_MMI, "iotlock_sms: %s para_type =%d", __FUNCTION__,para_type);
		switch(para_type) {
			case 0x01: //main server apn
				lock_debug_print(MOD_MMI, "iotlock_sms: %s server addr=%s", __FUNCTION__, *para);
				g_lock_cntx.new_domain.len = (paralen>IOT_LOCK_DOMAIN_MAX_LEN)?IOT_LOCK_DOMAIN_MAX_LEN:paralen;
				memcpy(g_lock_cntx.new_domain.domain_buffer, para, g_lock_cntx.new_domain.len);
				break;
			case 0x02: //main server ip or domain
				{
					Byte *p = para;
					Word tcpport = (*(p)<<8) + *(p+1);
					g_lock_cntx.new_domain.port = tcpport;
					g_lock_cntx.new_server_ip.port = tcpport;
					lock_debug_print(MOD_MMI, "iotlock_sms: %s server port=%d", __FUNCTION__, tcpport);
				}
				break;
			case 0x03: //server tcp port
				{
					Byte *p = para;
					Byte value = *p;
					lock_debug_print(MOD_MMI, "iotlock_sms: %s value =%d", __FUNCTION__, value);
					iot_set_bkup_nv_third_byte(value);
					g_lock_cntx.uart1_state = value;
					if(value==0)
					{
						iot_uart1_power_ctrl(0);
						kal_prompt_trace(MOD_ENG, "iotlock_sms: %s() line:%d: close uart1", __FUNCTION__,__LINE__);

					}
					else if(value==1)
					{
						iot_uart1_power_ctrl(1);
						kal_prompt_trace(MOD_ENG, "iotlock_sms: %s() line:%d: open uart1", __FUNCTION__,__LINE__);
					}
					else
					{
						kal_prompt_trace(MOD_ENG, "iotlock_sms: %s() line:%d: uart1 error state", __FUNCTION__,__LINE__);
					}
					lock_debug_print(MOD_MMI, "iotlock_sms: %s() line:%d uart1_state = %d", __FUNCTION__,__LINE__,
						g_lock_cntx.uart1_state);
				}
				break;
			default:
				lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s NO THIS COMMAND", __FUNCTION__);
				break;
		}
		ptr =ptr + 2 + paralen;
	}
	WriteRecordIot(NVRAM_EF_IOT_LOCK_LID, 1, &info, NVRAM_EF_IOT_LOCK_SIZE);

	send_term_com_ans(g_sms_serial_num, SMS_SET_DEVICE_PARAM_TYPE, 0);
	if(domain_update()){
		reg_counts = 0;
		auth_counts = 0;
		lock_debug_print(MOD_MMI, "iotlock_sms: %s create TYPE_NEW_LOCK_SOC", __FUNCTION__);
		lock_socket_create(TYPE_NEW_LOCK_SOC);
	}
	
}

void sms_parse_set_ctr_cmd(Byte *buffer)
{
	Byte *ptr = buffer+SMS_MSG_HEADER_LEN;
	Byte flag = *ptr;
	Byte cmd = *(ptr+1);
	Byte ret = 0;

	 lock_debug_print(MOD_MMI, "iotlock_sms: %s, flag=%d, cmd=%d", __FUNCTION__, flag, cmd);
	 if(flag == 0x1)
	{
		if(cmd == 0) {
			ret = 0;
		} else if(cmd == 1) {
			ret = 0;
			#ifdef __IOT_LOCK_BEEPSPK__
			open_beep_ms(150, 6);
			#else
			iot_play_tone_with_filepath(IOT_TONE_WARN);
			#endif
		} else {
			ret = 0;
			g_lock_cntx.speaker_play_counts = cmd;
			play_tone();
		}
		lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s OPEN TONE", __FUNCTION__);

	} 
	else if(flag == 0x2) {
		if(cmd == 0) {
			ret = 0;
		} else if(cmd == 1) {
			ret = 0;
			iot_led_ctrl(2,200,3);
		} else {
			ret = 0;
			g_lock_cntx.led_flash_counts = cmd;
			gprs_and_sms_find_bike_led_flash();
		}
		lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s OPEN FLASH", __FUNCTION__);
	} 
	else if(flag == 0x6) {
		if(cmd == 0) {
			ret = 0;
		} else if(cmd == 1) {
			ret = 0;
			iot_led_speak_warning_callback();
		} else {
			ret = 0;
			g_lock_cntx.speaker_led_flash_counts = cmd;
			gprs_and_sms_find_bike_speaker_led_flash();
		}
		lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s OPEN TONE and FLASH", __FUNCTION__);
	} 
	 else {
		ret = 3;
	}

	send_term_com_ans(g_sms_serial_num, SMS_SET_CTR_CMD_TYPE, ret);
}

void play_tone(void)
{
	if(g_lock_cntx.speaker_play_counts>0)
	{
		iot_play_tone_with_filepath_ext(IOT_TONE_WARN,SRV_PROF_RING_TYPE_ONCE,play_tone);
		g_lock_cntx.speaker_play_counts--;
	}
	
}

Byte HexChar2Number(char c)
{
	if('0' <= c && c <= '9')
	{
		return (Byte)(c - '0');
	}
	if('A' <= c && c <= 'F')
	{
		return (Byte)(c - 'A' + 10);
	}
	if('a' <= c && c <= 'f')
	{
		return (Byte)(c - 'a' + 10);
	}
	lock_sms_dbg_print(MOD_ENG,"iotlock_sms error char: %c\n", c);
	return 0;
}
Byte String2Hex(char* pStr)
{
	Byte bRet = HexChar2Number(pStr[0]), bLow = HexChar2Number(pStr[1]);
	bRet <<= 4;
	bRet |= bLow;
	return bRet;
}

void lock_string_to_hex(Byte *buff_int,Byte *buff_out,Byte *buff_out_len)
{
	Byte *ptr = buff_int;
	char temp_buf[128] = {0};
	int i =0;
	Byte len = strlen(ptr) / 2;
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms:%s() line:%d strlen(ptr) = %d len = %d",__func__,__LINE__,strlen(ptr),len);
	for( i = 0; i < len; ++i)
	{
		temp_buf[i] = String2Hex(ptr + i * 2);
		lock_sms_dbg_print(MOD_ENG, "iotlock_sms:%s() line:%d %02x",__func__,__LINE__,temp_buf[i]);
	}

	memcpy(buff_out,temp_buf,len);
	*buff_out_len = len;
	
}

extern mmi_sms_context_struct g_sms_cntx;
void lock_msg_entry_new_msg_ind(void)
{
	kal_uint8 content[SMS_MAX_CONTENT_LEN] = {0};
	kal_uint8 recv_content[SMS_MAX_CONTENT_LEN] = {0}; 
	const char  *temp_content;
	const char *ptr = content + 2;
	kal_uint32 type;
	MYTIME time;
	kal_uint8 number[SRV_SMS_MAX_ADDR_LEN];
	kal_uint8 recv_buff[SMS_MAX_CONTENT_LEN] = {0}; 
	int i;
	kal_uint8 recv_buff_len = 0;
	//for  test
	unsigned char  *temp_sms_data ="gpVo57EMQb36QQ+7eHs0XZIE6nTN6tJyHIcyJAsGVmSKK7JFxqahlY0tcjg9dP2VC67BQTJOUfWe+RJTbfYEWXmY/vwnx52SBbGZl0kgtuY=";
	U8 buffer[SMS_MAX_CONTENT_LEN+1];
	U8 de_buffer[SMS_MAX_CONTENT_LEN+1];
	kal_uint8 de_buffer_len = 0;
	int buflen = 0;
	int content_len = 0;

	
	memset(lock_number,0,sizeof(lock_number));
	memset(bkss_content,0,sizeof(bkss_content));
	srv_sms_delete_msg(g_sms_cntx.ind_msg_index,NULL,NULL);
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms:%s() line:%d",__func__,__LINE__);
	srv_sms_get_msg_address(g_sms_cntx.ind_msg_index, (S8*)lock_number);
	temp_content = srv_sms_get_msg_content(g_sms_cntx.ind_msg_index);
	
	srv_sms_get_msg_timestamp(g_sms_cntx.ind_msg_index, (MYTIME *) &time);

	lock_sms_dbg_print(MOD_ENG, "iotlock_sms :%s() line:%d; %d.%d.%d.%d.%d.%d.%d",__func__,__LINE__,
												time.nYear,time.nMonth,time.nDay,time.nHour,time.nMin,time.nSec);

	lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s() line:%d temp_content = %s",__func__,__LINE__,temp_content);
	mmi_ucs2_to_asc((CHAR *)recv_content,(CHAR *)temp_content);
	mmi_ucs2_to_asc((CHAR *)number,(CHAR *)lock_number);
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s() line:%d recv_content = %s",__func__,__LINE__,recv_content);
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s() line:%d len=%d",__func__,__LINE__,strlen(recv_content));
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms :%s() line:%d number = %s",__func__,__LINE__,number);

	//for test sms
	//memcpy(content, temp_sms_data, strlen(temp_sms_data));
#ifdef __LOCK_ADD_AES_ENCRYPT__
	content_len = iotlock_base64_decode(content,recv_content,strlen(recv_content),120);
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s() line:%d decode_content = %s",__func__,__LINE__,content);
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s() line:%d decode_content_len = %d",__func__,__LINE__,content_len);
	init_key();
	memset(buffer,0,sizeof(buffer));
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms %s() %d aes128_key =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__
		,lock_aes128_key[0],lock_aes128_key[1],lock_aes128_key[2],lock_aes128_key[3],lock_aes128_key[4],lock_aes128_key[5],lock_aes128_key[6],lock_aes128_key[7],
		lock_aes128_key[8],lock_aes128_key[9],lock_aes128_key[10],lock_aes128_key[11],lock_aes128_key[12],lock_aes128_key[13],lock_aes128_key[14],lock_aes128_key[15]);	


	sms_aes128_ecb_decrypt(content, content_len, de_buffer,&de_buffer_len);
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s() line:%d strlen(de_buffer) = %d de_buffer_len = %d",__func__,__LINE__,strlen(de_buffer),de_buffer_len);
	for(i=0;i<de_buffer_len;i++)
	{
		lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s() line:%d decrypt_buffer[%d]=%x\r\n",__func__,__LINE__,i,*(de_buffer+i));
	}
	lock_string_to_hex(de_buffer,recv_buff,&recv_buff_len);
#else
	lock_string_to_hex(recv_content,recv_buff,&recv_buff_len);
#endif
	
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms:%s() line:%d strlen(recv_buff) = %d recv_buff_len = %d",__func__,__LINE__,strlen(recv_buff),recv_buff_len);
	
	handle_sms_recv_cmd(recv_buff,recv_buff_len);
}


void handle_sms_recv_cmd(Byte *buf,Byte len)
{

	U32 i;
	U8 buffer[SMS_MAX_CONTENT_LEN+1];
	char * revbuf = buf;
	const Byte *ptr = (Byte*)revbuf;
	Byte cmd_type = 0;
	U8 temp_buffer[SMS_MAX_CONTENT_LEN+1];
	U8 temp_buffer_len = 0;

	lock_sms_dbg_print(MOD_ENG, "iotlock_sms:%s() line:%d len = %d",__func__,__LINE__,len);
	for(i=0;i<len;i++)
	{
		lock_sms_dbg_print(MOD_ENG,"iotlock_sms:parse_sms_recv_cmd buf[%d]=%x\r\n",i,*(revbuf+i));
	}
	
	if(check_sms_cmd(revbuf))
	{
		cmd_type = *(revbuf+3); //ÿ»ñÈ¡ÀàÐÍ
		lock_sms_dbg_print(MOD_ENG, "iotlock_sms:%s() line:%d cmd_type = %d",__func__,__LINE__,cmd_type);

		switch(cmd_type) {
		case SMS_OPEN_LOCK_TYPE: 
			kal_prompt_trace(MOD_ENG, "iotlock_sms: SMS_OPEN_LOCK_TYPE");
			parse_open_lock_cmd(revbuf);
			break;
		case SMS_SET_DEVICE_PARAM_TYPE:
			kal_prompt_trace(MOD_ENG, "iotlock_sms: SMS_SET_DEVICE_PARAM_TYPE");
			parse_set_device_param_cmd(revbuf);
			break;
		case SMS_SET_DEVICE_STATE_TYPE:
			{
				Byte ret = 0x0;
				kal_prompt_trace(MOD_ENG, "iotlock_sms: SMS_SET_DEVICE_STATE_TYPE");
				ret = parse_set_device_state_cmd(revbuf+SMS_MSG_HEADER_LEN,g_sms_serial_num,SMS_SET_DEVICE_STATE_TYPE);
				send_term_com_ans(g_sms_serial_num, SMS_SET_DEVICE_STATE_TYPE, ret);
			}
			
			break;
		case SMS_SET_CTR_CMD_TYPE:
			kal_prompt_trace(MOD_ENG, "iotlock_sms: SMS_SET_CTR_CMD_TYPE");
			sms_parse_set_ctr_cmd(revbuf);
			break;
		case SMS_UPLOAD_FOTA_PACK_TYPE:
			kal_prompt_trace(MOD_ENG, "iotlock_sms: SMS_UPLOAD_FOTA_PACK_TYPE");
			send_term_com_ans(g_sms_serial_num, SMS_UPLOAD_FOTA_PACK_TYPE, 0);
			StartTimer(LOCK_FOTA_START_TIMER, 10 * 1000, adups_net_start_get_new_version);
			break;
		default:
			lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s NO THIS COMMAND", __FUNCTION__);
			break;
		}


	}
		
}
#ifdef __LOCK_ADD_AES_ENCRYPT__   
void sms_aes128_ecb_encrypt(kal_uint8* input, kal_uint8 int_len, kal_uint8* output)
{
	int i;
	int len;
	len = int_len/16 +1;
	for(i=0;i<len;i++)
	{
		iotlock_AES128_ECB_encrypt(input+16*i, lock_aes128_key,output+16*i);
		
	}
}

void sms_aes128_ecb_decrypt(kal_uint8* input, kal_uint8 in_len, kal_uint8* output,kal_uint8 *out_len)
{
	int i;
	int len;
	len = in_len/16 +1;
	for(i=0;i<len;i++)
	{
		iotlock_AES128_ECB_decrypt(input+16*i, lock_aes128_key,output+16*i);
	}
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s() line:%d output = %s",__func__,__LINE__,output);
	*out_len = strlen(output);
	lock_sms_dbg_print(MOD_ENG, "iotlock_sms: %s() line:%d strlen(output) = %d",__func__,__LINE__,strlen(output));
}
void init_key(void)  //³õÊ¼»¯ÃØÔ¿
{
	S16 error;
	U8 i = 0;

	nvram_ef_bel_bkss_struct info;

	memset(&info, 0, NVRAM_EF_BLE_BKSS_SIZE);
	ReadRecordIot(NVRAM_EF_BLE_BKSS_LID, 1, (void*)&info, NVRAM_EF_BLE_BKSS_SIZE);
	memset(lock_aes128_key,0,BT_KEY_MAX_LEN);
	for(i=0;i<BT_KEY_MAX_LEN;i++)
	{
		lock_aes128_key[i] = info.key[i];
	}
	if(error == NVRAM_READ_SUCCESS)
	{
		; 
	}
}


void message_add_aes_encrypt_ecb(Byte*buffer_in,kal_uint16 buff_in_len,Byte*buffer_out,kal_uint32*buff_out_len)
{
	init_key();
	lock_debug_print(MOD_MMI, "iotlock:  %s line:%d buff_in_len=%d", __FUNCTION__,__LINE__, buff_in_len);
	//aes ¼ÓÃÜ
	AES128_EncryptECB(MODE_PKCS7_PADDING,buffer_in,buff_in_len ,buffer_out,buff_out_len,lock_aes128_key);
	lock_debug_print(MOD_MMI, "iotlock:  %s line:%d buff_out_len=%d", __FUNCTION__, __LINE__,*buff_out_len);
}

void message_add_aes_decrypt_ecb(Byte*buffer_in,kal_uint16 buff_in_len,Byte*buffer_out,kal_uint32 *buff_out_len)
{
	init_key();
	lock_debug_print(MOD_MMI, "iotlock:  %s line:%d buff_in_len=%d", __FUNCTION__,__LINE__, buff_in_len);
	//aes ½âÃÜ
	AES128_DecryptECB(MODE_PKCS7_PADDING,buffer_in,buff_in_len ,buffer_out,buff_out_len,lock_aes128_key);
	lock_debug_print(MOD_MMI, "iotlock:  %s line:%d buff_out_len=%d", __FUNCTION__, __LINE__,*buff_out_len);
}

kal_bool check_message_is_aes_encrypt(MSG_HEADER *msg_header)
{
	Word header_attr = msg_header->attr;
	msg_header->attr = ((header_attr%256)<<8) + (header_attr>>8);
	lock_debug_print(MOD_MMI, "iotlock:  %s line:%d msg_header.attr=%x", __FUNCTION__, __LINE__,msg_header->attr);

	if((msg_header->attr&0x0400) == 0x0400)
	{
		lock_debug_print(MOD_MMI, "iotlock:  %s line:%d message have aes encrypt", __FUNCTION__, __LINE__);
		return KAL_TRUE;

	}
	return KAL_FALSE;
}


void split_aes_decrypt_buff_to_msg(Byte *buf_in, kal_uint16 buf_len, MESSAGE *msg)
{
	Byte *ptr = buf_in + 1; //remove start flag
	Byte msg_body[1024] = {0};
	kal_uint16 out_len = 0;

	MESSAGE aes_msg;
	if((*ptr & 0x20) == 1) {
		//split pack, not support
		aes_msg.headerlen = 16;
	} else {
		aes_msg.headerlen = 12;
	}
	msg->headerlen = aes_msg.headerlen;

	lock_debug_print(MOD_MMI, "iotlock: %s headerlen=%d", __FUNCTION__, aes_msg.headerlen);
	aes_msg.bodylen = buf_len - (3 + aes_msg.headerlen);
	if(aes_msg.bodylen > 0) {
		aes_msg.body = (Byte*)med_alloc_ext_mem(aes_msg.bodylen);
		memcpy(aes_msg.body, (ptr + aes_msg.headerlen), aes_msg.bodylen);
	} else {
		aes_msg.body = NULL;
	}
	
	dump_buffer(aes_msg.body, aes_msg.bodylen);//encrypt buff

	out_len = sizeof(msg_body);
	lock_debug_print(MOD_MMI, "iotlock: %s msg->bodylen=%d", __FUNCTION__, msg->bodylen);
	message_add_aes_decrypt_ecb(aes_msg.body, aes_msg.bodylen, msg_body, (kal_uint32*)&out_len);
	lock_debug_print(MOD_MMI, "iotlock:  %s line: %d out_len=%d", __FUNCTION__, __LINE__,out_len);
	msg->bodylen = out_len;
	memcpy(msg->body,msg_body, msg->bodylen);
	lock_debug_print(MOD_MMI, "iotlock:  %s line: %d msg->bodylen=%d", __FUNCTION__, __LINE__,msg->bodylen);
	dump_buffer(msg->body, msg->bodylen);//decrypt buff
	
	msg->header = (Byte*)med_alloc_ext_mem(msg->headerlen);
	memcpy(msg->header, ptr, msg->headerlen);
	lock_debug_print(MOD_MMI, "iotlock:  %s line:%d end aes buff to msg", __FUNCTION__, __LINE__);
	
}

void lock_msg_body_add_aes_encrypt_to_buff(Byte *buf_in, kal_uint16 in_buf_len, Byte *buf_out, kal_uint32* out_buf_len)
{


	Byte msg_body[1024] = {0};
	kal_uint16 out_len = 0;
	Byte checkcode;
	MESSAGE no_aes_msg;
	MESSAGE aes_msg;
	Word number;
	Byte check_code;
	Byte header[16] = {0};

	kal_uint16 len;
	Byte unes_buff[1024] = {0};
	len = message_unescape(buf_in, unes_buff, in_buf_len);
	lock_debug_print(MOD_MMI, "iotlock:  %s line: %d in_buf_len=%d", __FUNCTION__, __LINE__,in_buf_len);
	lock_debug_print(MOD_MMI, "iotlock:  %s line: %d len=%d", __FUNCTION__, __LINE__,len);
	split_buffer_to_msg(unes_buff, len, &no_aes_msg, &checkcode);
	{
		MSG_HEADER *msg_header = (MSG_HEADER*) no_aes_msg.header;
		out_len = sizeof(msg_body);
		dump_out_buffer(no_aes_msg.body, no_aes_msg.bodylen);
		lock_debug_print(MOD_MMI, "iotlock:  %s line: %d aes_body_len=%d", __FUNCTION__, __LINE__,no_aes_msg.bodylen);
		message_add_aes_encrypt_ecb(no_aes_msg.body, no_aes_msg.bodylen, msg_body, (kal_uint32*)&out_len);
		lock_debug_print(MOD_MMI, "iotlock:  %s line: %d out_len=%d", __FUNCTION__, __LINE__,out_len);
		aes_msg.bodylen = out_len;
		aes_msg.body = msg_body;
		lock_debug_print(MOD_MMI, "iotlock:  %s line: %d msg.bodylen=%d", __FUNCTION__, __LINE__,aes_msg.bodylen);
		dump_out_buffer(aes_msg.body, aes_msg.bodylen);

		convert_header_from_big_endian(msg_header);
		kal_prompt_trace(MOD_MMI, "iotlock: %s msg_id = %x", __FUNCTION__, msg_header->id);
		number = msg_header->serial_number;
		aes_msg.headerlen = get_msg_hearder(header, msg_header->id, SECRET_AES, aes_msg.bodylen, &number);

		aes_msg.header = header;
		check_code = compute_check_code(aes_msg);

		*out_buf_len = message_escape_and_addflag(buf_out, aes_msg, check_code);
		lock_debug_print(MOD_MMI, "iotlock:  %s line: %d *out_buf_len=%d", __FUNCTION__, __LINE__,*out_buf_len);
	}

	free_msg(no_aes_msg);
}

#endif


#endif

/**********************************sms end*********************/
