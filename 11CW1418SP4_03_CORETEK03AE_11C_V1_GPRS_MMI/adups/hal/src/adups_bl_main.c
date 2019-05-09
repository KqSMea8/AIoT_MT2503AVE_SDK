
#include "adups_typedef.h"
#include "adups_bl_main.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "bl_common.h"
#include "FTL.h"
#include "kal_release.h"
#include "flash_opt.h"
#include "bl_custom.h"
#include "bl_MTK_BB_REG.H"
#include "custom_img_config.h"
#include "adups_define.h"
#include "adups_flash.h"
#include "adups_mcu.h"


#ifdef 	__ADUPS_FOTA_DIFF__								//benny

#if defined(LQT_SUPPORT)&&!defined(__FUNET_ENABLE__)
#include "lcd_lqt.h"
adups_uint8 lcd_at_mode=LCD_AT_RELEASE_MODE;
ADUPS_BOOL lcd_update_permission = ADUPS_FALSE;
#endif

#ifdef __SV5_ENABLED__
extern FTL_FuncTbl *g_ftlFuncTbl;
#endif

#ifdef __LCD_DRIVER_IN_BL__
static ADUPS_BOOL lcd_inited = ADUPS_FALSE;
#endif /* __LCD_DRIVER_IN_BL__ */

adups_uint8 *   upi_working_buffer = 0;

#if !defined(__FUNET_ENABLE__)
extern adups_uint32 Image$$EXT_BL_ADUPS_FOTA_WORKING_BUF$$ZI$$Base;
extern adups_uint32 Image$$EXT_BL_ADUPS_FOTA_WORKING_BUF$$ZI$$Length;
#else
extern adups_uint32 Image$$EXT_BL_IOT_FOTA_WORKING_BUF$$ZI$$Base;
extern adups_uint32 Image$$EXT_BL_IOT_FOTA_WORKING_BUF$$ZI$$Length;
#endif


#define UPI_WORKING_BUFFER_LEN         3670016 //3.5*1024*1024 = 3.5M


static adups_int32 adups_bl_debug_switch = 0;
static adups_uint8 adups_detail=100; //200: for 256KB==>2MB, 100: for 64KB==>1MB
adups_uint8 adups_delta_num=0;
adups_uint8 adups_curr_delta=1;
multi_head_info_patch PackageInfo;
adups_char debug_patch_session[50]={0};
adups_char debug_patch_session1[50]={0};
adups_char adups_result[1024]={0};
adups_char patch_mcu_result[50]={0};
adups_char patch_result[ADUPS_MAX_DEBUG_BUF_LEN]={0};

adups_extern int adups_iot_patch(void);
adups_extern ADUPS_BOOL adups_IsPackgeFound(void);
adups_extern void adups_get_package_number(adups_uint8 *delta_num);
adups_extern adups_int32 adups_get_package_info(multi_head_info_patch *info,adups_uint8 curr_delta);
adups_extern ADUPS_BOOL adups_check_mcu_id(adups_uint32 address,adups_char *mcu_id);

adups_int32 adups_bl_DebugSwitch(adups_int32 nOn)
{
	adups_bl_debug_switch = nOn;

	return E_ADUPS_SUCCESS;
}

void adups_bl_debug_print(void* ctx, const adups_char* fmt, ...) 
{
	if(adups_bl_debug_switch) {
		va_list ap;
		va_start (ap, fmt);
		bl_print_internal(LOG_INFO, fmt, ap);
		if(ctx!=NULL)
		{
			if(ctx==(void *)ADUPS_LOG_LOOP)
			{
				memset(debug_patch_session,0,50);
				vsnprintf(debug_patch_session,50, fmt, ap);
			}
			else if(ctx==(void *)ADUPS_LOG_DELTA)
			{
				memset(debug_patch_session1,0,50);
				vsnprintf(debug_patch_session1,50, fmt, ap);
			}
			else if(ctx==(void *)ADUPS_LOG_MCU)
			{
				memset(patch_mcu_result,0,50);
				vsnprintf(patch_mcu_result,50, fmt, ap);
			}
			else if(ctx==(void *)ADUPS_LOG_DEBUG)
			{
				memset(patch_result,0,ADUPS_MAX_DEBUG_BUF_LEN);
				vsnprintf(patch_result,ADUPS_MAX_DEBUG_BUF_LEN, fmt, ap);
			}
		}
		va_end (ap);
		bl_print(LOG_INFO, "\r"); 
		}
}


