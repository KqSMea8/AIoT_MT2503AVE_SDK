#ifndef __MIR3DA_H__
#define __MIR3DA_H__

#include "motion_sensor.h"


#define CUST_VER                            ""                                          /* for Custom debug version */
#define CORE_VER                            "3.5.1_2017-02-22-11:00:00_"CUST_VER

extern const char gpio_ms_i2c_clk_pin;
extern const char gpio_ms_i2c_data_pin;

#define MS_SCL	gpio_ms_i2c_clk_pin
#define MS_SDA	gpio_ms_i2c_data_pin

/*****************************************************
	Set GPIO pins of Gsensor
******************************************************/		
#define MS_SDO	16
#define MS_CSB2	17
#define MS_DRDY	19

#define MS_INT1	24
#define MS_INT2	5 
#define MS_INT3	0

/*****************************************************
	ADC value configure of 0g 1g and -1g in X,Y,Z axis
******************************************************/				
#define ACC_0G_X		(2059)
#define ACC_1G_X		(2059+128)
#define ACC_MINUS1G_X	(2059+128)

#define ACC_0G_Y		(2059)
#define ACC_1G_Y		(2059+128)
#define ACC_MINUS1G_Y	(2059+128)

#define ACC_0G_Z		(2059)
#define ACC_1G_Z		(2059+128)
#define ACC_MINUS1G_Z	(2059+128)

#define MS_CLK_PIN_GPIO_MODE		GPIO_ModeSetup(MS_SCL,0)
#define	MS_DATA_PIN_GPIO_MODE		GPIO_ModeSetup(MS_SDA,0)
#define MS_I2C_CLK_OUTPUT			GPIO_InitIO(1,MS_SCL)
#define MS_I2C_DATA_OUTPUT			GPIO_InitIO(1,MS_SDA)
#define MS_I2C_DATA_INPUT		   	GPIO_InitIO(0,MS_SDA)
#define MS_I2C_CLK_HIGH				GPIO_WriteIO(1,MS_SCL)
#define MS_I2C_CLK_LOW				GPIO_WriteIO(0,MS_SCL)
#define MS_I2C_DATA_HIGH			GPIO_WriteIO(1,MS_SDA)
#define MS_I2C_DATA_LOW				GPIO_WriteIO(0,MS_SDA)
#define MS_I2C_GET_BIT				GPIO_ReadIO(MS_SDA)

                                                        
#define NSA_REG_SPI_I2C                 0x00
#define NSA_REG_WHO_AM_I                0x01
#define NSA_REG_ACC_X_LSB               0x02
#define NSA_REG_ACC_X_MSB               0x03
#define NSA_REG_ACC_Y_LSB               0x04
#define NSA_REG_ACC_Y_MSB               0x05
#define NSA_REG_ACC_Z_LSB               0x06
#define NSA_REG_ACC_Z_MSB               0x07
#define NSA_REG_MOTION_FLAG				0x09
#define NSA_REG_G_RANGE                 0x0f
#define NSA_REG_ODR_AXIS_DISABLE        0x10
#define NSA_REG_POWERMODE_BW            0x11
#define NSA_REG_SWAP_POLARITY           0x12
#define NSA_REG_FIFO_CTRL               0x14
#define NSA_REG_INTERRUPT_SETTINGS1     0x16
#define NSA_REG_INTERRUPT_SETTINGS2     0x17
#define NSA_REG_INTERRUPT_MAPPING1      0x19
#define NSA_REG_INTERRUPT_MAPPING2      0x1a
#define NSA_REG_INTERRUPT_MAPPING3      0x1b
#define NSA_REG_INT_PIN_CONFIG          0x20
#define NSA_REG_INT_LATCH               0x21
#define NSA_REG_ACTIVE_DURATION         0x27
#define NSA_REG_ACTIVE_THRESHOLD        0x28
#define NSA_REG_TAP_DURATION            0x2A
#define NSA_REG_TAP_THRESHOLD           0x2B
#define NSA_REG_CUSTOM_OFFSET_X         0x38
#define NSA_REG_CUSTOM_OFFSET_Y         0x39
#define NSA_REG_CUSTOM_OFFSET_Z         0x3a
#define NSA_REG_ENGINEERING_MODE        0x7f
#define NSA_REG_SENSITIVITY_TRIM_X      0x80
#define NSA_REG_SENSITIVITY_TRIM_Y      0x81
#define NSA_REG_SENSITIVITY_TRIM_Z      0x82
#define NSA_REG_COARSE_OFFSET_TRIM_X    0x83
#define NSA_REG_COARSE_OFFSET_TRIM_Y    0x84
#define NSA_REG_COARSE_OFFSET_TRIM_Z    0x85
#define NSA_REG_FINE_OFFSET_TRIM_X      0x86
#define NSA_REG_FINE_OFFSET_TRIM_Y      0x87
#define NSA_REG_FINE_OFFSET_TRIM_Z      0x88
#define NSA_REG_SENS_COMP               0x8c
#define NSA_REG_SENS_COARSE_TRIM        0xd1

void mir3da_init(void);
int mir3da_read_data(short *x, short *y, short *z);
extern MotionSensor_customize_function_struct *ms_GetFunc(void);
extern MotionSensor_custom_data_struct *ms_get_data(void);
void mir3da_pwr_up(void);
void mir3da_pwr_down(void);
#endif
