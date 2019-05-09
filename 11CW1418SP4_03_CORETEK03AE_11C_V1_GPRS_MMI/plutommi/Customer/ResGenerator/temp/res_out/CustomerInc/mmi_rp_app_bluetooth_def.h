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

/*******************************************************************************
 * Filename:
 * ---------
 *   mmi_rp_app_bluetooth_def.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   Resource populate function generated by XML resgen
 *
 * Author:
 * -------
 *   MTK resgenerator XML parser
 *
 *******************************************************************************/


#ifndef _MMI_RP_APP_BLUETOOTH_DEF_H
#define _MMI_RP_APP_BLUETOOTH_DEF_H




/******************** Menu resource IDs - begin ********************/
typedef enum
{
    MENU_CONN_BT_POWER = 30586 + 1, /* BASE_ID + 1 */
    MENU_CONN_BT_SCH_AUDIO_DEV,
    MENU_CONN_BT_MYDEV,
    MENU_CONN_BT_SET_NAME,
    MENU_CONN_BT_SET_AUDIO_PATH_PHONE,
    MENU_CONN_BT_SET_AUDIO_PATH_HF,
    MENU_CONN_BT_SET_DEV_ADDR,
    MENU_CONN_BT_SET_STORAGE,
    MENU_CONN_BT_SET_AUDIO_PATH,
    MENU_CONN_BT_SET_VIS,
    MENU_CONN_BT_SET,
    MENU_CONN_BT_MAIN,
    MMI_RP_APP_BLUETOOTH_MENU_MAX
}mmi_rp_app_bluetooth_menu_enum;
/******************** Menu resource IDs - finish ********************/


/******************** String resource IDs - begin ********************/
typedef enum
{
    STR_BT_MENU_TITLE = 30586 + 1, /* BASE_ID + 1 */
    STR_BT_POWER,
    STR_BT_SWITCHING,
    STR_BT_RELEASE_ALL_CONN,
    STR_BT_RELEASE_DEV_CONN,
    STR_BT_RELEASING,
    STR_BT_SCH_AUD_DEV,
    STR_BT_SCH_ALL_DEV,
    STR_BT_SCH_RESULT,
    STR_BT_INQUIRY_STOPPING,
    STR_BT_DEVICE_BEING_CONNECTED,
    STR_BT_ENTER_PASSCODE,
    STR_BT_PAIR_REQ,
    STR_BT_PAIR_FAILED,
    STR_BT_START_PAIRING,
    STR_BT_CONNECT_CONFIRM,
    STR_BT_ACCEPT_CONNECT_FROM,
    STR_BT_REACH_MAX_PAIR_NUM,
    STR_BT_REFRESHING,
    STR_BT_CONN_FAILED,
    STR_BT_MYDEVICE,
    STR_BT_MYDEV_OPT_SUP_SER,
    STR_BT_DEV_SET_VIS,
    STR_BT_DEV_SET_NAME,
    STR_BT_DEV_SET_AUDIO_PATH,
    STR_BT_DEV_SET_AUDIO_PATH_PHONE,
    STR_BT_DEV_SET_AUDIO_PATH_HF,
    STR_BT_DEV_SET_DEV_ADDR,
    STR_BT_ABOUT,
    STR_BT_BD_NAME,
    STR_BT_SERVICE,
    STR_BT_CONN_SERVICE,
    STR_BT_NO_SERVICE,
    STR_BT_BD_ADDR,
    STR_BT_PROF_UNKNOWN,
    STR_BT_NOT_READY,
    STR_BT_NO_SUPPORTED_PROFILE,
    STR_BT_PWR_ON_CNF,
    STR_BT_POWER_ON_IN_FLIGHT_MODE_CNF,
    STR_BT_DISCONN_ALL_CONN_CNF,
    STR_BT_DISCONN_ALL_CONN,
    STR_BT_DISCONN_DEV_CONN_CNF,
    STR_BT_DISCONN_DEV_CONN,
    STR_BT_DISCONN_AUD_DEV_CONN_CNF,
    STR_BT_PAIRED_DEV_FULL,
    STR_BT_PROHIBIT_VIS_BY_A2DP,
    STR_BT_OPERATION_NOT_ALLOWED_DIS_HFP_FIRST,
    STR_BT_PROHIBIT_BY_SIMAP,
    STR_BT_16_DIGITS_PASSKEY,
    STR_BT_INPUT_PASSCODE,
    STR_BT_NUMERIC_COMPARISON,
    STR_BT_ACCEPT_PAIRING,
    STR_BT_FROM,
    STR_BT_QUESTION_MARK,
    STR_BT_CONN_SINGLE_CONNECTING,
    STR_BT_SERVICE_UNAVAILABLE,
    STR_ID_FAILED_SHARE_AS_DILAER_CONNED,
    MMI_RP_APP_BLUETOOTH_STR_MAX
}mmi_rp_app_bluetooth_str_enum;
/******************** String resource IDs - finish ********************/


