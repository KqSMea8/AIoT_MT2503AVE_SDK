#if defined(MOTION_SENSOR_SUPPORT)

#include "kal_release.h"
#include "custom_config.h"
#include "gpio_sw.h"
#include "drv_comm.h"
#include "motion_sensor.h"
#include "motion_sensor_custom.h"
#include "us_timer.h"
#include "carMotion.h"
#include "mira_std.h"

/******************************************
	customizaton data
*******************************************/
MotionSensor_custom_data_struct  ms_custom_data_def = 
{          
   	/*X axis*/
   	ACC_0G_X,   
   	ACC_1G_X,
   	ACC_MINUS1G_X,     
    /*Y axis*/
   	ACC_0G_Y,   
   	ACC_1G_Y,
   	ACC_MINUS1G_Y,     
   	/*Z axis*/
   	ACC_0G_Z,   
   	ACC_1G_Z,
   	ACC_MINUS1G_Z    
};	

/*motion sensor pwr on flag*/
volatile kal_bool g_ms_is_power_on = KAL_TRUE;

/*motion sensor cord mapping define*/
/******************************************
XY_MIRROR_MAPPING 	for x cord and y cord mirror mapping
XY_90_MAPPING		for x cord and y cord clockwise 90 mapping
XY_180_MAPPING		for x cord and y cord clockwise 180 mapping
XY_270_MAPPING		for x cord and y cord clockwise 270 mapping
******************************************/
#define XY_MIRROR_MAPPING
#define Y_MINUS_MAPPING

#if defined(__IOT_LOCK__)
extern void bike_break_callback();
#endif
/******************************************
	Debug API for motion sensor
*******************************************/
//#define DBG_MOTION_SENSOR

#ifdef DBG_MOTION_SENSOR
#define ms_dbg_print kal_prompt_trace
#ifdef WIN32
#define ms_dbg_print_ps(...)
#else
//#define ms_dbg_print_ps dbg_print
#define ms_dbg_print_ps(...)
#endif
#else
#define ms_dbg_print(...)
#define ms_dbg_print_ps(...)
#endif


#define abs(x) (((x) < 0) ? -(x) : (x))

/******************************************
SD0=GND:
i2c_addr = 0x26;

SD0=High:
i2c_addr = 0x27;
*******************************************/	
kal_int8 i2c_addr = 0x27;
kal_uint32 MS_DELAY_TIME  = 100;
static unsigned int f_step = 0;
static kal_timerid g_pedometer_timerid = 0;

kal_bool mcube_i2c_read_buf(kal_uint8 reg, char* buf, kal_uint8 size)
{
	return KAL_TRUE;
}

/*return value: 0: is ok    other: is failed*/
int     i2c_read_byte_data( unsigned char addr, unsigned char *data){
	
		int ret = 0;

	        ms_i2c_configure((i2c_addr << 1), 300);
		ret = ms_i2c_receive((i2c_addr << 1),addr,data,1);
		return (!ret);
}

/*return value: 0: is ok    other: is failed*/
int     i2c_write_byte_data( unsigned char addr, unsigned char data){
		int ret = 0;
		
	        ms_i2c_configure((i2c_addr << 1), 300);
		ret =  ms_i2c_send((i2c_addr << 1),addr,&data,1);
		
		return (!ret);
}

/*return value: 0: is count    other: is failed*/
int     i2c_read_block_data( unsigned char base_addr, unsigned char count, unsigned char *data){
		int ret = 0;

		ret = ms_i2c_receive((i2c_addr << 1),base_addr,data,count);

		return count;
}

int mir3da_register_read( unsigned char addr, unsigned char *data){
    int     res = 0;

    res = i2c_read_byte_data(addr, data);
    kal_prompt_trace(MOD_MMI,"%s, res:%d, addr: %x, data:%x", __func__, res, addr, *data);
    if(res != 0) {
          return res;
    }	

    return res;
}

int mir3da_register_write( unsigned char addr, unsigned char data){
    int     res = 0;

    res = i2c_write_byte_data(addr, data);
    kal_prompt_trace(MOD_MMI,"%s, res:%d, addr: %x, data:%x", __func__, res, addr, data);
    if(res != 0) {
         return res;
    }

    return res;
}

