#ifndef _ADUPS_FLASH_H_
#define _ADUPS_FLASH_H_

adups_extern  adups_int32 Adups_write_flash(adups_uint8* data_ptr, adups_uint32 len);
adups_extern ADUPS_BOOL AdupsUseFlash(void);

adups_extern  adups_int32 Adups_write_flash(adups_uint8* data_ptr, adups_uint32 len);

adups_extern  adups_uint32 Adups_init_flash(void);

adups_extern void Adups_erase_delata(void);

adups_extern adups_uint32 AdupsGetBlockSize(void);
#endif

