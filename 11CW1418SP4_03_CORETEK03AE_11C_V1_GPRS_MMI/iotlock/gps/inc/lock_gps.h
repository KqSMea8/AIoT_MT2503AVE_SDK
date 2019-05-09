#ifndef __LOCK_GPS_H__
#define __LOCK_GPS_H__

#include "nbr_public_struct.h"
#include "mdi_gps.h"
#include "lock_data.h"

#define GPS_NONE_OK   0x0
#define GPS_GNGGA_OK  0x1
#define GPS_GNRMC_OK  0x2
#define GPS_GNVTG_OK  0x4
#define GPS_GPGSV_OK  0x5
#define GPS_ALL_OK    (GPS_GNGGA_OK|GPS_GNRMC_OK|GPS_GNVTG_OK|GPS_GPGSV_OK)

extern applib_time_struct g_gps_locate_time;

void gps_open();
kal_bool gps_open_test(void);
void gps_sleep();
void start_lbs_and_off_gps();
void gps_callback(mdi_gps_parser_info_enum type, void *buffer, U32 length);
void gps_callback_ext(mdi_gps_parser_info_enum type, void *buffer, U32 length);
kal_uint8 get_next(char *buffer, char*out, kal_uint8 index);
void copy_gps_data(GPS_DATA gps_data);
void copy_gps_counts(Byte  count);
void get_loc_msg_from_gps(char* buffer);
MDI_RESULT gps_open_ext();
kal_uint8 get_gps_count();
kal_uint8 get_gps_state();
kal_uint8 get_gps_duration_time();
void open_gps_and_get_loc(DWord gap, LOC_REPORT_TYPE type);
void open_timer_gps_and_get_loc();
void get_curr_time(BCD *datetime, kal_uint32 utc_sec);
void spconser_sal_stub_cell_reg_req(void);
void copy_nbr_data(Byte count, gas_nbr_cell_info_struct cell_info );
void spconser_sal_cell_reg_rsp(l4c_nbr_cell_info_ind_struct *msg_ptr);
void get_loc();
void report_loc_timer();
void report_loc(kal_bool report_amend);
void send_location_and_state(order_info *orderInfo, COMM_LOC *comm_loc, ATTACH_LOC *att_loc);
void gen_location_and_state_info(Byte *body, kal_uint16 *body_len, order_info *orderInfo, COMM_LOC *comm_loc, ATTACH_LOC *att_loc);
void gen_loc_request(kal_char* request);
void get_loc_from_json(kal_char *buffer);
void send_lbs_request(kal_int8 socket_id);
void lock_warn_report_loc(LOC_WARN_REPORT_TYPE type);
void unlock_time_over_valid_time_gap_report_loc( UNLOCK_TYPE type, Word ans_serial);
void iot_lock_amend_locate();
void delay_report_loc(order_info *info);
void loc_report_delay();
void has_same_order_num_report_loc( UNLOCK_TYPE type, Word ans_serial);
void ata_gps_open(void);
void ata_gps_sleep(void);
void iot_ata_gps_open(void);
void iot_ata_gps_sleep(void);

#endif