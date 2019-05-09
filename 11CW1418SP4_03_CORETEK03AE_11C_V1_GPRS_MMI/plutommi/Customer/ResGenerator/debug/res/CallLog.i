# 1 "temp/res/CallLog.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "temp/res/CallLog.c"
# 331 "temp/res/CallLog.c"
# 1 "../../mmi/inc/mmi_features.h" 1
# 69 "../../mmi/inc/mmi_features.h"
# 1 "../../mmi/inc/MMI_features_switch.h" 1
# 67 "../../mmi/inc/MMI_features_switch.h"
# 1 "../../mmi/inc/MMI_features_type.h" 1
# 68 "../../mmi/inc/MMI_features_switch.h" 2
# 70 "../../mmi/inc/mmi_features.h" 2
# 332 "temp/res/CallLog.c" 2
# 1 "../customerinc/CustResDef.h" 1
# 207 "../customerinc/CustResDef.h"
# 1 "../../mmi/inc/MMI_features.h" 1
# 208 "../customerinc/CustResDef.h" 2
# 333 "temp/res/CallLog.c" 2



<?xml version = "1.0" encoding = "UTF-8"?>


<APP id = "APP_CALLLOG"
# 349 "temp/res/CallLog.c"
>


<!----------------- XML Begin ------------------------------------------------->

<!----------------- Include Area ---------------------------------------------->

    <INCLUDE file = "GlobalResDef.h"/>
# 367 "temp/res/CallLog.c"
<!----------------- Common ---------------------------------------------------->

    <!----------------- Event Id ------------------------------------------->

    <!----------------- CallLogSrv Event Handler ----------------------->
    <RECEIVER id="EVT_ID_SRV_CLOG_READY" proc="mmi_clog_srv_evt_hdlr"/>
    <RECEIVER id="EVT_ID_SRV_CLOG_ADD_LOG" proc="mmi_clog_srv_evt_hdlr"/>
    <RECEIVER id="EVT_ID_SRV_CLOG_UPDATE_LOG" proc="mmi_clog_srv_evt_hdlr"/>
    <RECEIVER id="EVT_ID_SRV_CLOG_UPDATE_ALL" proc="mmi_clog_srv_evt_hdlr"/>
    <RECEIVER id="EVT_ID_SRV_CLOG_DEL_LOG" proc="mmi_clog_srv_evt_hdlr"/>
    <RECEIVER id="EVT_ID_SRV_CLOG_DEL_ALL" proc="mmi_clog_srv_evt_hdlr"/>
# 400 "temp/res/CallLog.c"
    <!----------------- CallLogApp Event Id ---------------------------->
    <EVENT id = "EVT_ID_MMI_CLOG_ACT_START_APPLY"/> <!-- Action --->
    <EVENT id = "EVT_ID_MMI_CLOG_ACT_APPLY_DONE"/>

    <EVENT id = "EVT_ID_MMI_CLOG_LT_EXIST"/> <!-- Layout --->

    <EVENT id = "EVT_ID_MMI_CLOG_VR_EXIT"/> <!-- Viewer --->


    <!----------------- Group Id ------------------------------------------->

    <SCREEN id = "GRP_ID_CLOG_BEGIN"/>
    <SCREEN id = "GRP_ID_CLOG_ROOT"/>
    <SCREEN id = "GRP_ID_CLOG_LAYOUT"/>
    <SCREEN id = "GRP_ID_CLOG_LAYOUT_MARK"/>
    <SCREEN id = "GRP_ID_CLOG_VIEWER"/>
    <SCREEN id = "GRP_ID_CLOG_OPTION"/>
    <SCREEN id = "GRP_ID_CLOG_OPTION_VIEWER"/>
    <SCREEN id = "GRP_ID_CLOG_ACTION"/>

    <SCREEN id = "GRP_ID_CLOG_ADVANCED"/>
    <SCREEN id = "GRP_ID_CLOG_CALL_TIME"/>
    <SCREEN id = "GRP_ID_CLOG_CALL_COST"/>
    <SCREEN id = "GRP_ID_CLOG_SMS_COUNT"/>
    <SCREEN id = "GRP_ID_CLOG_GPRS_COUNT"/>
