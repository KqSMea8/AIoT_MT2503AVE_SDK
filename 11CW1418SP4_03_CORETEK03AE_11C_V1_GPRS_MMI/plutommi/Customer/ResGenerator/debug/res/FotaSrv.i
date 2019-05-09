# 1 "temp/res/FotaSrv.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "temp/res/FotaSrv.c"

# 1 "../../mmi/inc/mmi_features.h" 1
# 69 "../../mmi/inc/mmi_features.h"
# 1 "../../mmi/inc/MMI_features_switch.h" 1
# 67 "../../mmi/inc/MMI_features_switch.h"
# 1 "../../mmi/inc/MMI_features_type.h" 1
# 68 "../../mmi/inc/MMI_features_switch.h" 2
# 70 "../../mmi/inc/mmi_features.h" 2
# 3 "temp/res/FotaSrv.c" 2
# 1 "../customerinc/custresdef.h" 1
# 207 "../customerinc/custresdef.h"
# 1 "../../mmi/inc/MMI_features.h" 1
# 208 "../customerinc/custresdef.h" 2
# 4 "temp/res/FotaSrv.c" 2


<?xml version="1.0" encoding="UTF-8"?>




<APP id="APP_ADUPS">


    <STRING id = "STR_ID_FOTASRV">"Fota"</STRING>
    <STRING id = "STR_ID_AUDPS_CHECK_VERSION">"Check Version"</STRING>
    <STRING id = "STR_ID_AUDPS_CK_ERROR_TOKEN">"Toke Error"</STRING>
    <STRING id = "STR_ID_AUDPS_CK_ERROR_PROJECT">"Project Error"</STRING>
    <STRING id = "STR_ID_AUDPS_CK_ERROR_PARAM">"Param Error"</STRING>
    <STRING id = "STR_ID_AUDPS_CK_ERROR_LATEST">"Latest Version"</STRING>
    <STRING id = "STR_ID_AUDPS_DL_ERROR">"Download fail"</STRING>
    <STRING id = "STR_ID_AUDPS_DL_RESET">"Please reset"</STRING>
    <STRING id = "STR_ID_AUDPS_IMEI_ERROR">"Get IMEI Error"</STRING>



    <SCREEN id="GRP_ID_ADUPS_MAIN"/>
    <SCREEN id="SCR_ADUPS_PROGRESS"/>
    <SCREEN id="SCR_ADUPS_CK_CNF"/>




    <IMAGE id="IMG_ID_FOTASRV" >"..\\\\..\\\\Customer\\\\Images""\\\\EmptyImage.bmp"</IMAGE>

    <MENU id = "MENU_SETTING_CHECK_VERSION" type = "APP_MAIN" str = "STR_ID_AUDPS_CHECK_VERSION" img = "IMG_ID_FOTASRV" highlight = "AdupsHighlightCV">
      </MENU>


    <CACHEDATA type="byte" id="NVRAM_ADUPS_SETTING_LID" restore_flag="TRUE">
        <DEFAULT_VALUE> [0x00] </DEFAULT_VALUE>
        <DESCRIPTION> ADUPS REMIDER SET </DESCRIPTION>
    </CACHEDATA>

</APP>
