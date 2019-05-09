/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
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
 *  resource_ecompass.c
 *
 * Project:
 * --------
 *  MAUI
 *
 * Description:
 * ------------
 *  Source file containing the Data Structures for Ecompass application
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
 #include "MMI_features.h"
 
#ifdef __MMI_ECOMPASS__

#include"EcompassDef.h"

const S16 g_ecompass_deg_mecca[MMI_EC_MAX_CITIES] = {
	252,
	242,
	252,
	251,
	245,
	253,
	133,
	135,
	134,
	132,
	105,
	110,
	107,
	105,
	108,
	101,
	107,
	109,
	99,
	38,
	34,
	38,
	40,
	37,
	40,
	66,
	66,
	76,
	78,
	76,
	81,
	76,
	283,
	280,
	278,
	279,
	275,
	279,
	278,
	295,
	278,
	277,
	134,
	137,
	131,
	57,
	58,
	246,
	246,
	279,
	279,
	278,
	278,
	278,
	67,
	67,
	123,
	123,
	125,
	124,
	64,
	274,
	274,
	274,
	73,
	74,
	74,
	72,
	74,
	133,
	135,
	136,
	69,
	70,
	66,
	69,
	291,
	291,
	150,
	141,
	148,
	150,
	142,
	152,
	281,
	284,
	281,
	284,
	287,
	288,
	288,
	59,
	57,
	55,
	25,
	54,
	59,
	57,
	59,
	55,
	17,
	40,
	65,
	58,
	60,
	57,
	59,
	58,
	74,
	84,
	82,
	279,
	277,
	283,
	283,
	285,
	282,
	63,
	65,
	65,
	65,
	65,
	42,
	45,
	45,
	60,
	60,
	60,
	137,
	131,
	131,
	133,
	59,
	56,
	56,
	60,
	56,
	156,
	138,
	141,
	134,
	136,
	138,
	63,
	63,
	63,
	292,
	292,
	67,
	67,
	65,
	67,
	67,
	66,
	66,
	66,
	136,
	124,
	136,
	136,
	136,
	144,
	141,
	138,
	139,
	56,
	55,
	163,
	157,
	158,
	160,
	5,
	5,
	351,
	14,
	350,
	12,
	282,
	158,
	158,
	155,
	158,
	119,
	116,
	115,
	119,
	119,
	73,
	73,
	73,
	73,
	183,
	193,
	137,
	127,
	128,
	133,
	64,
	65,
	64,
	98,
	135,
	136,
	132,
	136,
	84,
	67,
	295,
	295,
	54,
	55,
	68,
	68,
	62,
	62,
	285,
	141,
	141,
	111,
	106,
	111,
	273,
	289,
	288,
	280,
	278,
	267,
	281,
	261,
	295,
	293,
	295,
	294,
	295,
	222,
	221,
	208,
	226,
	217,
	235,
	219,
	237,
	200,
	218,
	202,
	200,
	220,
	196,
	196,
	202,
	192,
	114,
	128,
	124,
	124,
	124,
	123,
	128,
	60,
	60,
	59,
	60,
	288,
	289,
	293,
	291,
	293,
	161,
	161,
	160,
	231,
	234,
	247,
	13,
	359,
	0,
	7,
	9,
	245,
	246,
	224,
	225,
	223,
	225,
	151,
	155,
	162,
	165,
	162,
	161,
	164,
	160,
	163,
	67,
	67,
	65,
	67,
	113,
	116,
	110,
	98,
	109,
	123,
	285,
	350,
	346,
	291,
	292,
	291,
	293,
	292,
	292,
	293,
	293,
	301,
	72,
	76,
	74,
	336,
	336,
	47,
	37,
	36,
	43,
	44,
	27,
	46,
	271,
	268,
	90,
	94,
	96,
	95,
	91,
	95,
	99,
	95,
	91,
	97,
	7,
	8,
	9,
	29,
	29,
	28,
	268,
	272,
	272,
	270,
	126,
	261,
	256,
	260,
	256,
	76,
	71,
	72,
	64,
	64,
	65,
	68,
	285,
	286,
	285,
	285,
	284,
	286,
	139,
	226,
	283,
	165,
	34,
	49,
	263,
	266,
	290,
	260,
	267,
	256,
	268,
	260,
	260,
	254,
	256,
	258,
	259,
	154,
	151,
	154,
	158,
	156,
	157,
	156,
	69,
	72,
	289,
	152,
	146,
	148,
	98,
	101,
	98,
	253,
	149,
	154,
	187,
	195,
	150,
	176,
	22,
	245,
	219,
	245,
	245,
	101,
	172,
	115,
	244,
	157,
	285,
	74,
	70,
	293,
	293,
	293,
	138,
	336,
	353,
	345,
	23,
	10,
	15,
	15,
	287,
	286,
	287,
	111,
	104,
	99,
	106,
	295,
	296,
	292,
	294,
	55,
	28,
	39,
	53,
	148,
	123,
	121,
	165,
	170,
	168,
	177,
	287,
	244,
	246,
	1,
	16,
	287,
	160,
	181,
	152,
	162,
	162,
	260,
	258,
	257,
	258,
	118,
	118,
	119,
	118,
	119,
	52,
	49,
	44,
	35,
	52,
	45,
	24,
	58,
	25,
	57,
	240,
	123,
	66,
	65,
	65,
	284,
	331,
	332,
	309,
	326,
	139,
	142,
	17,
	15,
	13
};

const U16 g_ecompass_country_indices[MMI_EC_MAX_COUNTRIES + 1] = {
	0,
	6,
	10,
	19,
	23,
	25,
	27,
	32,
	42,
	45,
	47,
	49,
	54,
	56,
	60,
	61,
	64,
	69,
	72,
	76,
	78,
	84,
	88,
	91,
	94,
	102,
	108,
	111,
	117,
	122,
	125,
	128,
	132,
	137,
	138,
	142,
	143,
	146,
	148,
	156,
	165,
	167,
	171,
	177,
	178,
	182,
	187,
	191,
	193,
	197,
	200,
	201,
	205,
	206,
	207,
	209,
	211,
	213,
	215,
	216,
	218,
	221,
	229,
	234,
	244,
	251,
	252,
	258,
	262,
	267,
	270,
	273,
	278,
	280,
	284,
	286,
	293,
	297,
	303,
	304,
	306,
	314,
	315,
	318,
	320,
	327,
	329,
	339,
	342,
	345,
	349,
	350,
	354,
	357,
	361,
	367,
	368,
	374,
	376,
	386,
	393,
	395,
	396,
	399,
	402,
	403,
	405,
	409,
	410,
	420,
	421,
	422,
	425,
	426,
	429,
	433,
	436,
	440,
	444,
	448,
	449,
	451,
	455,
	456,
	458,
	460,
	461,
	465,
	466,
	470,
	475,
	485,
	486,
	487,
	490,
	491,
	495,
	497,
	498,
	MMI_EC_MAX_CITIES
};


const U8 g_ecompass_country_multitap[MMI_EC_MULTITAP_COUNTRIES] = {
	0,9,22,35,37,43,46,57,60,67,70,74,78,88,96,98,104,105,108,122,127,132,135,135,135,137};

#endif