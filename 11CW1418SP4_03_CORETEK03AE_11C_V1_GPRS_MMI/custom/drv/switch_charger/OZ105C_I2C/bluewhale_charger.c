/*****************************************************************************
* Copyright(c) O2Micro, 2013. All rights reserved.
*	
* O2Micro OZ63810G charger driver
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
//#if defined(OZ105C_SWCHR_SUPPORT)
#if 1

#include "bluewhale_charger.h"
//#include "uart_debug.h"
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
#if 0
extern uint16_t i2cm_read_word(uint8_t i2c_addr, uint8_t reg_addr, uint16_t *wdata);
extern uint16_t i2cm_write_word(uint8_t i2c_addr, uint8_t reg_addr, uint16_t wdata);
extern uint16_t i2cm_write_byte(uint8_t i2c_addr, uint8_t reg_addr, uint8_t bdata);
extern uint16_t i2cm_read_byte(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *bdata);
//extern void i2cm_pec_control(bool enable);
#endif
/**************************************************************
* global variables section
* add declaration of global variables here
**************************************************************/
bluewhale_platform_setup_t bluewhale_pdate_setup;
bluewhale_data_t	bluewhale_data;
kal_int8 oz105_i2c_addr = 0x10;

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

        ms_i2c_configure((oz105_i2c_addr << 1), 300);
	ret = ms_i2c_receive((oz105_i2c_addr << 1),addr,data,1);

	return (!ret);
}

/*return value: 0: is ok    other: is failed*/
static int i2c_write_byte_data( unsigned char addr, unsigned char data){
	int ret = 0;

        ms_i2c_configure((oz105_i2c_addr << 1), 300);
	ret =  ms_i2c_send((oz105_i2c_addr << 1),addr,&data,1);

	return (!ret);
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
		//ret = i2cm_read_byte(addr, index, &tmp);
		ret = i2c_read_byte_data(index, &tmp);
		if(ret == 0) break;
	}
	if (i >= RETRY_CNT) {
		kal_prompt_trace(MOD_MMI,"%s, res:%d, addr: %x, data:%x", __func__, ret, index, tmp);
		return ret;
	}
	*dat = tmp;
	kal_prompt_trace(MOD_MMI,"%s, res:%d, addr: %x, data:%x", __func__, ret, index, tmp);
	//uart_dbg("%s, res:%d, addr: %x, data:%x", __func__, ret, index, tmp);
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
		//ret = i2cm_write_byte(addr, index, dat);
		ret = i2c_write_byte_data(index, dat);
		if(ret == 0) break;
	}
	if(i >= RETRY_CNT) {
		kal_prompt_trace(MOD_MMI,"%s, res:%d, addr: %x, data:%x", __func__, ret, index, dat);
		return ret;
	}
	kal_prompt_trace(MOD_MMI,"%s, res:%d, addr: %x, data:%x", __func__, ret, index, dat);
	//uart_dbg("%s, res:%d, addr: %x, data:%x", __func__, ret, index, dat);
	return ret;
}

