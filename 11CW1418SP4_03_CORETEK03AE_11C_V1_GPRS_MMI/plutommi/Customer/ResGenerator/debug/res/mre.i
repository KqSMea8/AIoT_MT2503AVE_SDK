# 1 "temp/res/mre.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "temp/res/mre.c"
# 1 "../../mmi/inc/MMI_features.h" 1
# 69 "../../mmi/inc/MMI_features.h"
# 1 "../../mmi/inc/MMI_features_switch.h" 1
# 67 "../../mmi/inc/MMI_features_switch.h"
# 1 "../../mmi/inc/MMI_features_type.h" 1
# 68 "../../mmi/inc/MMI_features_switch.h" 2
# 70 "../../mmi/inc/MMI_features.h" 2
# 2 "temp/res/mre.c" 2
# 1 "../customerinc/custresdef.h" 1
# 207 "../customerinc/custresdef.h"
# 1 "../../mmi/inc/MMI_features.h" 1
# 208 "../customerinc/custresdef.h" 2
# 3 "temp/res/mre.c" 2
# 1 "../../../mre/commoninc/vmswitch.h" 1
# 296 "../../../mre/commoninc/vmswitch.h"
# 1 "../../mmi/inc/mmi_features.h" 1
# 297 "../../../mre/commoninc/vmswitch.h" 2
# 1 "../../../mre/commoninc/vmswitch_cfg.h" 1
# 298 "../../../mre/commoninc/vmswitch.h" 2
# 4 "temp/res/mre.c" 2

<?xml version="1.0" encoding="UTF-8"?>


<APP id="APP_MRE" name="STR_ID_APP_MRE">
# 17 "temp/res/mre.c"
                                <MEMORY heap="$(MRE_SYS_ASM_APP_MEMPOOL_SIZE)" inc="vmopt.h"/>
