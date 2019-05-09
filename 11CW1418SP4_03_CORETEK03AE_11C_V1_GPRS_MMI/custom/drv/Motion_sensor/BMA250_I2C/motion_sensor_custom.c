/**************************************************************************************************/
/*  $Date: March-1-20111
 *   $Revision: 1.01$
 *
 */

/**************************************************************************************************
* Copyright (C) 2011 Bosch Sensortec GmbH
*
* BOSCH algorithm API
*
* Usage:		BOSCH portrait landscape and skake snap algorithm
*
* Author:       yongsheng.gao@bosch-sensortec.com
**************************************************************************************************/
/*  Disclaimer
*
* Common:
* Bosch Sensortec products are developed for the consumer goods industry. They may only be used
* within the parameters of the respective valid product data sheet.  Bosch Sensortec products are
* provided with the express understanding that there is no warranty of fitness for a particular purpose.
* They are not fit for use in life-sustaining, safety or security sensitive systems or any system or device
* that may lead to bodily harm or property damage if the system or device malfunctions. In addition,
* Bosch Sensortec products are not fit for use in products which interact with motor vehicle systems.
* The resale and/or use of products are at the purchaser抯 own risk and his own responsibility. The
* examination of fitness for the intended use is the sole responsibility of the Purchaser.
*
* The purchaser shall indemnify Bosch Sensortec from all third party claims, including any claims for
* incidental, or consequential damages, arising from any product use not covered by the parameters of
* the respective valid product data sheet or not approved by Bosch Sensortec and reimburse Bosch
* Sensortec for all costs in connection with such claims.
*
* The purchaser must monitor the market for the purchased products, particularly with regard to
* product safety and inform Bosch Sensortec without delay of all security relevant incidents.
*
* Engineering Samples are marked with an asterisk (*) or (e). Samples may vary from the valid
* technical specifications of the product series. They are therefore not intended or fit for resale to third
* parties or for use in end products. Their sole purpose is internal client testing. The testing of an
* engineering sample may in no way replace the testing of a product series. Bosch Sensortec
* assumes no liability for the use of engineering samples. By accepting the engineering samples, the
* Purchaser agrees to indemnify Bosch Sensortec from all claims arising from the use of engineering
* samples.
*
* Special:
* This software module (hereinafter called "Software") and any information on application-sheets
* (hereinafter called "Information") is provided free of charge for the sole purpose to support your
* application work. The Software and Information is subject to the following terms and conditions:
*
* The Software is specifically designed for the exclusive use for Bosch Sensortec products by
* personnel who have special experience and training. Do not use this Software if you do not have the
* proper experience or training.
*
* This Software package is provided `` as is `` and without any expressed or implied warranties,
* including without limitation, the implied warranties of merchantability and fitness for a particular
* purpose.
*
* Bosch Sensortec and their representatives and agents deny any liability for the functional impairment
* of this Software in terms of fitness, performance and safety. Bosch Sensortec and their
* representatives and agents shall not be liable for any direct or indirect damages or injury, except as
* otherwise stipulated in mandatory applicable law.
*
* The Information provided is believed to be accurate and reliable. Bosch Sensortec assumes no
* responsibility for the consequences of use of such Information nor for any infringement of patents or
* other rights of third parties which may result from its use. No license is granted by implication or
* otherwise under any patent or patent rights of Bosch. Specifications mentioned in the Information are
* subject to change without notice.
*
* It is not allowed to deliver the source code of the Software to any third party without permission of
* Bosch Sensortec.
*/
/*************************************************************************************************/


#include "kal_release.h"
#include "gpio_sw.h"
#include "motion_sensor.h"
#include "bma250.h"
#include "motion_sensor_custom.h"

#ifdef MCU_208M
   #define i2c_delay   0 //40
#elif defined(MCU_104M)
   #define i2c_delay   0
#elif defined(MCU_52M)
   #define i2c_delay   0
#else
   #define i2c_delay   0
#endif

#ifdef MCU_208M
   #define delay_short 20//100
   #define delay_long  40//200
#elif defined(MCU_104M)
   #define delay_short 10
   #define delay_long  20
#elif defined(MCU_52M)
   #define delay_short 0		//No test
   #define delay_long  0		//No test
#else
	#define delay_short 20		 //for test
	#define delay_long	40		 //for test
#endif

kal_uint32 MS_DELAY_TIME  = 100;