adups_uint8* adups_bl_get_working_buffer(void)
{
#if !defined(__FUNET_ENABLE__)
	return (adups_uint8 *)(&Image$$EXT_BL_ADUPS_FOTA_WORKING_BUF$$ZI$$Base);
#else
	return (adups_uint8 *)(&Image$$EXT_BL_IOT_FOTA_WORKING_BUF$$ZI$$Base);
#endif
}

adups_uint64 adups_bl_get_working_buffer_len(void)
{
#if !defined(__FUNET_ENABLE__)

	adups_bl_debug_print(NULL, "working_buflen: 0x%x", (adups_uint64)(&Image$$EXT_BL_ADUPS_FOTA_WORKING_BUF$$ZI$$Length));	

	//return 0x600000;
	return (adups_uint64)(&Image$$EXT_BL_ADUPS_FOTA_WORKING_BUF$$ZI$$Length);
#else
	return (adups_uint64)(&Image$$EXT_BL_IOT_FOTA_WORKING_BUF$$ZI$$Length);
#endif
}

adups_uint16 adups_bl_set_working_buffer_detail(void)
{	
	/*100=2M*/
	return adups_detail;	
}

adups_uint32 adups_bl_get_app_base(void)
{
	//return custom_ROM_baseaddr();
	return PackageInfo.multi_bin_address;
}

adups_uint32 adups_bl_get_rom_len(void)
{
	return NOR_BOOTING_NOR_FS_BASE_ADDRESS;
}


adups_uint32 adups_bl_get_delta_base(void)
{
	//return custom_ROM_baseaddr();
	return PackageInfo.multi_bin_offset;
}


adups_uint32 adups_bl_get_diff_param_size(void)
{
	//return ADUPS_DIFF_PARAM_SIZE;
	 adups_uint32 block_size=AdupsGetBlockSize();
	return block_size;
}


adups_uint32 adups_bl_get_blocksize(adups_uint32 block_idx) 
{
#if defined(_NAND_FLASH_BOOTING_) || defined(__EMMC_BOOTING__)
	  
	   return 0;   
	   
#else	
	   adups_uint32 block_size = 0;
	   block_size = g_ftlFuncTbl->FTL_GetBlockSize(block_idx, NULL);
	
	   adups_bl_debug_print(NULL, "ENTER adups_get_blocksize, ret=%d\n\n\r", block_size);	
	   return block_size;
#endif
}


void adups_bl_DL_InitLCD()
{	
#if 0//defined(__LCD_DRIVER_IN_BL__)

   if(!lcd_inited)
   {
      adups_bl_debug_print(NULL, "Init LCD\n\r");
      BL_LCDHWInit();
      BL_ShowUpdateFirmwareInitBackground();
      BL_LCDSetBackLight();

      {
         DCL_HANDLE rtc_handler;
         DCL_HANDLE pw_handle;

         rtc_handler = DclRTC_Open(DCL_RTC,FLAGS_NONE);
         DclRTC_Control(rtc_handler, RTC_CMD_SETXOSC, (DCL_CTRL_DATA_T *)NULL);
         DclRTC_Control(rtc_handler, RTC_CMD_HW_INIT, (DCL_CTRL_DATA_T *)NULL);
         DclRTC_Close(rtc_handler);

         pw_handle=DclPW_Open(DCL_PW, FLAGS_NONE);
         DclPW_Control(pw_handle,PW_CMD_POWERON,NULL);
         DclPW_Close(pw_handle);
      }

      lcd_inited = KAL_TRUE;

   }

#endif /* __LCD_DRIVER_IN_BL__ */
}


void adups_bl_progress(adups_int32 percent)
{
	adups_bl_debug_print(NULL, "ENTER adups_progress current = %d\n\n\r", percent);

	WacthDogRestart();

	if(percent <= 100)
	{
#if 0//defined(__LCD_DRIVER_IN_BL__)
	  adups_bl_DL_InitLCD();
	  BL_ShowUpdateFirmwareProgress(percent);
#endif // __LCD_DRIVER_IN_BL__ 

	}
}