# 191 "temp/res/mre.c"
    <CACHEDATA type="double" id="NVRAM_MRE_SRV_DA_SIM1">
        <DEFAULT_VALUE>[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]</DEFAULT_VALUE>
        <DESCRIPTION>MRE Dataaccount SIM1</DESCRIPTION>
    </CACHEDATA>

    <CACHEDATA type="double" id="NVRAM_MRE_SRV_DA_SIM1_PX">
        <DEFAULT_VALUE>[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]</DEFAULT_VALUE>
        <DESCRIPTION>MRE Dataaccount SIM1 with proxy</DESCRIPTION>
    </CACHEDATA>

    <CACHEDATA type="double" id="NVRAM_MRE_SRV_DA_SIM2">
        <DEFAULT_VALUE>[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]</DEFAULT_VALUE>
        <DESCRIPTION>MRE Dataaccount SIM2</DESCRIPTION>
    </CACHEDATA>

    <CACHEDATA type="double" id="NVRAM_MRE_SRV_DA_SIM2_PX">
        <DEFAULT_VALUE>[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]</DEFAULT_VALUE>
        <DESCRIPTION>MRE Dataaccount SIM2 with proxy</DESCRIPTION>
    </CACHEDATA>


    <CACHEDATA type="double" id="NVRAM_MRE_SRV_DA_SIM3">
        <DEFAULT_VALUE>[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]</DEFAULT_VALUE>
        <DESCRIPTION>MRE Dataaccount SIM3</DESCRIPTION>
    </CACHEDATA>

    <CACHEDATA type="double" id="NVRAM_MRE_SRV_DA_SIM3_PX">
        <DEFAULT_VALUE>[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]</DEFAULT_VALUE>
        <DESCRIPTION>MRE Dataaccount SIM3 with proxy</DESCRIPTION>
    </CACHEDATA>


    <CACHEDATA type="double" id="NVRAM_MRE_SRV_DA_SIM4">
        <DEFAULT_VALUE>[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]</DEFAULT_VALUE>
        <DESCRIPTION>MRE Dataaccount SIM4</DESCRIPTION>
    </CACHEDATA>

    <CACHEDATA type="double" id="NVRAM_MRE_SRV_DA_SIM4_PX">
        <DEFAULT_VALUE>[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]</DEFAULT_VALUE>
        <DESCRIPTION>MRE Dataaccount SIM4 with proxy</DESCRIPTION>
    </CACHEDATA>


    <CACHEDATA type="byte" id="NVRAM_MRE_SRV_DA_IS_WIFI_ONLY">
        <DEFAULT_VALUE>[0x0]</DEFAULT_VALUE>
        <DESCRIPTION>MRE Dataaccount only use wifi</DESCRIPTION>
    </CACHEDATA>

     <!--------------------------------------------------Update app area----------------------------------------------------->
     <STRING id="STR_MRE_UPDATE_TITLE"/>
     <STRING id="STR_MRE_UPDATE_QUERY"/>
     <STRING id="STR_MRE_UPDATE_UPDATING"/>
     <STRING id="STR_MRE_UPDATE_SUCCESS"/>
     <STRING id="STR_MRE_UPDATE_FAIL"/>


     <SCREEN id="SCR_ID_MRE_UPDATE_QUERY"/>
     <SCREEN id="SCR_ID_MRE_UPDATE_WAIT"/>
     <SCREEN id="SCR_ID_MRE_UPDATE_RESULT"/>

    <TIMER id="MRE_SYS_TIMER_0"/>
    <TIMER id="MRE_SYS_TIMER_1"/>
    <TIMER id="MRE_SYS_TIMER_2"/>
    <TIMER id="MRE_SYS_TIMER_3"/>
    <TIMER id="MRE_SYS_TIMER_4"/>
    <TIMER id="MRE_SYS_TIMER_5"/>
    <TIMER id="MRE_SYS_TIMER_6"/>
    <TIMER id="MRE_SYS_TIMER_7"/>
    <TIMER id="MRE_SYS_TIMER_8"/>
    <TIMER id="MRE_SYS_TIMER_9"/>
    <TIMER id="MRE_SYS_TIMER_10"/>
    <TIMER id="MRE_SYS_TIMER_11"/>
    <TIMER id="MRE_SYS_TIMER_12"/>
    <TIMER id="MRE_SYS_TIMER_13"/>
    <TIMER id="MRE_SYS_TIMER_14"/>
    <TIMER id="MRE_SYS_TIMER_15"/>
    <TIMER id="MRE_SYS_TIMER_16"/>
    <TIMER id="MRE_SYS_TIMER_17"/>
    <TIMER id="MRE_SYS_TIMER_18"/>
    <TIMER id="MRE_SYS_TIMER_19"/>
    <TIMER id="MRE_SYS_TIMER_20"/>
    <TIMER id="MRE_SYS_TIMER_21"/>
    <TIMER id="MRE_SYS_TIMER_22"/>
    <TIMER id="MRE_SYS_TIMER_23"/>
    <TIMER id="MRE_SYS_TIMER_24"/>
    <TIMER id="MRE_SYS_TIMER_25"/>
    <TIMER id="MRE_SYS_TIMER_26"/>
    <TIMER id="MRE_SYS_TIMER_27"/>
    <TIMER id="MRE_SYS_TIMER_28"/>
    <TIMER id="MRE_SYS_TIMER_29"/>
    <TIMER id="MRE_SYS_TIMER_30"/>
    <TIMER id="MRE_SYS_TIMER_31"/>
    <TIMER id="MRE_SYS_TIMER_32"/>
    <TIMER id="MRE_SYS_TIMER_33"/>
    <TIMER id="MRE_SYS_TIMER_34"/>
    <TIMER id="MRE_SYS_TIMER_35"/>
    <TIMER id="MRE_SYS_TIMER_36"/>
    <TIMER id="MRE_SYS_TIMER_37"/>
    <TIMER id="MRE_SYS_TIMER_38"/>
    <TIMER id="MRE_SYS_TIMER_39"/>
    <TIMER id="MRE_SYS_TIMER_40"/>
    <TIMER id="MRE_SYS_TIMER_41"/>
    <TIMER id="MRE_SYS_TIMER_42"/>
    <TIMER id="MRE_SYS_TIMER_43"/>
    <TIMER id="MRE_SYS_TIMER_44"/>
    <TIMER id="MRE_SYS_TIMER_45"/>
    <TIMER id="MRE_SYS_TIMER_46"/>
    <TIMER id="MRE_SYS_TIMER_47"/>
    <TIMER id="MRE_SYS_TIMER_48"/>
    <TIMER id="MRE_SYS_TIMER_49"/>
    <TIMER id="MRE_SYS_TIMER_50"/>
    <TIMER id="MRE_SYS_TIMER_51"/>
    <TIMER id="MRE_SYS_TIMER_52"/>
    <TIMER id="MRE_SYS_TIMER_53"/>
    <TIMER id="MRE_SYS_TIMER_54"/>
    <TIMER id="MRE_SYS_TIMER_55"/>
    <TIMER id="MRE_SYS_TIMER_56"/>
    <TIMER id="MRE_SYS_TIMER_57"/>
    <TIMER id="MRE_SYS_TIMER_58"/>
    <TIMER id="MRE_SYS_TIMER_59"/>
    <TIMER id="MRE_SYS_TIMER_60"/>
    <TIMER id="MRE_SYS_TIMER_61"/>
    <TIMER id="MRE_SYS_TIMER_62"/>
    <TIMER id="MRE_SYS_TIMER_63"/>
    <TIMER id="MRE_SYS_TIMER_64"/>
    <TIMER id="MRE_SYS_TIMER_65"/>
    <TIMER id="MRE_SYS_TIMER_66"/>
    <TIMER id="MRE_SYS_TIMER_67"/>
    <TIMER id="MRE_SYS_TIMER_68"/>
    <TIMER id="MRE_SYS_TIMER_69"/>
    <TIMER id="MRE_SYS_TIMER_70"/>
    <TIMER id="MRE_SYS_TIMER_71"/>
    <TIMER id="MRE_SYS_TIMER_72"/>
    <TIMER id="MRE_SYS_TIMER_73"/>
    <TIMER id="MRE_SYS_TIMER_74"/>
    <TIMER id="MRE_SYS_TIMER_75"/>
    <TIMER id="MRE_SYS_TIMER_76"/>
    <TIMER id="MRE_SYS_TIMER_77"/>
    <TIMER id="MRE_SYS_TIMER_78"/>
    <TIMER id="MRE_SYS_TIMER_79"/>
    <TIMER id="MRE_SYS_TIMER_80"/>
    <TIMER id="MRE_SYS_TIMER_81"/>
    <TIMER id="MRE_SYS_TIMER_82"/>
    <TIMER id="MRE_SYS_TIMER_83"/>
    <TIMER id="MRE_SYS_TIMER_84"/>
    <TIMER id="MRE_SYS_TIMER_85"/>
    <TIMER id="MRE_SYS_TIMER_86"/>
    <TIMER id="MRE_SYS_TIMER_87"/>
    <TIMER id="MRE_SYS_TIMER_88"/>
    <TIMER id="MRE_SYS_TIMER_89"/>
    <TIMER id="MRE_SYS_TIMER_90"/>
    <TIMER id="MRE_SYS_TIMER_91"/>
    <TIMER id="MRE_SYS_TIMER_92"/>
    <TIMER id="MRE_SYS_TIMER_93"/>
    <TIMER id="MRE_SYS_TIMER_94"/>
    <TIMER id="MRE_SYS_TIMER_95"/>
    <TIMER id="MRE_SYS_TIMER_96"/>
    <TIMER id="MRE_SYS_TIMER_97"/>
    <TIMER id="MRE_SYS_TIMER_98"/>
    <TIMER id="MRE_SYS_TIMER_99"/>
# 1641 "temp/res/mre.c"
</APP>
