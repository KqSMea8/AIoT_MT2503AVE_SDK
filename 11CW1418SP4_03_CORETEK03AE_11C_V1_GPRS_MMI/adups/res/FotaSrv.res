/* Needed header files of the compile option in XML files, if you need others need to add here */
#include "mmi_features.h"
#include "custresdef.h"

/* Need this line to tell parser that XML start, must after all #include. */
<?xml version="1.0" encoding="UTF-8"?>

#ifdef __ADUPS_FOTA_SOC__

/* APP tag, include your app name defined in MMIDataType.h */
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




    <IMAGE id="IMG_ID_FOTASRV" >CUST_IMG_BASE_PATH"\\\\EmptyImage.bmp"</IMAGE>

    <MENU id = "MENU_SETTING_CHECK_VERSION" type = "APP_MAIN" str = "STR_ID_AUDPS_CHECK_VERSION" img = "IMG_ID_FOTASRV" highlight = "AdupsHighlightCV">
      </MENU>


    <CACHEDATA type="byte" id="NVRAM_ADUPS_SETTING_LID" restore_flag="TRUE">
        <DEFAULT_VALUE> [0x00] </DEFAULT_VALUE>
        <DESCRIPTION> ADUPS REMIDER SET </DESCRIPTION>
    </CACHEDATA>
    
</APP>
#endif