# 506 "temp/res/CallLog.c"
    <!----------------- String Id ----------------------------------------->




    <!----------------- Image Id ------------------------------------------>

    <!----------------- Menu Id ------------------------------------------->

<!----------------- Common End ------------------------------------------------>
# 636 "temp/res/CallLog.c"
<!----------------- Call Cost ------------------------------------------------->
# 735 "temp/res/CallLog.c"
<!----------------- Call Cost End --------------------------------------------->



<!----------------- SMS Counter ----------------------------------------------->
# 799 "temp/res/CallLog.c"
<!----------------- SMS Counter End ------------------------------------------->






<!----------------- GPRS Counter ---------------------------------------------->
# 905 "temp/res/CallLog.c"
<!----------------- GPRS Counter End ------------------------------------------>


<!----------------- Call Log -------------------------------------------------->

    <!----------------- Screen Id ----------------------------------------->
     <SCREEN id = "SCR_ID_CLOG_NAME_LIST"/>
    <SCREEN id = "SCR_ID_CLOG_MAIN_MENU"/>
    <SCREEN id = "SCR_ID_CLOG_LAYOUT"/>
    <SCREEN id = "SCR_ID_CLOG_DIALED_CALL"/>
    <SCREEN id = "SCR_ID_CLOG_RECVED_CALL"/>
    <SCREEN id = "SCR_ID_CLOG_MISSED_CALL"/>
    <SCREEN id = "SCR_ID_CLOG_ALL_CALL"/>
    <SCREEN id = "SCR_ID_CLOG_OPTION"/>
    <SCREEN id = "SCR_ID_CLOG_ADVANCED"/>
    <SCREEN id = "SCR_ID_CLOG_VIEWER"/>

    <!----------------- String Id ----------------------------------------->
    <STRING id = "STR_ID_CLOG_CALL_LOG"/>

    <STRING id = "STR_ID_CLOG_ALL_CALL"/>
    <STRING id = "STR_ID_CLOG_DIALED_CALL"/>
    <STRING id = "STR_ID_CLOG_RECVED_CALL"/>
    <STRING id = "STR_ID_CLOG_MISSED_CALL"/>
    <STRING id = "STR_ID_CLOG_UNKOWN_CALL"/>

    <STRING id = "STR_ID_CLOG_DELETE_BOX"/>
# 944 "temp/res/CallLog.c"
    <STRING id = "STR_ID_CLOG_SINGLE_MISSED_CALL"/>
    <STRING id = "STR_ID_CLOG_MULTI_MISSED_CALLS"/>

    <STRING id = "STR_ID_CLOG_UNKNOWN_NUMBER"/>


    <STRING id = "STR_ID_CLOG_ANON_URI"/>


    <STRING id = "STR_ID_CLOG_NO_ENTRY"/>
    <STRING id = "STR_ID_CLOG_ENTRY"/>
    <STRING id = "STR_ID_CLOG_ENTRIES"/>
# 967 "temp/res/CallLog.c"
    <STRING id = "STR_ID_CLOG_TYPE"/>

    <STRING id = "STR_ID_CLOG_TIME"/>
    <STRING id = "STR_ID_CLOG_TIMES"/>
    <STRING id = "STR_ID_CLOG_TOTAL_NUM"/>

    <STRING id = "STR_ID_CLOG_NAME"/>
# 986 "temp/res/CallLog.c"
    <STRING id = "STR_ID_CLOG_NUMBER"/>
    <STRING id = "STR_ID_CLOG_TIMESTAMP"/>
# 1012 "temp/res/CallLog.c"
    <!----------------- Image Id ------------------------------------------>



    <IMAGE id = "IMG_ID_CLOG_MAIN_ICON">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\Submenu\\\\Call\\\\CC_callhistory.bmp"</IMAGE>
