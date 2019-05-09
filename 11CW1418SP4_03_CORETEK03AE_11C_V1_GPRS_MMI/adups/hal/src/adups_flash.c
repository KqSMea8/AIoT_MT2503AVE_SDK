#include "adups_typedef.h"
#include "adups_define.h"
#include "kal_general_types.h"
#include "custom_MemoryDevice.h"


#ifdef __NOR_SUPPORT_RAW_DISK__
#ifndef WIN32
adups_extern kal_int32 Adups_write_buffer(kal_uint8* data_ptr, kal_uint32 len);
adups_extern kal_uint32 Adups_erase_disk(void);
adups_extern void Adups_delete_header(void);
adups_extern kal_int32  Adups_read_buffer(kal_uint32 addr, kal_uint8* data_ptr, kal_uint32 len);
#else
kal_int32 Adups_write_buffer(kal_uint8* data_ptr, kal_uint32 len){return 0;}
kal_uint32 Adups_erase_disk(void){return 	0;}
void Adups_delete_header(void){}
kal_int32  Adups_read_buffer(kal_uint32 addr, kal_uint8* data_ptr, kal_uint32 len){return 0;}
#endif
#endif

ADUPS_BOOL AdupsUseFlash(void)
{
	#ifdef __NOR_SUPPORT_RAW_DISK__
	    return ADUPS_TRUE;
	#else
	   return ADUPS_FALSE;
	#endif
}

adups_uint32 AdupsGetBlockSize(void)
{
	return ADUPS_BLOCK_SIZE;
}
adups_uint32 AdupsGetFlashDiskSize(void)
{
	adups_uint32 block_size;

	block_size=AdupsGetBlockSize();
	
#ifdef __NOR_SUPPORT_RAW_DISK__
 #if (NOR_BOOTING_NOR_DISK_NUM > 1)
 	    return NOR_BOOTING_NOR_DISK1_SIZE-block_size;
 #else
	    return NOR_BOOTING_NOR_DISK0_SIZE-block_size;
 #endif
	#else
	   return 0;
	#endif
}



 adups_int32 Adups_write_flash(adups_uint8* data_ptr, adups_uint32 len)
{
	adups_int32 result;
	#ifdef __NOR_SUPPORT_RAW_DISK__
	result=Adups_write_buffer(data_ptr,len);
	return result;
	#endif	
}

 adups_int32 Adups_read_flash(adups_uint32 addr,adups_uint8* data_ptr, adups_uint32 len)
{
	adups_int32 result;
	#ifdef __NOR_SUPPORT_RAW_DISK__	
   	result = Adups_read_buffer(addr, data_ptr,len); 
	return result;
	#endif
}

 adups_uint32 Adups_init_flash(void)
 {
 	#ifdef __NOR_SUPPORT_RAW_DISK__
 	return Adups_erase_disk();
	#else
	   return 0;		
	#endif
 }

void Adups_erase_delata(void)
{
 	#ifdef __NOR_SUPPORT_RAW_DISK__
	Adups_delete_header();
	#endif
}
 
	