/******************** Screen resource IDs - begin ********************/
typedef enum
{
    SCR_EXTDEV_BT_BEGIN = 30586 + 1, /* BASE_ID + 1 */
    GRP_ID_BT_CM,
    GRP_ID_BT_CM_POWER_ON_CNF,
    GRP_ID_BT_CM_SELECT_DEVICE,
    GRP_ID_BT_CM_INQUIRY_DEVICE,
    GRP_ID_BT_CM_PROGRESS,
    GRP_ID_BT_CM_PASSKEY_INPUT,
    GRP_ID_BT_CM_SECURITY_CNF,
    GRP_ID_BT_CM_PASSKEY_NOTIFY,
    GRP_ID_BT_CM_AUTH,
    GRP_ID_BT_CM_BLOCK_CNF,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_HFP,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_HSP,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_BIP,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_FTP,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_OPP,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_SPP,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_DUN,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_FAX,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_PBAP,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_HID,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_SYNC,
    GRP_ID_BT_CM_CONNECT_ACCEPT_CNF_SAP,
    GRP_ID_BT_PBAPC_AUTHENTICATE,
    GRP_ID_BT_AUTO_CONNECT,
    SCR_BT_MAIN,
    SCR_BT_POWER_ON_CNF,
    SCR_BT_RELEASE_DEV_CONN_CNF,
    SCR_BT_RELEASE_ALL_CONN_CNF,
    SCR_BT_RELEASE_AUD_CONN_CNF,
    SCR_BT_POWER_PRO,
    SCR_BT_REL_ALL_CONN_PRO,
    SCR_BT_REL_DEV_CONN_PRO,
    SCR_BT_REL_PROFILE_CONN_PRO,
    SCR_BT_SWITCH_LIMIT_DISCOVER_MODE,
    SCR_BT_LIMIT_DISCOVER_MODE,
    SCR_BT_SCH_PRO,
    SCR_BT_SCH_DISCONNECT_SCO,
    SCR_BT_SCH_RES,
    SCR_BT_SCH_CANCEL_PRO,
    SCR_BT_BOND,
    SCR_BT_REFREAH_PROFILE,
    SCR_BT_PIN_INPUT,
    SCR_PBAPC_PASSWORD,
    SCR_BT_PAIR_PRO,
    SCR_BT_AUTH_CONFIRM,
    SCR_BT_CONNECT_CNF,
    SCR_BT_CONNECTING,
    SCR_BT_CONNECT_ACCEPT_CNF_HFP,
    SCR_BT_CONNECT_ACCEPT_CNF_HSP,
    SCR_BT_CONNECT_ACCEPT_CNF_BIP,
    SCR_BT_CONNECT_ACCEPT_CNF_FTP,
    SCR_BT_CONNECT_ACCEPT_CNF_OPP,
    SCR_BT_CONNECT_ACCEPT_CNF_SPP,
    SCR_BT_CONNECT_ACCEPT_CNF_DUN,
    SCR_BT_CONNECT_ACCEPT_CNF_FAX,
    SCR_BT_CONNECT_ACCEPT_CNF_PBAP,
    SCR_BT_CONNECT_ACCEPT_CNF_HID,
    SCR_BT_CONNECT_ACCEPT_CNF_SYNC,
    SCR_BT_CONNECT_ACCEPT_CNF_SAP,
    SCR_BT_NUMERIC_CONFIRM,
    SCR_BT_PASSKEY_NOTIFY,
    SCR_BT_UNBLOCKING,
    SCR_BT_DEVICE_BLOCK_CNF,
    SCR_BT_REMOTE_CTRL_POWER_CONFIRM,
    SCR_BT_REMOTE_CTRL_CONN_AUTH,
    SCR_BT_REMOTE_CTRL_APP_SELECT,
    SCR_BT_REMOTE_CTRL_APP,
    SCR_BT_REMOTE_CTRL_UNPLUG_CONFIRM,
    SCR_BT_REMOTE_CTRL_DISC_CONFIRM,
    SCR_BT_REMOTE_CTRL_PROGRESS,
    SCR_BT_MYDEV,
    SCR_BT_MYDEV_OPT,
    SCR_BT_MYDEV_PROF_CHO,
    SCR_BT_MYDEV_CON,
    SCR_BT_MYDEV_CONNECTED_PROF,
    SCR_BT_MYDEV_DISCON,
    SCR_BT_MYDEV_SIM_CNF,
    SCR_BT_MYDEV_SIM_SEL,
    SCR_BT_MYDEV_REN,
    SCR_BT_MYDEV_REN_OPT,
    SCR_BT_MYDEV_AUTH,
    SCR_BT_MYDEV_DEL,
    SCR_BT_MYDEV_DEL_ALL,
    SCR_BT_MYDEV_SUP_PROFILE,
    SCR_BT_MYDEV_SCH,
    SCR_BT_ACT_DEV,
    SCR_BT_DISC_PROC,
    SCR_BT_ENFORCE_DISC_CNF,
    SCR_BT_SET,
    SCR_BT_SET_NAME,
    SCR_BT_SET_NAME_OPT,
    SCR_BT_SET_VIS,
    SCR_BT_SET_VIS_MENU,
    SCR_BT_SET_AUTH,
    SCR_BT_SET_AUDIO_PATH,
    SCR_BT_SET_BLOCK_LIST,
    SCR_BT_SET_FTP,
    SCR_BT_SET_SIMAP,
    SCR_BT_SET_STORAGE,
    SCR_BT_SET_SHARING,
    SCR_BT_SET_PERMISSION,
    SCR_BT_SET_IMAGE_FOLDER,
    SCR_BT_SET_STORAGE_CONFIRM,
    SCR_BT_SEL_DRV1,
    SCR_BT_SEL_DRV2,
    SCR_BT_SEL_DRV3,
    SCR_BT_SEL_DRV4,
    SCR_BT_SEND_DUMMY,
    SCR_BT_SEND_DEVICE_OPTION,
    SCR_BT_SEND_CNF_TO_DEFAULT,
    SCR_BT_SEND_CNF_SET_DEFAULT,
    SCR_BT_SEND_CNF_POWER_ON,
    SCR_BT_ABOUT,
    SCR_BT_MY_SPECIFIC_DEV,
    SCR_BT_MY_SPECIFIC_DEV_DUMMY,
    SCR_BT_SWITCH_SPEECH_PATH,
    SCR_BT_CONNECT_LAST_HEADSET,
    SCR_BT_NOT_ALLOWED,
    SCR_BT_NOT_PROHIBIT_BY_SIMAP,
    SCR_BT_SET_DEFAULT_HS,
    SCR_BT_CM_LAST,
    SCR_HF_CONNECTING,
    SCR_HF_RELEASING,
    SCR_OPP_NOTY_FILE_RECV,
    SCR_OPP_NOTY_RECV_ACCEPT,
    SCR_OPP_SENDING,
    SCR_OPP_ABORTING,
    SCR_OPP_DISCONNECTING,
    SCR_FTP_SETTING,
    SCR_FTP_ACCESS_RIGHT_OPTION,
    SCR_FTP_SELECT_SHARED_FOLDER,
    SCR_FTP_NOTY_CONN_REQ,
    SCR_FTP_FILE_BROWSER,
    SCR_FTP_NOTY_FILE_RECV,
    SCR_FTP_RECEIVING,
    SCR_FTP_DISCONNECTING,
    SCR_FTP_ABORTING,
    SCR_FTP_DETAIL_INFO,
    SCR_FTP_MARK_SEVERAL,
    SCR_FTP_CONNECTING,
    SCR_FTP_OPENING_FOLDER,
    SCR_FTP_CONNECT_CANCELING,
    SCR_ID_BT_AVCRP_CT_MAIN,
    SCR_BPP_DUMMY,
    SCR_BPP_CONNECTING,
    SCR_BPP_AUTHENTICATING,
    SCR_BPP_GETTING,
    SCR_BPP_PRINT_SETTING,
    SCR_BPP_LAYOUT_PREVIEW,
    SCR_BPP_CREATING,
    SCR_BPP_COMPOSING,
    SCR_BPP_PRINTING,
    SCR_BPP_TROUBLE,
    SCR_BPP_DISCONNECTING,
    SCR_OBEX_INPUT_AUTHPWD,
    SCR_BPP_SP_PRINT_SETUP,
    SCR_BPP_SP_PREVIEW,
    SCR_BPP_SP_PRINTING,
    SCR_BPP_SP_PASSWORD_INPUT,
    SCR_BPP_SP_TROUBLE,
    SCR_BPP_SP_DUMMY,
    SCR_BPP_SP_PRINT_OPTION,
    SCR_BPP_PRINTER_SETUP_OPTION,
    GRP_ID_BIP_OBJ_DETAIL,
    SCR_BIP_SETTING,
    SCR_BIP_SENDING,
    SCR_BIP_CONNECTING,
    SCR_BIP_ABORTING,
    SCR_BIP_RECEIVING,
    SCR_BIP_DISCONNECTING,
    SCR_BIP_IMGLIST_BROWSER,
    SCR_BIP_IMG_DETAIL,
    SCR_BIP_THM_PREVIEW,
    SCR_BIP_IMGLIST_OPTION,
    SCR_BIP_GETTING_IMGPROP,
    SCR_BIP_NOTY_CONN_REQ,
    SCR_BIP_NOTY_FILE_RECV,
    SCR_BIP_NOTY_CONTINUE_SEND,
    SCR_BIP_EXIT_CONFIRM,
    SCR_BIP_CONNECT_CANCELING,
    SCR_BIP_PUSH_ABORTING,
    SCR_BIP_RECV_FILE_CONFIRM,
    GRP_ID_PBAP_AUTHENTICATION,
    SCR_PBAP_AUTHENTICATING_ID,
    SCR_PBAP_NOTY_CONN_REQ_ID,
    SCR_EXTDEV_BT_LAST,
    GRP_ID_BT_BPP,
    MMI_RP_APP_BLUETOOTH_SCR_MAX
}mmi_rp_app_bluetooth_scr_enum;
/******************** Screen resource IDs - finish ********************/


