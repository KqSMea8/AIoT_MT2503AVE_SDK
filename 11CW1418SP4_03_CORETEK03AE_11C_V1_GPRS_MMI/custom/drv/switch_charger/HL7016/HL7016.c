/*****************************************************************************
* Copyright(c) Sprocomm, 2017. All rights reserved.
*	
* TI  BQ25601 charger driver
* File: bluewhale_charger.c

* This program is free software and can be edistributed and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*	
* This Source Code Reference Design for O2MICRO OZ63810G charger access (\Reference Design\) 
* is sole for the use of PRODUCT INTEGRATION REFERENCE ONLY, and contains confidential 
* and privileged information of O2Micro International Limited. O2Micro shall have no 
* liability to any PARTY FOR THE RELIABILITY, SERVICEABILITY FOR THE RESULT OF PRODUCT 
* INTEGRATION, or results from: (i) any modification or attempted modification of the 
* Reference Design by any party, or (ii) the combination, operation or use of the 
* Reference Design with non-O2Micro Reference Design.
*****************************************************************************/
/**************************************************************
* #include section
* add #include here if any
**************************************************************/
//#if defined(BQ25601_SWCHR_SUPPORT)
#if 1

#include "bluewhale_charger.h"
#include "bmt_utility.h"

/**************************************************************
* #define section
* add constant #define here if any
**************************************************************/
#define		RETRY_CNTR_MID		10000
/**************************************************************
* ADT section
* add Abstract Data Type definition here
**************************************************************/
/**************************************************************
* extern variable declaration section
**************************************************************/
/**************************************************************
* global variables section
* add declaration of global variables here
**************************************************************/
bluewhale_platform_setup_t bluewhale_pdate_setup;
bluewhale_data_t	bluewhale_data;
char ilimit_i = 0;
char find_ilimit = 0;
char set_ilimit = 0;

kal_int8 bq25601_i2c_addr = 0x6B;		//bq25601 iic slave addr

int16_t bluewhale_set_ilimit(void);

extern void rmmi_write_to_uart(kal_uint8 * buffer, kal_uint16 length, kal_bool stuff); 