#define EDGE_SENSITIVE           	KAL_TRUE
#define LEVEL_SENSITIVE          	KAL_FALSE

//#define InterruptPin_Config()		GPIO_ModeSetup(MOTION_SENSOR_EINT_PORT, 3)

#define SET_I2C_CLK_OUTPUT 		GPIO_InitIO(OUTPUT,ACC_SENSOR_SCK); {int j; for(j=0;j<i2c_delay;j++);}
#define SET_I2C_DATA_OUTPUT		GPIO_InitIO(OUTPUT,ACC_SENSOR_SDA);{int j; for(j=0;j<i2c_delay;j++);}
#define SET_I2C_DATA_INPUT		GPIO_InitIO(INPUT,ACC_SENSOR_SDA);{int j; for(j=0;j<i2c_delay;j++);}
#define SET_I2C_CLK_HIGH		GPIO_WriteIO(1,ACC_SENSOR_SCK); {int j; for(j=0;j<i2c_delay;j++);}
#define SET_I2C_CLK_LOW			GPIO_WriteIO(0,ACC_SENSOR_SCK); {int j; for(j=0;j<i2c_delay;j++);}
#define SET_I2C_DATA_HIGH		GPIO_WriteIO(1,ACC_SENSOR_SDA);{int j; for(j=0;j<i2c_delay;j++);}
#define SET_I2C_DATA_LOW		GPIO_WriteIO(0,ACC_SENSOR_SDA);{int j; for(j=0;j<i2c_delay;j++);}
#define GET_I2C_DATA_BIT		GPIO_ReadIO(ACC_SENSOR_SDA)

//defination only for MTK platform compiler
#define ACC_0G_X		(2059)
#define ACC_1G_X		(2059+128)
#define ACC_MINUS1G_X	(2059+128)

#define ACC_0G_Y		(2059)
#define ACC_1G_Y		(2059+128)
#define ACC_MINUS1G_Y	(2059+128)

#define ACC_0G_Z		(2059)
#define ACC_1G_Z		(2059+128)
#define ACC_MINUS1G_Z	(2059+128)

const kal_uint8 BMA250_EINT_NO = 12;		//BMA250_INT2

extern kal_int32 EINT_SW_Debounce_Modify(kal_uint8 eintno, kal_uint8 debounce_time);
extern void EINT_Mask(kal_uint8 eintno);
extern void EINT_Registration(kal_uint8 eintno, kal_bool Dbounce_En, kal_bool ACT_Polarity, void (reg_hisr)(void), kal_bool auto_umask);
extern kal_uint32 EINT_Set_Sensitivity(kal_uint8 eintno, kal_bool sens);

void bosch_motion_sensor_eint_hisr(void)
{
   kal_int8 int_has_opened = -1;
   EINT_Mask(BMA250_EINT_NO);
   int_has_opened = GPIO_ReadIO(BMA250_EINT_NO);
   kal_prompt_trace(MOD_MMI, "Interrupt!! GPIO:%d", int_has_opened);
   EINT_UnMask(BMA250_EINT_NO);
}

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

MotionSensor_customize_function_struct ms_custom_func=
{
     ms_get_data,
     acc_sensor_get_6bytes,
     acc_sensor_init_with_interrupt,			// with INT out
     acc_sensor_pwr_up,
     acc_sensor_pwr_down
};

bma250_t t_bma250;

static void acc_sensor_interrput_configuration(void)
{
   EINT_Set_HW_Debounce(BMA250_EINT_NO, 2);
   EINT_Registration(BMA250_EINT_NO, KAL_TRUE, ACT_LOW,
                     bosch_motion_sensor_eint_hisr, KAL_TRUE);
   EINT_Set_Sensitivity(BMA250_EINT_NO, EDGE_SENSITIVE);
   EINT_Set_Polarity(BMA250_EINT_NO, KAL_FALSE);
}

static void acc_sensor_interrput_disable(void)
{
	unsigned char i;
	bma250_set_mode(0x00);

	for (i=0; i<11; i++)
		bma250_set_Int_Enable(i, 0);

	bma250_reset_interrupt();
}

void acc_sensor_write_1byte(kal_uint8 i2c_addr,kal_int8 addr, kal_int8 data)
{
	ms_i2c_configure((i2c_addr << 1), 300);
	ms_i2c_send((i2c_addr << 1),addr,&data,1);
}

