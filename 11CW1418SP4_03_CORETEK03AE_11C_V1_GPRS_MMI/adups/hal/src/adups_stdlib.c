#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "math.h"
#include "adups_typedef.h"
#include "adups_stdlib.h"

adups_int32 adups_sprintf(adups_char * buffer, const adups_char *format, ...)
{
	adups_int32 iReturn;
	va_list Args;
	va_start(Args,format);
	iReturn = vsprintf(buffer,format,Args);
	va_end(Args);
	return iReturn;
}


adups_int32 adups_snprintf(adups_char * buffer, adups_int32 size,const adups_char *format, ...)
{
	adups_int32 iReturn;
	va_list Args;
	va_start(Args,format);
	iReturn = vsnprintf(buffer,size,format,Args);
	va_end(Args);
	return iReturn;
}


void * adups_memcpy(void * dest, const void * src, adups_uint32 count)
{
   if((dest!=NULL)&&(src!=NULL)&&(count!=0))
	return memcpy(dest,src,count);
   else
   	return NULL;
}

void * adups_memset(void * dest, char c, adups_uint32 count )
{
   if((dest!=NULL)&&(count!=0))
	return memset(dest,(adups_int32)c,count);
   else
   	return NULL;
}

adups_int32 adups_memcmp(void * buf1, void * buf2, adups_uint32 count)
{
   if((buf1!=NULL)&&(buf2!=NULL)&&(count!=0))
	return memcmp((const void *)buf1,(const void *)buf2,count);
   else
   	return 0;
}


adups_char * adups_strcat(adups_char * strDestination, const adups_char * strSource )
{
   if((strDestination!=NULL)&&(strSource!=NULL))
	return strcat(strDestination,strSource);
   else
   	return NULL;
}


adups_char * adups_strncat(adups_char * strDestination, const adups_char * strSource,adups_uint32 len)
{
   if((strDestination!=NULL)&&(strSource!=NULL)&&(len!=0))
	return strncat(strDestination,strSource,len);
   else
   	return NULL;
}


adups_uint32 adups_strlen(const adups_char * str)
{
   if(str!=NULL)
	return (adups_uint32)strlen(str);
   else
   	return 0;
}

adups_int32 adups_strncmp(const adups_char * string1, adups_char * string2, adups_uint32 count)
{
	return strncmp(string1,string2,count);

}

adups_int32 adups_strcmp(const adups_char * string1, const adups_char * string2 )
{
	return strcmp(string1,string2); 	
}


adups_char * adups_strcpy(adups_char * strDestination, const adups_char * strSource)
{
   if((strDestination!=NULL)&&(strSource!=NULL))
	return strcpy(strDestination, strSource);
   else
   	return NULL;
}

adups_char * adups_strncpy(adups_char * strDest, const adups_char * strSource, adups_uint32 count)
{
   if((strDest!=NULL)&&(strSource!=NULL)&&(count!=0))
	return strncpy(strDest,strSource,count);
   else
   	return NULL;
}


adups_char * adups_strstr(const adups_char * str, const adups_char * strSearch)
{
   if(str!=NULL)
	return strstr(str,strSearch);
   else
   	return NULL;
}

adups_char * adups_strchr(const adups_char * str, adups_int32 ch)
{
   if(str!=NULL)
	return strchr(str,ch);
   else
   	return NULL;
}



void adups_itoa(adups_uint32 i, adups_char *buf, adups_uint8 base)
{
	adups_char *s;
	#define ADUPS_ITOA_LEN	20
	adups_int32 rem;
	static adups_char rev[ADUPS_ITOA_LEN+1];

	rev[ADUPS_ITOA_LEN] = 0;
	if (i == 0)
		{
		buf[0] = '0';
		++(buf);
		return;
		}
	s = &rev[ADUPS_ITOA_LEN];
	while (i)
		{
		rem = i % base;
		if (rem < 10)
			*--s = rem + '0';
		else if (base == 16)
			*--s = "abcdef"[rem - 10];
		i /= base;
		}
	while (*s)
		{
		buf[0] = *s++;
		++(buf);
		}
}

adups_uint32 adups_atoi(adups_char * str)
{
 	if(str!=NULL)
		return (adups_uint32)atoi(str);
	else
		return 0;
}




