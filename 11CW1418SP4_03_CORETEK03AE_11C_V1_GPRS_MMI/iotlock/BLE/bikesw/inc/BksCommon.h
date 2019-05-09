#ifndef __BKSS_COMMON_H__
#define __BKSS_COMMON_H__

#include "bt_mmi.h"
#include "bluetooth_gattc_struct.h"
#include "GattSrv.h"

/*
  *  Define the Pedometer enable status
  *  (Locator/Target)
  *
  */
#define __BKSS_SUPPORT_WECHAT__
typedef U8 BKSS_STATUS;

#define BKSS_STATUS_DISABLED      0
#define BKSS_STATUS_ENABLING      1
#define BKSS_STATUS_ENABLED       2
#define BKSS_STATUS_DISABLING     3

/*
  *  Define the Pedometer current connection status
  *  (Locator/Target)
  *
  */
typedef U8 BKSS_CONNECTION_STATUS;

#define BKSS_STATUS_DISCONNECTED    0
#define BKSS_STATUS_CONNECTING      1
#define BKSS_STATUS_CONNECTED       2
#define BKSS_STATUS_DISCONNECTING   3

/*
  *  Define the Pedometer service type
  *
  */
typedef U8 BKSS_SERVICE_TYPE;

#define BKSS_SERVICE_TYPE           0  //BKSS Service, 0x18AA
#define BKSS_SERVICE_NUM            1  //BKSS Service Number

#define BKSS_SERVICE    0xFEE7  //bikesw Service, 0x18AA
#define BKSS_WRITE_UUID      0x36F5  //bikesw Characteristic, 0x2AAA
#define BKSS_READ_UUID 0x36F6

#define BKSS_WECHAT_WRITE_UUID 0xFEC7
#define BKSS_WECHAT_READ_INDI_UUID 0xFEC8
#define BKSS_CHAR_CONFIG_UUID 0x2902
#define BKSS_WECHAT_READ_UUID 0xFEC9


#endif
