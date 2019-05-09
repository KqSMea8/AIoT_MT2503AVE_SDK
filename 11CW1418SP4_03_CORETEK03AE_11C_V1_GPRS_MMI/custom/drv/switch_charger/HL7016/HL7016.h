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
//#include "chip.h"
//#include "oz105_swchr_I2C.h"
#include "kal_release.h"
#include "dcl.h"
#include "us_timer.h"
#include "motion_sensor_I2C.h"
/**************************************************************
* #define section
* add constant #define here if any
**************************************************************/
#define		CHARGER_VERSION				2017060910

#define 	BLUEWHALE_FULL     			0x55	// full power confirm code
#define 	RETRY_CNT				    10

#define     STOP_CHG_ILIMIT				500
#define     STOP_CHG_CURRENT			0
#define		T12_CHG_CURRENT				600
#define		AC1_CHG_ILIMIT				900

/**************************************************************
* MOUDLE LEVEL ERROR EVENT NUMBER DEFINITION
**************************************************************/
#define		OZ_STATUS_OK				(00)
#define		OZ_STATUS_TIMEOUT			(51)
#define		OZ_STATUS_INIT_FAIL		    (52)
#define		OZ_STATUS_NODEVICE          (53)
#define		OZ_STATUS_NOT_MATCH		    (54)
#define		OZ_STATUS_INVALID_INPUT 	(55)
#define		OZ_STATIS_BATTERY_OK		(56)

/**************************************************************
* ADT section
* add Abstract Data Type definition here
**************************************************************/
typedef unsigned char    bool;
typedef short           int16_t;
typedef int             int32_t;
typedef unsigned short  uint16_t;
typedef unsigned char   uint8_t;
typedef char            kal_char;

#define _BIT(n) (1 << (n))
#define _BITMASK(field_width) ( _BIT(field_width) - 1)

typedef enum {
	CHG_ADAPTER_NULL = 0,
	CHG_ADAPTER_PC,
	CHG_ADAPTER_NORMAL,
	CHG_ADAPTER_EC,
	CHG_ADAPTER_MTK,
	CHG_ADAPTER_QC,
	CHG_ADAPTER_FIX,
} O2_CHARGER_TYPE_t;

typedef struct t_bluewhale_platform_setup {
        int16_t		max_charger_currentmA;
        int16_t		max_charger_voltagemV;
        int16_t		termination_currentmA;
		int16_t		T34_charger_voltagemV;
		int16_t		T45_charger_voltagemV;
		int16_t		wakeup_voltagemV;
		int16_t		wakeup_currentmA;
		int16_t		recharge_voltagemV;
		int16_t		min_vsys_voltagemV;
		int16_t		vbus_limit_currentmA;
		bool	    rthm_select;	            // 1 for 10K, 0 for 100K thermal resistor
/* these are update according to parameter*/
  //  	int32_t		rsense;				//rsense in uOhm 10mOhm ==> 10000
//	    int32_t		board_offset;		//set board offset manually if not 0

} bluewhale_platform_setup_t;

typedef enum e_bluewhale_thermal {
		THM_DISABLE = 0,
        THM_UNDER_T1 = 1,	//THM<T1
        THM_RANGE_T12,		//T1<THM<T2
        THM_RANGE_T23,		//T2<THM<T3
        THM_RANGE_T34,		//T3<THM<T4
        THM_RANGE_T45,		//T4<THM<T5
        THM_OVER_T5,		//T5<THM
		THM_ITOT
} bluewhale_thermal_t;

typedef struct t_bluewhale_data {
    bool        vbus_ok;            //vbus ok status
    bool        vsys_ovp;           //vsys ovp status
	bool		in_cc_state;        //charge in cc mode status
	bool		in_cv_state;        //charge in cv mode status
	bool		in_wakeup_state;    //wakeup mode status
	bool		initial_state;      //initialized status
	bool		in_full_state;        // full
    bluewhale_thermal_t thermal_status; //thermal zone status

	int32_t		battery_voltage;
	int32_t		battery_current;
	int32_t		prev_current;
    int32_t     vbus_voltage;
    int32_t     charger_type;
    int32_t     ilimit_target;
    
}	bluewhale_data_t;

/**************************************************************
* extern variable declaration section
**************************************************************/
extern bluewhale_platform_setup_t 	bluewhale_pdate_setup;
extern bluewhale_data_t				bluewhale_data;
/**************************************************************
* Function prototypes section
* add function prototype here if any
**************************************************************/
int16_t bluewhale_set_min_vsys(uint16_t min_vsys_mv);
int16_t bluewhale_get_min_vsys(void);
int16_t bluewhale_set_chg_volt(uint16_t chgvolt_mv);
int16_t bluewhale_get_chg_volt(void);
int16_t bluewhale_set_wakeup_volt(uint16_t wakeup_mv);
int16_t bluewhale_set_eoc_current(uint16_t eoc_ma);
int16_t bluewhale_get_eoc_current(void);
int16_t bluewhale_set_vbus_current(uint16_t ilmt_ma);
int16_t bluewhale_get_vbus_current(void);
int16_t bluewhale_set_rechg_hystersis(uint16_t hyst_mv);
int16_t bluewhale_set_charger_current(uint16_t chg_ma);
int16_t bluewhale_get_charger_current(void);

short bluewhale_charger_init(void);
//int16_t bluewhale_battery_monitor_init(void);
//int16_t bluewhale_get_current(int32_t *fcurr);
//int16_t bluewhale_get_cell_volt(int32_t *voltage);
int16_t bluewhale_ac_charge(void);
int16_t bluewhale_usb_charge(void);
int16_t bluewhale_stop_charge(void);
//int16_t bluewhale_sleep(void);
//int16_t bluewhale_wakeup(void);
int16_t bluewhale_get_status(void);
int16_t bluewhale_set_ilimit_target(uint16_t ilmt_ma);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
//		C H G R    O P E R A T I O N    R E G I S T E R    M A P
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define BQ24195_CON0      		0x00
#define BQ24195_CON1      		0x01
#define BQ24195_CON2      		0x02
#define BQ24195_CON3     		0x03
#define BQ24195_CON4     		0x04
#define BQ24195_CON5      		0x05
#define BQ24195_CON6      		0x06
#define BQ24195_CON7      		0x07
#define BQ24195_CON8      		0x08
#define BQ24195_CON9      		0x09
#define BQ24195_CON10      		0x0A

#define BQ24195_REG_WATCHDOG_RESET 	0x5B
#define BQ24195_REG_VAL0		0x47	//0x42//0x47, max current limit 2a
#define BQ24195_REG_VAL1		0x1B
#define BQ24195_REG_VAL2		0x60	//0x00//0x60//0x80,1.5a charge current limit
#define BQ24195_REG_VAL3		0x11		//0x00 
#define BQ24195_REG_VAL4		0xB2
#define BQ24195_REG_VAL5		0x0A	//0x9A   //0x0A
#define BQ24195_REG_VAL6		0x03
#define BQ24195_REG_VAL7		0x4B
#define BQ24195_REG_VAL0_usb	0x2b	//0x37, max current limit 2a
#define BQ24195_REG_VAL2_usb	0x10	//0x60//0x80,1.5a charge current limit

#define			CHG_CURRT_STEP					64	//step 64mA


