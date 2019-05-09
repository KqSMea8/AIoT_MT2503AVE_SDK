#ifndef _ADUPS_FILE_H_
#define _ADUPS_FILE_H_

adups_extern void adups_get_fota_drv_size(adups_uint64* total_size,adups_uint64* free_size);
adups_extern adups_int32 adups_Open_File(adups_wchar *file_path,adups_uint32 flag);
adups_extern adups_int32 adups_Seek_File(adups_int32 file_handle,adups_uint32 offset,adups_int32 origin);
adups_extern adups_int32 adups_Remove_File(adups_wchar *file_path);
adups_extern adups_int32 adups_Write_File(adups_int32 file_handle,adups_char *inBuff,adups_uint32 len,adups_uint32 *write_len);
adups_extern adups_int32 adups_Read_File(adups_int32 file_handle,adups_char *outBuff,adups_uint32 len,adups_uint32 *write_len);
adups_extern void adups_Close_File(adups_int32 file_handle);

adups_extern adups_wchar* adups_get_delta_file_path(void);
adups_extern adups_wchar* adups_get_delta_file_path12(void);
adups_extern adups_wchar* adups_get_delta_file_path21(void);
adups_extern adups_wchar* adups_get_version_file_path(void);
adups_extern adups_wchar* adups_get_delta_id_path(void);
adups_extern adups_int32 adups_Get_File_Size(adups_int32 file_handle,adups_uint32 *file_size);
adups_extern adups_wchar* adups_get_login_file_path(void);
adups_extern adups_wchar* adups_get_backup_file_path(void);


#endif