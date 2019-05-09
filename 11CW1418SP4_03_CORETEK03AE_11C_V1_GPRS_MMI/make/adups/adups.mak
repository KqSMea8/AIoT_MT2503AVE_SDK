# Define source file lists to SRC_LIST

ifeq ($(strip $(ADUPS_FOTA_DIFF_ENABLE)),TRUE)
ifeq ($(strip $(ADUPS_FOTA_SOC_ENABLE)),TRUE)
ifneq ($(strip $(ADUPS_FOTA_DIFF_LIB)),TRUE)
   	 SRC_LIST += adups\app\src\adupsfota.c \
	 adups\app\src\http_parser.c
endif

   	 SRC_LIST += adups\hal\src\adups_debug.c \
	 adups\hal\src\adups_device.c \
	 adups\hal\src\adups_display.c \
	 adups\hal\src\adups_file.c \
	 adups\hal\src\adups_flash.c \
	 adups\hal\src\adups_net.c \
	 adups\hal\src\adups_stdlib.c \
	 adups\hal\src\adups_timer.c \
	 adups\hal\src\adups_md5.c \
	 adups\hal\src\adups_thread.c
endif
endif

# Define include path lists to INC_DIR
INC_DIR = hal\video\demuxer\provider\inc \
          custom\video \
          drm\include \
          gps\inc \
          irda\inc \
          j2me\interface \
          j2me\jal\include \
          j2me\jal\include \
          media\image\include \
          media\rtp\inc \
          media\stream\include \
          media\mtv\src \
          media\mtv\include \
          usb\include \
          verno \
          wapadp\include \
          xmlp\include \
          applib\inet\engine\include \
          applib\mem\include \
          applib\misc\include \
          nvram\include \
          plutommi\customer\customerinc \
          plutommi\mmi\asyncevents\asynceventsinc \
          plutommi\mmi\inc \
          plutommi\Framework\Interface \
          plutommi\Framework\IndicLanguages\indiclanguagesinc \
          plutommi\Framework\CommonFiles\commoninc \
          plutommi\Framework\EventHandling\eventsinc \
          plutommi\Framework\History\historyinc \
          plutommi\Framework\Scenario \
          plutommi\Framework\MemManager\memmanagerinc \
          plutommi\Framework\NVRAMManager\nvrammanagerinc \
          plutommi\Framework\Tasks\tasksinc \
          plutommi\Framework\ThemeManager\thememanagerinc \
          plutommi\Framework\GUI\gui_inc \
          plutommi\Framework\GUI\oem_inc \
          plutommi\Framework\Fontengine \
          plutommi\mmi\miscframework\miscframeworkinc \
          plutommi\Framework\InputMethod\Inc \
          plutommi\Framework\GDI\gdiinc \
          plutommi\service\mdi\mdiinc \
          plutommi\mmi\Organizer\OrganizerInc \
          plutommi\mmi\datetime\datetimeinc \
          plutommi\Framework\DebugLevels\debuglevelinc \
          plutommi\mmi\setting\settinginc \
          plutommi\service\Inc \
          plutommi\service\UmSrv \
          plutommi\Service\UcSrv \
          plutommi\MMI\UnifiedComposer\UnifiedComposerInc \
          plutommi\Service\DtcntSrv \
          plutommi\mtkapp\filemgr\filemgrinc \
          plutommi\Service\UmmsSrv \
          plutommi\mmi\UnifiedMMS\MMSApp\MMSAppInc \
          plutommi\mtkapp\JavaAgency\JavaAgencyInc \
          plutommi\MtkApp\Connectivity\ConnectivityInc \
          plutommi\MMI\BrowserApp\UPPApp\UPPAppInc \
          plutommi\Service\UriAgentSrv \
          plutommi\mtkapp\DMUIApp\DMUIAppInc \
          btstacka\inc \
          btadp\include \
          plutommi\mtkapp\MREAPP\MREAPPInc \
          venusmmi\app \
          venusmmi\framework \
          venusmmi\vrt\interface \
          vendor\widget\google\adaptation \
          plutommi\Framework\CommonScreens\commonscreensinc \
          plutommi\mmi\gpio\gpioinc \
          plutommi\mtkapp\audioplayer\audioplayerinc \
          plutommi\mtkapp\connectivity\connectivityinc \
          plutommi\mtkapp\connectivity\connectivityinc\btcommon \
          plutommi\mtkapp\connectivity\connectivityinc\btmtk \
          plutommi\mmi\phonebook\phonebookinc \
          plutommi\mmi\phonebook\core \
          plutommi\CUI\Inc \
          plutommi\mtkapp\mtkappinc \
          plutommi\mtkapp\syncml\syncmlinc \
          xmlp\include \
          vcard\include \
          wapadp\include \
          plutommi\MtkApp\vObjects\vObjectInc \
          plutommi\MtkApp\CentralConfigAgent\CentralConfigAgentInc \
          plutommi\mmi\Messages\MessagesInc \
          plutommi\MMI\Messages \
          plutommi\mmi\profiles\ProfilesInc \
          plutommi\MMI\IdleScreen\IdleScreenInc \
          plutommi\MMI\IdleScreen\IdleScreenInc\DemoAppInc \
          plutommi\mmi\Audio\AudioInc \
          plutommi\mmi\ebookreader\ebookinc \
          plutommi\service\UcmSrv \
          plutommi\mmi\BT_UCM\UcmBTInc\
          plutommi\mmi\gsmcallapplication\commonfiles\commoninc \
          plutommi\mmi\gsmcallapplication\incomingcallmanagement\incominginc \
          plutommi\mmi\gsmcallapplication\outgoingcallmanagement\outgoinginc \
          plutommi\mmi\cphs\cphsinc \
          plutommi\mtkapp\bgsound\bgsoundinc \
          plutommi\AppCore\ssc \
          plutommi\mtkapp\answermachine\answermachineinc \
          plutommi\MMI\VoIP\VoIPInc \
          plutommi\MtkApp\GPS\GPSInc \
          drm\include \
          plutommi\MMI\DataAccount\DataAccountInc \
          plutommi\VendorApp\DevConfig\DevConfigInc \
          $(strip $(PS_FOLDER))\smslib\include \
          $(strip $(PS_FOLDER))\l4\include \
          custom\common\pluto_mmi \
          custom\common\VENUS_MMI \
          custom\common\hal_public \
          interface\media \
          interface\ps\include \
          interface\ps\enum \
          plutommi\mmi\sat\satinc \
          plutommi\service\Gsm3GCallSrv \
          plutommi\service\CtmSrv \
          $(strip $(PS_FOLDER))\l4\csm\cc\include \
          $(strip $(PS_FOLDER))\interfaces\enum \
          plutommi\service\WapPushSrv \
          plutommi\mmi\BrowserApp\PushInbox\PushInboxInc \
          plutommi\mmi\emailapp\emailappinc \
          plutommi\Service\EmailSrv \
          plutommi\Service\CloudSrv \
          applib\sqlite3\inc \
          plutommi\MtkApp\Camera\CameraInc \
          plutommi\mtkapp\mediaplayer\mediaplayerinc \
          plutommi\mtkapp\rightsmgr\rightsmgrinc \
          plutommi\Service\PlstSrv \
          plutommi\Service\PlstSlimSrv \
          plutommi\MtkApp\WidgetDelegator\WidgetDelegatorInc \
          plutommi\MMI\NITZHandler\NITZInc \
          plutommi\Service\AppMgrSrv \
          plutommi\Service\NativeAppFactorySrv \
          plutommi\Service\ProfilesSrv \
          plutommi\Service\SoundEffectSrv \
          plutommi\Service\SoundRecSrv \
          plutommi\Service\BthScoPathSrv \
          plutommi\Service\A2dpSrv \
          plutommi\Service\AvrcpSrv \
          plutommi\mtkapp\fmradio\fmradioinc \
          plutommi\mmi\ATHandler\ATHandlerInc \
          plutommi\MMI\MiscFramework\MiscFrameworkInc \
          plutommi\MMI\Idle\IdleInc \
          plutommi\service\NetSetSrv \
          plutommi\service\ModeSwitchSrv \
          plutommi\service\BrowserSrv \
          plutommi\mmi\BrowserApp\Browser\BrowserInc \
          plutommi\mmi\DataAccount\DataAccountInc \
          vendor\wap\obigo_Q05A\adaptation\modules\bam\include \
          interface\wap \
          plutommi\MtkApp\DLAgent\DLAgentInc \
          plutommi\service\CallLogSrv \
          plutommi\Service\CcaSrv \
          plutommi\mmi\SIMProvAgent\SIMProvAgentInc \
          plutommi\mmi\connectmanagement\connectmanagementinc \
          plutommi\Service\vCalSrv \
          plutommi\Service\CharBatSrv \
          venusmmi\app\Cosmos\Dataaccount \
          plutommi\Service\ProvBoxSrv \
          vendor\player\ipeer\inc \
          vendor\player\ipeer\adaptation\inc \
          vendor\scalado\include \
          vendor\wap\obigo_Q03C\adaptation\integration\include \
          verno \
          interface\hal \
          interface\hal\video \
          interface\inet_ps \
          venusmmi\app\Cosmos\interface\app \
          venusmmi\app\pluto_variation\adapter\ShellApp\panel \
          venusmmi\app\pluto_variation\adapter\ShellApp\panel\HomeScreen \
          venusmmi\app\pluto_variation\ShellApp\panel \
          venusmmi\app\pluto_variation\ShellApp\panel\HomeScreen \
          plutommi\Service\InetSrv \
          plutommi\Service\GetSrvSrv \
          plutommi\Service\Inc\Gsinc \
          interface\ps \
          interface\mmi \
          interface\hal\drv_sw_def \
          plutommi\Service\SyncSrv \
          plutommi\Service\SyncSrv\Database \
          plutommi\Service\SyncSrv\Sync \
          plutommi\Service\TetheringSrv \
          plutommi\Service\ProvisioningSrv \
          plutommi\Service\UPPSrv \
          plutommi\Service\PhbSrv \
          plutommi\Service\CnmgrSrv \
          plutommi\Service\ImgEdtSrv \
          plutommi\Service\UpgradeSrv \
          plutommi\Service\SsSrv \
          venusmmi\app\common\ime \
          venusmmi\framework\ui_core\mvc \
          venusmmi\framework\ui_core\pme \
          venusmmi\framework\ui_core\base \
          venusmmi\framework\mmi_core \
          plutommi\cui\fseditorcui \
          venusmmi\app\Cosmos\Calendar \
          venusmmi\app\Cosmos\JavaApp \
          plutommi\Service\DmSrv \
          plutommi\Service\DmSRsrv \
          interface\fota \
          btmt\rf_desense \
          venusmmi\app\cosmos\OPMmApp\
          venusmmi\app\pluto_variation\LauncherKey\OPMainmenu \
          inet_ps\ntpd\include \
          plutommi\Service\PnSrv \
          vendor\push_notification_engine\HPNSTask\inc \
          vendor\push_notification_engine\PnsLib\JsonParser \
          plutommi\Service\SppSrv \
          interface\hal\bluetooth \
          custom\common\hal_public \
          interface\media\video \
          vendor\sensor\mcube\inc\
          venusmmi\framework\interface \
          venusmmi\visual\cp \
          plutommi\mtkapp\video\videoinc \
          venusmmi\vrt\interface \
          venusmmi\app\common\ncenter \
          venusmmi\app\common\interface\app\
          plutommi\Service\TcardSrv \
          plutommi\Service\NotificationSettingSrv \
          plutommi\Service\SsoSrv\plugin\yahoo \
          hal\system\dcmgr\inc \
          plutommi\Service\MapcSrv \
          plutommi\Service\SmsbtmapcSrv \
          plutommi\Service\PbapSrv \
          plutommi\Service\BTNotificationConn

