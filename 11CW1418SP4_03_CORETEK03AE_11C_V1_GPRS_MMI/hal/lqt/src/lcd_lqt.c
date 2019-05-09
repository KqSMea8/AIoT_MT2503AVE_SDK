/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005-2007
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   lcd_lqt.c
 *
 * Project:
 * --------
 *   Maui
 *
 * Description:
 * ------------
 *   LCD interface between upper application layer and driver for LCM Qualification Tool
 *
 * Author:
 * -------
 * -------
 *   
 *
 *============================================================================
 * HISTORY
 * Below this line, this part is controlled by ClearCase. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *
 * $Log$
 *
 * 01 03 2013 joey.pan
 * removed!
 * Add LCD color enable control.
 *
 * 04 25 2011 jun.pei
 * removed!
 * .
 *
 * 03 22 2011 chelun.tsai
 * removed!
 * lqt move to HAL.
 *
 * 03 03 2011 chelun.tsai
 * removed!
 * .
 *
 * 02 08 2011 chelun.tsai
 * removed!
 * move lcd_lqt.c back to mcu\lqt..
 *
 * 01 10 2011 chelun.tsai
 * removed!
 * move lqt dummy func back for Modis .
 *
 * 12 22 2010 chelun.tsai
 * removed!
 * DDv2 move lqt.
 *
 * 12 14 2010 chelun.tsai
 * removed!
 * RHR check in.
 *
 * 11 17 2010 chelun.tsai
 * removed!
 * remove BB macro MT6253D.
 *
 * 11 09 2010 chelun.tsai
 * removed!
 * .
 *
 * 10 15 2010 chelun.tsai
 * removed!
 * .
 *
 * removed!
 * removed!
 * add compile option for MT6268A
 *
 * removed!
 * removed!
 * to solve superman29 compile error
 *
 * removed!
 * removed!
 * to solve build error on MT6253-series
 *
 * removed!
 * removed!
 * add MT6253-series compile option
 *
 * removed!
 * removed!
 * Add MT6251 compile option
 *
 * removed!
 * removed!
 * Add MT6255/MT6276 Compile option
 *
 * removed!
 * removed!
 * compile option check-in for UBL
 *
 * removed!
 * removed!
 * 
 *
 * removed!
 * removed!
 * correct MT6236B build error
 *
 * removed!
 * removed!
 * correct MT6236B build error
 *
 * removed!
 * removed!
 * modify macro for MT6236
 *
 * removed!
 * removed!
 * 
 *
 * removed!
 * removed!
 * update lqt.c for compile option of RGB565 mode.
 *
 * removed!
 * removed!
 * Update lqt codes
 *
 * removed!
 * removed!
 * to avoid build error when FOTA support
 *
 * removed!
 * removed!
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by ClearCase. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(LQT_SUPPORT)

#include "kal_general_types.h"
#include "kal_public_api.h"
#include "reg_base.h"
#include "lcd_if_hw.h"
#include "lcm_if.h"
#include "lcd_if.h"
#include "lcd_lqt.h"
#include "lcd_sw_inc.h"
#include "lcd_sw.h"
#include "drv_features.h" 

kal_uint8 lcd_at_mode=LCD_AT_RELEASE_MODE;
kal_bool lcd_update_permission = KAL_FALSE; 

