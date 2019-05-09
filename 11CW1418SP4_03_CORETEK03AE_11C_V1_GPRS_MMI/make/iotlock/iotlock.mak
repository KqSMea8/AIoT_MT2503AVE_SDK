# Define source file lists to SRC_LIST
INC_DIR = iotlock\inc
SRC_LIST = iotlock\src\iotlock.c
ifneq ($(filter __IOT_BLE_BKSS_SUPPORT__, $(strip $(MODULE_DEFS))),)
SRC_LIST += iotlock\ble\bikesw\src\BkssSrv.c \
           iotlock\ble\bikesw\src\Bkss.c \
           iotlock\ble\bikesw\src\aes.c \
           iotlock\ble\bikesw\wechatSrc\epb.c \
           iotlock\ble\bikesw\wechatSrc\epb_MmBp.c \
           iotlock\ble\bikesw\wechatSrc\ble_wechat_util.c \
           iotlock\ble\bikesw\wechatSrc\md5.c

INC_DIR += iotlock\ble\bikesw\inc \
          iotlock\ble\bikesw\wechatInc
endif

ifneq ($(filter __IOT_LOCK__, $(strip $(MODULE_DEFS))),)
SRC_LIST += iotlock\gprs\src\lock_net.c \
           iotlock\gprs\src\lock_utils.c \
           iotlock\gprs\src\linked_list.c \
           iotlock\gps\src\lock_gps.c \
           iotlock\drv\src\lock_drv.c \
           iotlock\ata\src\Atatest.c \
           iotlock\gprs\src\aes_tiny.c \
		   iotlock\gprs\src\base64.c

INC_DIR += plutommi\Service\MDI\MDIInc \
          iotlock\gprs\inc \
          iotlock\gps\inc \
          iotlock\drv\inc \
          custom\drv\switch_charger 
endif
ifneq ($(filter __NSK_ECG__, $(strip $(MODULE_DEFS))),)
SRC_LIST += iotlock\ecg\src\fft.c \
           iotlock\ecg\src\hrv.c \
           iotlock\ecg\src\ha.c \
           iotlock\ecg\src\htz.c \
           iotlock\ecg\src\imath.c \
           iotlock\ecg\src\median.c \
           iotlock\ecg\src\mood.c \
           iotlock\ecg\src\noise.c \
           iotlock\ecg\src\nsk_ecg_core.c \
           iotlock\ecg\src\nsk_ecg_core_state.c \
           iotlock\ecg\src\nsk_ecg_ext.c \
           iotlock\ecg\src\prep.c \
           iotlock\ecg\src\resampler.c \
           iotlock\ecg\src\rhr.c \
           iotlock\ecg\src\rpeak.c \
           iotlock\ecg\src\rsa.c \
           iotlock\ecg\src\spectr.c \
           iotlock\ecg\src\sq.c \
           iotlock\ecg\src\stress.c

INC_DIR += iotlock\ecg\inc
endif

# Define the specified compile options to COMP_DEFS
COMP_DEFS =
SRC_PATH = iotlock\src