# 1037 "temp/res/CallLog.c"
    <IMAGE id = "IMG_ID_CLOG_DELETE_BOX">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\Submenu\\\\Call\\\\SB_CRSET.bmp"</IMAGE>


    <IMAGE id = "IMG_ID_CLOG_ALL_CALL">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\Submenu\\\\Call\\\\SB_CALL.bmp"</IMAGE>
# 1053 "temp/res/CallLog.c"
    <IMAGE id = "IMG_ID_CLOG_DIALED_CALL">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\Call\\\\SubMenu\\\\SB_CDIA.bmp"</IMAGE>
    <IMAGE id = "IMG_ID_CLOG_MISSED_CALL">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\Call\\\\SubMenu\\\\SB_CMIS.bmp"</IMAGE>
    <IMAGE id = "IMG_ID_CLOG_RECVED_CALL">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\Call\\\\SubMenu\\\\SB_CREC.bmp"</IMAGE>
    <IMAGE id = "IMG_ID_CLOG_UNKOWN_CALL">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\Call\\\\SubMenu\\\\SB_UNKOWN.bmp"</IMAGE>
# 1121 "temp/res/CallLog.c"
    <!----------------- Menu Id ------------------------------------------->
    <MENUSET id = "MENU_ID_CLOG_MENSET_SUB_MODULES">
# 1135 "temp/res/CallLog.c"
    </MENUSET>





    <MENUITEM id = "MENU_ID_CLOG_DIALED_CALL" str = "STR_GLOBAL_DIALLED_CALLS" img = "IMG_ID_CLOG_DIALED_CALL"
            shortcut = "ON" shortcut_img = "IMG_ID_CLOG_DIALED_CALL" launch= "mmi_clog_dclt_launch"/>

    <MENUITEM id = "MENU_ID_CLOG_MISSED_CALL" str = "STR_GLOBAL_MISSED_CALLS" img = "IMG_ID_CLOG_MISSED_CALL"
            shortcut = "ON" shortcut_img = "IMG_ID_CLOG_MISSED_CALL" launch= "mmi_clog_mclt_launch"/>

    <MENUITEM id = "MENU_ID_CLOG_RECVED_CALL" str = "STR_GLOBAL_RECEIVED_CALLS" img = "IMG_ID_CLOG_RECVED_CALL"
            shortcut = "ON" shortcut_img = "IMG_ID_CLOG_RECVED_CALL" launch= "mmi_clog_rclt_launch"/>

    <MENUITEM id = "MENU_ID_CLOG_ALL_CALL" str = "STR_ID_CLOG_ALL_CALL" img = "IMG_ID_CLOG_ALL_CALL"
            shortcut = "OFF" shortcut_img = "IMG_ID_CLOG_ALL_CALL" launch= "mmi_clog_aclt_launch"/>
