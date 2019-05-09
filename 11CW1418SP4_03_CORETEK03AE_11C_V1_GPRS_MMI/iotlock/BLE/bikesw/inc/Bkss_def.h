#ifndef __BKSS_DEF_H__
#define __BKSS_DEF_H__

//#ifdef MMI_SRV_BT_SOGPS_PROFILE 

#include "bt_mmi.h"
#include "stack_msgs.h"
#include "BksCommon.h"
#include "BkssSrv.h"
#include "GattServicesFactorySrv.h"
#include "GattSrv.h"
#include "GattsSrv.h"

typedef enum {
	BKSS_OP_INIT = 0,
	BKSS_OP_GET_SERVICE,
	BKSS_OP_ADD_SERVICE,
	BKSS_OP_DEL_SERVICE,
	BKSS_OP_DEINIT,
}BKSS_OP_CODE;

typedef struct{
	ATT_HANDLE     handle;
	ATT_VALUE      dec_value;
	BOOL            is_dec_write;
}bkss_dec_struct;

typedef struct {
    ListEntry             conn_node;
	void                 *conn_ctx;
	int                   conn_status;
	char                  bdaddr[BD_ADDR_SIZE];
	bkss_dec_struct        dec;
}BkssConnCntx;

typedef struct {
	void                 *reg_ctx;
	bkss_callback_t      *callback;
	BKSS_STATUS           state;
	bt_uuid_struct        uid;
	ListEntry             conn_list; 
	int                   op_flag;
	gatts_srvs_lst_struct srv_list;
}BkssCntx;

#endif