#if !defined(__FUE__) && !defined(__UBL__) /* !!CUATION!! for FOTA or bootloader support */
void lcd_config_lqt(kal_uint8 mode,kal_uint8 level, kal_uint8 color)
{
#ifndef DRV_LCD_NOT_EXIST
	  volatile kal_uint32 j;	
    lcd_power_ctrl(KAL_TRUE);
    lcd_at_mode=LCD_AT_LQT_MODE;						
    lcd_update_permission = KAL_TRUE;	
         /*Configure full-screen uppate layer*/    	   
         //#if !(defined(MT6236)||defined(MT6236B)||defined(MT6255)||defined(MT6276)||defined(MT6251)) 
         #if (defined(MT6225)||defined(MT6226)||defined(MT6226D)||defined(MT6226M)||defined(MT6227)||defined(MT6227D)||defined(MT6228)||defined(MT6229)||defined(MT6230)||defined(MT6235)||defined(MT6235B)||defined(MT6238)||defined(MT6239)||defined(MT6268)||defined(MT6268A)||defined(MT6253)||defined(MT6253E)||defined(MT6253L)||defined(MT6252H)||defined(MT6252))   	   
    	   DISABLE_LCD_LAYER0_OPACITY;
	       SET_LCD_LAYER0_OPACITY(0);	
	       DISABLE_LCD_LAYER0_SOURCE_KEY;
	       ENABLE_LAYER0_ADDR_AUTO_INC;
	       SET_LCD_LAYER0_SOURCE_KEY(0);
	       SET_LCD_LAYER0_ROTATE(LCD_LAYER_ROTATE_NORMAL);
	       DISABLE_LAYER0_READ_DATA_SWAP;
	       #else/*(defined(MT6236)||defined(MT6236B)||defined(MT6255)||defined(MT6256)||defined(MT6276)||defined(MT6251)) */
	       DISABLE_LCD_LAYER_OPACITY(0);
	       SET_LCD_LAYER_OPACITY(0,0);
	       DISABLE_LCD_LAYER_SOURCE_KEY(0);
	       SET_LCD_LAYER_SOURCE_KEY(0,0);
	       ENABLE_LAYER_ADDR_AUTO_INC(0);
	       SET_LCD_LAYER_ROTATE(0,0);
	       DISABLE_LAYER_READ_DATA_SWAP(0);
	       #endif
	       
	       #if !(defined(MT6225)||defined(MT6226)||defined(MT6226D)||defined(MT6226M)||defined(MT6227)||defined(MT6227D))
             //#if !(defined(MT6236)||defined(MT6236B)||defined(MT6255)||defined(MT6276)||defined(MT6251))   
             #if (defined(MT6228)||defined(MT6229)||defined(MT6230)||defined(MT6235)||defined(MT6235B)||defined(MT6238)||defined(MT6239)||defined(MT6268)||defined(MT6268A)||defined(MT6253)||defined(MT6253E)||defined(MT6253L)||defined(MT6252H)||defined(MT6252))   	   
	           SET_LCD_LAYER0_RGB565_MODE;
	           #else/*(defined(MT6236)||defined(MT6236B)||defined(MT6255)||defined(MT6256)||defined(MT6276)||defined(MT6251)) */
	           SET_LCD_LAYER_COLOR_FORMAT(0, LCD_LAYER_COLOR_RGB565);
	           #endif 
	       #endif 
	       
	       #ifdef DRV_LCD_ROI_BACKGROUND_COLOR
         REG_LCD_ROI_BACKGROUND_COLOR = 0x0;//Black background
         #endif          
         //#if !(defined(MT6236)||defined(MT6236B)||defined(MT6255)||defined(MT6276)||defined(MT6251))   
         #if (defined(MT6225)||defined(MT6226)||defined(MT6226D)||defined(MT6226M)||defined(MT6227)||defined(MT6227D)||defined(MT6228)||defined(MT6229)||defined(MT6230)||defined(MT6235)||defined(MT6235B)||defined(MT6238)||defined(MT6239)||defined(MT6268)||defined(MT6268A)||defined(MT6253)||defined(MT6253E)||defined(MT6253L)||defined(MT6252H)||defined(MT6252))   	                    
	       SET_LCD_LAYER0_WINDOW_OFFSET(512,512);
	       SET_LCD_LAYER0_WINDOW_SIZE(LCD_WIDTH,LCD_HEIGHT);	
         #else/*(defined(MT6236)||defined(MT6236B)||defined(MT6255)||defined(MT6256)||defined(MT6276)||defined(MT6251)) */
         SET_LCD_LAYER_WINDOW_OFFSET(0, 512,512);
         SET_LCD_LAYER_WINDOW_SIZE(0, LCD_WIDTH,LCD_HEIGHT);	
         #endif
         /*ROI settings*/
	       SET_LCD_ROI_WINDOW_OFFSET(512,512);
	       SET_LCD_ROI_WINDOW_SIZE(LCD_WIDTH,LCD_HEIGHT);
	       REG_LCD_ROI_CTRL &= ~0xF0000000;
         REG_LCD_ROI_CTRL |= 0x80000000;         	
         SET_LCD_ROI_CTRL_OUTPUT_FORMAT(MAIN_LCD_OUTPUT_FORMAT);	  
   switch(mode)
   {
      case Gamma:
         lcd_at_mode=LCD_AT_LQT_MODE;
         MainLCD->LCD_gamma_test(level,color);
			   MainLCD->BlockWrite(0,0,LCD_WIDTH-1, LCD_HEIGHT-1);
         for (j=0;j<100;j++);
         while (LCD_IS_RUNNING){};   
         break;
      case Flicker:
         lcd_at_mode=LCD_AT_LQT_MODE;
         MainLCD->LCD_flicker_test(level);
         MainLCD->BlockWrite(0,0,LCD_WIDTH-1, LCD_HEIGHT-1);
         for (j=0;j<100;j++);
         while (LCD_IS_RUNNING){};   
         break;
      case TEARING:
         ////MainLCD->LCD_tearing_test(level, color);
         break;
      case Release:
         lcd_at_mode=LCD_AT_RELEASE_MODE;
         break;
      default:
         ASSERT(0);
   }			
     lcd_power_ctrl(KAL_FALSE);
     lcd_update_permission = KAL_FALSE;			      
#endif     
}

void lcm_config_lqt(kal_uint8 mode, kal_uint16 cmd, kal_uint16 num, kal_uint16 *ptr)
{
#ifndef DRV_LCD_NOT_EXIST
    lcd_power_ctrl(KAL_TRUE);

    switch(mode)
    {
        case LCM_REG_READ:
            ////MainLCD->LCD_LCMRegRead(cmd, num, ptr);
            break;

        case LCM_RAM_READ:
            ////MainLCD->LCD_LCMRamRead(0, 0, (LCD_WIDTH - 1), (LCD_HEIGHT - 1));
            break;

        case LCM_REG_WRITE:
            ////MainLCD->LCD_LCMRegWrite(cmd, num, ptr);
            break;

        default:
            ASSERT(0);
            break;        
    }

    lcd_power_ctrl(KAL_FALSE);
#endif
}

#endif /* !__FUE__ */

#endif/*LQT_SUPPORT*/