//In fact in this version's driver, only support 1 byte write
//If needed multi bytes write, must modify this function
char acc_sensor_write_bytes(kal_uint8 dev_addr, kal_uint8 reg_addr,
                            kal_uint8 *reg_data, kal_uint8 wr_len)
{
   int i = 0;
   for (i = 0; i < wr_len; i++)
   {
      acc_sensor_write_1byte(dev_addr, (reg_addr + i), *(reg_data + i));
   }

   return 1;
}

char acc_sensor_read_bytes(kal_uint8 i2c_addr, kal_uint8 base_addr, kal_uint8
                           *data, kal_uint8 count)
{
   int ret = 0;
   ms_i2c_configure((i2c_addr << 1), 300);
   ret = ms_i2c_receive((i2c_addr << 1),base_addr,data,count);
   return count;
} /* I2C_send_byte() */

void System_Delay_1ms(kal_uint8 delay_count)
{
#if 0
   kal_uint32 x;
   volatile kal_uint32 delay;

   #define INSTRUCTION_CLK_NUM 4
   #ifdef MCU_13M //   CLK = 1/13us 1ms=CLK*13000=1ms 1 ins= 6CLK
      #define MCU_CLK_HZ 13000
   #elif defined(MCU_26M)
      #define MCU_CLK_HZ 26000
   #elif defined(MCU_39M)
      #define MCU_CLK_HZ 39000
   #elif defined(MCU_52M)
      #define MCU_CLK_HZ 52000
   #elif defined(MCU_104M)
      #define MCU_CLK_HZ 104000
   #elif defined(MCU_208M)
      #define MCU_CLK_HZ 208000
   #else
      #define MCU_CLK_HZ 260000
   #endif

   for (delay = 0; delay < delay_count; delay++)
   {
      for (x = 0; x < (MCU_CLK_HZ / INSTRUCTION_CLK_NUM); x++){}
   }
#endif
	ms_i2c_udelay(1000);
}

void acc_sensor_pwr_up(void)
{
   bma250_set_mode(bma250_MODE_NORMAL);
   System_Delay_1ms(10);
   kal_prompt_trace(MOD_MMI, "%s:BMA250 PWR_ON", __FUNCTION__);
}

void acc_sensor_pwr_down(void)
{
   bma250_set_mode(bma250_MODE_SUSPEND);
   System_Delay_1ms(1);
   kal_prompt_trace(MOD_MMI, "%s:BMA250 PWR_OFF", __FUNCTION__);
}

void acc_sensor_init(void)
{
	static kal_uint8 p_flag = 0;
	System_Delay_1ms(1);
	if (p_flag == 0)
	{
	  p_flag=1;
	  bma250_init(&t_bma250);
	}
	acc_sensor_pwr_up();
	acc_sensor_interrput_disable();

	bma250_set_range(bma250_RANGE_2G);
	bma250_set_bandwidth(bma250_BW_15_63HZ);
	{
#if 1//def __BOSCH_BMA250_INTERRUPT_DEBUG__
	  unsigned char chip_id = 0;
	  bma250_get_chip_id(&chip_id);
	  //kal_prompt_trace(MOD_MMI, "%s  -- acc_sensor_init line 586", __FUNCTION__);
#endif
	}
	{
#ifdef __BOSCH_BMA250_INTERRUPT_DEBUG__
		bma250acc_t accel;
		bma250_read_accel_xyz(&accel);
		kal_prompt_trace(MOD_MMI, "%s:acc_x: %d, acc_y: %d, acc_z: %d", accel.x, accel.y, accel.z, __FUNCTION__);
#endif 
	}
}

