#ifndef _ADUPS_MAIN_H
#define _ADUPS_MAIN_H
 

#ifdef __SERIAL_FLASH_EN__
#define NOR_PAGE_SIZE             (512)         //512 bytes
#else
#define NOR_PAGE_SIZE             (2048)        //2048 bytes
#endif

#define __MULTI_PACKAGE__


typedef struct multi_head_info{
	adups_uint32 multi_bin_offset;
	adups_uint32 multi_bin_address;
	adups_uint8 multi_bin_method;
	adups_uint8 multi_bin_resever[7];
}multi_head_info_patch;

typedef enum
{
   ADUPS_LOG_DEBUG=1,
   ADUPS_LOG_LOOP=2,
   ADUPS_LOG_DELTA=3,
   ADUPS_LOG_MCU=4,
   ADUPS_LOG_NONE,
}ADUPS_LOG_LEVEL;


//Error code

#define ADUPS_FUDIFFNET_ERROR_NONE                                               (0)
#define ADUPS_FUDIFFNET_ERROR_INSUFFICIENT_WORKING_BUF                           (-100)

//Updating related
#define ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_START                                 (-600)
#define ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_END                                   (-699)

#define ADUPS_MCU_PATCH_FAIL     (-700)


adups_extern void AUDPSProcedure(void);
adups_extern adups_int32 adups_bl_write_block(adups_uint8* src, adups_uint32 start, adups_uint32 size);
adups_extern adups_int32 adups_bl_read_block(adups_uint8* dest, adups_uint32 start, adups_uint32 size);
adups_extern void adups_bl_progress(adups_int32 percent);
adups_extern void adups_bl_debug_print(void* ctx, const adups_char* fmt, ...);
adups_extern adups_uint8* adups_bl_get_working_buffer(void);
adups_extern adups_uint64 adups_bl_get_working_buffer_len(void);
adups_extern adups_uint32 adups_bl_get_app_base(void);
adups_extern adups_uint32 adups_bl_get_diff_param_size(void);
adups_extern adups_uint32 adups_bl_get_blocksize(adups_uint32 block_idx) ;
adups_extern adups_uint32 adups_bl_get_delta_base(void);
adups_extern adups_uint16 adups_bl_set_working_buffer_detail(void);
adups_extern adups_uint32 adups_bl_get_rom_len(void);



 
#endif