void adups_bl_ShowUpateSuccess(void)
{
#if 0//defined(__LCD_DRIVER_IN_BL__)
   BL_ShowUpdateFirmwareOK();
#endif /* __LCD_DRIVER_IN_BL__ */
}




void adups_bl_ShowUpdateError(adups_int16 status)
{
#if 0//defined(__LCD_DRIVER_IN_BL__)
   adups_uint32 i;

   typedef struct {
      adups_int16 error_start;
      adups_int16 error_end;
      adups_uint16 rgb[3];
   } ERROR_MAP;

   const ERROR_MAP error_map[] =
   {
      {ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_START,         ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_END,          {255,   0,   0} }, //Red
   };

   if(status == ADUPS_FUDIFFNET_ERROR_NONE)
   {
      return;
   }

   adups_bl_DL_InitLCD();

   for(i=0; i<sizeof(error_map)/sizeof(*error_map); i++)
   {
      if(status <= error_map[i].error_start && status >= error_map[i].error_end)
      {
         BL_ShowUpdateFirmwareFail(error_map[i].rgb[0], error_map[i].rgb[1], error_map[i].rgb[2]);
         return;
      }
   }

   //Other erros
   BL_ShowUpdateFirmwareFail(0, 0, 0);
#endif /* __LCD_DRIVER_IN_BL__ */

}