int motion_sensor_state = -1;
/** lock state use **/
void acc_sensor_init_with_interrupt(void)
{
	static kal_uint8 p_flag = 0;
	kal_uint8 chip_id = 0;
	System_Delay_1ms(1);
	bma250_init(&t_bma250);
	acc_sensor_pwr_up();
	acc_sensor_interrput_disable();

	bma250_set_range(bma250_RANGE_2G);
	bma250_set_bandwidth(bma250_BW_31_25HZ);

	// normal settings
	//bma250_set_range(bma250_RANGE_8G);			// 4G
	//bma250_set_bandwidth(bma250_BW_125HZ);		//125HZ
	bma250_set_Int_Mode(0x02);  // latch 2S
	bma250_set_slope_duration(1);		// 2
	bma250_set_slope_threshold(40);		//32

	// set interrupt latch duration.
	//bma250_set_Int_Mode(0x0B);	// latch 1ms

#if 1
	// map all interrupt to INT1
	//bma250_set_int_data_sel(0);	// map new data interrupt to INT1.
	//bma250_set_int1_pad_sel(0);	// low G 		--> INT1
	//bma250_set_int1_pad_sel(1);	// high G 		--> INT1
	//bma250_set_int1_pad_sel(2);	// slope 		--> INT1
	//bma250_set_int1_pad_sel(3);	// double tap 	--> INT1
	//bma250_set_int1_pad_sel(4);	// single tap 	--> INT1
	//bma250_set_int1_pad_sel(5);	// orient 		--> INT1
	bma250_set_int1_pad_sel(6);	// flat 		--> INT1
	//bma250_set_int2_pad_sel();

	// set INT1 trigger level, push pull and open drive

	//bma250_set_int_set(2, 1);	// INT2 level high
	//bma250_set_int_set(3, 0);	// INT2 push pull(0), open drive(1)
	//bma250_set_int_src();

	bma250_set_int_set(0, 0);	// INT1 level high
	bma250_set_int_set(1, 1);	// INT1 push pull(0), open drive(1)

	bma250_set_int_set(2, 0);	// INT2 level high
	bma250_set_int_set(3, 1);	// INT2 push pull(0), open drive(1)
#endif

	// map all interrupt to INT2
	//bma250_set_int_data_sel(1);	// map new data interrupt to INT2.
	//bma250_set_int2_pad_sel(0);	// low G 		--> INT2
	//bma250_set_int2_pad_sel(1);	// high G 		--> INT2
	bma250_set_int2_pad_sel(2);	// slope 		--> INT2
	//bma250_set_int2_pad_sel(3);	// double tap 	--> INT2
	//bma250_set_int2_pad_sel(4);	// single tap 	--> INT2
	//bma250_set_int2_pad_sel(5);	// orient 		--> INT2
	//bma250_set_int2_pad_sel(6);	// flat 		--> INT2
	//bma250_set_int2_pad_sel();

	// set INT trigger level, push pull and open drive
	//bma250_set_int_set(0, 0);	// INT1 level high
	//bma250_set_int_set(1, 0);	// INT1 push pull(0), open drive(1)

	//bma250_set_int_src();

	// example, set slope(any motion) interrupt

	//bma250_reset_interrupt();
	bma250_set_enable_tap_interrupt(3);		//enable flat INT
	bma250_set_enable_slope_interrupt(3);		// enable XYZ INT
	//bma250_reset_interrupt();

	bma250_get_chip_id(&chip_id);
	kal_prompt_trace(MOD_MMI, "%s:acc_sensor ID:%x", __FUNCTION__, chip_id);
	if(motion_sensor_state == -1)
	{
		if (chip_id != 0xFA) 
		{
			motion_sensor_state = 0;		//G-sensor 异常
		}
		else
		{
			motion_sensor_state = 1;		//G-sensor 正常
		}
	}
	kal_prompt_trace(MOD_MMI, "%s:motion_sensor_state:%d", __FUNCTION__, motion_sensor_state);
	kal_prompt_trace(MOD_MMI, "%s:acc_sensor Init OK!", __FUNCTION__);
}

