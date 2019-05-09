#ifndef _ADUPS_DEBUG_H_
#define _ADUPS_DEBUG_H_

extern adups_int32 adups_DebugSwitch(adups_int32 nOn);
extern void adups_DebugPrint(const adups_char *content, ...);
extern void adups_DebugPrint_Ext(const adups_char *key_string,const adups_char *content, ...);




#endif

