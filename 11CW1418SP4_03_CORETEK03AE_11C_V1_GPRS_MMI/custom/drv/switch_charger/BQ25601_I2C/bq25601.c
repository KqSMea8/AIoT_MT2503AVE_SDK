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
#ifdef __BQ25601_SWCHR_SUPPORT__

#include "bq25601.h"
#include "bmt_utility.h"

kal_int8 bq25601_i2c_addr = 0x6B;		//bq25601 iic slave addr

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
	i2c_read_byte_data(BQ25601_CON10, &chip_id);
	//kal_prompt_trace(MOD_BMT,"%s, bq25601 chip_id:%x", __func__, chip_id);
}

/*****************************************************************************
 * Description:
 *		bluewhale_charger_init, called during system power on init.
 * Parameters:
 *		n/a
 * Return:
 *		negative errno if error happens
 *****************************************************************************/
void bq25601_charger_init(void)
{
	kal_uint8 i;
	unsigned char data;
	i2c_write_byte_data(BQ25601_CON0,BQ25601_REG_VAL0);
	i2c_write_byte_data(BQ25601_CON1,BQ25601_REG_VAL1);
	i2c_write_byte_data(BQ25601_CON2,BQ25601_REG_VAL2);
	i2c_write_byte_data(BQ25601_CON3,BQ25601_REG_VAL3);
	i2c_write_byte_data(BQ25601_CON4,BQ25601_REG_VAL4);
	i2c_write_byte_data(BQ25601_CON5,BQ25601_REG_VAL5);
	i2c_write_byte_data(BQ25601_CON6,BQ25601_REG_VAL6);
	i2c_write_byte_data(BQ25601_CON7,BQ25601_REG_VAL7);
	//bq25601_read_chip_id();
#if 0
	for(i=0;i<11;i++)
	{
		i2c_read_byte_data(i, &data);
		kal_prompt_trace(MOD_BMT,"%s, bq25601 REG[%d]:%x", __func__, i, data);
		data = 0;
	}
#endif
	//return 0;     
}

void bq25601_charger_en(kal_bool chr_en)
{
	if(chr_en)
	{
		kal_prompt_trace(MOD_MMI, "%s -- BAT charge ON", __FUNCTION__);
		i2c_write_byte_data(BQ25601_CON1,BQ25601_REG_VAL1);
	}
	else
	{
		kal_prompt_trace(MOD_MMI, "%s -- BAT charge OFF", __FUNCTION__);
		i2c_write_byte_data(BQ25601_CON1,0x0A);
	}
}

void iot_bat_charger_en(kal_bool chr_en)
{
	bq25601_charger_en(chr_en);
}

#endif

