#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdio.h>
#include <stdarg.h>
#include "adups_typedef.h"
#include "MMI_features.h"
#include "kal_release.h"

//#define ADUPS_DELTA_ADD_SIZE		300 //2000 //1500 //800 //300

#if (ADUPS_DELTA_ADD_SIZE == 100)
#include "adups_debug_RES_0100KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 150)
#include "adups_debug_RES_0150KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 200)
#include "adups_debug_RES_0200KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 300)
#include "adups_debug_RES_0300KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 500)
#include "adups_debug_RES_0500KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 800)
#include "adups_debug_RES_0800KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 1000)
#include "adups_debug_RES_1000KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 1500)
#include "adups_debug_RES_1500KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 2000)
#include "adups_debug_RES_2000KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 2500)
#include "adups_debug_RES_2500KB.h"
#elif (ADUPS_DELTA_ADD_SIZE == 4000)
#include "adups_debug_RES_4000KB.h"
#endif


static adups_int32 adups_debug_switch = 0;

adups_int32 adups_DebugSwitch(adups_int32 nOn)
{
	adups_debug_switch = nOn;

	return E_ADUPS_SUCCESS;
}

void adups_DebugPrint(const adups_char *content, ...)
{
	if(adups_debug_switch) {
		va_list Args;
		adups_char out_buf[ADUPS_MAX_DEBUG_BUF_LEN] = {0};
		adups_int32 len;
		
		va_start(Args,content);
		len = vsnprintf(out_buf,ADUPS_MAX_DEBUG_BUF_LEN, content, Args);

#ifndef WIN32
		kal_prompt_trace(MOD_ADUPS, "%s\n", out_buf);
#else
		OutputDebugStringA(out_buf);
		OutputDebugStringA("\n");
#endif
		va_end(Args);
	}

}

void adups_DebugPrint_Ext(const adups_char *key_string,const adups_char *content, ...)
{
		va_list Args;
		adups_char out_buf[ADUPS_MAX_DEBUG_BUF_LEN] = {0};
		adups_int32 len;
		adups_uint8 key_string_len=0;
		
		va_start(Args,content);
		key_string_len=adups_strlen(key_string);
		adups_strncpy(out_buf,key_string,key_string_len);
		len = vsnprintf(out_buf+key_string_len,ADUPS_MAX_DEBUG_BUF_LEN, content, Args);

#ifndef WIN32
		kal_prompt_trace(MOD_ADUPS, "%s\n", out_buf);
#else
		OutputDebugStringA(out_buf);
		OutputDebugStringA("\n");
#endif
		va_end(Args);

}

void adups_trace2chs(adups_write func, const adups_char *content, ...)
{
	if(adups_debug_switch) {
		va_list Args;
		adups_char out_buf[ADUPS_MAX_DEBUG_BUF_LEN] = {0};
		adups_int32 len;
		
		va_start(Args,content);
		len = vsnprintf(out_buf,ADUPS_MAX_DEBUG_BUF_LEN, content, Args);

		adups_DebugPrint(out_buf);

		if(func)
			func(out_buf, len);

		va_end(Args);
	}

}

void adups_Debug_print_content(adups_char *content,adups_uint16 size)
{
	adups_uint8 i=0;
	adups_uint16 content_size=size;
	while((content_size-128)>0)
	{
		adups_DebugPrint("%s",content+(i*128));
		i++;
		content_size-=128;
	}
	
	adups_DebugPrint("%s",content+(i*128));	
	
}


void adups_Debug_DeltaNew(void)
{
	adups_char *deviceVersion = (adups_char *)adups_get_device_version();
	adups_uint32 len_tmp=0, idx=0, lenmax=strlen(deviceVersion);
	adups_uint32 tmp[20]={0};
	adups_char *res_data=NULL;

#ifdef ADUPS_DELTA_ADD_SIZE

	if(lenmax > 3)
		lenmax = 3;
	
	for(idx=0;idx<lenmax;idx++)
	{	
#if (ADUPS_DELTA_ADD_SIZE == 100)		
		res_data=(adups_char *)RES_0100KB;
#elif (ADUPS_DELTA_ADD_SIZE == 150)		
		res_data=(adups_char *)RES_0150KB;
#elif (ADUPS_DELTA_ADD_SIZE == 200)		
		res_data=(adups_char *)RES_0200KB;
#elif (ADUPS_DELTA_ADD_SIZE == 300)		
		res_data=(adups_char *)RES_0300KB;
#elif (ADUPS_DELTA_ADD_SIZE == 500)		
		res_data=(adups_char *)RES_0500KB;
#elif (ADUPS_DELTA_ADD_SIZE == 800)		
		res_data=(adups_char *)RES_0800KB;
#elif (ADUPS_DELTA_ADD_SIZE == 1000)		
		res_data=(adups_char *)RES_1000KB;
#elif (ADUPS_DELTA_ADD_SIZE == 1500)		
		res_data=(adups_char *)RES_1500KB;
#elif (ADUPS_DELTA_ADD_SIZE == 2000)		
		res_data=(adups_char *)RES_2000KB;
#elif (ADUPS_DELTA_ADD_SIZE == 2500)		
		res_data=(adups_char *)RES_2500KB;
#endif
		if(res_data)
		{
			len_tmp=res_data[idx]<<24 | res_data[idx+1]<<16 | res_data[idx+2]<<8 | res_data[idx+3]; 
			adups_DebugPrint("adups_Debug_DeltaNew:type: %dKB, idx:%d, len:%d",ADUPS_DELTA_ADD_SIZE, idx, len_tmp);
			adups_memcpy(tmp, res_data, lenmax);
		}
	}
#endif	
}








