# 1 "temp/res/000_FileMgr.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "temp/res/000_FileMgr.c"
# 1 "../../mmi/inc/MMI_features.h" 1
# 69 "../../mmi/inc/MMI_features.h"
# 1 "../../mmi/inc/MMI_features_switch.h" 1
# 67 "../../mmi/inc/MMI_features_switch.h"
# 1 "../../mmi/inc/MMI_features_type.h" 1
# 68 "../../mmi/inc/MMI_features_switch.h" 2
# 70 "../../mmi/inc/MMI_features.h" 2
# 2 "temp/res/000_FileMgr.c" 2
# 1 "../customerinc/custresdef.h" 1
# 207 "../customerinc/custresdef.h"
# 1 "../../mmi/inc/MMI_features.h" 1
# 208 "../customerinc/custresdef.h" 2
# 3 "temp/res/000_FileMgr.c" 2



<?xml version="1.0" encoding="UTF-8"?>

<APP id="APP_FILEMANAGER"
# 18 "temp/res/000_FileMgr.c"
  name = "STR_FMGR_TITLE"

>
    <INCLUDE file="GlobalResDef.h"/>
    <INCLUDE file="RightsMgrResDef.h"/>
    <INCLUDE file="FileMgrServiceResDef.h"/>



    <INCLUDE file="gdi_features.h"/>




        <MEMORY heap = "$(MMI_FMGR_APP_MEM_SIZE)" inc = "FileMgrGProt.h"/>


<!--
**************************************** Image section ****************************************
-->
# 71 "temp/res/000_FileMgr.c"
    <IMAGE id="IMG_ID_FMGR_DRIVE_PHONE">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\FileManager\\\\FM_phone.bmp"</IMAGE>





    <IMAGE id="IMG_ID_FMGR_ICON_FILE_TYPE_FOLDER">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\FileManager\\\\FM_SIFOLDER.bmp"</IMAGE>
    <IMAGE id="IMG_ID_FMGR_ICON_FILE_TYPE_UNKNOWN">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\FileManager\\\\FM_SIUNKNOWN.bmp"</IMAGE>
# 108 "temp/res/000_FileMgr.c"
<!--
**************************************** String section ***************************************
-->

    <STRING id="STR_ID_FMGR_NO_FORWARD_OPTION"/>


    <STRING id="STR_FMGR_TITLE"/>
    <STRING id="STR_FMGR_FOLDER_CREATE"/>



    <STRING id="STR_FMGR_FORMATING"/>
# 130 "temp/res/000_FileMgr.c"
    <STRING id="STR_FMGR_UNMOUNTING"/>





    <STRING id="STR_FMGR_USE"/>



    <STRING id="STR_FMGR_COPY_TO"/>
    <STRING id="STR_FMGR_MOVE_TO"/>
# 170 "temp/res/000_FileMgr.c"
    <STRING id="STR_ID_FMGR_FILE_DATE"/>
    <STRING id="STR_ID_FMGR_FILE_SIZE"/>
    <STRING id="STR_ID_FMGR_SYSTEM_BUSY_TO_DECODE"/>




    <STRING id="STR_ID_FMGR_NO_AVAILABLE_STORAGE"/>

    <STRING id="STR_ID_FMGR_NEW_FOLDER_LSK"/>
    <STRING id="STR_ID_FMGR_FOLDER_NAME"/>





    <STRING id="STR_ID_FMGR_NO_FILES"/>
    <STRING id="STR_ID_FMGR_NO_FOLDERS"/>
<!--
**************************************** Menu section *****************************************
-->

    <MENUSET id="MENU_ID_FMGR_GENERAL_OPTION">
        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_FORWARD_SEND</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_FORWARD_USE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_CREATE</MENUITEM_ID>



        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_RENAME</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_DELETE</MENUITEM_ID>






        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_COPY</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_MOVE</MENUITEM_ID>



        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_DETAIL</MENUITEM_ID>
    </MENUSET>
    <MENUSET id="MENU_ID_FMGR_GEN_SEND_OPTION_SET">