/******************** Timer resource IDs - begin ********************/
typedef enum
{
    BT_LP2_TESTING_TIMER = 30586 + 1, /* BASE_ID + 1 */
    BT_LP2_DELAY_TIMER,
    BT_MMI_MAP_CONNECT_TIMER,
    BT_AUTHORIZE_SCREEN_OPP_TIMER,
    BT_AUTHORIZE_SCREEN_FTP_TIMER,
    BT_AUTHORIZE_SCREEN_BIP_TIMER,
    BT_AUTHORIZE_HID_TIMER,
    BT_AUTHORIZE_SCREEN_HFP_TIMER,
    BT_AUTHORIZE_SCREEN_HSP_TIMER,
    BT_AUTHORIZE_SCREEN_SPP_TIMER,
    BT_AUTHORIZE_SCREEN_DUN_TIMER,
    BT_AUTHORIZE_SCREEN_FAX_TIMER,
    BT_AUTHORIZE_SCREEN_SIMAP_TIMER,
    BT_AUTHORIZE_SCREEN_PBAP_TIMER,
    BT_AUTHORIZE_SCREEN_SYNC_TIMER,
    BT_SEND_DUMMY_TIMER,
    BT_PROG_UPDATE_TIMER,
    MMI_RP_APP_BLUETOOTH_TIMER_MAX
}mmi_rp_app_bluetooth_timer_enum;
/******************** Timer resource IDs - finish ********************/


