#include "adups_typedef.h"
#include "adups_debug.h"
#include "adups_timer.h"
#include "MMI_include.h"
#include "TimerEvents.h"


static ADUPS_TIMER_STRUCT g_adups_timer[ADUPS_TIMER_MAXNUM-ADUPS_TIMER0]={0};


void adups_start_timer(adups_int32 delay,adups_timer_callback func_ptr, ADUPS_BOOL period)
{
	adups_uint8 idx = 0, type=0;
	
	//adups_DebugPrint("%s: 111 delay:%d, func:0x%08x, period:%d", __func__, delay, (adups_uint32)func_ptr, period);

	for(idx=0;idx<(ADUPS_TIMER_MAXNUM-ADUPS_TIMER0);idx++)
	{
		if(g_adups_timer[idx].func_ptr == NULL)
		{
			g_adups_timer[idx].func_ptr = func_ptr;
			g_adups_timer[idx].delay = delay;
			break;
		}
	}
	//adups_DebugPrint("%s: idx=%d", __func__, idx);

	if(idx >= ADUPS_TIMER_MAXNUM) 
		return; //timer is full

	//adups_DebugPrint("%s: 222 delay:%d, func:0x%08x", __func__, delay, (adups_uint32)func_ptr);


	StartTimer(ADUPS_TIMER0+idx, delay, (FuncPtr)func_ptr);	
}

void adups_stop_timer(adups_timer_callback func_ptr)
{
	adups_uint8 idx = 0;

	//adups_DebugPrint("%s: timer func: 0x%08x", __func__, (adups_uint32)func_ptr);

	for(idx=0;idx<(ADUPS_TIMER_MAXNUM-ADUPS_TIMER0);idx++)
	{
		if(g_adups_timer[idx].func_ptr == func_ptr)
		{
			g_adups_timer[idx].func_ptr = NULL;
			g_adups_timer[idx].delay = 0;
			break;
		}
	}

	if(idx >= ADUPS_TIMER_MAXNUM) 
		return; //timer is not found
		
	//adups_DebugPrint("%s: timer idx:%d", __func__, idx);
	
	StopTimer(ADUPS_TIMER0+idx);
	
}

void adups_timer_handler(adups_uint16 timerid)
{
	
}