int mir3da_register_read_continuously( unsigned char addr,unsigned char *data,unsigned char count)
{
    int     res = 0;

    res = (count==i2c_read_block_data(addr, count, data)) ? 0 : 1;
    if(res != 0) {
         return res;
    }

    return res;
}

int mir3da_register_mask_write(unsigned char addr, unsigned char mask, unsigned char data){
    int     res = 0;
    unsigned char      tmp_data;

    res = mir3da_register_read(addr, &tmp_data);
    if(res) {
        return res;
    }

    tmp_data &= ~mask; 
    tmp_data |= data & mask;
    res = mir3da_register_write(addr, tmp_data);

    return res;
}

s8_m sendEvent(carMotion_EVENT event,s32_m data)
{
  switch(event){
	case TEVENT_SLOPE_NOTIFY:	
		kal_prompt_trace(MOD_MMI,"iotlock TEVENT_SLOPE_NOTIFY");
		//车辆跌倒时，代码执行该处，打印输出上面log！
	break;
	
	case TEVENT_VIDEOING_NOTIFY:	
		kal_prompt_trace(MOD_MMI,"iotlock TEVENT_VIDEOING_NOTIFY");
		//产生震动时，代码执行该处，打印输出上面log！
	break;
	
	case TEVENT_PARKING_NOTIFY:
		kal_prompt_trace(MOD_MMI,"iotlock TEVENT_PARKING_NOTIFY");
	break;
	
	default:
	break;
  }
  
  return 0;
}

s8_m mir3da_save_cali_file(s16_m x, s16_m y,s16_m z)
{
	return 0;
}

s8_m mir3da_get_cali_file(s16_m *x, s16_m *y,s16_m *z)
{
	return 0;
}

u32_m mir3da_create_timer(void){
			
	if (g_pedometer_timerid == 0)
	{
		g_pedometer_timerid = kal_create_timer("mir3da Motion sensor");
	}
		
	return (u32_m)g_pedometer_timerid;
}

s8_m mir3da_start_timer(u32_m id, u32_m msDelay,u8_m *auto_reset,void(*pCallback)()){
	
	*auto_reset = 0;
	kal_cancel_timer(g_pedometer_timerid);   
	kal_set_timer(g_pedometer_timerid, (kal_timer_func_ptr)pCallback, NULL, 22, 0);
	
	return 0;
}

s8_m mir3da_cancel_timer(u32_m id){
	kal_cancel_timer(g_pedometer_timerid);  
}  

struct carMotion_op_s     ops_handle = {{PIN_TWO,PIN_LEVEL_HIGH,PIN_ONE,1},
										sendEvent, 
										{mir3da_register_read_continuously, mir3da_register_write},
										{mir3da_save_cali_file,mir3da_get_cali_file},
										{mir3da_create_timer,mir3da_start_timer,mir3da_cancel_timer},
										NULL};
										
/*return value: 0: is ok    other: is failed*/
void mir3da_init(void){
	int res = 0;
	unsigned char data=0;
	mir3da_register_write(NSA_REG_INT_PIN_CONFIG, 0);
	ms_i2c_configure((i2c_addr << 1), 300);
	mir3da_register_read(NSA_REG_WHO_AM_I,&data);
	if(data != 0x13){
	      i2c_addr = 0x27;
		  ms_i2c_configure((i2c_addr << 1), 300);
		  mir3da_register_read(NSA_REG_WHO_AM_I,&data);
	      if(data != 0x13){
	            ms_dbg_print_ps("------mir3da read chip id  error= %x-----\r\n",data); 
		      return;// -1;
	      	}
	}
	
	carMotion_Init(&ops_handle);
	
	carMotion_Direction_Set_Parma(1);
	
	//carMotion_Slope_Set_Parma(1,3,30,60);
	
	carMotion_Control(VIDEOING_T,ENABLE_T);
	carMotion_Control(PARKING_T,ENABLE_T);
	carMotion_Control(VIOLENT_T,DISABLE_T);
	carMotion_Control(STATE_T,DISABLE_T);
	carMotion_Control(JOLT_T,DISABLE_T);
	carMotion_Control(SLOPE_T,ENABLE_T);
	
	//mir3da_open_interrupt(1,5);
	
	mir3da_register_write(NSA_REG_INT_PIN_CONFIG, 0);

	if(i2c_addr == 0x26){
		mir3da_register_mask_write(0x8c, 0x40, 0x00);
	}
	mir3da_pwr_down();
  	return ;
}