adups_int32 adups_bl_read_block(adups_uint8* dest, adups_uint32 start, adups_uint32 size)
{
	long ret_val = -1; // read error
	
#if defined(_NAND_FLASH_BOOTING_) || defined(__EMMC_BOOTING__)	
	   //Currently not used
	   return = 0; 	   
#else
	{
	   adups_uint32 block_idx = 0,page_idx =0, page_offset = 0, current_page = 0, page_count	= 0;
	   adups_int32 status = 0;
	   adups_uint32 destination = (adups_uint32)dest;
	   adups_uint32 page_buff[NOR_PAGE_SIZE >> 2];
	   adups_uint32 nor_page_size = g_ftlFuncTbl->FTL_GetPageSize(); 
	   adups_uint32 need_to_read = size;
	
	   adups_uint32 NOR_FLASH_Base = custom_get_NORFLASH_Base();
	   adups_uint32 __pageSize = g_ftlFuncTbl->FTL_GetPageSize();
	   adups_uint32 __blockSize = g_ftlFuncTbl->FTL_GetBlockSize(0, NULL);
	   WacthDogRestart();
	   
	   adups_bl_debug_print(NULL, "NOR_FLASH_Base = 0x%x  ::: 201511231740 \n\r", NOR_FLASH_Base);
	   adups_bl_debug_print(NULL, "NOR_FLASH page size %d, block size %d \n\r", __pageSize, __blockSize);		
	
	
	   
	   adups_bl_debug_print(NULL, "adups_bl_read_block: 0x%x(%d) => 0x%x \n\r", start, size, dest);
	   
	   page_offset = (adups_uint32)start - 0x10000000; 
	   page_offset &= ~(custom_get_NORFLASH_Base());
	   while (page_offset >= g_ftlFuncTbl->FTL_GetBlockSize(block_idx, NULL))
	   {
		  page_offset -= g_ftlFuncTbl->FTL_GetBlockSize(block_idx, NULL);
		  block_idx++;
	   }
	
	   start |= custom_get_NORFLASH_Base();	
	
	   if (status != 0)
	   {
		  adups_bl_debug_print((void *)ADUPS_LOG_DEBUG, "ERROR1 adups_bl_read_block errcode = %d \n\r", status);	
		  return ret_val;
	   }
	
  	   status = g_ftlFuncTbl->FTL_AddrToBlockPage(start, &block_idx, &page_idx, NULL);

          ASSERT(status == FTL_SUCCESS);
		  
	   page_count = (g_ftlFuncTbl->FTL_GetBlockSize(block_idx, NULL) / nor_page_size);
	
	   adups_bl_debug_print((void *)ADUPS_LOG_DEBUG, "adups_bl_read_block block_idx = %d, page_count = %d, page_offset = %d \n\r", block_idx,page_count, page_offset);

	
	   while (page_offset >= nor_page_size) 
	   {  
		  page_offset -= nor_page_size;
		  current_page++;
		  if (current_page == page_count) 
		  { 
			 block_idx++;
			 current_page = 0;		 
			 page_count = (g_ftlFuncTbl->FTL_GetBlockSize(block_idx, NULL) / nor_page_size);
		  }
	   }
	
	   adups_bl_debug_print(NULL, "adups_bl_read_block page_buff = %x, block_idx = %d, current_page= %d \n\r",page_buff,block_idx, current_page);

	
	   status = g_ftlFuncTbl->FTL_ReadPage(block_idx, current_page, page_buff, NULL);
	
	   if (status != FTL_SUCCESS)
	   {
		  adups_bl_debug_print((void *)ADUPS_LOG_DEBUG, "ERROR2 adups_bl_read_block errcode = %d, block_idx = %d, current_page = %d, page_offset = %d \n\r", status, block_idx, current_page, page_offset);	
		  return ret_val;
	   }
	
	   if ( size >= (nor_page_size - page_offset)) 
	   {
		  memcpy((void *)destination, (const void *)((adups_uint8 *)page_buff +page_offset), (nor_page_size - page_offset));		  
		  adups_bl_debug_print(NULL, "destination = %x %x %x \n\r", *(adups_int32*)destination, *(adups_int32*)(destination+4), *(adups_int32*)(destination +8)); //debug
		  destination += nor_page_size - page_offset;
		  size -= nor_page_size - page_offset;
		  page_offset = 0;
	   }
	   else
	   {
		  memcpy((void *)destination, (const void *)((adups_uint8 *)page_buff +page_offset), size);	  
		  adups_bl_debug_print(NULL, "destination = %x %x %x \n\r", *(adups_int32*)destination, *(adups_int32*)(destination+4), *(adups_int32*)(destination +8)); //debug
		  destination += size;
		  page_offset += size;
		  size -= size;
	   }
	   if (current_page == (page_count -1)) 
	   { 
		  block_idx++;
		  current_page = 0;
	   }
	   else
	   {
		  current_page++;
	   }
	
		while ( size > 0)
		{
		  if (size >= nor_page_size)
		  {
			if((current_page==0) || (current_page>=(page_count-1)))
			{
			 	adups_bl_debug_print(NULL, "Case1 adups_bl_read_block destination = %x, block_idx = %d, current_page - %d \n\r", destination, block_idx, current_page );			 
			}
			
			 status = g_ftlFuncTbl->FTL_ReadPage(block_idx, current_page, (adups_uint32 *)destination, NULL);
			 
			 if (status != FTL_SUCCESS)
			 {
				adups_bl_debug_print(NULL, "ERROR3 adups_bl_read_block errcode = %d, block_idx = %d, current_page = %d, page_offset = %d \n",status, block_idx, current_page, page_offset);
				return ret_val;
			 }

			 destination += nor_page_size;
			 size -= nor_page_size;
		  }
		  else
		  {
		  	if((current_page==0) || (current_page>=(page_count-1)))
		  	{
				adups_bl_debug_print(NULL, "Case2 adups_bl_read_block destination = %x, block_idx = %d, current_page - %d \n\r", destination, block_idx,current_page );	
		  	}
			
			 status = g_ftlFuncTbl->FTL_ReadPage(block_idx, current_page, page_buff, NULL);
			 
			 if (status != FTL_SUCCESS)
			 {
				adups_bl_debug_print((void *)ADUPS_LOG_DEBUG, "ERROR4 adups_bl_read_block errcode = %d, block_idx = %d, current_page = %d, page_offset = %d \n",status, block_idx, current_page, page_offset);	
				return ret_val;
			 }
	
			 memcpy((void *)destination, (const void *)page_buff, size);
			 destination += size;
			 size -= size;
		  }
	
		  current_page++;
	
		  if (current_page == page_count)
		  {
			 block_idx++;
			 current_page = 0;
			 page_count = (g_ftlFuncTbl->FTL_GetBlockSize(block_idx, NULL) / nor_page_size);
	
			 adups_bl_debug_print((void *)ADUPS_LOG_DEBUG, "Case1 adups_bl_read_block  block_idx = %d, current_page = %d , page_count = %d \n\r",block_idx,current_page,page_count );
		  }
	
	   }
	
	   ret_val = need_to_read - size;
	}
#endif
	
	   return ret_val;
	}