/** unlock state use **/
void acc_sensor_init_violent(void)
{
	static kal_uint8 p_flag = 0;
	kal_uint8 chip_id = 0;
	System_Delay_1ms(1);
	bma250_reset_interrupt();
	bma250_init(&t_bma250);
	acc_sensor_pwr_up();
	acc_sensor_interrput_disable();

	bma250_set_range(bma250_RANGE_16G);
	bma250_set_bandwidth(bma250_BW_31_25HZ);

	// normal settings
	//bma250_set_range(bma250_RANGE_8G);			// 4G
	//bma250_set_bandwidth(bma250_BW_125HZ);		//125HZ

	//bma250_set_Int_Mode(0x03);  // latch 1S
#if 1
	//bma250_set_slope_duration(3);		// 2
	//bma250_set_slope_threshold(254);		//32
#endif
	// set interrupt latch duration.
	//bma250_set_Int_Mode(0x0B);	// latch 1ms

#if 1
	// map all interrupt to INT1
	//bma250_set_int_data_sel(0);	// map new data interrupt to INT1.
	//bma250_set_int1_pad_sel(0);	// low G 		--> INT1
	bma250_set_int1_pad_sel(1);	// high G 		--> INT1
	//bma250_set_int1_pad_sel(2);	// slope 		--> INT1
	//bma250_set_int1_pad_sel(3);	// double tap 	--> INT1
	//bma250_set_int1_pad_sel(4);	// single tap 	--> INT1
	//bma250_set_int1_pad_sel(5);	// orient 		--> INT1
	//bma250_set_int1_pad_sel(6);	// flat 		--> INT1
	//bma250_set_int2_pad_sel();

	// set INT1 trigger level, push pull and open drive

	//bma250_set_int_set(2, 1);	// INT2 level high
	//bma250_set_int_set(3, 0);	// INT2 push pull(0), open drive(1)
	//bma250_set_int_src();

	bma250_set_int_set(0, 0);	// INT1 level high
	bma250_set_int_set(1, 1);	// INT1 push pull(0), open drive(1)

	//bma250_set_int_set(2, 0);	// INT2 level high
	//bma250_set_int_set(3, 1);	// INT2 push pull(0), open drive(1)
#endif

	// map all interrupt to INT2
	//bma250_set_int_data_sel(1);	// map new data interrupt to INT2.
	//bma250_set_int2_pad_sel(0);	// low G 		--> INT2
	//bma250_set_int2_pad_sel(1);	// high G 		--> INT2
	//bma250_set_int2_pad_sel(2);	// slope 		--> INT2
	//bma250_set_int2_pad_sel(3);	// double tap 	--> INT2
	//bma250_set_int2_pad_sel(4);	// single tap 	--> INT2
	//bma250_set_int2_pad_sel(5);	// orient 		--> INT2
	//bma250_set_int2_pad_sel(6);	// flat 		--> INT2
	//bma250_set_int2_pad_sel();

	// set INT trigger level, push pull and open drive
	//bma250_set_int_set(0, 0);	// INT1 level high
	//bma250_set_int_set(1, 0);	// INT1 push pull(0), open drive(1)

	//bma250_set_int_src();

	// example, set slope(any motion) interrupt
#if 0
	bma250_set_tap_duration(0x47);
	bma250_set_tap_threshold(0xDF);
#endif
	//bma250_set_enable_tap_interrupt(3);		//enable flat INT
	//bma250_set_enable_slope_interrupt(0);		// enable XYZ INT
	bma250_set_enable_high_g_interrupt(3);		// enable X
	bma250_set_high_g_duration(0x0F);
	bma250_set_high_g_threshold(0x28);
	bma250_set_high_hy(0x81);	//0x2

	//bma250_reset_interrupt();

	bma250_get_chip_id(&chip_id);
	kal_prompt_trace(MOD_MMI, "%s:acc_sensor ID:%x", __FUNCTION__, chip_id);
	if(motion_sensor_state == -1)
	{
		if (chip_id != 0xFA)
		{
			motion_sensor_state = 0;		//G-sensor 异常
		}
		else
		{
			motion_sensor_state = 1;		//G-sensor 正常
		}
	}
	kal_prompt_trace(MOD_MMI, "%s:motion_sensor_state:%d", __FUNCTION__, motion_sensor_state);
	kal_prompt_trace(MOD_MMI, "%s:acc_sensor_init_violent Init OK2!", __FUNCTION__);
	//Bma250_read_reg_data();
}

//This function only for MTK platfrom compiler
void acc_sensor_get_6bytes(kal_uint16 *accl_x,kal_uint16 *accl_y,kal_uint16 *accl_z)
{
	bma250acc_t accl;
	bma250_read_accel_xyz(&accl);

	*accl_x=accl.x;
	*accl_y=accl.y;
	*accl_z=accl.z;
}

void acc_sensor_get_xyz(kal_int16 *accl_x,kal_int16 *accl_y,kal_int16 *accl_z)
{
	bma250acc_t accl;
	bma250_read_accel_xyz(&accl);

	*accl_x=accl.x;
	*accl_y=accl.y;
	*accl_z=accl.z;
	//kal_prompt_trace(MOD_MMI, "%s:accl_x: %d, accl_y: %d, accl_z: %d", __FUNCTION__, *accl_x, *accl_y, *accl_z);
}

MotionSensor_custom_data_struct *ms_get_data(void)
{
   return (&ms_custom_data_def);
}