static int16_t bluewhale_update_bits( uint8_t reg, uint8_t mask, uint8_t data)
{
	uint16_t ret;
	uint8_t tmp;

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
	if (wak_ma < 100)
		wak_ma = 100;		//limit to 100mA
	if (wak_ma > 400)
		wak_ma = 400;		//limit to 400mA
	wak_curr = wak_ma / WK_CURRT_STEP;	//step is 10mA

	ret = bluewhale_write_byte(REG_WAKEUP_CURRENT, wak_curr);
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

	if (chgvolt_mv < 4000)
		chgvolt_mv = 4000;		//limit to 4.0V
	else if (chgvolt_mv > 4600)
		chgvolt_mv = 4600;		//limit to 4.6V

	chg_volt = (chgvolt_mv - 4000) / CHG_VOLT_STEP;	//step is 25mV

       ret = bluewhale_update_bits(REG_T34_CHG_VOLTAGE, _BITMASK(5), chg_volt);
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

	if (chgvolt_mv < 4000)
		chgvolt_mv = 4000;		//limit to 4.0V
	else if (chgvolt_mv > 4600)
		chgvolt_mv = 4600;		//limit to 4.6V

	chg_volt = (chgvolt_mv - 4000) / CHG_VOLT_STEP;	//step is 25mV
	
    ret = bluewhale_update_bits( REG_T45_CHG_VOLTAGE, _BITMASK(5), chg_volt);
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

	if (min_vsys_mv < 1800)
		min_vsys_mv = 1800;		//limit to 1.8V
	else if (min_vsys_mv > 3600)
		min_vsys_mv = 3600;		//limit to 3.6V
	vsys_val = min_vsys_mv / VSYS_VOLT_STEP;	//step is 200mV
	
    ret = bluewhale_update_bits( REG_MIN_VSYS_VOLTAGE, _BITMASK(5), vsys_val);
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
	uint8_t data;
	int16_t ret = 0;

	ret = bluewhale_read_byte(REG_MIN_VSYS_VOLTAGE, &data);
	if (ret != 0)
		return (-ret);
	//data &= 0x1f;
    data &= _BITMASK(5);
	return (data * VSYS_VOLT_STEP);	//step is 200mV
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

	if (chgvolt_mv < 4000)
		chgvolt_mv = 4000;		//limit to 4.0V
	else if (chgvolt_mv > 4600)
		chgvolt_mv = 4600;		//limit to 4.6V

	chg_volt = (chgvolt_mv - 4000) / CHG_VOLT_STEP;	//step is 25mV

    ret = bluewhale_update_bits(REG_CHARGER_VOLTAGE, _BITMASK(5), chg_volt);
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

	ret = bluewhale_read_byte(REG_CHARGER_VOLTAGE, &data);
	if (ret != 0)
		return (-ret);

	return (int16_t)(data * CHG_VOLT_STEP);	//step is 25mA
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

	if (wakeup_mv < 1500)
		wakeup_mv = 1500;		//limit to 1.5V
	else if (wakeup_mv > 3000)
		wakeup_mv = 3000;		//limit to 3.0V

	wak_volt = wakeup_mv / WK_VOLT_STEP;	//step is 100mV
	ret = bluewhale_write_byte(REG_WAKEUP_VOLTAGE, wak_volt);	
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

	//Notice: with 00h, end of charge function function is disabled
	if (eoc_ma <= 0)
		eoc_ma = 0;	    	//min value is 0mA
	else if (eoc_ma > 320)
		eoc_ma = 320;		//limit to 320mA
	eoc_curr = eoc_ma / EOC_CURRT_STEP;	//step is 10mA

	ret = bluewhale_write_byte(REG_END_CHARGE_CURRENT, eoc_curr);	
	return (-ret);
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

	ret = bluewhale_read_byte(REG_END_CHARGE_CURRENT, &data);
	if (ret != 0)
		return (-ret);
	return (int16_t)(data * EOC_CURRT_STEP);	//step is 10mA
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

	ret = bluewhale_write_byte(REG_VBUS_LIMIT_CURRENT, input_curr);
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

	ret = bluewhale_read_byte(REG_VBUS_LIMIT_CURRENT, &data);
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
		
	if (hyst_mv > 200) {
		hyst_mv = 200;			//limit to 200mV
	}
	//Notice: with 00h, recharge function is disabled
	rechg = hyst_mv / RECHG_VOLT_STEP;	//step is 50mV

	ret = bluewhale_write_byte(REG_RECHARGE_HYSTERESIS, rechg);	
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

	ret = bluewhale_write_byte(REG_CHARGE_CURRENT, chg_curr);	
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

	ret = bluewhale_read_byte(REG_CHARGE_CURRENT, &data);
	if (ret != 0)
		return (-ret);
	return (int16_t)(data * CHG_CURRT_STEP);	//step is 100mA
}

/*****************************************************************************
 * Description:
 *		bluewhale_charger_init, called during system power on init.
 * Parameters:
 *		n/a
 * Return:
 *		negative errno if error happens
 *****************************************************************************/
