#ifndef _ADUPS_STDLIB_H_
#define _ADUPS_STDLIB_H_


adups_extern adups_int32 adups_sprintf(adups_char * buffer, const adups_char *format, ...);
adups_extern adups_int32 adups_snprintf(adups_char * buffer, adups_int32 size,const adups_char *format, ...);

adups_extern void * adups_memcpy(void * dest, const void * src, adups_uint32 count);
adups_extern void * adups_memset(void * dest, char c, adups_uint32 count );
adups_extern adups_int32 adups_memcmp(void * buf1, void * buf2, adups_uint32 count);
adups_extern adups_char * adups_strcat(adups_char * strDestination, const adups_char * strSource );
adups_extern adups_char * adups_strncat(adups_char * strDestination, const adups_char * strSource,adups_uint32 len);

adups_extern adups_uint32 adups_strlen(const adups_char * str);
adups_extern adups_int32 adups_strncmp(const adups_char * string1, adups_char * string2, adups_uint32 count);
adups_extern adups_int32 adups_strcmp(const adups_char * string1, const adups_char * string2 );
adups_extern adups_char * adups_strcpy(adups_char * strDestination, const adups_char * strSource);
adups_extern adups_char * adups_strncpy(adups_char * strDest, const adups_char * strSource, adups_uint32 count);
adups_extern adups_char * adups_strstr(const adups_char * str, const adups_char * strSearch);
adups_extern adups_char * adups_strchr(const adups_char * str, adups_int32 ch);

adups_extern void adups_itoa(adups_uint32 i, adups_char *buf, adups_uint8 base);
adups_extern adups_uint32 adups_atoi(adups_char * str);


#endif