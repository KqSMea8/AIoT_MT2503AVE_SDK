#include "adups_typedef.h"
#include "adups_debug.h"
#include "adups_define.h"
#include "adups_mcu.h"
#if defined(__EXT_BOOTLOADER__)
#include "adups_bl_main.h"
#endif



AdupsMcuPatch adups_patch_mcu[ADUPS_FOTA_SDK_MAX_MCU_NUM]=
{
	{ADUPS_FOTA_ADDRESS2,adups_patch_mcu1},
};


#ifdef __ADUPS_FOTA_GPS__
adups_extern int GpsFotaCheck(void);
adups_extern int GPS_DL();
#endif

adups_int16 adups_patch_mcu1(void)
{
#if defined(__EXT_BOOTLOADER__)
   adups_int16 status = ADUPS_FUDIFFNET_ERROR_NONE;
  #ifdef __ADUPS_FOTA_GPS__
   if( GpsFotaCheck()==1)
   {
	adups_bl_debug_print(NULL,"adups_patch_mcu1:entry GPS DL ");
	WacthDogDisable();

	adups_bl_progress(10);

	status=GPS_DL();	

	adups_bl_debug_print(NULL,"adups_patch_mcu1:GPS DL status=%d ",status);
	if(status==0)
      		adups_bl_ShowUpateSuccess();
	else
	{
     		 adups_bl_ShowUpdateError(ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_END);
		return ADUPS_MCU_PATCH_FAIL;
	}
	
	return status;
   }
   else
   	return ADUPS_MCU_PATCH_FAIL;
#endif
#endif

}


adups_int16 adups_patch_mcu2(void)
{
	
}





