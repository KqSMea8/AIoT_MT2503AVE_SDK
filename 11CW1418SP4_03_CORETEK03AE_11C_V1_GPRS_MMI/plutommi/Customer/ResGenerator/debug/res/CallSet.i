# 1 "temp/res/CallSet.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "temp/res/CallSet.c"

# 1 "../../mmi/inc/mmi_features.h" 1
# 69 "../../mmi/inc/mmi_features.h"
# 1 "../../mmi/inc/MMI_features_switch.h" 1
# 67 "../../mmi/inc/MMI_features_switch.h"
# 1 "../../mmi/inc/MMI_features_type.h" 1
# 68 "../../mmi/inc/MMI_features_switch.h" 2
# 70 "../../mmi/inc/mmi_features.h" 2
# 3 "temp/res/CallSet.c" 2
# 1 "../customerinc/custresdef.h" 1
# 207 "../customerinc/custresdef.h"
# 1 "../../mmi/inc/MMI_features.h" 1
# 208 "../customerinc/custresdef.h" 2
# 4 "temp/res/CallSet.c" 2

<?xml version="1.0" encoding="UTF-8"?>

<APP id="APP_CALLSET">



    <!--Include Area-->
    <INCLUDE file="GlobalResDef.h,SettingResDef.h,GlobalMenuItems.h"/>
    <INCLUDE file="Mmi_rp_all_defs.h"/>

    <!-----------------------------------------------------Event Resource Area----------------------------------------------------->
    <RECEIVER id="EVT_ID_SRV_CALLSET_LINE_SWITCHED" proc="mmi_callset_hdlr_lsw_ind"/>
    <RECEIVER id="EVT_ID_SRV_CALLSET_CFU_STATUS_IND" proc="mmi_callset_hdlr_cfu_status_ind"/>
    <RECEIVER id="EVT_ID_SRV_SIM_CTRL_AVAILABILITY_CHANGED" proc="mmi_callset_hdlr_sim_availability_changed"/>




    <!-----------------------------------------------------String Resource Area----------------------------------------------------->

    <STRING id="STR_ID_CALLSET_CALL_CENTER"/>
    <STRING id="STR_ID_CALLSET_CALL_SETTING"/>
# 37 "temp/res/CallSet.c"
    <STRING id="STR_ID_CALLSET_MORE"/>

    <STRING id="STR_ID_CALLSET_CALLFWD"/>
    <STRING id="STR_ID_CALLSET_CFU_VOICE"/>



    <STRING id="STR_ID_CALLSET_CFNRC"/>




    <STRING id="STR_ID_CALLSET_CFNRY"/>




    <STRING id="STR_ID_CALLSET_CFB"/>







    <STRING id="STR_ID_CALLSET_CFU_DATA"/>
    <STRING id="STR_ID_CALLSET_CALLFWD_CANCELALL"/>
    <STRING id="STR_ID_CALLSET_CALLFWD_OPT"/>
    <STRING id="STR_ID_CALLSET_CF_TO_NEW"/>
    <STRING id="STR_ID_CALLSET_CF_TO_VM"/>
# 85 "temp/res/CallSet.c"
    <STRING id="STR_ID_CALLSET_CALLFWD_TO_NUM"/>
    <STRING id="STR_ID_CALLSET_CF_NUM_EDIT_FROM_PHB"/>
    <STRING id="STR_ID_CALLSET_CFNRY_WAIT_TIME"/>
    <STRING id="STR_ID_CALLSET_CFNRY_5_SEC"/>
    <STRING id="STR_ID_CALLSET_CFNRY_10_SEC"/>
    <STRING id="STR_ID_CALLSET_CFNRY_15_SEC"/>
    <STRING id="STR_ID_CALLSET_CFNRY_20_SEC"/>
    <STRING id="STR_ID_CALLSET_CFNRY_25_SEC"/>
    <STRING id="STR_ID_CALLSET_CFNRY_30_SEC"/>
# 107 "temp/res/CallSet.c"
    <STRING id="STR_ID_CALLSET_CALLWAIT"/>
    <STRING id="STR_ID_CALLSET_NUMBER_TOO_LONG"/>
# 130 "temp/res/CallSet.c"
    <STRING id="STR_ID_CALLSET_QUERY"/>
# 139 "temp/res/CallSet.c"
    <STRING id="STR_ID_CALLSET_ANS_MODE_ANYKEY"/>
    <STRING id="STR_ID_CALLSET_ANS_MODE"/>





    <STRING id="STR_ID_CALLSET_ANS_MODE_AUTO"/>
# 229 "temp/res/CallSet.c"
    <!-----------------------------------------------------Image Resource Area------------------------------------------------------>


    <IMAGE id="IMG_ID_CALLSET_CALL_SETTING">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\SubMenu\\\\Call\\\\CC_setting.bmp"</IMAGE>
