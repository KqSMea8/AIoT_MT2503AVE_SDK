# 1 "temp/res/ConnectivityMainmenu.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "temp/res/ConnectivityMainmenu.c"
# 198 "temp/res/ConnectivityMainmenu.c"
# 1 "../../mmi/inc/MMI_features.h" 1
# 69 "../../mmi/inc/MMI_features.h"
# 1 "../../mmi/inc/MMI_features_switch.h" 1
# 67 "../../mmi/inc/MMI_features_switch.h"
# 1 "../../mmi/inc/MMI_features_type.h" 1
# 68 "../../mmi/inc/MMI_features_switch.h" 2
# 70 "../../mmi/inc/MMI_features.h" 2
# 199 "temp/res/ConnectivityMainmenu.c" 2
# 1 "../customerinc/CustResDef.h" 1
# 207 "../customerinc/CustResDef.h"
# 1 "../../mmi/inc/MMI_features.h" 1
# 208 "../customerinc/CustResDef.h" 2
# 200 "temp/res/ConnectivityMainmenu.c" 2



<?xml version = "1.0" encoding = "UTF-8"?>

<APP id = "APP_CONNECTIVITY_MAINMENU"
# 214 "temp/res/ConnectivityMainmenu.c"
>
    <INCLUDE file = "GlobalMenuItems.h"/>
    <INCLUDE file = "GlobalResDef.h"/>
    <INCLUDE file = "mmi_rp_app_bluetooth_def.h"/>






    <INCLUDE file = "mmi_rp_app_mainmenu_def.h"/>


    <INCLUDE file = "mmi_rp_app_dataaccount_def.h"/>
# 245 "temp/res/ConnectivityMainmenu.c"
    <!-- string ----------------------------------------------------------------------------------->

    <STRING id = "STR_MENU_ID_CONNECTIVITY_TEXT"/>
    <STRING id = "STR_ID_CONNECTIVITY_TEXT"/>
# 259 "temp/res/ConnectivityMainmenu.c"
    <!-- image ------------------------------------------------------------------------------------>

    <IMAGE id = "IMG_MENU_ID_CONNECTIVITY_ICON">"..\\\\..\\\\Customer\\\\Images\\\\PLUTO128X32""\\\\MainLCD\\\\SubMenu\\\\Settings\\\\ST_connect.bmp"</IMAGE>
# 272 "temp/res/ConnectivityMainmenu.c"
    <!-- screen ----------------------------------------------------------------------------------->


    <!-- menu ------------------------------------------------------------------------------------->




    <MENU id = "MENU_ID_CONNECTIVITY" type = "APP_MAIN" str = "STR_MENU_ID_CONNECTIVITY_TEXT" parent = "IDLE_SCREEN_MENU_ID" img = "MAIN_MENU_CONNECTIVITY_ICON" highlight = "highlight_mainmenu_connectivity">






        <MENUITEM_ID>MENU_CONN_BT_MAIN</MENUITEM_ID>
# 302 "temp/res/ConnectivityMainmenu.c"
        <MENUITEM_ID>MENU_ID_DATA_CONNECTION_SIM</MENUITEM_ID>


        <MENUITEM_ID>SERVICES_DATA_CONNECT_MAIN_MENU_ID</MENUITEM_ID>
# 335 "temp/res/ConnectivityMainmenu.c"
</MENU>
# 346 "temp/res/ConnectivityMainmenu.c"
</APP>