# 224 "temp/res/000_FileMgr.c"
    </MENUSET>
    <MENU id="MENU_ID_FMGR_DRIVE_OPTIONS" type="OPTION" str="STR_GLOBAL_OPTIONS">
        <MENUITEM_ID>MENU_ID_FMGR_DRIVE_OPEN</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_CREATE</MENUITEM_ID>







        <MENUITEM_ID>MENU_ID_FMGR_CARD_DETAIL</MENUITEM_ID>
        </MENU>
    <MENU id="MENU_ID_FMGR_APP_DRIVE_OPTIONS" type="OPTION" str="STR_GLOBAL_OPTIONS">
        <MENUITEM_ID>MENU_ID_FMGR_DRIVE_OPEN</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_DRIVE_SELECT_DONE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_CREATE</MENUITEM_ID>
    </MENU>
    <MENUITEM id="MENU_ID_FMGR_DRIVE_OPEN" str="STR_GLOBAL_OPEN"/>






    <MENUITEM id="MENU_ID_FMGR_DRIVE_SELECT_DONE" str="STR_GLOBAL_SELECT"/>






    <MENU id="MENU_ID_FMGR_FOLDER_OPTIONS" type="OPTION" str="STR_GLOBAL_OPTIONS">
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_OPEN</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_CREATE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_RENAME</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_DELETE</MENUITEM_ID>
# 271 "temp/res/000_FileMgr.c"
        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_DETAIL</MENUITEM_ID>
    </MENU>
    <MENU id="MENU_ID_FMGR_APP_FOLDER_OPTIONS" type="OPTION" str="STR_GLOBAL_OPTIONS">
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_OPEN</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_SELECT_DONE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_CREATE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_RENAME</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_GEN_OPTION_DELETE</MENUITEM_ID>
    </MENU>
    <MENUITEM id="MENU_ID_FMGR_FOLDER_OPEN" str="STR_GLOBAL_OPEN"/>
    <MENUITEM id="MENU_ID_FMGR_FOLDER_CREATE" str="STR_FMGR_FOLDER_CREATE"/>
    <MENUITEM id="MENU_ID_FMGR_FOLDER_SELECT_DONE" str="STR_GLOBAL_SELECT"/>
    <MENU id="MENU_ID_FMGR_HYPERLINK_OPTIONS" type="OPTION" str="STR_GLOBAL_OPTIONS">
        <MENUITEM_ID>MENU_ID_FMGR_HYPERLINK_APP_FUNC</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_CREATE</MENUITEM_ID>
    </MENU>
    <MENUITEM id="MENU_ID_FMGR_HYPERLINK_APP_FUNC" str="STR_GLOBAL_ERROR"/>
    <MENU id="MENU_ID_FMGR_TREE_OPTIONS" type="OPTION" str="STR_GLOBAL_OPTIONS">
        <MENUITEM_ID>MENU_ID_FMGR_TREE_SELECT</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_TREE_OPEN</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_FOLDER_CREATE</MENUITEM_ID>
    </MENU>
    <MENUITEM id="MENU_ID_FMGR_TREE_SELECT" str="STR_GLOBAL_DONE"/>
    <MENUITEM id="MENU_ID_FMGR_TREE_OPEN" str="STR_GLOBAL_OPEN"/>







    <MENU id="MENU_ID_FMGR_GEN_TYPE_OPTIONS" type="OPTION" str="STR_GLOBAL_OPTIONS">
        <MENUSET_ID>MENU_ID_FMGR_GENERAL_OPTION</MENUSET_ID>
    </MENU>
    <MENUITEM id="MENU_ID_FMGR_GEN_OPTION_FORWARD_USE" str="STR_FMGR_USE"/>
    <MENUITEM id="MENU_ID_FMGR_GEN_OPTION_FORWARD_SEND" str="STR_GLOBAL_SEND"/>
    <MENUITEM id="MENU_ID_FMGR_GEN_OPTION_DETAIL" str="STR_GLOBAL_DETAILS"/>



    <MENUITEM id="MENU_ID_FMGR_GEN_OPTION_RENAME" str="STR_GLOBAL_RENAME"/>
    <MENUITEM id="MENU_ID_FMGR_GEN_OPTION_COPY" str="STR_GLOBAL_COPY"/>
    <MENUITEM id="MENU_ID_FMGR_GEN_OPTION_MOVE" str="STR_GLOBAL_MOVE"/>