# 276 "temp/res/CallSet.c"
    <!------------------------------------------------------Menu Resource Area------------------------------------------------------>


    <!--Menu Tree Area-->





    <MENU id="MENU_ID_CALLSET_CALL_CENTER" type="APP_SUB" str="STR_ID_CALLSET_CALL_CENTER"



        flag="MOVEABLEWITHINPARENT|INSERTABLE"



        parent="MAIN_MENU_CALL_CENTER"

    >



        <MENUITEM id="MENU_ID_CALLSET_CALL_LOG" str="MAIN_MENU_CALL_LOG_TEXT" img="IMG_ID_CLOG_MAIN_ICON"/>
# 308 "temp/res/CallSet.c"
        <MENUITEM_ID>MENU_ID_CALLSET_CALL_SETTING</MENUITEM_ID>

    </MENU>




    <MENUITEM id="MENU_ID_CALLSET_CALL_WAIT" str="STR_ID_CALLSET_CALLWAIT"/>







    <MENU id="MENU_ID_CALLSET_CALL_SETTING" type="APP_SUB" str="STR_ID_CALLSET_CALL_SETTING"
        img="IMG_ID_CALLSET_CALL_SETTING" flag="MOVEABLEWITHINPARENT"





        shortcut="ON" shortcut_img="IMG_ID_CALLSET_CALL_SETTING"

        launch="mmi_callset_call_setting_launch"




        parent="MAIN_MENU_CALL_CENTER"

    >
# 353 "temp/res/CallSet.c"
        <MENUITEM_ID>MENU_ID_CALLSET_CALL_WAIT</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_CALL_FWD</MENUITEM_ID>
# 369 "temp/res/CallSet.c"
        <MENUITEM_ID>MENU_ID_CALLSET_ANS_MODE_OPT</MENUITEM_ID>


    </MENU>
# 469 "temp/res/CallSet.c"
    <MENUITEM id="MENU_ID_CALLSET_CFU_VOICE" str="STR_ID_CALLSET_CFU_VOICE"/>



    <MENUITEM id="MENU_ID_CALLSET_CFNRC_VOICE" str="STR_ID_CALLSET_CFNRC"/>




    <MENUITEM id="MENU_ID_CALLSET_CFNRY_VOICE" str="STR_ID_CALLSET_CFNRY"/>




    <MENUITEM id="MENU_ID_CALLSET_CFB_VOICE" str="STR_ID_CALLSET_CFB"/>







    <MENUITEM id="MENU_ID_CALLSET_CFU_DATA" str="STR_ID_CALLSET_CFU_DATA"/>
    <MENUITEM id="MENU_ID_CALLSET_CALLFWD_CANCELALL" str="STR_ID_CALLSET_CALLFWD_CANCELALL"/>


    <MENU id="MENU_ID_CALLSET_CALL_FWD" type="APP_SUB" str="STR_ID_CALLSET_CALLFWD" flag="NONMOVEABLE">
        <MENUITEM_ID>MENU_ID_CALLSET_CFU_VOICE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_CFNRC_VOICE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_CFNRY_VOICE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_CFB_VOICE</MENUITEM_ID>






        <MENUITEM_ID>MENU_ID_CALLSET_CFU_DATA</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_CALLFWD_CANCELALL</MENUITEM_ID>
    </MENU>
# 519 "temp/res/CallSet.c"
    <MENU id="MENU_ID_CALLSET_MORE" type="APP_SUB" str="STR_ID_CALLSET_MORE" flag="MOVEABLEWITHINPARENT">
# 553 "temp/res/CallSet.c"
        <MENUITEM id="MENU_ID_CALLSET_ANS_MODE" str="STR_ID_CALLSET_ANS_MODE"/>
# 562 "temp/res/CallSet.c"
    </MENU>