void adups_save_update_result(adups_char* result_str,adups_char* mcu_id,adups_uint8 result)
{
	adups_char temp_str[10]={0};
	sprintf(temp_str,"%s=%d",mcu_id,result);
	if(strlen(result_str)==0)
		strcpy(result_str,temp_str);
	else
		strcat(result_str,temp_str);
	
}

adups_int32 adups_bl_write_block(adups_uint8* src, adups_uint32 start, adups_uint32 size)
{
	   long ret_val = -1;
#if defined(_NAND_FLASH_BOOTING_) || defined(__EMMC_BOOTING__)
		 //Currently not used
		 return 0;
#else
	{ 
	   //Logical_Flash_info_st info;
	   adups_int32  status = 0;
	   adups_uint32 block_idx = 0,page_idx =0, page_count = 0, current_page = 0;
	   adups_uint32 address = start;
	   adups_uint32 nor_page_size = g_ftlFuncTbl->FTL_GetPageSize(); 
	   adups_uint32 source = (adups_uint32)src;
	   adups_int32 need_to_write = size;
	   adups_int32 write_size = size;

	   adups_bl_debug_print(NULL, "ENTER adups_bl_write_block \n\r");

	   
	   (adups_uint32)address -= 0x10000000; 

  	   status = g_ftlFuncTbl->FTL_AddrToBlockPage((adups_uint32)address, &block_idx, &page_idx, NULL);

          ASSERT(status == FTL_SUCCESS);
	

	   adups_bl_debug_print(NULL, "adups_bl_write_block do erase  block_idx = %d  \n\r", block_idx);

	   
	   status = g_ftlFuncTbl->FTL_EraseBlock(block_idx, NULL);
	   
	   if (status != FTL_SUCCESS)
	   {

		  adups_bl_debug_print((void *)ADUPS_LOG_DEBUG, "Error adups_bl_write_block  errcode = %d, block	= %d \n\r", status, block_idx);		  
		  return ret_val;
	   }
	
	   //Erase done, write data
	   adups_bl_debug_print(NULL, "ENTER adups_bl_write_block write *address = %x, source = %x, size = %d \n\r",address, source, size);

	   
  	   status = g_ftlFuncTbl->FTL_AddrToBlockPage((adups_uint32)address, &block_idx, &page_idx, NULL);

          ASSERT(status == FTL_SUCCESS);
	   
	   page_count = (g_ftlFuncTbl->FTL_GetBlockSize(block_idx, NULL)/nor_page_size);
	
	   while (write_size > 0 )
	   {
		  if ( current_page >= page_count)
		  {
	
			 adups_bl_debug_print(NULL, "Change block	adups_bl_write_block block_idx = %d, page_count = %d, current_page = %d \n\r",block_idx, page_count, current_page);
		 
			 current_page = 0;
			 block_idx++;	
			status = g_ftlFuncTbl->FTL_EraseBlock(block_idx, NULL);			
			if (status != FTL_SUCCESS)
			{
			   adups_bl_debug_print((void *)ADUPS_LOG_DEBUG, "Error adups_bl_write_block  errcode = %d, block = %d \n\r", status, block_idx);			   
			   return -1;
			}
			 
			 page_count = (g_ftlFuncTbl->FTL_GetBlockSize(block_idx, NULL)/nor_page_size);
		  }
	   
	   	  if((current_page==0) || (current_page>=(page_count-1)))
	   	  {
			  adups_bl_debug_print(NULL, "Write a page adups_bl_write_block address = %x, source = %x, size =  %d \n\r",address, source, size);
			  adups_bl_debug_print(NULL, "CASE1 adups_bl_write_block *source = %x, block_idx = %d, current_page = %d \n\r",source, block_idx, current_page);
	   	  }
		  
		  status = g_ftlFuncTbl->FTL_WritePage(block_idx, current_page, (adups_uint32 *)source, NULL);
	
		  if (status == FTL_SUCCESS)
		  {
			 (adups_uint32)address += nor_page_size;
			 source += nor_page_size;
			 current_page++;
			 write_size -= nor_page_size;
		  }
		  else
		  {
			 adups_bl_debug_print((void *)ADUPS_LOG_DEBUG, " ERROR adups_bl_write_block  errcode = %d, address %x, source = %x, size = %d, block_idx = %d, current_page = %d \n\r", \
																	   status, address, source, write_size, block_idx, current_page);
			 return ret_val;
		  }
	   }
	
	   ret_val = need_to_write - write_size;
	   
	}
#endif
	
	   return ret_val;
	}