# 356 "temp/res/000_FileMgr.c"
    <MENUITEM id="MENU_ID_FMGR_GEN_OPTION_DELETE" str="STR_GLOBAL_DELETE"/>
# 372 "temp/res/000_FileMgr.c"
    <MENU id="MENU_ID_FMGR_EDITOR_OPTION" type="OPTION" str="STR_GLOBAL_OPTIONS">
        <MENUITEM_ID>MENU_ID_FMGR_EDITOR_DONE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_FMGR_EDITOR_INPUT_METHOD</MENUITEM_ID>
        </MENU>
    <MENUITEM id="MENU_ID_FMGR_EDITOR_DONE" str="STR_GLOBAL_DONE"/>
    <MENUITEM id="MENU_ID_FMGR_EDITOR_INPUT_METHOD" str="STR_GLOBAL_INPUT_METHOD"/>

    <RECEIVER id="EVT_ID_IDLE_GROUP_ENTER" proc="mmi_fmgr_nor_sweep_device"/>
        <RECEIVER id="EVT_ID_SRV_FMGR_NOTIFICATION_DEV_PLUG_IN" proc="mmi_fmgr_notify_hdlr"/>
        <RECEIVER id="EVT_ID_SRV_FMGR_NOTIFICATION_DEV_PLUG_OUT" proc="mmi_fmgr_notify_hdlr"/>
        <RECEIVER id="EVT_ID_SRV_FMGR_NOTIFICATION_DEV_SOFT_MOUNT" proc="mmi_fmgr_notify_hdlr"/>
        <RECEIVER id="EVT_ID_SRV_FMGR_NOTIFICATION_ADV_ACTION" proc="mmi_fmgr_notify_hdlr"/>
# 393 "temp/res/000_FileMgr.c"
<!--
**************************************** Timer section ***************************************
-->
    <TIMER id="FMGR_AUDIO_TIMER"/>
    <TIMER id="FMGR_SCR_TIMER"/>
</APP>


<FILE_TYPES>
<!--
**************************************** Default Type *****************************************
-->

    <FILE_GROUP_TYPE type_id="FMGR_GROUP_COMMON">




        <FILE_TYPE type_id="FMGR_TYPE_UNKNOWN" small_icon="IMG_ID_FMGR_ICON_FILE_TYPE_UNKNOWN" option="MENU_ID_FMGR_GEN_TYPE_OPTIONS" send_option="MENU_ID_FMGR_GEN_SEND_OPTION"/>
        <FILE_TYPE type_id="FMGR_TYPE_FOLDER" small_icon="IMG_ID_FMGR_ICON_FILE_TYPE_FOLDER" option="MENU_ID_FMGR_FOLDER_OPTIONS"/>




        <FILE_TYPE type_id="FMGR_TYPE_HYPER_LINK"/>
    </FILE_GROUP_TYPE>

    <FILE_OPTION_HDLR option="MENU_ID_FMGR_FOLDER_OPTIONS" enabler="mmi_fmgr_general_option_enabler"/>

    <FILE_OPTION_HDLR option="MENU_ID_FMGR_GEN_OPTION_FORWARD_SEND" enabler="mmi_fmgr_general_option_enabler"/>
    <FILE_OPTION_HDLR option="MENU_ID_FMGR_GEN_OPTION_FORWARD_USE" enabler="mmi_fmgr_general_option_enabler"/>
# 442 "temp/res/000_FileMgr.c"
    <FILE_OPTION_HDLR option="MENU_ID_FMGR_AUD_TYPE_OPTIONS" enabler="mmi_fmgr_option_audio_enabler_ex"/>
    <FILE_OPTION_HDLR option="MENU_ID_FMGR_IMG_TYPE_OPTIONS" enabler="mmi_fmgr_option_image_enabler_ex"/>
</FILE_TYPES>
