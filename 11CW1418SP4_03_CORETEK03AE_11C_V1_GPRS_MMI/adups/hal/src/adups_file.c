#include "adups_typedef.h"
#ifdef __EXT_BOOTLOADER__
#include "adups_bl_main.h"
#else
#include "adups_debug.h"
#endif
#include "fs_type.h"
#include "fs_func.h"
#include "fs_errcode.h"

#define ADUPS_ROOT_FOLDER	L"Z:\\@fota_adups\\"

#define ADUPS_DELTAFILE_PATH  L"Z:\\@fota_adups\\adupsfota.delta"
#define ADUPS_DELTAFILE_PATH12 L"Z:\\@fota_adups\\adupsfota12.delta"
#define ADUPS_DELTAFILE_PATH21 L"Z:\\@fota_adups\\adupsfota21.delta"

#define ADUPSVERSION L"Z:\\@fota_adups\\adupsUpState.txt"
#define ADUPSDETAID L"Z:\\@fota_adups\\adupsDetaId.txt"
#define ADUPS_BACKUP_PATH  L"Z:\\@fota_adups\\backup.txt"
#define ADUPS_LOGIN_PATH L"Z:\\@fota_adups\\login.txt"



void adups_get_fota_drv_size(adups_uint64* total_size,adups_uint64* free_size)
{
	#ifndef __EXT_BOOTLOADER__
	srv_fmgr_drv_get_size('Z',total_size,free_size);
	#endif
}

adups_wchar* adups_get_delta_file_path(void)
{
	return ADUPS_DELTAFILE_PATH;
}
adups_wchar* adups_get_delta_file_path12(void)
{
	return ADUPS_DELTAFILE_PATH12;
}
adups_wchar* adups_get_delta_file_path21(void)
{
	return ADUPS_DELTAFILE_PATH21;
}
adups_wchar* adups_get_backup_file_path(void)
{
	return ADUPS_BACKUP_PATH;
}
adups_wchar* adups_get_version_file_path(void)
{
	return ADUPSVERSION;
}

adups_wchar* adups_get_login_file_path(void)
{
	return ADUPS_LOGIN_PATH;
}


adups_wchar* adups_get_delta_id_path(void)
{
	return ADUPSDETAID;
}

adups_wchar* adups_get_adups_root_path(void)
{
	return ADUPS_ROOT_FOLDER;
}

ADUPS_BOOL adups_fs_pathorfile_exist(const adups_wchar *path, ADUPS_BOOL pathorfile)
{
    adups_int32 handle=-1;
	adups_uint32 nFlag=0;
#if !defined(__EXT_BOOTLOADER__)||defined(__FUNET_ENABLE__)

	if(pathorfile)
    	nFlag = FS_OPEN_DIR | FS_READ_ONLY;
	else
		nFlag = FS_READ_ONLY;
	
	handle = FS_Open(path, nFlag);	
    if (handle < 0)
        return ADUPS_FALSE;

    FS_Close(handle);
	
#endif
    return ADUPS_TRUE;
}

adups_int32 adups_Open_File(adups_wchar *file_path,adups_uint32 flag)
{
	
	adups_int32 result;
	adups_uint32 nFlag;
#if !defined(__EXT_BOOTLOADER__)||defined(__FUNET_ENABLE__)
	if((ADUPS_READONLY & flag) == ADUPS_READONLY)
	{
		nFlag = FS_READ_ONLY;
	}
	else if(((ADUPS_READWRITE & flag) == ADUPS_READWRITE) || ((ADUPS_APPEND&flag) == ADUPS_APPEND) || 
			((ADUPS_TRUNCATE & flag) == ADUPS_TRUNCATE))
	{
		nFlag = FS_READ_WRITE;
		if((ADUPS_TRUNCATE & flag) == ADUPS_TRUNCATE)
			nFlag |= FS_CREATE_ALWAYS;
		else if((ADUPS_CREATE & flag) == ADUPS_CREATE)
			nFlag |= FS_CREATE;
	}
	else { 
		#ifdef __EXT_BOOTLOADER__
		  adups_bl_debug_print(NULL,"adups_Open_File: bad flag, flag = %d", flag);	
		#else
		adups_DebugPrint("adups_Open_File: bad flag, flag = %d", flag);
		#endif
		return NULL;
	}

	#ifndef __EXT_BOOTLOADER__
	if(!adups_fs_pathorfile_exist(adups_get_adups_root_path(), 1))
	{
		adups_DebugPrint("adups_Open_File: create root folder");
		result = FS_CreateDir(adups_get_adups_root_path());
		if(result < 0)
		{
			adups_DebugPrint("adups_Open_File: create fail!!!");
			return result;
		}
		else
			FS_Close(result);
	}
	#endif
	#ifdef __EXT_BOOTLOADER__
	adups_bl_debug_print(NULL,"adups_Open_File: nFlag = %d\r\n", nFlag);
	#else
	adups_DebugPrint("adups_Open_File: nFlag = %d", nFlag);
	#endif
	result=FS_Open(file_path,nFlag);

	#ifdef __EXT_BOOTLOADER__
	adups_bl_debug_print(NULL,"adups_Open_File: result = %d\r\n", result);
	#else
	adups_DebugPrint("adups_Open_File: result = %d", result);
	#endif
#endif
	return result;
		
}