int mir3da_set_enable(char enable)
{
		int res = 0;
		if(enable)
		res = mir3da_register_mask_write(NSA_REG_POWERMODE_BW,0xC0,0x5E);
		else	
		res = mir3da_register_mask_write(NSA_REG_POWERMODE_BW,0xC0,0x80);
	
	return res;	
}

void mir3da_pwr_up(void){
	
	mir3da_set_enable(1);
}

void mir3da_pwr_down(void){
	mir3da_set_enable(0);
}

//reference num : 1 or 2 th :5~255
int mir3da_open_interrupt(int num,unsigned char th){
	int   res = 0;

	res = mir3da_register_write(NSA_REG_INTERRUPT_SETTINGS1,0x87);
	res = mir3da_register_write(NSA_REG_ACTIVE_DURATION,0x00 );
	res = mir3da_register_write(NSA_REG_ACTIVE_THRESHOLD,th);
			
	switch(num){

		case 1:
			res = mir3da_register_write(NSA_REG_INTERRUPT_MAPPING1,0x04 );
			break;

		case 2:
			res = mir3da_register_write(NSA_REG_INTERRUPT_MAPPING3,0x04 );
			break;
			
		default :
			break;
	}

	return res;
}

int mir3da_close_interrupt(int num){
	int   res = 0;

	res = mir3da_register_write(NSA_REG_INTERRUPT_SETTINGS1,0x00 );
			
	switch(num){

		case 1:
			res = mir3da_register_write(NSA_REG_INTERRUPT_MAPPING1,0x00 );
			break;

		case 2:
			res = mir3da_register_write(NSA_REG_INTERRUPT_MAPPING3,0x00 );
			break;
		default :
			break;
	}

	return res;
}

void mir3da_read_adc(kal_int16 *x, kal_int16 *y, kal_int16 *z)
{
    unsigned char    tmp_data[6] = {0};

    if (mir3da_register_read_continuously(NSA_REG_ACC_X_LSB,  tmp_data,6) != 0) 
	{
        return;
    }
    
    *x = (kal_int16)((tmp_data[1] << 4) | (tmp_data[0]>> 4));
    *y = (kal_int16)((tmp_data[3] << 4) | (tmp_data[2]>> 4));
    *z = (kal_int16)((tmp_data[5] << 4) | (tmp_data[4]>> 4));

     kal_prompt_trace(MOD_TST,"oringnal adc %d %d %d ",*x,*y,*z); 	

    //return 0;
}

/*return value: 0: is ok    other: is failed*/
int mir3da_read_data(short *x, short *y, short *z)
{
    unsigned char    tmp_data[6] = {0};

    if (mir3da_register_read_continuously(NSA_REG_ACC_X_LSB, tmp_data,6) != 0) {
        return -1;
    }
    
    *x = ((short)(tmp_data[1] << 8 | tmp_data[0]))>> 4;
    *y = ((short)(tmp_data[3] << 8 | tmp_data[2]))>> 4;
    *z = ((short)(tmp_data[5] << 8 | tmp_data[4]))>> 4;	

    return 0;
}

MotionSensor_custom_data_struct* mir3da_get_data(void) 
{
   return (&ms_custom_data_def);
}

MotionSensor_customize_function_struct ms_custom_func =
{
    mir3da_get_data,
	mir3da_read_adc,
    mir3da_init,
    mir3da_pwr_up,
    mir3da_pwr_down
};

MotionSensor_customize_function_struct *ms_GetFunc(void)
{
   return (&ms_custom_func);  
}

/*return value: 0: is ok    other: is failed*/

kal_bool mir3da_read_data_test(void)
{
    unsigned char    tmp_data[6] = {0};

	mir3da_init();

    if (mir3da_register_read_continuously(NSA_REG_ACC_X_LSB, tmp_data,6) != 0) {
		
        return KAL_FALSE;
    }
    
    

    return KAL_TRUE;
}

#endif



