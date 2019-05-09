#ifndef __BQ25601_H__
#define __BQ25601_H__

/* ========================================================================== */
/*                                                                            */
/*   bluewhale_charger.h                                                      */
/*   (c) 2001 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/* This program is free software; you can redistribute it and/or modify it    */
/* under the terms of the GNU General Public License version 2 as published   */
/* by the Free Software Foundation.											  */
/*																			  */
/* This program is distributed in the hope that it will be useful, but        */
/* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY */
/* or FITNESS FOR A PARTICULAR PURPOSE.										  */
/* See the GNU General Public License for more details.						  */
/*																			  */
/* You should have received a copy of the GNU General Public License along	  */
/* with this program.  If not, see <http://www.gnu.org/licenses/>.			  */
/* ========================================================================== */
/**************************************************************
* #include section
* add #include here if any
**************************************************************/
#include "kal_release.h"
#include "i2c_drv.h"

void bq25601_charger_init(void);
void bq25601_read_chip_id(void);
void bq25601_charger_en(kal_bool chr_en);	//bat charge enable
void iot_bat_charger_en(kal_bool chr_en);


#define 	BQ25601_CON0      		0x00
#define 	BQ25601_CON1      		0x01
#define 	BQ25601_CON2      		0x02
#define 	BQ25601_CON3     		0x03
#define 	BQ25601_CON4     		0x04
#define 	BQ25601_CON5      		0x05
#define 	BQ25601_CON6      		0x06
#define 	BQ25601_CON7      		0x07
#define 	BQ25601_CON8      		0x08
#define 	BQ25601_CON9      		0x09
#define 	BQ25601_CON10      		0x0A

#define 	BQ25601_REG_WATCHDOG_RESET 	0x5B
/*  TI BQ25601 */
#define 	BQ25601_REG_VAL0		0x17
#define 	BQ25601_REG_VAL1		0x1A
#define 	BQ25601_REG_VAL2		0xA2	//0x00//0x60//0x80,1.5a charge current limit
#define 	BQ25601_REG_VAL3		0x11		//0x00 
#define 	BQ25601_REG_VAL4		0x58
#define 	BQ25601_REG_VAL5		0x07	//10 hour charge disable charge timer
#define 	BQ25601_REG_VAL6		0xAB	//0xA6	VINDPM threshold:5V
#define 	BQ25601_REG_VAL7		0x4C
#define 	BQ25601_REG_VAL0_usb	0x2b	//0x37, max current limit 2a
#define 	BQ25601_REG_VAL2_usb	0x10	//0x60//0x80,1.5a charge current limit

#define		CHG_CURRT_STEP			64	//step 64mA

#endif