# 1169 "temp/res/CallLog.c"
    <MENUITEM id = "MENU_ID_CLOG_DEL_DIALED_CALL" str = "STR_GLOBAL_DIALLED_CALLS" img = "IMG_ID_CLOG_DIALED_CALL"/>

    <MENUITEM id = "MENU_ID_CLOG_DEL_MISSED_CALL" str = "STR_GLOBAL_MISSED_CALLS" img = "IMG_ID_CLOG_MISSED_CALL"/>

    <MENUITEM id = "MENU_ID_CLOG_DEL_RECVED_CALL" str = "STR_GLOBAL_RECEIVED_CALLS" img = "IMG_ID_CLOG_RECVED_CALL"/>

    <MENUITEM id = "MENU_ID_CLOG_DEL_ALL_CALL" str = "STR_ID_CLOG_ALL_CALL" img = "IMG_ID_CLOG_DELETE_BOX"/>


    <MENU id = "MENU_ID_CLOG_DELETE_BOX" type = "APP_SUB" str = "STR_ID_CLOG_DELETE_BOX" img = "IMG_ID_CLOG_DELETE_BOX">
        <MENUITEM_ID>MENU_ID_CLOG_DEL_DIALED_CALL</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CLOG_DEL_MISSED_CALL</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CLOG_DEL_RECVED_CALL</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CLOG_DEL_ALL_CALL</MENUITEM_ID>
    </MENU>



    <MENUSET id = "MENU_ID_CLOG_MENUSET_CALL_ITEMS">
        <MENUITEM_ID>MENU_ID_CLOG_DIALED_CALL</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CLOG_MISSED_CALL</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CLOG_RECVED_CALL</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CLOG_ALL_CALL</MENUITEM_ID>
    </MENUSET>

    <MENU id = "MEMU_ID_CLOG_CALL_LOG" type = "APP_SUB" str = "STR_ID_CLOG_CALL_LOG">
        <MENUSET_ID>MENU_ID_CLOG_MENUSET_CALL_ITEMS</MENUSET_ID>
        <MENUITEM_ID>MENU_ID_CLOG_DELETE_BOX</MENUITEM_ID>
        <MENUSET_ID>MENU_ID_CLOG_MENSET_SUB_MODULES</MENUSET_ID>
    </MENU>
# 1210 "temp/res/CallLog.c"
    <MENUSET id = "MENU_ID_CLOG_MENSET_SEND_MSG">



        <MENUITEM id = "MENU_ID_CLOG_SEND_SMS" str = "STR_GLOBAL_SEND_TEXT_MESSAGE"/>




    </MENUSET>


    <MENUSET id = "MENU_ID_CLOG_MENSET_OP_DEL">
        <MENUITEM id = "MENU_ID_CLOG_DELETE" str = "STR_GLOBAL_DELETE"/>
        <MENUITEM id = "MENU_ID_CLOG_DELETE_ALL" str = "STR_GLOBAL_DELETE_ALL"/>
    </MENUSET>

    <MENUSET id = "MENU_ID_CLOG_MENSET_OP_EVENT">
    </MENUSET>
# 1237 "temp/res/CallLog.c"
    <MENU id = "MENU_ID_CLOG_OPTION" type = "OPTION" str = "STR_GLOBAL_OPTIONS">
        <MENUITEM id = "MENU_ID_CLOG_VIEW" str = "STR_GLOBAL_VIEW"/>
        <MENUITEM id = "MENU_ID_CLOG_MAKE_CALL" str = "STR_GLOBAL_DIAL"/>

        <MENUSET_ID>MENU_ID_CLOG_MENSET_SEND_MSG</MENUSET_ID>

        <MENUITEM id = "MENU_ID_CLOG_SAVE_TO_PHB" str = "STR_GLOBAL_SAVE_TO_PHONEBOOK"/>
# 1255 "temp/res/CallLog.c"
        <MENUITEM id = "MENU_ID_CLOG_EDIT_BEFORE_CALL" str = "STR_GLOBAL_EDIT_BEFORE_CALL"/>

        <MENUSET_ID>MENU_ID_CLOG_MENSET_OP_DEL</MENUSET_ID>
        <MENUSET_ID>MENU_ID_CLOG_MENSET_OP_EVENT</MENUSET_ID>
# 1292 "temp/res/CallLog.c"
    </MENU>
# 1316 "temp/res/CallLog.c"
<!----------------- Call Time ------------------------------------------------->
# 1399 "temp/res/CallLog.c"
        <STRING id = "STR_ID_CLOG_VIEW_MISSED_CALL"/>
        <STRING id = "STR_ID_CLOG_VIEW_RING_ONES"/>



        <STRING id = "STR_ID_CLOG_VIEW_UNKNOWN"/>
# 1466 "temp/res/CallLog.c"
<!----------------- Call Log End ---------------------------------------------->





<!----------------- XML END --------------------------------------------------->

</APP>