short bluewhale_charger_init(char chr_type)
{
    int16_t ret = 0;
	int16_t ilimit_current;
	int16_t charger_current;
    char buffer[50];

    //platform data initialize, change following parameters if needed
    //bluewhale_pdate_setup.rsense = 10000;					//10mOhm * 1000
    //bluewhale_pdate_setup.board_offset = BOARDOFFSET_DEFAULT;
    
    //kal_prompt_trace(MOD_MMI, "%s enter", __FUNCTION__);
	
    bluewhale_pdate_setup.rthm_select = 0;    // 1 for 10K, 0 for 100K
    bluewhale_pdate_setup.vbus_limit_currentmA = 1000;    //default 1000A
    bluewhale_pdate_setup.min_vsys_voltagemV = 3400;      //3.4V min VSYS
    bluewhale_pdate_setup.recharge_voltagemV = 100;       //100mv
    bluewhale_pdate_setup.wakeup_currentmA = 100;         //100mA
    bluewhale_pdate_setup.wakeup_voltagemV = 2500;        //2.5V
    bluewhale_pdate_setup.T45_charger_voltagemV = 4100;   //4.1V
    bluewhale_pdate_setup.T34_charger_voltagemV = 4100;   //4.1V
    bluewhale_pdate_setup.termination_currentmA = 160;    //160mA
    bluewhale_pdate_setup.max_charger_voltagemV = 4200;   //4.2V
    bluewhale_pdate_setup.max_charger_currentmA = 1000;   //1000mA

    ms_i2c_configure((oz105_i2c_addr << 1), 300);
    
    //device initialize --- charger
	//*************************************************************************************
	// note: you must test usb type and set vbus limit current.
	// for wall charger you can set current more than 500mA
	// but for pc charger you may set current not more than 500 mA for protect pc usb port
	//************************************************************************************/
	// write rthm  10k/100k
	ret = bluewhale_update_bits(REG_CHARGER_CONTROL,  _BITMASK(1), bluewhale_pdate_setup.rthm_select);
    if (ret != 0)   return (-ret);
	/* Min VSYS:3.6V */
    ret = bluewhale_set_min_vsys(bluewhale_pdate_setup.min_vsys_voltagemV);
	if (ret < 0)	return (ret);
	/* EOC CHARGE:150 mA */
	ret = bluewhale_set_eoc_current(bluewhale_pdate_setup.termination_currentmA);
	if (ret < 0)	return (ret);
	/* TIMER:disabled*/
	ret = bluewhale_write_byte(REG_SAFETY_TIMER, 0);
	if (ret != 0)	return (-ret);
	/* RECHG HYSTERESIS:0.1V */
	ret = bluewhale_set_rechg_hystersis(bluewhale_pdate_setup.recharge_voltagemV);
	if (ret < 0)	return (ret);
	/* T34 CHARGE VOLTAGE:4.0V */
	ret = bluewhale_set_t34_cv(bluewhale_pdate_setup.T34_charger_voltagemV);
	if (ret < 0)	return (ret);
	/* T45 CHARGE VOLTAGE:4.0V */
	ret = bluewhale_set_t45_cv(bluewhale_pdate_setup.T45_charger_voltagemV);
	if (ret < 0)	return (ret);
	/* WAKEUP VOLTAGE:default 2.5V */
	ret = bluewhale_set_wakeup_volt(bluewhale_pdate_setup.wakeup_voltagemV);
	if (ret < 0)	return (ret);
	/* WAKEUP CURRENT:default 0.1A */
	ret = bluewhale_set_wakeup_current(bluewhale_pdate_setup.wakeup_currentmA);
	if (ret < 0)	return (ret);
	/* CHARGER CURRENT:default 1A */
	ret = bluewhale_set_charger_current(bluewhale_pdate_setup.max_charger_currentmA);
	if (ret < 0)	return (ret);
	/* CHARGE VOLTAGE:default 4.2V*/
	ret = bluewhale_set_chg_volt(bluewhale_pdate_setup.max_charger_voltagemV);
	if (ret < 0)	return (ret);
	/* VBUS ILIMIT:default 0.5A*/
	ret = bluewhale_set_vbus_current(bluewhale_pdate_setup.vbus_limit_currentmA);
	if (ret < 0)	return (ret);
	bluewhale_data.prev_current = 0;
	
	
	ret = bluewhale_data_collect();
	ilimit_current = bluewhale_get_vbus_current();
	charger_current = bluewhale_get_charger_current();
   
//#if OZ115_TRACE
#if 0
    kal_prompt_trace(MOD_MMI, "%s: VBUS Current:%d,Charge Current: %d, Thermal Zone:%d,VBUS OK: %d", 
		__FUNCTION__, 
		ilimit_current,
		charger_current,
		bluewhale_data.thermal_status,
		bluewhale_data.vbus_ok);				
#endif

     //sprintf (buffer,"\r\n bluewhale: chr_type: %d\r\n", chr_type);\
     //rmmi_write_to_uart((kal_uint8*)buffer, strlen(buffer), KAL_TRUE);

#if 1		
		
     /*
	  0: AC CHARGER 
      1: USB CHARGER 
      2: NON-STANDARD AC CHARGER 
      3: USB CHARGING HOST
      4:Indicate NO charger
     */
        
		//if (bluewhale_data.vbus_ok == 1) 
		
			if (chr_type == 0) {
				bluewhale_ac_charge();
			}
			else {
				if (chr_type == 4)
				  bluewhale_stop_charge();
			}
		
#endif		
		
		return 0;
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
    int16_t ret = 0;
	int16_t ilimit_now = 0;
    //disable suspend mode
	ret = bluewhale_update_bits(REG_CHARGER_CONTROL, 0x02, 0 << 1);
    if (ret != 0)   return (-ret);

	//get ilimit_now
	ilimit_now = bluewhale_get_vbus_current();
	if (ret < 0)   return (ret);

	if (STOP_CHG_ILIMIT != ilimit_now)	//if ilimit is not 500
	{
		//collect current now
		//ret = bluewhale_get_current(&bluewhale_data.battery_current);
    	if (ret < 0)   return (ret);
		if (bluewhale_data.battery_current < bluewhale_data.prev_current)
		{
			ret = bluewhale_set_vbus_current(STOP_CHG_ILIMIT);
    		if (ret < 0)   return (ret);
			bluewhale_data.prev_current = bluewhale_data.battery_current;
		}
	}
	else
	{
		if (0 == bluewhale_data.prev_current)	//if prev_current is 0, first time here.
		{
			bluewhale_data.prev_current = bluewhale_data.battery_current;
			ret = bluewhale_set_vbus_current(AC1_CHG_ILIMIT);
			return (ret);
		}
	}
	return ret;
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
    //disable suspend mode
	ret = bluewhale_update_bits(REG_CHARGER_CONTROL, 0x02, 0 << 1);
    if (ret != 0)   return (-ret);
	ret = bluewhale_set_vbus_current(STOP_CHG_ILIMIT);
    if (ret < 0)   return (ret);
	bluewhale_data.prev_current = 0;
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
	int16_t ret = 0;
	ret = bluewhale_set_vbus_current(STOP_CHG_ILIMIT);
    if (ret < 0)   return (ret);
	bluewhale_data.prev_current = 0;
    return ret;
}