//#define bluewhale_dbg(fmt, args...) printk(DBG_LEVEL"[bluewhale]:"pr_fmt(fmt)"\n", ## args)
#define uart_dbg(fmt, args...) \
do{\
    char buffer[100];\
    sprintf (buffer,fmt"\n", ##args);\
    rmmi_write_to_uart((kal_uint8*)buffer, strlen(buffer), KAL_TRUE);\
}while(0)

#define OZ115_TRACE (1)


/**************************************************************
* local variables section
* define local variables (will be referred only in this file) here,
* static keyword should be used to limit scope of local variable to this file
**************************************************************/
/**************************************************************
* Function prototypes section
* add function prototype here if any
**************************************************************/
/**************************************************************
* Private Functions
**************************************************************/
static int16_t bluewhale_set_t34_cv(uint16_t chgvolt_mv);
static int16_t bluewhale_set_t45_cv(uint16_t chgvolt_mv);

/*return value: 0: is ok    other: is failed*/
static int i2c_read_byte_data( unsigned char addr, unsigned char *data){

	int ret = 0;
    ms_i2c_configure((bq25601_i2c_addr << 1), 300);
	ret = ms_i2c_receive((bq25601_i2c_addr << 1),addr,data,1);

	return (!ret);
}

/*return value: 0: is ok    other: is failed*/
static int i2c_write_byte_data( unsigned char addr, unsigned char data){
	int ret = 0;
    ms_i2c_configure((bq25601_i2c_addr << 1), 300);
	ret =  ms_i2c_send((bq25601_i2c_addr << 1),addr,&data,1);

	return (!ret);
}


void bq25601_read_chip_id(void)
{
	unsigned char chip_id;
	i2c_read_byte_data(BQ24195_CON10, &chip_id);
	kal_prompt_trace(MOD_BMT,"%s, bq25601 chip_id:%x", __func__, chip_id);
}
/*****************************************************************************
 * Description:
 *		bluewhale_read_byte 
 * Parameters:
 *		index:	register index to be read
 *		*dat:	buffer to store data read back
 * Return:
 *      zero on success, others are error
 *****************************************************************************/
static uint16_t bluewhale_read_byte(uint8_t index, uint8_t *dat)
{
	uint16_t ret;
	uint8_t tmp;
	int16_t i = 0;

	for (i = 0; i < RETRY_CNT; i++)
	{
		ret = i2c_read_byte_data(index, &tmp);
		if(ret == 0) break;
	}
	if (i >= RETRY_CNT) {
		//kal_prompt_trace(MOD_BMT,"%s, res:%d, addr: %x, data:%x", __func__, ret, index, tmp);
		return ret;
	}
	*dat = tmp;
	return ret;
}

/*****************************************************************************
 * Description:
 *		bluewhale_write_byte 
 * Parameters:
 *		index:	register index to be write
 *		dat:		write data
 * Return:
 *      zero on success, others are error
 *****************************************************************************/
static uint16_t bluewhale_write_byte(uint8_t index, uint8_t dat)
{
	uint16_t ret;
	int16_t i = 0;

	for (i = 0; i < RETRY_CNT; i++)
	{
		ret = i2c_write_byte_data(index, dat);
		if(ret == 0) break;
	}
	if(i >= RETRY_CNT) {
		//kal_prompt_trace(MOD_BMT,"%s, res:%d, addr: %x, data:%x", __func__, ret, index, dat);
		return ret;
	}
	return ret;
}

static int16_t bluewhale_update_bits( uint8_t reg, uint8_t mask, uint8_t data)
{
	uint16_t ret;
	uint8_t tmp;
#if 0
	ret = bluewhale_read_byte(reg, &tmp);
	if (ret != 0)
		return ret;
	if ((tmp & mask) != data)
	{
		tmp &= ~mask;
		tmp |= data & mask;
		return bluewhale_write_byte(reg, tmp);
	}
	else
#endif
		return 0;
}

/*****************************************************************************
 * Description:
 *		bluewhale_set_wakeup_current 
 * Parameters:
 *		wak_ma:	set wakeup current
 *		Only 100mA, 120mA, ... , 400mA can be accurate
 * Return:
 *      zero on success.
 *****************************************************************************/
static int16_t bluewhale_set_wakeup_current(uint16_t wak_ma)
{
	int16_t ret = 0;
	uint8_t wak_curr = 0; 
#if 0
	if (wak_ma < 100)
		wak_ma = 100;		//limit to 100mA
	if (wak_ma > 400)
		wak_ma = 400;		//limit to 400mA
	wak_curr = wak_ma / WK_CURRT_STEP;	//step is 10mA

	ret = bluewhale_write_byte(REG_WAKEUP_CURRENT, wak_curr);
	return (-ret);
#endif
    ret = bluewhale_write_byte(BQ24195_CON3, 0x11);	
	return (-ret); 
}

/*****************************************************************************
 * Description:
 *		bluewhale_set_t34_cv
 * Parameters:
 *		chgvolt_mv:	charge voltage to be written at t34
 * Return:
 *      zero on success.
 *****************************************************************************/
static int16_t bluewhale_set_t34_cv(uint16_t chgvolt_mv)
{
	int16_t ret = 0;
	uint8_t chg_volt = 0; 
#if 0
	if (chgvolt_mv < 4000)
		chgvolt_mv = 4000;		//limit to 4.0V
	else if (chgvolt_mv > 4600)
		chgvolt_mv = 4600;		//limit to 4.6V

	chg_volt = (chgvolt_mv - 4000) / CHG_VOLT_STEP;	//step is 25mV

       ret = bluewhale_update_bits(REG_T34_CHG_VOLTAGE, _BITMASK(5), chg_volt);
       return (-ret);
#endif
	   ret = bluewhale_write_byte(BQ24195_CON4, 0xB2); 
	   return (-ret); 

}

/*****************************************************************************
 * Description:
 *		bluewhale_set_t45_cv 
 * Parameters:
 *		chgvolt_mv:	charge voltage to be written at t45
 * Return:
 *      zero on success.
 *****************************************************************************/
static int16_t bluewhale_set_t45_cv(uint16_t chgvolt_mv)
{
	int16_t ret = 0;
	uint8_t chg_volt = 0; 
#if 0
	if (chgvolt_mv < 4000)
		chgvolt_mv = 4000;		//limit to 4.0V
	else if (chgvolt_mv > 4600)
		chgvolt_mv = 4600;		//limit to 4.6V

	chg_volt = (chgvolt_mv - 4000) / CHG_VOLT_STEP;	//step is 25mV
	
    ret = bluewhale_update_bits( REG_T45_CHG_VOLTAGE, _BITMASK(5), chg_volt);
    return (-ret);
#endif
    ret = bluewhale_write_byte(BQ24195_CON4, 0xB2);	
	return (-ret); 
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/*****************************************************************************
 * Description:
 *		bluewhale_set_min_vsys 
 * Parameters:
 *		min_vsys_mv:	min sys voltage to be written
 * Return:
 *      zero on success.
 *****************************************************************************/
int16_t bluewhale_set_min_vsys(uint16_t min_vsys_mv)
{
	int16_t ret = 0;
	uint8_t vsys_val = 0; 
#if 0
	if (min_vsys_mv < 1800)
		min_vsys_mv = 1800;		//limit to 1.8V
	else if (min_vsys_mv > 3600)
		min_vsys_mv = 3600;		//limit to 3.6V
	vsys_val = min_vsys_mv / VSYS_VOLT_STEP;	//step is 200mV
	
    ret = bluewhale_update_bits( REG_MIN_VSYS_VOLTAGE, _BITMASK(5), vsys_val);
    return (-ret);
#endif 
   
	ret = bluewhale_write_byte(BQ24195_CON0, 0x1B);	
	return (-ret); 
}

/*****************************************************************************
 * Description:
 *		bluewhale_get_min_vsys 
 * Parameters:
 *		n/a
 * Return:
 *      min_vsys in mV.
 *****************************************************************************/
int16_t bluewhale_get_min_vsys(void)
{
#if 0
	uint8_t data;
	int16_t ret = 0;
	ret = bluewhale_read_byte(REG_MIN_VSYS_VOLTAGE, &data);
	if (ret != 0)
		return (-ret);
	//data &= 0x1f;
    data &= _BITMASK(5);
	return (data * VSYS_VOLT_STEP);	//step is 200mV
#endif
   return 0; 
}

/*****************************************************************************
 * Description:
 *		bluewhale_set_chg_volt 
 * Parameters:
 *		chgvolt_mv:	charge voltage to be written
 * Return:
 *      zero on success.
 *****************************************************************************/
int16_t bluewhale_set_chg_volt(uint16_t chgvolt_mv)
{
	int16_t ret = 0;
	uint8_t chg_volt = 0;
#if 0
	if (chgvolt_mv < 4000)
		chgvolt_mv = 4000;		//limit to 4.0V
	else if (chgvolt_mv > 4600)
		chgvolt_mv = 4600;		//limit to 4.6V

	chg_volt = (chgvolt_mv - 4000) / CHG_VOLT_STEP;	//step is 25mV

	ret = bluewhale_write_byte(BQ24195_CON0, 0x1B);	
	return (-ret);
    ret = bluewhale_update_bits(REG_CHARGER_VOLTAGE, _BITMASK(5), chg_volt);
    return (-ret);
#endif

ret = bluewhale_write_byte(BQ24195_CON4, 0xB2); 
return (-ret);

}

/*****************************************************************************
 * Description:
 *		bluewhale_get_chg_volt 
 * Parameters:
 *		n/a
 * Return:
 *      chg_volt in mV.
 *****************************************************************************/
int16_t bluewhale_get_chg_volt(void)
{
	uint8_t data;
	int16_t ret = 0;
#if 0
	ret = bluewhale_read_byte(REG_CHARGER_VOLTAGE, &data);
	if (ret != 0)
		return (-ret);

	return (int16_t)(data * CHG_VOLT_STEP);	//step is 25mA
#endif 
   return 0; 
}

/*****************************************************************************
 * Description:
 *		bluewhale_set_wakeup_volt 
 * Parameters:
 *		wakeup_mv:	set wake up voltage
 * Return:
 *      zero on success.
 *****************************************************************************/
int16_t bluewhale_set_wakeup_volt(uint16_t wakeup_mv)
{
	int16_t ret = 0;
	uint8_t wak_volt = 0; 
#if 0
	if (wakeup_mv < 1500)
		wakeup_mv = 1500;		//limit to 1.5V
	else if (wakeup_mv > 3000)
		wakeup_mv = 3000;		//limit to 3.0V

	wak_volt = wakeup_mv / WK_VOLT_STEP;	//step is 100mV
#endif 

	ret = bluewhale_write_byte(BQ24195_CON1, 0x1B);	
	return (-ret);
}

/*****************************************************************************
 * Description:
 *		bluewhale_set_eoc_current 
 * Parameters:
 *		eoc_ma:	set end of charge current
 *		Only 0mA, 20mA, 40mA, ... , 320mA can be accurate
 * Return:
 *      zero on success.
 *****************************************************************************/
int16_t bluewhale_set_eoc_current(uint16_t eoc_ma)
{
	int16_t ret = 0;
	uint8_t eoc_curr = 0; 
#if 0
	//Notice: with 00h, end of charge function function is disabled
	if (eoc_ma <= 0)
		eoc_ma = 0;	    	//min value is 0mA
	else if (eoc_ma > 320)
		eoc_ma = 320;		//limit to 320mA
	eoc_curr = eoc_ma / EOC_CURRT_STEP;	//step is 10mA

	ret = bluewhale_write_byte(REG_END_CHARGE_CURRENT, eoc_curr);	
	return (-ret);
#endif
    return 0; 
}

/*****************************************************************************
 * Description:
 *		bluewhale_get_eoc_current 
 * Parameters:
 *		n/a
 * Return:
 *      eoc_ma:	end of charge current in mA
 *****************************************************************************/
int16_t bluewhale_get_eoc_current(void)
{
	uint8_t data;
	int16_t ret = 0;
#if 0
	ret = bluewhale_read_byte(REG_END_CHARGE_CURRENT, &data);
	if (ret != 0)
		return (-ret);
	return (int16_t)(data * EOC_CURRT_STEP);	//step is 10mA
#endif
    return 0; 
}

/*****************************************************************************
 * Description:
 *		bluewhale_set_vbus_current
 * Parameters:
 *		ilmt_ma:	set input current limit
 *		Only 100mA, 500mA, 700mA, 900mA, 1000mA, 1200mA, 1400mA, 1500mA, 1700mA, 1900mA, 2000mA can be accurate
 * Return:
 *      zero on success.

 *****************************************************************************/
int16_t bluewhale_set_vbus_current(uint16_t ilmt_ma)
{
	int16_t ret = 0;
	uint8_t input_curr = 0;
  #if 0  
	if(ilmt_ma < 300)
		input_curr = 0x01;		//100mA
	else if(ilmt_ma >= 300 && ilmt_ma < 600)
		input_curr = 0x05;		//500mA
	else if(ilmt_ma >= 600 && ilmt_ma < 800)
		input_curr = 0x0c;	//700mA
	else if(ilmt_ma >= 800 && ilmt_ma < 950)
		input_curr = 0x09;	//900mA
	else if(ilmt_ma >= 950 && ilmt_ma < 1100)
		input_curr = 0x10;	//1000mA
	else if(ilmt_ma >= 1100 && ilmt_ma < 1300)
		input_curr = 0x12;	//1200mA
	else if(ilmt_ma >= 1300 && ilmt_ma < 1450)
		input_curr = 0x0e;	//1400mA
	else if(ilmt_ma >= 1450 && ilmt_ma < 1600)
		input_curr = 0x0f;	//1500mA
	else if(ilmt_ma >= 1600 && ilmt_ma < 1800)
		input_curr = 0x11;	//1700mA
	else if(ilmt_ma >= 1800 && ilmt_ma < 1950)
		input_curr = 0x13;	//1900mA
	else if(ilmt_ma >= 1950)
		input_curr = 0x14;	//2000mA
  #endif
	ret = bluewhale_write_byte(BQ24195_CON0,  0x47);
	return (-ret);
}

/*****************************************************************************
 * Description:
 *		bluewhale_get_vbus_current
 * Parameters:
 *		n/a
 * Return:
 *		Vbus input current in mA
 *****************************************************************************/
int16_t bluewhale_get_vbus_current(void)
{
	int16_t ret = 0;
	uint8_t data = 0;

	ret = bluewhale_read_byte(BQ24195_CON0, &data);
	if (ret != 0)
		return (-ret);

	switch(data)
	{
		case 0x00: return 0;
		case 0x01: ;
		case 0x02: ;
		case 0x03: return 100;
		case 0x04: ;
		case 0x05: ;
		case 0x06: ;
		case 0x07: return 500;
		case 0x08: ;
		case 0x09: ;
		case 0x0a: ;
		case 0x0b: return 900;
		case 0x0c: ;
		case 0x0d: return 700;
		case 0x0e: return 1400;
		case 0x0f: return 1500;
		case 0x10: return 1000;
		case 0x11: return 1700;
		case 0x12: return 1200;
		case 0x13: return 1900;
		case 0x14: return 2000;
		default:
				   return -1;
	}
}

/*****************************************************************************
 * Description:
 *		bluewhale_set_rechg_hystersis
 * Parameters:
 *		hyst_mv:	set Recharge hysteresis Register
 * Return:
 *      zero on success.
 *****************************************************************************/
int16_t bluewhale_set_rechg_hystersis(uint16_t hyst_mv)
{
	int16_t ret = 0;
	uint8_t rechg = 0; 
#if 0		
	if (hyst_mv > 200) {
		hyst_mv = 200;			//limit to 200mV
	}
	//Notice: with 00h, recharge function is disabled
	rechg = hyst_mv / RECHG_VOLT_STEP;	//step is 50mV
#endif 

	ret = bluewhale_write_byte(BQ24195_CON0, 0x37);	
	return (-ret);
}

/*****************************************************************************
 * Description:
 *		bluewhale_set_charger_current
 * Parameters:
 *		chg_ma:	set charger current
 *		Only 600mA, 800mA, 1000mA, ... , 3800mA, 4000mA can be accurate
 * Return:
 *      zero on success.
 *****************************************************************************/
int16_t bluewhale_set_charger_current(uint16_t chg_ma)
{
	int16_t ret = 0;
	uint8_t chg_curr = 0; 
#if 0
	if (chg_ma > 4000) {
		chg_ma = 4000;		//limit to 4A
	}
	else {
		if (chg_ma < 600)
			chg_ma = 600;		//limit to 600 mA
	}

	//notice: chg_curr value less than 06h, charger will be disabled.
	//charger can power system in this case.
	chg_curr = chg_ma / CHG_CURRT_STEP;	//step is 100mA
#endif

	ret = bluewhale_write_byte(BQ24195_CON2, 0x60);	

	return (-ret);
}

/*****************************************************************************
 * Description:
 *		bluewhale_get_charger_current
 * Parameters:
 *		n/a
 * Return:
 *      chg_ma:	charger current in mA
 *****************************************************************************/
int16_t bluewhale_get_charger_current(void)
{
	uint8_t data;
	int16_t ret = 0;
#if 0
	ret = bluewhale_read_byte(REG_CHARGE_CURRENT, &data);
	if (ret != 0)
		return (-ret);
	return (int16_t)(data * CHG_CURRT_STEP);	//step is 100mA
#endif 
    ret = bluewhale_read_byte(BQ24195_CON2, &data);
    if (ret != 0)
		return (-ret);
	return (int16_t)(((data >>2) * CHG_CURRT_STEP)+512);	//step is 100mA
}

/*****************************************************************************
 * Description:
 *		bluewhale_charger_init, called during system power on init.
 * Parameters:
 *		n/a
 * Return:
 *		negative errno if error happens
 *****************************************************************************/
short bluewhale_charger_init(void)
{
	kal_uint8 i;
	unsigned char data;
	bluewhale_write_byte(BQ24195_CON0,BQ24195_REG_VAL0);
	bluewhale_write_byte(BQ24195_CON1,BQ24195_REG_VAL1);
	bluewhale_write_byte(BQ24195_CON2,BQ24195_REG_VAL2);
	bluewhale_write_byte(BQ24195_CON3,BQ24195_REG_VAL3);
	bluewhale_write_byte(BQ24195_CON4,BQ24195_REG_VAL4);
	bluewhale_write_byte(BQ24195_CON5,BQ24195_REG_VAL5);
	bluewhale_write_byte(BQ24195_CON6,BQ24195_REG_VAL6);
	bluewhale_write_byte(BQ24195_CON7,BQ24195_REG_VAL7);
	bq25601_read_chip_id();
#if 1
	for(i=0;i<10;i++)
	{
		i2c_read_byte_data(i, &data);
		kal_prompt_trace(MOD_BMT,"%s, bq25601 REG[%d]:%x", __func__, i, data);
		data = 0;
	}
#endif
	return 0;     
}


#define I_NUM (11)
#define ARG_NUM (20)
#define VBUS_LOW (4400)
#define VBUS_PER_STEP (150)
#define SDPM_VBUS

#define BW_ABS(a,b) ( ((a) > (b)) ? ((a) - (b)) : ((b) - (a)) )


int limit_l[I_NUM] = {100,500,700, 900, 1000,1200, 1400, 1500, 1700, 1900,2000};
int cc_l[I_NUM] =   {600,1000,1000,2000,2000,2000,2000,2000,2000,2000,2000};
//int bat_cur[I_NUM] = {0,0,0,0,0};
int vbus_l[I_NUM] = {0,0,0,0,0};
int vbat_l[I_NUM] = {0,0,0,0,0};
int vbus_arg[ARG_NUM];
int vbat_arg[ARG_NUM];

//static int arg_num = 0;
static int retr_num = 0;


static void adji_bubble_sorting(int *vstart, int size)
{
	int32_t i;
	int32_t j;
	int32_t tmp;

	for(i=0; i<(size-1); i++) {
		for(j=0; j<(size-1-i); j++) {
			if(vstart[j] > vstart[j+1]) {
				tmp = vstart[j+1] ;
				vstart[j+1] = vstart[j];
				vstart[j] = tmp ;
			}
		}
	}
}

static int32_t adji_cal_avg_info(int32_t *vstart, int32_t size)
{
	if(size == 1)
		return vstart[0] ;

	if(size == 2)
		return ( (vstart[0] + vstart[1])/2 ) ;

	if(size >= 3) {
		int32_t i ;
		int32_t tmp=0 ;
	
		for(i=1; i<(size -1); i++) //del the largest one and the smallest one
			tmp += vstart[i] ;

		return ( tmp/(size-2) ) ;
	}

	return 0 ;
}

void bluewhale_get_adc(void)
{
    kal_uint32 vbat;
    kal_uint32 vcharger;
    //kal_uint32 visense;
    //kal_uint32 vbattmp;
    kal_uint32 Ibat;
    int i = 0;

    bmt_get_avg_adc_channel_voltage(DCL_VBAT_ADC_CHANNEL, &vbat, 1); 

    vbat /= 1000;

    for (i=0; i<ARG_NUM; i++){
    bmt_get_avg_adc_channel_voltage(DCL_VCHARGER_ADC_CHANNEL, &vcharger, 20);
    vcharger /= 1000;
    vbus_arg[i] = vcharger;
    }
    
    adji_bubble_sorting(vbus_arg, ARG_NUM);
	bluewhale_data.vbus_voltage = adji_cal_avg_info(vbus_arg, ARG_NUM);
    

    //bmt_get_avg_adc_channel_voltage(DCL_VISENSE_ADC_CHANNEL, &visense, 20);
    //bmt_get_avg_adc_channel_voltage(DCL_VBATTMP_ADC_CHANNEL, &vbattmp, 20);
    
    //visense /= 1000;
    //vbattmp /=1000;
    //Ibat = (visense - vbat)*100; //Rsense = 10m oh
    
    //kal_prompt_trace(MOD_BMT, "[oz115]vbat: %d, vcharger:%d,visense:%d,vbattmp:%d, Ibat:%d\n", 
        //vbat, vcharger, visense, vbattmp, Ibat);
   
    bluewhale_data.battery_voltage = vbat;
    //bluewhale_data.battery_current = Ibat;
}



int16_t bluewhale_measure_adc(void)
{    
    bluewhale_get_adc();

    vbat_l[ilimit_i] = bluewhale_data.battery_voltage;
    vbus_l[ilimit_i] = bluewhale_data.vbus_voltage;
}

int16_t bluewhale_set_ilimit(void)
{
    int16_t ret = 0;
	int16_t ilimit_now = 0;
    int i = 0;
	return 0;
#if 0
    //char buffer[100];

    
    //disable suspend mode
	ret = bluewhale_update_bits(REG_CHARGER_CONTROL, 0x02, 0 << 1);
    if (ret != 0)   return (-ret);

	//get ilimit_now
    ilimit_now = bluewhale_get_vbus_current();
    if (ret < 0)   return (ret);
        
    if (bluewhale_get_status() != 1) return find_ilimit;

    
#ifdef SDPM_VBUS
    
    bluewhale_measure_adc();

    //sprintf(buffer, "[oz115] %d, [%d]: %d, %d\n", 
    //      ilimit_now,ilimit_i, vbus_l[ilimit_i], find_ilimit);
    //rmmi_write_to_uart((kal_uint8*)buffer, strlen(buffer), KAL_TRUE);

    kal_prompt_trace(MOD_BMT, "[oz115] %d, [%d]=%d, [%d]=%d , [%d-%d], %d,%d\n", 
    ilimit_now,ilimit_i, vbus_l[ilimit_i], ilimit_i, vbat_l[ilimit_i], vbus_arg[ARG_NUM-1], vbus_arg[0], find_ilimit, set_ilimit);
#if 0
    //for test
    if (find_ilimit){
       ilimit_i=0;
       find_ilimit = 0;
       bluewhale_set_vbus_current(limit_l[ilimit_i]);
	   bluewhale_set_charger_current(2000);
       return find_ilimit;
    } 
 #endif

    if (bluewhale_data.vbus_voltage > 5500 && vbus_arg[0]>5500){ //must be solar panel
	    ilimit_i = 1;  //500mA
	    find_ilimit = 1;
	    set_ilimit = 1;

	    bluewhale_set_vbus_current(limit_l[ilimit_i]);
	    bluewhale_set_charger_current(cc_l[ilimit_i]);
	    return find_ilimit & set_ilimit;
    }
    
    else if (vbat_l[ilimit_i] < 3400){ //LDO
		ilimit_i = 1;  //500mA
		find_ilimit = 0;
		set_ilimit = 0;
    }
	else if (!find_ilimit)
	{   
		set_ilimit = 0;

		if (ilimit_i == 0 ){
			if(  vbus_l[ilimit_i] > VBUS_LOW 
                && (vbus_arg[ARG_NUM-1] - vbus_arg[0]) < 150
                )
				ilimit_i +=1;
			else {
				if(retr_num < 3){ 
					retr_num ++;  //retry 3 
					find_ilimit = 0;
				}
				else {
					find_ilimit = 1;
				}

				ilimit_i = 0;
			}
		}
		else if (ilimit_i == (I_NUM-1)){
			find_ilimit = 1;  
			if (vbus_l[ilimit_i] < VBUS_LOW
					|| (vbus_arg[ARG_NUM-1] - vbus_arg[0]) > 150
					|| BW_ABS(vbus_l[ilimit_i], vbus_l[ilimit_i - 1]) > VBUS_PER_STEP)
				ilimit_i -=1;
		}
        else if (ilimit_i==1){
            if (vbus_l[ilimit_i] < VBUS_LOW
					|| (vbus_arg[ARG_NUM-1] - vbus_arg[0]) > 150
					|| BW_ABS(vbus_l[ilimit_i], vbus_l[ilimit_i - 1]) > 300)

			{
				ilimit_i -=1;
				find_ilimit = 1;
			}
			else {
				ilimit_i +=1;
			}

        }
		else if (ilimit_i > 0){
			if (vbus_l[ilimit_i] < VBUS_LOW
					|| (vbus_arg[ARG_NUM-1] - vbus_arg[0]) > 150
					|| BW_ABS(vbus_l[ilimit_i], vbus_l[ilimit_i - 1]) > VBUS_PER_STEP)

			{
				ilimit_i -=1;
				find_ilimit = 1;
			}
			else {
				ilimit_i +=1;
			}
		}
        
        if (find_ilimit && set_ilimit ==0 && ilimit_i > 1){ //ilimit > 500
	    	bluewhale_set_vbus_current(500);
	        bluewhale_set_charger_current(1000);
                return find_ilimit && set_ilimit;
        }
        else if (find_ilimit) set_ilimit = 1;

	}
	else { //check if ilimit can work
	    if (find_ilimit && set_ilimit ==0 && ilimit_i > 1){ //ilimit > 500)
	    	bluewhale_set_vbus_current(limit_l[ilimit_i]);
	        bluewhale_set_charger_current(cc_l[ilimit_i]);
                set_ilimit = 1;
            
                return find_ilimit && set_ilimit;
            }
		if (ilimit_i == 0 
            && (vbus_l[0] > VBUS_LOW + 200) 
            && (vbus_arg[ARG_NUM-1] - vbus_arg[0]) < 150){
			ilimit_i = 0;
			find_ilimit = 0;
			set_ilimit = 0;
		}
		else if (vbus_l[ilimit_i] < VBUS_LOW
				|| (vbus_arg[ARG_NUM-1] - vbus_arg[0]) > 150
				|| BW_ABS(vbus_l[ilimit_i], vbus_l[ilimit_i - 1]) > (VBUS_PER_STEP+ ilimit_i>6 ? 100 : 0))
		{
			ilimit_i = 0;
			find_ilimit = 0;
			set_ilimit = 0;
		}
	}

	bluewhale_set_vbus_current(limit_l[ilimit_i]);
	bluewhale_set_charger_current(cc_l[ilimit_i]);

	//if (!find_ilimit)
		//bmt_timer_config( KAL_TICKS_1_SEC * 2); // 0.5s measure adc

	kal_prompt_trace(MOD_BMT, "[oz115]set limit_l[%d]=%d, cc_l[%d]= %d, find_ilimit=%d\n", 
			ilimit_i, limit_l[ilimit_i], 
			ilimit_i, cc_l[ilimit_i],
			find_ilimit);
#if 0
	sprintf(buffer, "[oz115]set limit_l[%d]=%d, cc_l[%d]= %d, find_ilimit=%d\n",
			ilimit_i, limit_l[ilimit_i], 
			ilimit_i, cc_l[ilimit_i],
			find_ilimit);
	rmmi_write_to_uart((kal_uint8*)buffer, strlen(buffer), KAL_TRUE);
#endif

    
#elif defined(SDPM_IBAT)

    kal_prompt_trace(MOD_BMT, "[oz115]ilimit_now: %d, bat_cur[%d]: %d,vbat: %d, vcharger:%d, find_ilimit=%d\n", 
        ilimit_now,ilimit_i, bat_cur[ilimit_i],bluewhale_data.battery_voltage ,bluewhale_data.vbus_voltage, find_ilimit);

    if (bluewhale_data.vbus_voltage > 7000){
		if (bluewhale_data.ilimit_target >= limit_l[I_NUM-1])
			ilimit_i = I_NUM-1;
		else if (bluewhale_data.ilimit_target <= limit_l[0])
			ilimit_i = 0;
		else {
			for (i =0; i< I_NUM; i++){
				if (bluewhale_data.ilimit_target < limit_l[i]){
					ilimit_i = i-1;
					break;
				}
				continue;
			}
		}

		find_ilimit = 0;
    }
    else if (!find_ilimit){
        if (ilimit_i == 0 ){
            if(bat_cur[0] >0)
				ilimit_i +=1;
            else{
				ilimit_i = 0;
                find_ilimit = 1;
            }
        }
        else if (ilimit_i == 3){
			find_ilimit = 1;  
			if (bat_cur[ilimit_i] < bat_cur[ilimit_i -1])
				ilimit_i -=1;
        }
        else if (ilimit_i > 0){
            if (bat_cur[ilimit_i] > bat_cur[ilimit_i -1])
				ilimit_i +=1;
            else {
				ilimit_i -=1;
                find_ilimit = 1;
            }
        }
		if(bat_cur[ilimit_i] < 0) //set 500 and wait AC get back to normal
		{
            bluewhale_set_vbus_current(limit_l[0]);
	        bluewhale_set_charger_current(cc_l[0]);
			//mdelay(300);
			//bmt_timer_config(3*KAL_TICKS_1_SEC); //3//3s to measure adc

		}
    }
    else { //check if ilimit can work
        if (ilimit_i > 0 && bat_cur[ilimit_i]< bat_cur[ilimit_i --]){
            ilimit_i = 0;
            find_ilimit = 0;
        }
	}

	bluewhale_set_vbus_current(limit_l[ilimit_i]);
	bluewhale_set_charger_current(cc_l[ilimit_i]);
    
    if (!find_ilimit)
        bmt_timer_config(KAL_TICKS_1_SEC); //3//1s to measure adc

	kal_prompt_trace(MOD_BMT, "[oz115]set limit_l[%d]=%d, cc_l[%d]= %d, find_ilimit=%d\n", 
			ilimit_i, limit_l[ilimit_i], 
			ilimit_i, cc_l[ilimit_i],
			find_ilimit);
#else
    if (!find_ilimit){
	    bluewhale_set_vbus_current(500);
	    bluewhale_set_charger_current(2000);
	    find_ilimit = 1;
	    set_ilimit = 1;
    }
    kal_prompt_trace(MOD_BMT, "[oz115] limit=%d, cc= %d, find_ilimit=%d\n", 
		    bluewhale_get_vbus_current(), bluewhale_get_charger_current(), find_ilimit);
#endif
    
	return find_ilimit && set_ilimit;
#endif
}

int bluewhale_ilimit_fixed(void)
{
    return find_ilimit && set_ilimit;
}

int16_t bluewhale_set_ilimit_target(uint16_t ilmt_ma)
{
   if(bluewhale_data.ilimit_target==0)
   {//initial ilimit
       ilimit_i=0;
       bluewhale_set_vbus_current(limit_l[ilimit_i]);
	   bluewhale_set_charger_current(2000);
    }
    
    bluewhale_data.ilimit_target = ilmt_ma;
}


/*****************************************************************************
 * Description:
 *		bluewhale_ac_charge
 * Parameters:
 *		None
 * Return:
 *      negative errno if any error
 *****************************************************************************/

int16_t bluewhale_ac_charge(void)
{
	return 0;//test
}

/*****************************************************************************
 * Description:
 *		bluewhale_usb_charge
 * Parameters:
 *		None
 * Return:
 *      negative errno if any error
 *****************************************************************************/
int16_t bluewhale_usb_charge(void)
{
    int16_t ret = 0;
#if 0
    kal_prompt_trace(MOD_BMT, "[oz115]%s\n", __func__);
    //disable suspend mode
	ret = bluewhale_update_bits(REG_CHARGER_CONTROL, 0x02, 0 << 1);
    if (ret != 0)   return (-ret);
	ret = bluewhale_set_vbus_current(STOP_CHG_ILIMIT);

	ret = bluewhale_set_charger_current(2000);
	if (ret < 0)   return (ret);
    
	bluewhale_data.prev_current = 0;
    
    //collect Vbat, Vbus, Ibat
    //bluewhale_get_adc();

    find_ilimit = 0;
    set_ilimit = 0;
#endif
    return ret;
}

/*****************************************************************************
 * Description:
 *		bluewhale_stop_charge, should be called when adapter plug out
 * Parameters:
 *		None
 * Return:
 *      negative errno if any error
 *****************************************************************************/
int16_t bluewhale_stop_charge(void)
{
   	//sprintf(buffer, "[oz115]%s\n", __func__);
    
	//rmmi_write_to_uart((kal_uint8*)buffer, strlen(buffer), KAL_TRUE);
    kal_prompt_trace(MOD_BMT, "[oz115]%s\n", __func__);				

    bluewhale_set_vbus_current(limit_l[ilimit_i]);
	bluewhale_set_charger_current(2000);
    
    ilimit_i=0;
    find_ilimit = 0;
    set_ilimit = 0;
    bluewhale_data.ilimit_target==0;
        
	bluewhale_data.prev_current = 0;
    bluewhale_data.charger_type = 4;
    
    return 0;
}


/*****************************************************************************
 * Description:
 *		bluewhale_get_status
 * Parameters:
 *		None
 * Return:
 *      negative errno if any error
 *****************************************************************************/
int16_t bluewhale_get_status(void)
{
    int16_t ret = 0;
	uint8_t bval = 0;
	bluewhale_thermal_t thm_now = 0;
	int i = 0;
#if 0

	//get VBUS status
	ret = bluewhale_read_byte(REG_VBUS_STATUS, &bval);
	if (ret != 0)	return (-ret);
	bluewhale_data.vbus_ok = (bval & VBUS_OK_FLAG) ? 1 : 0;
	bluewhale_data.vsys_ovp = (bval & VSYS_OVP_FLAG) ? 1 : 0;
	//get charger status
	ret = bluewhale_read_byte(REG_CHARGER_STATUS, &bval);
	if (ret != 0)	return (-ret);
	bluewhale_data.initial_state = (bval & CHARGER_INIT) ? 1 : 0;
	bluewhale_data.in_wakeup_state = (bval & IN_WAKEUP_STATE) ? 1 : 0;
	bluewhale_data.in_cc_state = (bval & IN_CC_STATE) ? 1 : 0;
	bluewhale_data.in_cv_state = (bval & IN_CV_STATE) ? 1 : 0;
	bluewhale_data.in_full_state = (bval & IN_FULL_STATE) ? 1 : 0;
	//get charger thermal zone
	ret = bluewhale_read_byte(REG_THM_STATUS, &bval);
	if (ret != 0)	return (-ret);
	if (!bval)
		thm_now = THM_DISABLE;
	else {
		for (i = 0; i < 7; i ++) {
			if (bval & (1 << i))
				thm_now = i + 1;
		}
	}
    
#if 1
    kal_prompt_trace(MOD_BMT, "[oz115]status: VBUS OK: %d, cc:%d,cv:%d, full:%d", 
		//bluewhale_get_vbus_current(),
		//bluewhale_get_charger_current(),
		bluewhale_data.vbus_ok,
		bluewhale_data.in_cc_state,
		bluewhale_data.in_cv_state,
		bluewhale_data.in_full_state);				
#endif
		    
    return bluewhale_data.vbus_ok;
#endif
	return 0;
}
#endif