adups_int32 adups_Seek_File(adups_int32 file_handle,adups_uint32 offset,adups_int32 origin)
{
	
	adups_int32 result;	
	adups_int32 whence;	
#if !defined(__EXT_BOOTLOADER__)||defined(__FUNET_ENABLE__)

	switch(origin)
	{
		case ADUPS_SEEK_START:
			whence = FS_FILE_BEGIN;
			break;
		case ADUPS_SEEK_END:
			whence = FS_FILE_END;
			break;
		case ADUPS_SEEK_CURRENT:
			whence = FS_FILE_CURRENT;
			break;
		default:
			return E_ADUPS_FAILURE;
	}
	result=FS_Seek(file_handle,offset,whence);
#endif
	#ifdef __EXT_BOOTLOADER__
	adups_bl_debug_print(NULL,"adups_Seek_File: file_handle=%d,result = %d\r\n",file_handle,result);
	#endif
	return ((result >= 0)? E_ADUPS_SUCCESS: E_ADUPS_FAILURE);	
	
}


adups_int32 adups_Remove_File(adups_wchar *file_path)
{
	adups_int32 ret=0;

#if !defined(__EXT_BOOTLOADER__)||defined(__FUNET_ENABLE__)
	ret=FS_Delete(file_path);
#endif
#ifdef __EXT_BOOTLOADER__
	adups_bl_debug_print(NULL,"adups_Remove_File file delete ret=%d",ret);
#else
	adups_DebugPrint("adups_Remove_File file delete ret=%d",ret);
#endif
	if(ret == FS_NO_ERROR)
		return E_ADUPS_SUCCESS;
	else 
		return E_ADUPS_FAILURE;
	
	
}

adups_int32 adups_Write_File(adups_int32 file_handle,adups_char *inBuff,adups_uint32 len,adups_uint32 *write_len)
{
	adups_int32 ret = -1;
#if !defined(__EXT_BOOTLOADER__)||defined(__FUNET_ENABLE__)

	#ifdef __EXT_BOOTLOADER__
		adups_bl_debug_print(NULL,"adups_Write_File =inBuff  %s" , inBuff);	
	#else
		adups_DebugPrint(  "adups_Write_File =inBuff  %s" , inBuff);
	#endif
	
	ret = FS_Write(file_handle,inBuff,len,write_len);

	#ifdef __EXT_BOOTLOADER__
		adups_bl_debug_print(NULL,"adups_Write_File ret=%d" , ret);	
	#else
		adups_DebugPrint(  "adups_Write_File ret=%d" , ret);
	#endif
#endif	
	return ret;
}

adups_int32 adups_Read_File(adups_int32 file_handle,adups_char *outBuff,adups_uint32 len,adups_uint32 *write_len)
{
	adups_int32 ret = -1;

#if !defined(__EXT_BOOTLOADER__)||defined(__FUNET_ENABLE__)
	
	ret = FS_Read(file_handle,outBuff,len,write_len);

	#ifdef __EXT_BOOTLOADER__
		adups_bl_debug_print(NULL,"adups_Read_File ret=%d", ret);	
	#else
		adups_DebugPrint(  "adups_Read_File ret=%d" , ret);
	#endif
#endif	
	return ret;
}

adups_int32 adups_Get_File_Size(adups_int32 file_handle,adups_uint32 *file_size)
{
	adups_int32 ret = -1;	

#if !defined(__EXT_BOOTLOADER__)||defined(__FUNET_ENABLE__)
	
       ret = FS_GetFileSize(file_handle, file_size);
#endif
	return ret;
}

void adups_Close_File(adups_int32 file_handle)
{
#if !defined(__EXT_BOOTLOADER__)||defined(__FUNET_ENABLE__)

	FS_Close(file_handle);
	#ifdef __EXT_BOOTLOADER__
		adups_bl_debug_print(NULL,"adups_Close_File file_handle=%d\r\n", file_handle);	
	#endif
#endif
	
}