/*****************************************************************************
 * Description:
 *		bluewhale_data_collect
 * Parameters:
 *		None
 * Return:
 *      negative errno if any error
 *****************************************************************************/
int16_t bluewhale_data_collect(void)
{
    int16_t ret = 0;
	//int32_t ltemp = 0;
	uint8_t bval = 0;
	bluewhale_thermal_t thm_now = 0;
	int i = 0;
        char buffer[100];
#if 0
    //get battery voltage
	ret = bluewhale_get_cell_volt(&bluewhale_data.battery_voltage);
    if (ret < 0)   return (ret);
	//get battery current
	ret = bluewhale_get_current(&bluewhale_data.battery_current);
    if (ret < 0)   return (ret);
	//check board_offset
	ret = bluewhale_board_offset_read(&ltemp);
	if (ret < 0)	return ret;
	if (bluewhale_pdate_setup.board_offset != 0)
	{
		if (ltemp != bluewhale_pdate_setup.board_offset) {
			ret = bluewhale_board_offset_write(bluewhale_pdate_setup.board_offset);
			if (ret < 0)	return ret;
		}
	}
	else
	{
		if ((ltemp > BOARDOFFSET_DEFAULT) || (ltemp < 0))
		{
			ret = bluewhale_board_offset_write(BOARDOFFSET_DEFAULT);
			if (ret < 0)	return ret;			
		}
	}
#endif

#if 0
//this will make HL100 log buffer full. don't know why.
    //sprintf(buffer, "%s: registers:", __func__);
    for(i=0;i<=0x5;i++)
    {
       ret = bluewhale_read_byte(i, &bval);
       //sprintf(buffer, "[0x%x]=%x", i, bval);
    }
    for(i=0x10;i<=0x13;i++)
    {
       ret = bluewhale_read_byte(i, &bval);
       //sprintf(buffer, "[0x%x]=%x", i, bval);
    }
    
    for(i=0x20;i<=0x30;i=0x30)
    {
       ret = bluewhale_read_byte(i, &bval);
       //sprintf(buffer, "[0x%x]=%x", i, bval);
    }

    for(i=0x40;i<=0x42;i++)
    {
       ret = bluewhale_read_byte(i, &bval);
       //sprintf(buffer, "[0x%x]=%x", i, bval);
    }
       //sprintf(buffer, "\n");
       //rmmi_write_to_uart((kal_uint8*)buffer, strlen(buffer), KAL_TRUE);

#endif
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
	if (bluewhale_data.thermal_status != thm_now)
	{
		bluewhale_data.thermal_status = thm_now;
		//THM RANGE judgdment
		if ((bluewhale_data.thermal_status <= THM_UNDER_T1)
			|| (bluewhale_data.thermal_status >= THM_RANGE_T34))
		{
			ret = bluewhale_set_charger_current(STOP_CHG_CURRENT);
			if (ret < 0)	return ret;
		}
		else if (bluewhale_data.thermal_status == THM_RANGE_T12)
		{
			ret = bluewhale_set_charger_current(T12_CHG_CURRENT);
			if (ret < 0)	return ret;
		}
		else {
			ret = bluewhale_set_charger_current(bluewhale_pdate_setup.max_charger_currentmA);
			if (ret < 0)	return ret;		
		}
	}
    return ret;
}
#endif
