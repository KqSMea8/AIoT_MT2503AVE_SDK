# 1 "temp/res/MDI.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "temp/res/MDI.c"
# 172 "temp/res/MDI.c"
# 1 "../../mmi/inc/MMI_features.h" 1
# 69 "../../mmi/inc/MMI_features.h"
# 1 "../../mmi/inc/MMI_features_switch.h" 1
# 67 "../../mmi/inc/MMI_features_switch.h"
# 1 "../../mmi/inc/MMI_features_type.h" 1
# 68 "../../mmi/inc/MMI_features_switch.h" 2
# 70 "../../mmi/inc/MMI_features.h" 2
# 173 "temp/res/MDI.c" 2
# 1 "../customerinc/custresdef.h" 1
# 207 "../customerinc/custresdef.h"
# 1 "../../mmi/inc/MMI_features.h" 1
# 208 "../customerinc/custresdef.h" 2
# 174 "temp/res/MDI.c" 2
<?xml version="1.0" encoding="UTF-8"?>

<APP id="SRV_MDI">
    <INCLUDE file="mdi_resdef.h"/>


    <CACHEDATA type="byte" id="NRRAM_GPS_LOGGING_SWITCH">
        <DEFAULT_VALUE> [0xFF] </DEFAULT_VALUE>
        <DESCRIPTION> Byte Cache </DESCRIPTION>
    </CACHEDATA>

    <CACHEDATA type="byte" id="NVRAM_GPS_PAYLOAD_STORAGE">
        <DEFAULT_VALUE> [0xFF] </DEFAULT_VALUE>
        <DESCRIPTION> NVRAM_GPS_PAYLOAD_STORAGE </DESCRIPTION>
    </CACHEDATA>



<!----------------------------------------------------------------------------------------
******** String section
------------------------------------------------------------------------------------------>

    <STRING id="STR_ID_MDI_COMMON_FAIL"/>
    <STRING id="STR_ID_MDI_COMMON_OPEN_FAIL"/>
    <STRING id="STR_ID_MDI_COMMON_PLAY_FAIL"/>
    <STRING id="STR_ID_MDI_COMMON_RECORD_FAIL"/>

    <STRING id="STR_ID_MDI_COMMON_WRITE_PROTECTION"/>
    <STRING id="STR_ID_MDI_COMMON_HW_NOT_READY"/>
    <STRING id="STR_ID_MDI_COMMON_INVALID_FILE"/>



    <STRING id="STR_ID_MDI_AUDIO_NO_AUD_TRACK"/>
    <STRING id="STR_ID_MDI_CAMERA_PREVIEW_FAILED"/>
    <STRING id="STR_ID_MDI_CAMERA_CAPTURE_FAILED"/>
    <STRING id="STR_ID_MDI_CAMERA_XENONFLASH_TIMEOUT"/>
    <STRING id="STR_ID_MDI_CAMERA_XENONFLASH_LOW_BATTERY"/>
    <STRING id="STR_ID_MDI_WEBCAM_USB_NOTIFY_ERROR"/>
# 262 "temp/res/MDI.c"
    <STRING id="STR_ID_MDI_MOTION_NO_SLOT"/>
    <STRING id="STR_ID_MDI_MOTION_CB_NOT_FOUND"/>


    <STRING id="STR_ID_MDI_GPS_UART_ERR_PORT_ALREADY_OPEN"/>
    <STRING id="STR_ID_MDI_GPS_UART_ERR_PORT_NUMBER_WRONG"/>
    <STRING id="STR_ID_MDI_GPS_UART_ERR_PARAM_ERROR"/>
    <STRING id="STR_ID_MDI_GPS_UART_ERR_PORT_ALREADY_CLOSED"/>
    <STRING id="STR_ID_MDI_GPS_UART_ERR_PORT_ERR_UNKNOW"/>
    <STRING id="STR_ID_MDI_GPS_UART_ERR_PORT_ERR_NOT_OPEN"/>
    <STRING id="STR_ID_MDI_GPS_UART_ERR_NO_SLOT"/>


<!----------------------------------------------------------------------------------------
******** Audio section
------------------------------------------------------------------------------------------>
# 1510 "temp/res/MDI.c"
<!----------------------------------------------------------------------------------------
******** Timer
------------------------------------------------------------------------------------------>

    <TIMER id="TIMER_PROFILES_ALERT_PLAY"/>
    <TIMER id="MDI_AUDIO_PLAYER_TEXT_TIMER"/>
    <TIMER id="MDI_AUDIO_CRESCENDO_TIMER"/>

<!----------------------------------------------------------------------------------------
******** Callback Manager
------------------------------------------------------------------------------------------>
    <EVENT id="EVT_ID_MDI_AUDIO_SPEECH_STATUS_IND" type="SENDER"/>
    <EVENT id="EVT_ID_MDI_AUDIO_PRE_MODE_SWITCH" type="SENDER"/>
    <EVENT id="EVT_ID_MDI_AUDIO_MODE_SWITCHED" type="SENDER"/>


    <RECEIVER id="EVT_ID_SRV_SHUTDOWN_HANDLER_RESET" proc="mdi_audio_cb_evt_hdlr"/>
    <RECEIVER id="EVT_ID_SRV_SHUTDOWN_DEINIT" proc="mdi_audio_cb_evt_hdlr"/>
    <RECEIVER id="EVT_ID_GPIO_EARPHONE_PLUG_IN" proc="mdi_audio_cb_evt_hdlr"/>
    <RECEIVER id="EVT_ID_GPIO_EARPHONE_PLUG_OUT" proc="mdi_audio_cb_evt_hdlr"/>

</APP>
