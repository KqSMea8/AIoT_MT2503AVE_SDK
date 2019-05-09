#ifndef _ADUPS_TIMER_H_
#define _ADUPS_TIMER_H_

adups_extern void adups_start_timer(adups_int32 delay, adups_timer_callback func_ptr, ADUPS_BOOL period);
adups_extern void adups_stop_timer(adups_timer_callback func_ptr);
adups_extern void adups_timer_handler(adups_uint16 timerid);


#endif /* _ADUPS_TIMER_H_ */