MotionSensor_customize_function_struct *ms_GetFunc(void)
{
   return (&ms_custom_func);
}

kal_bool Bma250_read_reg_data(void)
{
	bma250acc_t accel;
	int i = 0,j=0;
	//unsigned char dat[63];
	unsigned char data = 0x0;
	//data=dat;
	bma250_read_reg(0x0F,&data,1);
	kal_prompt_trace(MOD_MMI,"reg 0x0F = %x",data);
	bma250_read_reg(0x10,&data,1);
	kal_prompt_trace(MOD_MMI,"reg 0x10 = %x",data);

	bma250_read_reg(0x17,&data,1);
	kal_prompt_trace(MOD_MMI,"reg 0x17 = %x",data);
	bma250_read_reg(0x1A,&data,1);
	kal_prompt_trace(MOD_MMI,"reg 0x1A = %x",data);
	bma250_read_reg(0x1B,&data,1);
	kal_prompt_trace(MOD_MMI,"reg 0x1B = %x",data);
	bma250_read_reg(0x1C,&data,1);
	kal_prompt_trace(MOD_MMI,"reg 0x1C = %x",data);

	bma250_read_reg(0x24,&data,1);
	kal_prompt_trace(MOD_MMI,"reg 0x24 = %x",data);
	bma250_read_reg(0x25,&data,1);
	kal_prompt_trace(MOD_MMI,"reg 0x25 = %x",data);
	bma250_read_reg(0x26,&data,1);
	kal_prompt_trace(MOD_MMI,"reg 0x26 = %x",data);

	bma250_read_accel_xyz(&accel);
	kal_prompt_trace(MOD_MMI, "%s:acc_x: %d, acc_y: %d, acc_z: %d", __FUNCTION__, accel.x, accel.y, accel.z);

#if 0
	for(i=0;i<2;i++)
	{
		bma250_read_reg(0x00,data,0x3f);
		for(j=0;j<63;j++)
		{
			kal_prompt_trace(MOD_MMI,"reg data[%d]: %x",j,data[j]);
		}
		bma250_read_accel_xyz(&accel);
		kal_prompt_trace(MOD_MMI, "%s:acc_x: %d, acc_y: %d, acc_z: %d", __FUNCTION__, accel.x, accel.y, accel.z);
		//System_Delay_1ms(1);
	}
#endif
}

/*return value: 0: is ok    other: is failed*/

kal_bool Bma250_read_data_test(void)
{
#if 0
	bma250acc_t accel;
	int i = 0,j=0;
	unsigned char dat[63];
	unsigned char *data;
	data=dat;
	//acc_sensor_init();
	//acc_sensor_init_with_interrupt();
	for(i=0;i<5;i++)
	{
		bma250_read_reg(0x00,data,0x3f);
		for(j=0;j<63;j++)
		{
			kal_prompt_trace(MOD_MMI,"reg data[%d]: %x",j,data[j]);
		}
		bma250_read_accel_xyz(&accel);
		kal_prompt_trace(MOD_MMI, "%s:acc_x: %d, acc_y: %d, acc_z: %d", __FUNCTION__, accel.x, accel.y, accel.z);
		System_Delay_1ms(10);
	}
#endif
	bma250acc_t accl;
	char buffer[128+1];

	acc_sensor_init_with_interrupt();

	bma250_read_accel_xyz(&accl);
	kal_prompt_trace(MOD_MMI, "%s:accl_x: %d, accl_y: %d, accl_z: %d", __FUNCTION__, accl.x, accl.y, accl.z);

	sprintf(buffer, "\r\n+EGSENSOR:x:%d,y:%d,z:%d\r\n",accl.x, accl.y, accl.z);
	rmmi_write_to_uart((kal_uint8*)buffer, strlen(buffer), KAL_TRUE);

	return KAL_TRUE;
}

void Bma250_read_data_xyz(void)
{

	bma250acc_t accl;
	char buffer[128+1];

	acc_sensor_init_with_interrupt();

	bma250_read_accel_xyz(&accl);
	kal_prompt_trace(MOD_MMI, "%s:accl_x: %d, accl_y: %d, accl_z: %d", __FUNCTION__, accl.x, accl.y, accl.z);

	sprintf(buffer, "GSENSOR=x:%d,y:%d,z:%d",accl.x, accl.y, accl.z);
	rmmi_write_to_uart((kal_uint8*)buffer, strlen(buffer), KAL_TRUE);

}