ifeq ($(filter __MTK_TARGET__ , $(strip $(MODULE_DEFS))),)
INC_DIR += MoDIS_VC9\MoDIS  # for w32_utility.h
endif

ifeq ($(strip $(MMI_VERSION)),COSMOS_MMI)
INC_DIR += venusmmi\app\Cosmos\Ucm
else
INC_DIR += plutommi\mmi\Ucm\UcmInc
endif

ifeq ($(strip $(ADUPS_FOTA_DIFF_ENABLE)),TRUE)
ifeq ($(strip $(ADUPS_FOTA_SOC_ENABLE)),TRUE)
INC_DIR += adups\hal\include
ifneq ($(strip $(ADUPS_FOTA_DIFF_LIB)),TRUE)
INC_DIR += adups\app\include
endif
endif
endif
# Define the specified compile options to COMP_DEFS
COMP_DEFS = MMI_ON_HARDWARE_P  \
            __EMS__ \
            SIM_APP_TOOLKIT \
            TETRIS_GAME_APP \
            CM_APPLICATION_ENABLE \
            MESSAGES_APPLICATION \
            __UCS2_ENCODING \
            CELLBROADCAST_APP \
            __CB__ \
            VM_SUPPORT \
            SHORTCUTS_APP \
            T9LANG_English \
            T9LANG_Chinese \
            T9LANG_Spanish \
            T9LANG_French \
            T9LANG_German \
            T9LANG_Italian \
            EMS_LITTLE_ENDIAN \
            EMS_MEM_MANAGEMENT \
            T9ALPHABETIC \
            T9CHINESE \
            MMS_VIRTUAL_FILE_SUPPORT \
            COMPILER_SUPPORTS_LONG=1