#ifdef __ADUPS_FOTA_GPS__
adups_extern int GpsFotaCheck(void);
adups_extern int GPS_DL();
#endif



void adups_remove_all(void)
{
   	adups_int32 temp_file;
   	adups_int32 result;
   	adups_int32 len;
	adups_Remove_File(adups_get_delta_file_path());
	if(strlen(adups_result)!=0)
	{
	   temp_file=adups_Open_File(adups_get_backup_file_path(), ADUPS_TRUNCATE | ADUPS_READWRITE);

	   if(temp_file)
	   {
		result=adups_Seek_File(temp_file,0,ADUPS_SEEK_START);
		if(result==E_ADUPS_SUCCESS)
		{
			adups_Write_File(temp_file, adups_result, strlen(adups_result), &len);
			if(strlen(adups_result)==len)
				result=E_ADUPS_SUCCESS;
			else
				result=E_ADUPS_FAILURE;
		}
		adups_Close_File(temp_file);
	   }
	   else
		result=E_ADUPS_FAILURE;
	}
	else
		adups_Remove_File(adups_get_backup_file_path());		
}


int adups_iot_full_patch(void)
{

	return 0;
}
void adups_get_patch_result(void)
{
	adups_uint16 result_len=0;
	memset(adups_result,0,1024);
	result_len=strlen(debug_patch_session)+strlen(debug_patch_session1)+strlen(patch_result)+strlen(patch_mcu_result);
	if(result_len==0)
		return;
	else
		sprintf(adups_result,"%d:",(result_len+9));
	
	if(strlen(debug_patch_session1)!=0)
	{
		strncat(adups_result,debug_patch_session1,strlen(debug_patch_session1));
		strncat(adups_result,";",1);
	}

	if(strlen(debug_patch_session)!=0)
	{
		strncat(adups_result, debug_patch_session, strlen(debug_patch_session));
		strncat(adups_result,";",1);
	}

	if(strlen(patch_result)!=0)
	{
		strncat(adups_result, patch_result, strlen(patch_result));
		strncat(adups_result,";",1);
	}

	if(strlen(patch_mcu_result)!=0)
	{
		strncat(adups_result, patch_mcu_result, strlen(patch_mcu_result));
		strncat(adups_result,";",1);
	}
		
}
void AUDPSProcedure(void)
{
	adups_int16 status = ADUPS_FUDIFFNET_ERROR_NONE;
	adups_uint8 i;
	adups_int8 mcu_flag=-1;
   
	adups_bl_DebugSwitch(1);

#ifdef __MULTI_PACKAGE__

	if(!adups_IsPackgeFound())
	{
		status=ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_END;	
		adups_get_patch_result();
		if(AdupsUseFlash()==ADUPS_TRUE)
		{
			Adups_erase_delata();
			Adups_erase_backup_region();

			if(strlen(adups_result)!=0)
				Adups_write_backup_region(adups_result,strlen(adups_result));
		}
		else
			adups_remove_all();
		return;
	}	

	adups_get_package_number(&adups_delta_num);

	adups_bl_debug_print((void*)ADUPS_LOG_DEBUG, "package_number = %d",adups_delta_num);

if((adups_delta_num>ADUPS_MAX_DELTA_NUM)||(adups_delta_num==0))
	return;

#else
	#ifdef __ADUPS_FOTA_GPS__
		if( GpsFotaCheck()==1)
		{
			WacthDogDisable();

			adups_bl_progress(10);

			status=GPS_DL();	
			if(status==0)
					adups_bl_ShowUpateSuccess();
			else
					 adups_bl_ShowUpdateError(ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_END);

			return;
		}
	#endif

	   if( !adups_IsDeltaFileFound())
	   {
		status=ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_END;	
		return;
	   }
#endif


   /*Init memory buffer*/   
   
   if (NULL == adups_bl_get_working_buffer()) 
  {
       adups_bl_debug_print((void*)ADUPS_LOG_DEBUG, "Can not alloc UPI working buffer\n");
       status=ADUPS_FUDIFFNET_ERROR_INSUFFICIENT_WORKING_BUF;
   }

   WacthDogRestart();

   adups_bl_debug_print(NULL, "%x\n\r", adups_bl_get_working_buffer()); 
   

   if(status == ADUPS_FUDIFFNET_ERROR_NONE)
   {

	WacthDogDisable();

	CacheInit(); 
	adups_bl_debug_print(NULL, "== chenhong test ready to call function adups_iot_patch\n\r");

#ifdef __MULTI_PACKAGE__
	while(adups_curr_delta<=adups_delta_num)
	{
		adups_bl_progress(0);
		memset(&PackageInfo,0,sizeof(multi_head_info_patch));
		adups_bl_debug_print((void *)ADUPS_LOG_DELTA, "adups_curr_delta = %d\n\r",adups_curr_delta);
		status=adups_get_package_info(&PackageInfo,adups_curr_delta);
		if(status!=ADUPS_FUDIFFNET_ERROR_NONE)
			break;
		adups_bl_debug_print(NULL, "PackageInfo add=%x,m=%d,offset=%d\n\r",PackageInfo.multi_bin_address,PackageInfo.multi_bin_method,PackageInfo.multi_bin_offset);   

		for(i=0; i<ADUPS_FOTA_SDK_MAX_MCU_NUM; i++)
		{
			if(adups_check_mcu_id(PackageInfo.multi_bin_address,adups_patch_mcu[i].mcu_id)==ADUPS_TRUE)
			{
				mcu_flag=i;
				break;
			}
		}
		if(mcu_flag>=0)
		{
			adups_bl_debug_print(NULL, "patch mcu mcu_flag = %d\r\n",mcu_flag);   	
			status = 	adups_patch_mcu[mcu_flag].adups_patch();
		
		}
		else if(PackageInfo.multi_bin_method==1)
		{
			adups_bl_debug_print(NULL, "full patch\r\n"); 
 			status = adups_iot_full_patch();
	
		}
		else
		{
			adups_bl_debug_print(NULL, "diff patch\r\n");   	
 			status = adups_iot_patch();
		}

		
		if(status != ADUPS_FUDIFFNET_ERROR_NONE&&status!=ADUPS_MCU_PATCH_FAIL)
		{
			adups_bl_debug_print(NULL, "patch fail\r\n"); 
			break;
		}
		else
		{

			if(status==ADUPS_MCU_PATCH_FAIL)
			{
				adups_bl_debug_print(NULL, "mcu patch fail\r\n"); 
				//adups_save_update_result(patch_mcu_result,adups_patch_mcu[i].mcu_id,99);
				adups_curr_delta++;
				mcu_flag=-1;				
			}
			else
			{
				adups_bl_debug_print(NULL, "patch sucess\r\n"); 
				adups_curr_delta++;
				mcu_flag=-1;
			}
		}
	}
#else
	adups_bl_progress(0);

 	status = adups_iot_patch();	
#endif

	CacheDeinit();

	WacthDogRestart();

	adups_bl_debug_print(NULL, "IOT_ADUPS_PATCH 20151126-1544\r\n");

      
      if(status != ADUPS_FUDIFFNET_ERROR_NONE)
      {
      	  status = ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_END;  
         adups_bl_debug_print(NULL, "FOTA update failed...%d\n\r", status);
      }

   }


   if(status == ADUPS_FUDIFFNET_ERROR_NONE)
   {
      adups_bl_ShowUpateSuccess();
	if(AdupsUseFlash()==ADUPS_TRUE)
	{
      		Adups_erase_delata();
	  	Adups_erase_backup_region();
	}
	else
		adups_remove_all();
	
      adups_bl_debug_print(NULL, "[FUDIFFNET] Update done, reboot now ...\n\r");
      SetWDTNormalResetFlag();
      WacthDogDisable();   
   }
   else
   {
      adups_bl_ShowUpdateError(status);
      adups_get_patch_result();
     if(AdupsUseFlash()==ADUPS_TRUE)
     {
      	Adups_erase_delata();
		Adups_erase_backup_region();
	
	if(strlen(adups_result)!=0)
		Adups_write_backup_region(adups_result,strlen(adups_result));
     }
     else
		adups_remove_all();
      adups_bl_debug_print(NULL, "***Something wrong during update, status=%d\n\r", status);
      SetWDTNormalResetFlag();                                                                     
      WacthDogDisable();          
      
   }
}
#endif