/******************** Events resource IDs - begin ********************/
typedef enum
{
    EVT_ID_SPPC_CONNECT_CNF = 30586 + 1, /* BASE_ID + 1 */
    EVT_ID_SPPC_DISCONNECT_CNF,
    EVT_ID_SPPC_READY_TO_READ,
    EVT_ID_SPPC_READY_TO_WRITE,
    EVT_ID_SPPS_CONNECT_IND,
    EVT_ID_SPPS_DISCONNECT_CNF,
    EVT_ID_SPPS_READY_TO_READ,
    EVT_ID_SPPS_READY_TO_WRITE,
    MMI_RP_APP_BLUETOOTH_EVENT_MAX
}mmi_rp_app_bluetooth_event_enum;
/******************** Events resource IDs - begin ********************/
/******************** Template resource IDs - Begin ********************/
/******************** Template resource IDs - End ********************/


/******************** MMI Cache IDs - Begin ********************/
typedef enum
{
    NVRAM_BT_RECEIVE_DRIVE = 30586 + 1, /* BASE_ID + 1 */       /*         BYTE */ 
    NVRAM_BT_RECEIVE_DRIVE_ASK,                                 /*         BYTE */ 
    NVRAM_BT_POWER_STATUS,                                      /*         BYTE */ 
    MMI_RP_APP_BLUETOOTH_MMI_CACHE_MAX
} mmi_rp_app_bluetooth_mmi_cache_enum;
/******************** MMI Cache IDs - End ********************/


/******************** Image resource IDs - begin ********************/
typedef enum
{
    IMG_BT_MENU_TITLE = 30586 + 1, /* BASE_ID + 1 */
    IMG_BT_SCH_TITLE,
    IMG_BT_DEV_PRT,
    IMG_BT_DEV_LAP,
    IMG_BT_DEV_MOB,
    IMG_BT_DEV_EAR,
    IMG_BT_DEV_OTHER,
    IMG_BT_CONNECTION_DATA,
    MMI_RP_APP_BLUETOOTH_IMG_MAX
}mmi_rp_app_bluetooth_img_enum;
/******************** Image resource IDs - finish ********************/


#endif /* _MMI_RP_APP_BLUETOOTH_DEF_H */