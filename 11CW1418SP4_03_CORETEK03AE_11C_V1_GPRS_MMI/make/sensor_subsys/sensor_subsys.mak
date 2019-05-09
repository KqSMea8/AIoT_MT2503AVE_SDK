# Define source file lists to SRC_LIST
#MT2511#
ifneq ($(filter __MTK_SENSOR_SUBSYS_SUPPORT__, $(strip $(MODULE_DEFS))),)
SRC_LIST = sensor_subsys\src\sensor_manager.c \
           sensor_subsys\src\algo_adaptor\algo_adaptor.c \
           sensor_subsys\src\algo_adaptor\blood_pressure_monitor_adaptor.c \
           sensor_subsys\src\algo_adaptor\heart_rate_monitor_adaptor.c \
           sensor_subsys\src\algo_adaptor\heart_rate_variability_monitor_adaptor.c \
           sensor_subsys\driver\mems\src\sensor_manager_driver.c \
           sensor_subsys\driver\mems\src\bma255_sensor_adaptor.c \
           sensor_subsys\driver\mems\src\bma2x2.c \
           sensor_subsys\driver\mems\src\bma255_i2c_operation.c \
           sensor_subsys\driver\mems\src\mems_bus.c \
           sensor_subsys\driver\biosensor\mt2511\src\vsm_driver.c \
           sensor_subsys\driver\biosensor\mt2511\src\vsm_platform_function.c \
           sensor_subsys\driver\biosensor\mt2511\src\vsm_sensor_subsys_adaptor.c \
           sensor_subsys\driver\biosensor\mt2511\src\vsm_spi_operation.c \
           sensor_subsys\driver\biosensor\mt2511\src\vsm_i2c_operation.c
endif
#MT2511#

# Third party package, for SMT AT command use
SRC_LIST += sensor_subsys\third_party\kiss_fft\kiss_fft.c

# Define include path lists to INC_DIR
INC_DIR = sensor_subsys\inc \
          sensor_subsys\inc\algo_adaptor \
          sensor_subsys\driver\mems\inc \
          sensor_subsys\driver\biosensor\mt2511\inc \
          sensor_subsys\driver\biosensor\mt2511\module\everlight \
          hal\peripheral\inc \
          sensor_subsys\third_party\kiss_fft

#algo inc#
INC_DIR += sensor_subsys\fusion_algo\common\inc \
		   sensor_subsys\fusion_algo\blood_pressure\inc \
		   sensor_subsys\fusion_algo\heart_rate\inc \
		   sensor_subsys\fusion_algo\ppg_control\inc \
		   sensor_subsys\fusion_algo\heart_rate_variability\inc
		
# Define the specified compile options to COMP_DEFS
COMP_DEFS = FUSION_BLOOD_PRESSURE_USE=M_INHOUSE_BLOOD_PRESSURE  \
            FUSION_HEART_RATE_MONITOR_USE=M_INHOUSE_HEART_RATE_MONITOR \
            MTK_SENSOR_ACCELEROMETER_USE_BMA255 \
            BMA255_USE_POLLING \
            MTK_SENSOR_BIO_USE_MT2511 \
            MT2511_USE_SPI 