# 668 "temp/res/CallSet.c"
    <MENUITEM id="MENU_ID_CALLSET_ACTIVATE" str="STR_GLOBAL_ACTIVATE"/>
    <MENUITEM id="MENU_ID_CALLSET_DEACTIVATE" str="STR_GLOBAL_DEACTIVATE"/>
    <MENUITEM id="MENU_ID_CALLSET_QUERY" str="STR_ID_CALLSET_QUERY"/>

    <MENUITEM id="MENU_ID_CALLSET_CF_NUM_EDIT_FROM_PHB" str="STR_ID_CALLSET_CF_NUM_EDIT_FROM_PHB"/>

    <MENUSET id="MENU_SET_CALLSET_CISS_OPT">
        <MENUITEM_ID>MENU_ID_CALLSET_ACTIVATE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_DEACTIVATE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_QUERY</MENUITEM_ID>
    </MENUSET>

    <MENU id="MENU_ID_CALLSET_CALL_WAIT_OPT" type="APP_SUB" str="STR_ID_CALLSET_CALLWAIT" flag="NONMOVEABLE">
        <MENUSET_ID>MENU_SET_CALLSET_CISS_OPT</MENUSET_ID>
    </MENU>

    <MENU id="MENU_ID_CALLSET_ACTIVATE_CF" type="APP_SUB" str="STR_GLOBAL_ACTIVATE" flag="NONMOVEABLE">
        <MENUITEM id="MENU_ID_CALLSET_CF_TO_NEW" str="STR_ID_CALLSET_CF_TO_NEW"/>
        <MENUITEM id="MENU_ID_CALLSET_CF_TO_VM" str="STR_ID_CALLSET_CF_TO_VM"/>
    </MENU>

    <MENU id="MENU_ID_CALLSET_CFU_VOICE_OPT" type="APP_SUB" str="STR_ID_CALLSET_CALLFWD_OPT" flag="NONMOVEABLE">
        <MENUITEM_ID>MENU_ID_CALLSET_ACTIVATE_CF</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_DEACTIVATE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_QUERY</MENUITEM_ID>
    </MENU>

    <MENU id="MENU_ID_CALLSET_CFNRC_VOICE_OPT" type="APP_SUB" str="STR_ID_CALLSET_CALLFWD_OPT" flag="NONMOVEABLE">
        <MENUITEM_ID>MENU_ID_CALLSET_ACTIVATE_CF</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_DEACTIVATE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_QUERY</MENUITEM_ID>
    </MENU>

    <MENU id="MENU_ID_CALLSET_CFNRY_VOICE_OPT" type="APP_SUB" str="STR_ID_CALLSET_CALLFWD_OPT" flag="NONMOVEABLE">
        <MENUITEM_ID>MENU_ID_CALLSET_ACTIVATE_CF</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_DEACTIVATE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_QUERY</MENUITEM_ID>
    </MENU>

    <MENU id="MENU_ID_CALLSET_CFB_VOICE_OPT" type="APP_SUB" str="STR_ID_CALLSET_CALLFWD_OPT" flag="NONMOVEABLE">
        <MENUITEM_ID>MENU_ID_CALLSET_ACTIVATE_CF</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_DEACTIVATE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_QUERY</MENUITEM_ID>
    </MENU>
# 739 "temp/res/CallSet.c"
    <MENU id="MENU_ID_CALLSET_CFU_DATA_OPT" type="APP_SUB" str="STR_ID_CALLSET_CALLFWD_OPT" flag="NONMOVEABLE">
        <MENUITEM_ID>MENU_ID_CALLSET_ACTIVATE_CF</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_DEACTIVATE</MENUITEM_ID>
        <MENUITEM_ID>MENU_ID_CALLSET_QUERY</MENUITEM_ID>
    </MENU>

    <MENU id="MENU_ID_CALLSET_CFNRY_TIME_OPT" type="APP_SUB" str="STR_ID_CALLSET_CFNRY_WAIT_TIME" flag="NONMOVEABLE">
        <MENUITEM id="MENU_ID_CALLSET_CFNRY_5_SEC" str="STR_ID_CALLSET_CFNRY_5_SEC"/>
        <MENUITEM id="MENU_ID_CALLSET_CFNRY_10_SEC" str="STR_ID_CALLSET_CFNRY_10_SEC"/>
        <MENUITEM id="MENU_ID_CALLSET_CFNRY_15_SEC" str="STR_ID_CALLSET_CFNRY_15_SEC"/>
        <MENUITEM id="MENU_ID_CALLSET_CFNRY_20_SEC" str="STR_ID_CALLSET_CFNRY_20_SEC"/>
        <MENUITEM id="MENU_ID_CALLSET_CFNRY_25_SEC" str="STR_ID_CALLSET_CFNRY_25_SEC"/>
        <MENUITEM id="MENU_ID_CALLSET_CFNRY_30_SEC" str="STR_ID_CALLSET_CFNRY_30_SEC"/>
    </MENU>
# 808 "temp/res/CallSet.c"
    <MENUITEM id="MENU_ID_CALLSET_OFF" str="STR_GLOBAL_OFF"/>
    <MENUITEM id="MENU_ID_CALLSET_ON" str="STR_GLOBAL_ON"/>
# 847 "temp/res/CallSet.c"
    <MENU id="MENU_ID_CALLSET_ANS_MODE_OPT" type="APP_SUB" str="STR_ID_CALLSET_ANS_MODE" flag="NONMOVEABLE">



        <MENUITEM id="MENU_ID_CALLSET_ANS_MODE_ANYKEY" str="STR_ID_CALLSET_ANS_MODE_ANYKEY"/>






        <MENUITEM id="MENU_ID_CALLSET_ANS_MODE_AUTO" str="STR_ID_CALLSET_ANS_MODE_AUTO"/>


    </MENU>
# 955 "temp/res/CallSet.c"
    <!------------------------------------------------------Other Resource---------------------------------------------------------->

    <SCREEN id="GRP_ID_CALLSET_COMMON"/>
    <SCREEN id="GRP_ID_CALLSET_CF"/>
# 993 "temp/res/CallSet.c"
</APP>
