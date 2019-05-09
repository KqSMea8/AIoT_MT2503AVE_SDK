# 1 "temp/res/MreApps.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "temp/res/MreApps.c"
# 75 "temp/res/MreApps.c"
# 1 "../../mmi/inc/mmi_features.h" 1
# 69 "../../mmi/inc/mmi_features.h"
# 1 "../../mmi/inc/MMI_features_switch.h" 1
# 67 "../../mmi/inc/MMI_features_switch.h"
# 1 "../../mmi/inc/MMI_features_type.h" 1
# 68 "../../mmi/inc/MMI_features_switch.h" 2
# 70 "../../mmi/inc/mmi_features.h" 2
# 76 "temp/res/MreApps.c" 2
# 1 "../customerinc/custresdef.h" 1
# 207 "../customerinc/custresdef.h"
# 1 "../../mmi/inc/MMI_features.h" 1
# 208 "../customerinc/custresdef.h" 2
# 77 "temp/res/MreApps.c" 2


<?xml version="1.0" encoding="UTF-8"?>


<APP id = "APP_SETTING_MREAPPS">



    <!--Include Area-->
    <INCLUDE file="GlobalResDef.h,SettingResDef.h"/>

    <!-----------------------------------------------------String Resource Area----------------------------------------------------->

    <STRING id="STR_ID_SETTING_MREAPPS"/>
    <STRING id="STR_ID_SETTING_MREAPPS_UNINSTALL_ALL"/>
    <STRING id="STR_ID_SETTING_MREAPPS_QUERY"/>
    <STRING id="STR_ID_SETTING_MREAPPS_UNINSTALL_PROCESS"/>
    <!-----------------------------------------------------Image Resource Area------------------------------------------------------>

    <!------------------------------------------------------Menu Resource Area------------------------------------------------------>

    <MENU id="MENU_ID_SETTING_MREAPPS" str="STR_ID_SETTING_MREAPPS" highlight="HighlightMreApps">
                <MENUITEM id="MENU_ID_SETTING_MREAPPS_UNINSTALL" str="STR_ID_SETTING_MREAPPS_UNINSTALL_ALL"></MENUITEM>
        </MENU>

    <!------------------------------------------------------Other Resource---------------------------------------------------------->
    <SCREEN id = "GRP_ID_SETTING_MREAPPS"/>
    <SCREEN id = "GRP_ID_SETTING_MREAPPS_PROCESSING"/>
    <SCREEN id = "SCR_ID_SETTING_MREAPPS"/>
    <SCREEN id = "SCR_ID_SETTING_MREAPPS_PROCESSING"/>
    <SCREEN id = "SCR_ID_SETTING_MREAPPS_CONFIRM"/>


</APP>
