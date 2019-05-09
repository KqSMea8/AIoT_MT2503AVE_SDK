#ifndef _ADUPS_TIMER_H_
#define _ADUPS_TIMER_H_

adups_extern adups_int16 adups_patch_mcu1(void);
adups_extern adups_int16 adups_patch_mcu2(void);

adups_extern AdupsMcuPatch adups_patch_mcu[ADUPS_FOTA_SDK_MAX_MCU_NUM];

#endif /* _ADUPS_TIMER_H_ */

