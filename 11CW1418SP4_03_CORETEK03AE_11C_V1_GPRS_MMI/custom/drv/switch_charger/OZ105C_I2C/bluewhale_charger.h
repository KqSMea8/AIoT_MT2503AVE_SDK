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
#define		CHARGER_VERSION				17011610002

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
		bool	    rthm_select;	            //1 for 10K, 0 for 100K thermal resistor
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
    bluewhale_thermal_t thermal_status; //thermal zone status

	int32_t		battery_voltage;
	int32_t		battery_current;
	int32_t		prev_current;
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

short bluewhale_charger_init(char chr_type);
//int16_t bluewhale_battery_monitor_init(void);
//int16_t bluewhale_get_current(int32_t *fcurr);
//int16_t bluewhale_get_cell_volt(int32_t *voltage);
int16_t bluewhale_ac_charge(void);
int16_t bluewhale_usb_charge(void);
int16_t bluewhale_stop_charge(void);
int16_t bluewhale_sleep(void);
int16_t bluewhale_wakeup(void);
int16_t bluewhale_data_collect(void);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
//		C H G R    O P E R A T I O N    R E G I S T E R    M A P
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define     BLUEWHALE_CHGR_ADDR         0x20

/* Voltage Registers (R/W) */
#define		REG_CHARGER_VOLTAGE			0x00
#define		REG_T34_CHG_VOLTAGE			0x01
#define		REG_T45_CHG_VOLTAGE			0x02
#define			CHG_VOLT_STEP					25	//step 25mV
#define		REG_WAKEUP_VOLTAGE			0x03
#define			WK_VOLT_STEP					100	//step 100mV
#define		REG_RECHARGE_HYSTERESIS		0x04
#define			RECHG_VOLT_STEP					50	//step 50mV
#define		REG_MIN_VSYS_VOLTAGE		0x05
#define			VSYS_VOLT_STEP					200	//step 200mV

/* Current Registers (R/W) */
#define		REG_CHARGE_CURRENT			0x10
#define			CHG_CURRT_STEP					100	//step 100mA
#define		REG_WAKEUP_CURRENT			0x11
#define			WK_CURRT_STEP					10	//step 10mA
#define		REG_END_CHARGE_CURRENT		0x12
#define			EOC_CURRT_STEP					10	//step 10mA
#define		REG_VBUS_LIMIT_CURRENT		0x13
#define			VBUS_ILMT_STEP					100	//step 100mA

/* Protection Register (R/W) */
#define		REG_SAFETY_TIMER			0x20
#define			WAKEUP_TIMER_MASK				0x0F
#define			WK_TIMER_15MIN					0x01	//15min wakeup timer
#define			WK_TIMER_30MIN					0x02	//30min wakeup timer
#define			WK_TIMER_45MIN					0x03	//45min wakeup timer
#define			WK_TIMER_60MIN					0x04	//60min wakeup timer
#define			WK_TIMER_75MIN					0x05	//60min wakeup timer
#define			WK_TIMER_90MIN					0x06	//60min wakeup timer
#define			WK_TIMER_105MIN					0x07	//60min wakeup timer
#define			CC_TIMER_MASK					0xF0
#define			CC_TIMER_120MIN					0x10	//120min CC charge timer
#define			CC_TIMER_180MIN					0x20	//180min CC charge timer
#define			CC_TIMER_240MIN					0x30	//240min CC charge timer
#define			CC_TIMER_300MIN					0x40	//300min CC charge timer
#define			CC_TIMER_390MIN					0x50	//390min CC charge timer
#define			CC_TIMER_480MIN					0x60	//480min CC charge timer
#define			CC_TIMER_570MIN					0x70	//570min CC charge timer

/* Charger Control Register (R/W) */
#define		REG_CHARGER_CONTROL			0x30
#define			RTHM_SELECT						0x01	//0:100K, 1:10K

/* Status Registers (R) */
#define		REG_VBUS_STATUS				0x40
#define			VSYS_OVP_FLAG					0x01	//VSYS OVP event flag
#define			VBUS_UVP_FLAG					0x10	//VBUS UVP event flag
#define			VBUS_OK_FLAG					0x20	//VBUS OK flag
#define			VBUS_OVP_FLAG					0x40	//VBUS OVP event flag
#define			VDC_PR_FLAG                     0x80    // 1 when VDC < VDC threshold for system priority

#define		REG_CHARGER_STATUS			0x41
#define			CHARGER_INIT					0x01	//Before init flag
#define			IN_WAKEUP_STATE					0x02	//In Wakeup State
#define			IN_CC_STATE						0x04	//In CC Charge State
#define			IN_CV_STATE						0x08	//In CV Charge State
#define			CHARGE_FULL_STATE				0x10	//Charge Full State
#define			WK_TIMER_FLAG					0x20	//WK CHG Timer Overflow
#define			CC_TIMER_FLAG					0x40	//CC CHG Timer Overflow 

#define		REG_THM_STATUS				0x42
#define			THM_T1_STATE					0x01	//T1 > THM state 
#define			THM_T12_STATE					0x02	//THM in T12 state 
#define			THM_T23_STATE					0x04	//THM in T23 state 
#define			THM_T34_STATE					0x08	//THM in T34 state 
#define			THM_T45_STATE					0x10	//THM in T45 state 
#define			THM_T5_STATE					0x20	//THM > T5 state 
#define			INT_OTP_FLAG					0x40	//Internal OTP event

