#ifndef _ADUPS_DEVICE_H_
#define _ADUPS_DEVICE_H_

adups_extern  adups_int32 adups_sim_ready(void);

adups_extern adups_int32 adups_get_device_mid(adups_char* mid);

adups_extern adups_char* adups_get_device_version(void);

adups_extern adups_char* adups_get_device_oem(void);

adups_extern adups_char* adups_get_device_model(void);

adups_extern adups_char* adups_get_device_product_id(void);

adups_extern adups_char* adups_get_device_product_sec(void);

adups_extern adups_char* adups_get_device_type(void);

adups_extern adups_char* adups_get_device_platform(void);

adups_extern adups_char* adups_get_device_network_type(void);

adups_extern void adups_get_device_time(Adups_time_struct* time);
adups_extern adups_uint32 adups_get_utc_time_sec(Adups_time_struct* time);


adups_extern adups_uint8 adups_get_device_battery_level(void);

adups_extern void adups_device_start_reboot(void);

adups_extern void* adups_memalloc(adups_int32 size);
adups_extern void adups_memfree(void *p);
adups_extern void* adups_memalloc_ext(adups_int32 size);
adups_extern void adups_memfree_ext(void *p);

adups_extern void adups_get_device_data_point1(adups_char** name,adups_char** value);
adups_extern void adups_get_device_data_point2(adups_char* name,adups_char* value);
adups_extern void adups_get_device_data_point3(adups_char* name,adups_char* value);
adups_extern void adups_get_device_data_point4(adups_char* name,adups_char* value);
adups_extern void adups_get_device_data_point5(adups_char* name,adups_char* value);


#ifdef __ADUPS_FOTA_GPS__
adups_extern adups_char* adups_get_gps_version(void);
adups_extern adups_char* adups_get_device_gps_model(void);
#endif
adups_extern void adups_process_status_cb(adups_int32 status,adups_uint8 fota_status);

//
adups_extern void adups_em_set_mode(ADUPS_BOOL mode);
adups_extern ADUPS_BOOL adups_em_mode(void);
adups_extern void adups_em_set_mid(adups_char* mid);
adups_extern void adups_em_get_mid(adups_char* mid);

#endif
