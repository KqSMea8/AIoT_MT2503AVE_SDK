# 1 "temp/res/DateTimeSrv.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "temp/res/DateTimeSrv.c"
# 55 "temp/res/DateTimeSrv.c"
# 1 "../../mmi/inc/MMI_features.h" 1
# 69 "../../mmi/inc/MMI_features.h"
# 1 "../../mmi/inc/MMI_features_switch.h" 1
# 67 "../../mmi/inc/MMI_features_switch.h"
# 1 "../../mmi/inc/MMI_features_type.h" 1
# 68 "../../mmi/inc/MMI_features_switch.h" 2
# 70 "../../mmi/inc/MMI_features.h" 2
# 56 "temp/res/DateTimeSrv.c" 2
# 1 "../customerinc/CustResDef.h" 1
# 207 "../customerinc/CustResDef.h"
# 1 "../../mmi/inc/MMI_features.h" 1
# 208 "../customerinc/CustResDef.h" 2
# 57 "temp/res/DateTimeSrv.c" 2

<?xml version = "1.0" encoding = "UTF-8"?>

<APP id = "SRV_DATETIME_SETTING">
    <CACHEDATA type="byte" id="GPS_TIME_SYNC_MODE" restore_flag="TRUE">
        <DEFAULT_VALUE> [0X00] </DEFAULT_VALUE>
        <DESCRIPTION> Gps time sync mode </DESCRIPTION>
    </CACHEDATA>

    <CACHEDATA type="byte" id="SRV_DT_TIME_ZONE" restore_flag="TRUE">
        <DEFAULT_VALUE> [0X30] </DEFAULT_VALUE>
        <DESCRIPTION> srv date time tz </DESCRIPTION>
    </CACHEDATA>

        <EVENT id="EVT_ID_SRV_DATE_AND_TIME_CHANGE" type="SENDER"/>
</APP>
