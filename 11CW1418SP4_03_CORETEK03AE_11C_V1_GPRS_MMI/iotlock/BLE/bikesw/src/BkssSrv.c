
/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2010
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *  BkssSrv.c
 *
 * Project:
 * --------
 *  MAUI
 *
 * Description:
 * ------------
 *  This file defines the SAP of the Pedometer Profile.
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 *
  *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include "MMI_features.h"

#ifdef __IOT_BLE_BKSS_SUPPORT__ 

#include "DebugInitDef_Int.h"
#include "BkssSrvGprot.h"
#include "BkssSrv.h"
#include "Bkss_def.h"
#include "aes.h"
#include "Kal_trace.h"
#include "common_nvram_editor_data_item.h"
/*UT*/
#include "BtcmSrvGprot.h"
#include "MMI_trc_Int.h"
#include "MMI_conn_app_trc.h"
#include "mmi_cb_mgr_gprot.h"
#include "mmi_rp_srv_blecm_def.h"
#include "blesrvgprot.h" // srv_ble_cm_get_connected_dev_num
#include "epb_MmBp.h"
#include "mpbledemo2.h"

#include "BtcmSrvProt.h"
extern srv_bt_cm_cntx_struct g_srv_bt_cm_cntx;
extern U8 bkss_get_battery_percentage();
#define  BKSS_BD_ADDR_SIZE                   6
static BtStatus bkss_disconnect_event_handler(BD_ADDR *bdaddr);
static void bkss_get_bd_addr(BD_ADDR *dst,const srv_bt_cm_bt_addr *src);
static mmi_ret bkss_recv_event_handler(mmi_event_struct *evt);
ATT_HANDLE g_bkssatt_handle = 0;
MMI_BOOL g_bkssfirt_write = MMI_FALSE;
char g_bkss_conn_addr[BKSS_BD_ADDR_SIZE] = {0};
kal_uint8 wechat_data[ATT_MAX_VALUE_LEN] = {0};
kal_uint8 resp_send_data[ATT_MAX_VALUE_LEN] = {0};
kal_uint16 resp_send_data_len = 0;
kal_uint8 wechat_data_len = 0;
kal_bool wechat_operate = KAL_FALSE;
kal_uint8 rep_send_data[ATT_MAX_VALUE_LEN] = {0};
kal_uint16 rep_send_data_len = 0;

void bkss_response_confirmation_callback(OS_STATUS status, ATT_HANDLE handle);

BkssCntx  g_bkss_cntx = {0};
U8        g_bkss_uuid[] = {
	0x18, 0xAA, 0x1F, 0x49, 0xFF, 0xE5, 0x40, 0x56,
	0X84, 0x5B, 0x6D, 0xF1, 0xF1, 0xB1, 0x6E, 0x9D
};
gatts_callback_struct g_bkss_cb = {
	bkss_register_server_callback,
    bkss_connection_callback,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	bkss_service_started_callback,
	NULL,
	NULL,
	bkss_request_read_callback,
	bkss_request_write_callback,
	bkss_request_exec_write_callback,
	bkss_response_confirmation_callback,
	NULL,
	NULL,
	NULL,
};
gatts_factory_callback_struct g_bkss_factory_cb = {
	NULL,
	NULL,
	bkss_add_services_callback,
	NULL,
};


int bkss_find_uuid_by_handle(ATT_HANDLE att_handle, ATT_UUID *svc_uuid, ATT_UUID *chr_uuid, ATT_UUID *desc_uuid)
{
	gatts_service_decl_struct *svc_list = (gatts_service_decl_struct *)mmi_GetHeadList(&g_bkss_cntx.srv_list.srvlist);

	if (mmi_IsListEmpty(&g_bkss_cntx.srv_list.srvlist))
	{
		return TYPE_UNKNOW;
	}

    if (att_handle == 0)
	{
	     return TYPE_UNKNOW;
	}
    
	while ((ListEntry *)svc_list != &g_bkss_cntx.srv_list.srvlist)
	{
		if (svc_list->handle == att_handle)
		{
			switch(svc_list->type)
			{
				case TYPE_SERVICE:
					memcpy(svc_uuid, &svc_list->uuid, sizeof(ATT_UUID));
					return TYPE_SERVICE;
				case TYPE_CHARACTERISTIC:
					memcpy(chr_uuid, &svc_list->uuid, sizeof(ATT_UUID));
					bkss_find_uuid_by_handle(svc_list->srvhandle, svc_uuid, chr_uuid,desc_uuid);
					return TYPE_CHARACTERISTIC;
				case TYPE_DESCRIPTOR:
					memcpy(desc_uuid, &svc_list->uuid, sizeof(ATT_UUID));
					bkss_find_uuid_by_handle(svc_list->charhandle, svc_uuid, chr_uuid,desc_uuid);
					return TYPE_DESCRIPTOR;	
				default:
					return svc_list->type;
			}
		}
			
		svc_list = (gatts_service_decl_struct *)mmi_GetNextNode(&svc_list->declnode);
	}
	return TYPE_UNKNOW;
}

void bkss_request_read_callback(gatt_conn_struct *conn, U16 trans_id, BD_ADDR *bd_addr,
                                      ATT_HANDLE attr_handle, U16 offset, BOOL is_long)
{
	BkssConnCntx    *pConnNode = (BkssConnCntx *)mmi_GetHeadList(&g_bkss_cntx.conn_list);
	ATT_UUID         svc_uuid;
	ATT_UUID         chr_uuid;
	ATT_UUID         desc_uuid;
	int              type = bkss_find_uuid_by_handle(attr_handle, &svc_uuid, &chr_uuid,&desc_uuid);
	U16              uuid = convert_array_to_uuid16(chr_uuid);
	ATT_VALUE        att_value;
    S32 ret_val;
   
	bkss_dbg_print(MOD_ENG,"%s() %d ",__func__,__LINE__);
	if(type == TYPE_CHARACTERISTIC)
	{	
	        if(uuid == BKSS_WRITE_UUID)
	        {
	            if(g_bkss_cntx.callback)
	            {        
	                ret_val = g_bkss_cntx.callback->read_data_request_cb(bd_addr->addr, &(att_value.value[offset]), (BKSS_MAX_VALUE_LEN - offset) );
	                if(ret_val >0 )
	                {
	                    att_value.len = ret_val;
	                    srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &att_value);   
	                }
	                else
	                {
	                    att_value.len = 0;
	                    srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &att_value);  
	                }
	            }
	                         
	        }
	        else  if(uuid == BKSS_WECHAT_WRITE_UUID)
	        {
	            if(g_bkss_cntx.callback)
	            {        
	bkss_dbg_print(MOD_ENG,"%s() %d ",__func__,__LINE__);
	                ret_val = g_bkss_cntx.callback->read_wechatdata_request_cb(bd_addr->addr, &(att_value.value[offset]), (BKSS_MAX_VALUE_LEN - offset) );
	                if(ret_val >0 )
	                {
	                    att_value.len = ret_val;
	                    srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &att_value);   
	                }
	                else
	                {
	                    att_value.len = 0;
	                    srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &att_value);  
	                }
	            }
	                         
	        }
		else if	(uuid == BKSS_WECHAT_READ_INDI_UUID)
	        {
	            if(g_bkss_cntx.callback)
	            {   
	            	bkss_dbg_print(0, "bkss_request_read_callback   ==>read_wechatindicate_request_cb \n");
	                ret_val = g_bkss_cntx.callback->read_wechatindicate_request_cb(bd_addr->addr, &(att_value.value[offset]), (BKSS_MAX_VALUE_LEN - offset) );
					if (ret_val > 0)
	                {
	                    att_value.len = ret_val;
	                    srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &att_value);   
	                }
	                else
	                {
	                    att_value.len = 0;
	                    srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &att_value);  
	                }                    
	            }
	                         
	        }
		else if	(uuid == BKSS_WECHAT_READ_UUID)
	        {
	            if(g_bkss_cntx.callback)
	            {      
	            	bkss_dbg_print(0, "bkss_request_read_callback   ==>read_wechatdata_request_cb \n");
	                ret_val = g_bkss_cntx.callback->read_wechatdata_request_cb(bd_addr->addr, &(att_value.value[offset]), (BKSS_MAX_VALUE_LEN - offset) );
	                if(ret_val >0 )
	                {
	                    att_value.len = ret_val;
	                    srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &att_value);   
	                }
	                else
	                {
	                    att_value.len = 0;
	                    srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &att_value);  
	                }
	            }
	                         
	        }
	        else
	        {
	            att_value.len = 1;
	            att_value.value[offset] = GATT_ERROR_REQUEST_NOT_SUPPORT;
	            srv_gatts_send_response(conn, trans_id, OS_STATUS_FAILED, attr_handle, &att_value);
	        }
	}
	else
	{
		att_value.len = 1;
		att_value.value[offset] = GATT_ERROR_REQUEST_NOT_SUPPORT;
		srv_gatts_send_response(conn, trans_id, OS_STATUS_FAILED, attr_handle, &att_value);
	}
	
}

#if defined(__BKSS_SUPPORT_WECHAT__)
#define MANUFACTURER_LEN 8
U8 bkss_manufacturer_data[MANUFACTURER_LEN]={0xFF,0x02,0,0,0,0,0,0};
#else
#define MANUFACTURER_LEN 13
U8 bkss_manufacturer_data[MANUFACTURER_LEN]={0xFF,0x02,0,0,0,0,0,0,0x02,2,3,4,5};
#endif
void bkss_set_adv_manufacturer_data(void)
{
	int i;
	U8 service_uuid[]={0xFE,0xE7};
	BD_ADDR bd_addr = {0};
	
	if(bkss_manufacturer_data[0] == 0xFF)
	{
		bkss_manufacturer_data[0] = 0x01;
	}
	bkss_get_bd_addr(&bd_addr,&g_srv_bt_cm_cntx.host_dev.le_bd_addr);  //le_bd_addr
	for(i=0;i<BD_ADDR_SIZE;i++)
	{
		bkss_manufacturer_data[i+2] = bd_addr.addr[BD_ADDR_SIZE - 1 - i];
	}
#if !defined(__BKSS_SUPPORT_WECHAT__)
	bkss_manufacturer_data[8] = 0x15;//IDL
	bkss_manufacturer_data[9] = iot_get_battery_percentage();
	bkss_manufacturer_data[10] = (read_lock_state() == LOCKED)?0:1;
	bkss_manufacturer_data[11] = 0x00;//IDH
	bkss_manufacturer_data[12] = 0x12;
#endif
}
extern void gatts_init_handle_connected_ind(void);
extern void gatts_deinit_handle_connected_ind(void);
void bkss_set_adv_data(void)
{
	U8 service_uuid[]={0xFE,0xE7};
	if(bkss_manufacturer_data[0] == 0xFF)
	{
		bkss_set_adv_manufacturer_data();
	}
	gatts_init_handle_connected_ind();
	srv_gatts_set_adv_data(g_bkss_cntx.reg_ctx,1,1,0,0,0,0,MANUFACTURER_LEN,bkss_manufacturer_data,0,NULL,2,service_uuid);
}
void bkss_reset_adv_data(void)
{
	U8 service_uuid[]={0xFE,0xE7};
	if(bkss_manufacturer_data[0] == 0xFF)
	{
		bkss_set_adv_manufacturer_data();
	}
	gatts_deinit_handle_connected_ind();
	srv_gatts_set_adv_data(g_bkss_cntx.reg_ctx,1,1,0,0,0,0,MANUFACTURER_LEN,bkss_manufacturer_data,0,NULL,2,service_uuid);
}
void bkss_update_adv_data(void)
{
	bkss_manufacturer_data[0] == 0xFF;
}
void bkss_request_exec_write_callback(gatt_conn_struct *conn, U16 trans_id,
                                            BD_ADDR *bd_addr, BOOL cancel)
{
	BkssConnCntx	 *pConnNode = (BkssConnCntx *)mmi_GetHeadList(&g_bkss_cntx.conn_list);
	ATT_UUID		 svc_uuid;
	ATT_UUID		 chr_uuid;
	ATT_UUID		 desc_uuid;
	int 			 type = bkss_find_uuid_by_handle(g_bkssatt_handle, &svc_uuid, &chr_uuid,&desc_uuid);
	U16 			 svc_uuid16 = convert_array_to_uuid16(svc_uuid);
	U16 			 chr_uuid16 = convert_array_to_uuid16(chr_uuid);
	ATT_VALUE        att_value;
		   
	bkss_dbg_print(MOD_ENG, "[BKSS] bkss_request_exec_write_callback attr_handle %x, svc_uuid16 %x, chr_uuid16 %x!\n", 
					 g_bkssatt_handle, svc_uuid16, chr_uuid16);	

	//MMI_TRACE(MMI_CONN_TRC_G7_BT, TRC_BLE_BKSS_EXEC_WRITE_CB, g_bkssatt_handle, svc_uuid16, chr_uuid16);

	if (mmi_IsListEmpty(&g_bkss_cntx.conn_list))
	{
		bkss_dbg_print(MOD_ENG, "[BKSS] bkss_request_exec_write_callback doesn't connected!\n");
		
		//MMI_TRACE(MMI_CONN_TRC_G7_BT, TRC_BLE_BKSS_CONNECT_FAIL);
		return;
	}
	while ((ListEntry *)pConnNode != &g_bkss_cntx.conn_list)
	{
		if (memcmp(bd_addr->addr, pConnNode->bdaddr, BD_ADDR_SIZE) == 0)
		{
			bkss_dbg_print(MOD_ENG, "[BKSS] bkss_request_exec_write_callback success trans_id = %d, cancel = %d", 
						  trans_id, cancel);
			//MMI_TRACE(MMI_CONN_TRC_G7_BT, TRC_BLE_BKSS_EXEC_WRITE_SUCCESS, trans_id, cancel);
				  
			if (svc_uuid16 == BKSS_SERVICE && chr_uuid16 == BKSS_WECHAT_READ_INDI_UUID)
			{					
				/*add for low power*/			
				//should send response
				att_value.len = 0;
				srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, g_bkssatt_handle, &att_value);  
			}
			return;
		}
		pConnNode = (BkssConnCntx *)mmi_GetNextNode(&pConnNode->conn_node);
	}
}
uint8_t challeange[CHALLENAGE_LENGTH] = {0x11,0x22,0x33,0x44}; //为了方便这里填了一组确定的数组，使用过程中请自行生成随机数
kal_uint16 bkss_seq = 0;
kal_uint16 get_bkss_seq(void)
{
	bkss_seq++;
	return bkss_seq;
}
void reset_bkss_seq(void)
{
	bkss_seq = 0;
}
void pack_auth_request(uint8_t *MD5, int MD5_len , int32_t Ver , uint8_t *Dataout, uint16_t *Dataout_len ,uint8_t *MAC )  
{  
    AuthRequest request;  
    char timezone[] = {"Beijing"};  
    char language[] = {"Chinese"};  
    char device_name[] = {"NokeLock"};  
    int Pack_len,ICount;  
    uint8_t Pack_buf[100] = {0};  
    kal_uint16 seq = get_bkss_seq();
	
    //-----MD5  
    request.has_md5_device_type_and_device_id = false ;  
    request.md5_device_type_and_device_id.data = MD5 ;  
    request.md5_device_type_and_device_id.len = MD5_len ;  
    //-----版本  
    request.proto_version = PROTO_VERSION ;  
    //-----  
    request.auth_proto = 1 ;  
    //----- 验证及加密的版本号  
    //request.auth_method = 1 ;  
    request.auth_method = EAM_macNoEncrypt ;  
  
    request.has_aes_sign = false ;  
    request.aes_sign.data = 0 ;  
    request.aes_sign.len = 0 ;  
  
    //-----mac address  
    request.has_mac_address = true;  
    request.mac_address.data = MAC ;  
    request.mac_address.len = 6 ;  
    ///-----时区  
    request.has_time_zone = false ;  
    request.time_zone.str = timezone ;  
    request.time_zone.len = strlen(timezone) ;  
    //-----语言  
    request.has_language = false ;  
    request.language.str = language ;  
    request.language.len = strlen(language) ;  
    //-----产品名  
    request.has_device_name = true ;  
    request.device_name.str = device_name ;  
    request.device_name.len = strlen(device_name) ;  
    //-----计算长度及打包  
      
    Pack_len = epb_auth_request_pack_size(&request) ;  
    epb_pack_auth_request(&request, Pack_buf, Pack_len) ;  
  
    *Dataout_len = Pack_len + 8 ;  
    Dataout[0] = 0xfe;  
    Dataout[1] = 0x01;  
    Dataout[2] = (uint8_t)(*Dataout_len)>>8;  
    Dataout[3] = (uint8_t)(*Dataout_len);  
    Dataout[4] = 0x27;  
    Dataout[5] = 0x11;  
    Dataout[6] = seq/256;  
    Dataout[7] = seq%256;  
    for(ICount=0; ICount<Pack_len; ICount++)  
    {  
        Dataout[ICount+8] = Pack_buf[ICount] ;  
    }  
}  
void pack_init_request(uint8_t *r_data, uint16_t *r_len)  
{  
	BaseRequest basReq = {NULL};
	static uint8_t fix_head_len = sizeof(BpFixHead);
	InitRequest initReq = {NULL,false, {NULL, 0},true, {challeange, CHALLENAGE_LENGTH}};
	BpFixHead fix_head = {0xFE, 1, 0, 0, 0};
	unsigned short nCmdId = htons(ECI_req_auth);

	initReq.base_request = &basReq;
	fix_head.nCmdId = nCmdId;
	*r_len = epb_init_request_pack_size(&initReq) + fix_head_len;
#if defined EAM_md5AndAesEnrypt
	uint8_t length = *r_len;				
	uint8_t *p = get_ctrl_buffer(AES_get_length( *r_len-fix_head_len));
	if(!p)
	{
		return;
	}
	*r_len = AES_get_length( *r_len-fix_head_len)+fix_head_len;
#endif
	if(epb_pack_init_request(&initReq, r_data+fix_head_len, *r_len-fix_head_len)<0)
	{
		r_data = NULL;
		return;
	}
	//encrypt body
#if defined EAM_md5AndAesEnrypt
	AES_Init(session_key);
	AES_Encrypt_PKCS7(r_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度
	memcpy(r_data + fix_head_len, p, *r_len-fix_head_len);
	if(p)free_ctrl_buffer(p);
#endif
	fix_head.nCmdId = htons(ECI_req_init);
	fix_head.nLength = htons(*r_len);
	fix_head.nSeq = get_bkss_seq();
	memcpy(r_data, &fix_head, fix_head_len);
}  
int unpack_push_recv(uint8_t *data, uint16_t len)  
{
	BpFixHead *fix_head = (BpFixHead *)data;
	uint8_t fix_head_len = sizeof(BpFixHead);
	RecvDataPush *recvDatPush;
#if defined EAM_md5AndAesEnrypt
	uint32_t length = len- fix_head_len;//加密后数据长度
	uint8_t temp;
	uint8_t *p = get_ctrl_buffer (length);
	if(!p)
	{
		 if(data)
		 	free(data);
		 data =NULL; 
		 return 0;
	 }
	AES_Init(session_key);
	//解密数据
	AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);

	temp = p[length - 1];//算出填充长度
	len = len - temp;//取加密前数据总长度
	memcpy(data + fix_head_len, p ,length -temp);//把明文放回
	if(p){free_ctrl_buffer(p);p = NULL;}
#endif
	recvDatPush = epb_unpack_recv_data_push(data+fix_head_len, len-fix_head_len);
	if(!recvDatPush)
	{
		return errorCodeUnpackRecvDataPush;
	}
	memset(data,0,len);
	memcpy(data, recvDatPush->data.data ,recvDatPush->data.len);

	return recvDatPush->data.len;
}
void unpack_resp_send_data(uint8_t *data, uint16_t* len)  
{
	BpFixHead *fix_head = (BpFixHead *)data;
	uint8_t fix_head_len = sizeof(BpFixHead);
	SendDataResponse sendDataResp;
	int i;

#if defined EAM_md5AndAesEnrypt		
	uint32_t length = *len- fix_head_len;//加密后数据长度
	uint8_t *p = get_ctrl_buffer (length);
	if(!p)
	{
		printf("\r\nNot enough memory!"); 
		if(data)
			free(data);
		data = NULL;
		return 0;
	}
	AES_Init(session_key);
	//解密数据
	AES_Decrypt(p,data+fix_head_len,*len- fix_head_len,session_key);
	
	uint8_t temp;
	temp = p[length - 1];//算出填充长度
	*len = *len - temp;//取加密前数据总长度
	memcpy(data + fix_head_len, p ,length -temp);//把明文放回
	if(p)
	{
		free_ctrl_buffer(p);
		p = NULL;
	}
#endif	
	for(i = 0;i<*len-fix_head_len;i++)
	{
		bkss_dbg_print(MOD_ENG,"%s() %d %x",__func__,__LINE__,data[fix_head_len+i]);
	}
	epb_unpack_send_data_response(&sendDataResp,data+fix_head_len,*len-fix_head_len);
	wechat_data_len =sendDataResp.data.len;
	memcpy(wechat_data, sendDataResp.data.data ,wechat_data_len);
	if(sendDataResp.base_response->err_code)
	{
		epb_unpack_send_data_response_free(&sendDataResp);
		return;
	}
	epb_unpack_send_data_response_free(&sendDataResp);
}
void pack_send_data(uint8_t *in,kal_bool has_type)  
{  
	BaseRequest basReq = {NULL};
	static uint16_t bleDemoHeadLen = sizeof(BlueDemoHead);
	kal_uint16 in_len = (has_type?16:sizeof(in));
	BlueDemoHead  *bleDemoHead = (BlueDemoHead*)get_ctrl_buffer(bleDemoHeadLen+in_len);
	SendDataRequest sendDatReq = {NULL, {NULL, 0}, false, (EmDeviceDataType)NULL};
	static uint8_t fix_head_len = sizeof(BpFixHead);
	BpFixHead fix_head = {0xFE, 1, 0, 0, 0};
	unsigned short nCmdId = htons(ECI_req_auth);
	kal_uint16 seq = get_bkss_seq();
	uint8_t tmp[16] ={0};
	uint8_t i;
	
	if(!bleDemoHead)
	{
		return;
	}
	memset(rep_send_data,0,ATT_MAX_VALUE_LEN);
	rep_send_data_len = 0;
	if(has_type)
	{
		sendDatReq.has_type= true;
		sendDatReq.type = EDDT_wxDeviceHtmlChatView;
	}
	sendDatReq.data.len = (bleDemoHeadLen + in_len);
	sendDatReq.data.data = (uint8_t*)bleDemoHead;
	fix_head.nCmdId = nCmdId;
	sendDatReq.base_request = &basReq;
	bleDemoHead->m_magicCode[0] = MPBLEDEMO2_MAGICCODE_H;
	bleDemoHead->m_magicCode[1] = MPBLEDEMO2_MAGICCODE_L;
	bleDemoHead->m_version = htons( MPBLEDEMO2_VERSION);
	bleDemoHead->m_totalLength = htons(bleDemoHeadLen + in_len);
	bleDemoHead->m_cmdid = htons(sendTextReq);
	bleDemoHead->m_seq = htons(seq);
	bleDemoHead->m_errorCode = 0;	
	/*connect body and head.*/
	/*turn to uint8_t* befort offset.*/
	memcpy((uint8_t*)bleDemoHead+bleDemoHeadLen, in, in_len);			
	rep_send_data_len = epb_send_data_request_pack_size(&sendDatReq) + fix_head_len;
	bkss_dbg_print(MOD_ENG, "%s() %d %d",__func__,__LINE__,rep_send_data_len);
#if defined EAM_md5AndAesEnrypt
	uint16_t length = rep_send_data_len;
	uint8_t *p = get_ctrl_buffer(AES_get_length( rep_send_data_len-fix_head_len));
	if(!p){printf("\r\nNot enough memory!");return;}
	rep_send_data_len = AES_get_length( rep_send_data_len-fix_head_len)+fix_head_len;
#endif
	if(epb_pack_send_data_request(&sendDatReq, rep_send_data+fix_head_len, rep_send_data_len-fix_head_len)<0)
	{
	bkss_dbg_print(MOD_ENG, "%s() %d %d",__func__,__LINE__,rep_send_data_len);
	#if defined EAM_md5AndAesEnrypt
		if(p)
		{
			free_ctrl_buffer(p);
			p = NULL;
		}
	#endif
		return;
	}
#if defined EAM_md5AndAesEnrypt
	//encrypt body
	AES_Init(session_key);
	AES_Encrypt_PKCS7(rep_send_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度
	memcpy(rep_send_data + fix_head_len, p, rep_send_data_len-fix_head_len);
	if(p){free_ctrl_buffer(p); p = NULL;}
#endif
	for(i=0;i<rep_send_data_len;i++)
	{
		bkss_dbg_print(MOD_ENG, "%s() %d %x",__func__,__LINE__,rep_send_data[i]);
	}
	fix_head.nCmdId = htons(ECI_req_sendData);
	fix_head.nLength = htons(rep_send_data_len);
	fix_head.nSeq = seq;
	memcpy(rep_send_data, &fix_head, fix_head_len);
	if(bleDemoHead)
	{
		free_ctrl_buffer(bleDemoHead);
		bleDemoHead = NULL;
	}
	for(i=0;i<rep_send_data_len;i++)
	{
		bkss_dbg_print(MOD_ENG, "%s() %d %x",__func__,__LINE__,rep_send_data[i]);
	}
}  
uint8_t bkss_md5_type_and_id[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
int32_t bkss_get_md5(void)
{
	int32_t error_code = 0;
	#if defined EAM_md5AndNoEnrypt || EAM_md5AndAesEnrypt
	char device_type[] = DEVICE_TYPE;
	char device_id[] = DEVICE_ID;
	char argv[sizeof(DEVICE_TYPE) + sizeof(DEVICE_ID) - 1];
	memcpy(argv,device_type,sizeof(DEVICE_TYPE));
/*when add the DEVICE_ID to DEVICE_TYPE, the offset shuld -1 to overwrite '\0'  at the end of DEVICE_TYPE */
	memcpy(argv + sizeof(DEVICE_TYPE)-1,device_id,sizeof(DEVICE_ID));
	error_code = md5(argv, bkss_md5_type_and_id);
#endif
	return error_code;
}
void bkss_response_confirmation_callback(OS_STATUS status, ATT_HANDLE handle)
{
}
void bkss_send_request(kal_uint8* buf,kal_uint16 len,const kal_uint16  chruuid16)
{
	gatts_service_decl_struct *svc_list = (gatts_service_decl_struct *)mmi_GetHeadList(&g_bkss_cntx.srv_list.srvlist);
	BkssConnCntx    *pConnNode = (BkssConnCntx *)mmi_GetHeadList(&g_bkss_cntx.conn_list);
	BkssConnCntx    *pConnNext = NULL;
	gatt_conn_struct gatt_conn;
	ATT_VALUE *att_value = NULL;
	ATT_UUID         svc_uuid;
	ATT_UUID         chr_uuid;
	 ATT_UUID        desc_uuid;
	int type;
	U16  svc_uuid16;
	U16  chr_uuid16;
	U32 send_size = 0; 
	U8 i = 0; 
	if(!mmi_IsListEmpty(&g_bkss_cntx.conn_list))
	{
		while ((ListEntry *)pConnNode != &g_bkss_cntx.conn_list)
		{
			pConnNext = (BkssConnCntx *)mmi_GetNextNode(&pConnNode->conn_node);
			if((pConnNode->conn_status == BKSS_STATUS_CONNECTED) )
			{
				gatt_conn.reg_ctx = g_bkss_cntx.reg_ctx;
				gatt_conn.conn_ctx = pConnNode->conn_ctx;

				while((ListEntry *)svc_list != &g_bkss_cntx.srv_list.srvlist)
				{          

					if (svc_list->type == TYPE_CHARACTERISTIC)
					{

						type = bkss_find_uuid_by_handle(svc_list->handle, &svc_uuid, &chr_uuid,&desc_uuid);   
						if (type != TYPE_CHARACTERISTIC)
						{
							break;                          
						}

						svc_uuid16 = convert_array_to_uuid16(svc_uuid);
						chr_uuid16 = convert_array_to_uuid16(chr_uuid);
						bkss_dbg_print(MOD_ENG, "%s() %d %x",__func__,__LINE__, chr_uuid16);
						if(chr_uuid16 == chruuid16)
						{  

							U8 str[BKSS_MAX_VALUE_LEN] = {0};

							send_size = len;
							if(len > BKSS_MAX_VALUE_LEN)
							{
								len = BKSS_MAX_VALUE_LEN;
							}

							/*add for low power*/						
							att_value = (ATT_VALUE *)get_ctrl_buffer(sizeof(ATT_VALUE));
							memcpy(str, (U8*)buf,len);
							for(i=0;i<(len);i++)
							{
								bkss_dbg_print(MOD_ENG, "%s() %d %x",__func__,__LINE__, buf[i]);
							}
							for(i=0;i<(len/20);i++)
							{
								bkss_dbg_print(MOD_ENG, "%s() %d %x",__func__,__LINE__, i);
								memcpy(att_value->value, (U8*)str+20*i, 20);
								att_value->len = 20;
								if((i == (len/20) -1)&&(len%20 == 0))
									srv_gatts_send_indication(&gatt_conn, svc_list->handle,TRUE, att_value);
								else
									srv_gatts_send_indication(&gatt_conn, svc_list->handle,FALSE, att_value);
							}
							if(len%20 != 0)
							{
								memcpy(att_value->value, (U8*)str+(len - len%20), len%20);
								bkss_dbg_print(MOD_ENG, "%s() %d %x",__func__,__LINE__, len%20);
								att_value->len = len%20;
								srv_gatts_send_indication(&gatt_conn, svc_list->handle,TRUE, att_value);
							}
							free_ctrl_buffer(att_value);
							att_value = NULL;
							break;
						}
					}
					svc_list = (gatts_service_decl_struct *)mmi_GetNextNode(&svc_list->declnode);
				}

				return;
			}
		pConnNode = pConnNext;
		}

	}
}
void bkss_start_auth_request(void)
{
	U8 i = 0; 
	kal_uint8 buf[100]={0};
	kal_uint16 len;
	BD_ADDR bd_addr = {0};
	uint8_t mac_addr[BD_ADDR_SIZE] = {0};  
	S32 error;
	bkss_dbg_print(MOD_ENG, "%s() %d ",__func__,__LINE__);
	bkss_get_bd_addr(&bd_addr,&g_srv_bt_cm_cntx.host_dev.le_bd_addr);  //le_bd_addr
	bkss_get_md5();
	for(i=0;i<BD_ADDR_SIZE;i++)
	{
		mac_addr[i] = bd_addr.addr[BD_ADDR_SIZE - 1 - i];
	}
	pack_auth_request(bkss_md5_type_and_id,strlen(bkss_md5_type_and_id),1,buf,&len,mac_addr);
	bkss_send_request(buf,len,BKSS_WECHAT_READ_INDI_UUID);
}
void bkss_start_init_request(void)
{
	kal_uint8 buf[100]={0};
	kal_uint16 len;
	pack_init_request(buf,&len);
	bkss_send_request(buf,len,BKSS_WECHAT_READ_INDI_UUID);
}

void bkss_start_send_data(void)
{
	pack_send_data(SEND_HELLO_WECHAT,false);
	bkss_send_request(rep_send_data,rep_send_data_len,BKSS_WECHAT_READ_INDI_UUID);
	memset(rep_send_data,0,ATT_MAX_VALUE_LEN);
	rep_send_data_len = 0;
}

void bkss_register_server_callback(void *reg_ctx, OS_STATUS status, bt_uuid_struct *app_uuid)
{
	if (memcmp(app_uuid, &g_bkss_cntx.uid, sizeof(bt_uuid_struct)) == 0)
	{
		if (g_bkss_cntx.state == BKSS_STATUS_ENABLING)
		{	
			if (status == OS_STATUS_SUCCESS)
			{
				g_bkss_cntx.reg_ctx = reg_ctx;
				if (g_bkss_cntx.op_flag == BKSS_OP_INIT)
				{
					g_bkss_cntx.op_flag = BKSS_OP_ADD_SERVICE;
					#ifndef BKSS_UT
					srv_gatts_profile_factory_get_services(g_bkss_cntx.reg_ctx, PROFILE_ID_BIKESW, &g_bkss_cntx.srv_list);
					srv_gatts_profile_factory_add_services(g_bkss_cntx.reg_ctx, &g_bkss_cntx.srv_list);
					#endif
					
					bkss_set_adv_data();
				}
			}
			else
			{
				g_bkss_cntx.reg_ctx = NULL;
				g_bkss_cntx.callback = NULL;
				g_bkss_cntx.op_flag = BKSS_OP_DEINIT;
				g_bkss_cntx.state = BKSS_STATUS_DISABLED;
			}
		}
		else if (g_bkss_cntx.state == BKSS_STATUS_DISABLING)
		{	
			if (g_bkss_cntx.op_flag == BKSS_OP_INIT)
			{
				g_bkss_cntx.state = BKSS_STATUS_ENABLING;
				#ifndef BKSS_UT
				srv_gatts_profile_factory_register(&g_bkss_cntx.uid, &g_bkss_cb, &g_bkss_factory_cb);
				#endif
			}
			else
			{
				g_bkss_cntx.reg_ctx = NULL;
				g_bkss_cntx.callback = NULL;
				g_bkss_cntx.op_flag = BKSS_OP_DEINIT;
				g_bkss_cntx.state = BKSS_STATUS_DISABLED;
			}
		}
	}
}

void bkss_connection_callback(gatt_conn_struct *conn, BOOL connected, BD_ADDR *bd_addr)
{
	BkssConnCntx    *pConnNode = (BkssConnCntx *)mmi_GetHeadList(&g_bkss_cntx.conn_list);
	BkssConnCntx    *pConnNext = NULL;

	//if (!mmi_IsListEmpty(&g_bkss_cntx.conn_list))
	//{
		while ((ListEntry *)pConnNode != &g_bkss_cntx.conn_list)
		{
			pConnNext = (BkssConnCntx *)mmi_GetNextNode(&pConnNode->conn_node);
			if (memcmp(bd_addr->addr, pConnNode->bdaddr, BD_ADDR_SIZE) == 0)
			{
				bkss_dbg_print(MOD_ENG,"%s() %d connected=%d,conn_status=%d",__func__,__LINE__,connected,pConnNode->conn_status);
				pConnNode->conn_ctx = conn->conn_ctx;
				if (connected && (pConnNode->conn_status != BKSS_STATUS_CONNECTED))
				{
					//do next step Discovery all
					#ifndef BKSS_UT
					//if (0<srv_ble_cm_get_connected_dev_num())
	                {
						srv_gatts_listen(conn->reg_ctx, FALSE);
						//srv_gatts_disconnect(conn->reg_ctx, bd_addr);
	                }
	                //srv_gatts_listen(conn->reg_ctx, FALSE);
					//srv_gatts_connect(conn->reg_ctx, bd_addr, FALSE);
					#endif
					bkss_dbg_print(MOD_ENG,"%s() %d connected = %d,need_rsp=%d",__func__,__LINE__);
					pConnNode->conn_status = BKSS_STATUS_CONNECTED;
					if (g_bkss_cntx.callback != NULL)
						g_bkss_cntx.callback->connection_state_cb(bd_addr->addr, BKSS_STATUS_CONNECTED);
				}
				else if (!connected)
				{
					#ifndef BKSS_UT
					srv_gatts_listen(conn->reg_ctx, TRUE);
					#endif
					mmi_RemoveEntryList(&pConnNode->conn_node);
					free_ctrl_buffer(pConnNode);
					if (g_bkss_cntx.callback != NULL)
						g_bkss_cntx.callback->connection_state_cb(bd_addr->addr, BKSS_STATUS_DISCONNECTED);
					bkss_set_adv_data();
					reset_bkss_seq();
					bkss_dbg_print(MOD_ENG,"%s() %d connected = %d,need_rsp=%d",__func__,__LINE__);
					wechat_operate = KAL_FALSE;
				}
				return;
			}
			pConnNode = pConnNext;
		}
	//}
	//else
	//{
		if (connected)
		{
			pConnNode = (BkssConnCntx *)get_ctrl_buffer(sizeof(BkssConnCntx));
			memcpy(pConnNode->bdaddr, bd_addr->addr, BD_ADDR_SIZE);
			pConnNode->conn_ctx = conn->conn_ctx;
			#ifndef BKSS_UT
	
			srv_gatts_listen(conn->reg_ctx, FALSE);
			//srv_gatts_connect(conn->reg_ctx, bd_addr, FALSE);
			#endif
			bkss_dbg_print(MOD_ENG,"%s() %d connected = %d,need_rsp=%d",__func__,__LINE__);
			pConnNode->conn_status = BKSS_STATUS_CONNECTED;
			mmi_InsertTailList(&g_bkss_cntx.conn_list, &pConnNode->conn_node);
			if (g_bkss_cntx.callback != NULL)
				g_bkss_cntx.callback->connection_state_cb(bd_addr->addr, BKSS_STATUS_CONNECTED);
			bkss_reset_adv_data();
		}
	//}
}
extern nvram_ef_bel_bkss_struct bel_bkss;
extern kal_uint8 aes128_key[BT_KEY_MAX_LEN + 1];
U32 bikesw_send_data(void* buf, U32 len)
{
#ifdef MMI_BLE_IAS_ALERT_STATUS_CHAR
	gatts_service_decl_struct *svc_list = (gatts_service_decl_struct *)mmi_GetHeadList(&g_bkss_cntx.srv_list.srvlist);
	BkssConnCntx        *pConnNode = (BkssConnCntx *)mmi_GetHeadList(&g_bkss_cntx.conn_list);
	BkssConnCntx        *pConnNext = NULL;
	gatt_conn_struct    gatt_conn;
	ATT_VALUE           *att_value = NULL;
	ATT_UUID            svc_uuid;
	ATT_UUID            chr_uuid;
	ATT_UUID         desc_uuid;
	int                 type = 0;
	U16                 svc_uuid16 = 0;
	U16                 chr_uuid16 = 0;
	U32                 send_size = 0; 
	U8 buffer[16];
	U8* temp = buf;
	U8 i;

	bkss_dbg_print(MOD_ENG, "[BKSS] bkss_send_data ==> \n");

	if(buf == NULL)
	{
		return send_size;
	}
	memset(buffer,0,sizeof(buffer));
	bkss_dbg_print(MOD_ENG, "%s() %d temp =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__
		,temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6],temp[7],temp[8],temp[9],temp[10],temp[11],temp[12],temp[13],temp[14],temp[15]);
	bkss_dbg_print(MOD_ENG, "%s() %d aes128_key =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__
		,aes128_key[0],aes128_key[1],aes128_key[2],aes128_key[3],aes128_key[4],aes128_key[5],aes128_key[6],aes128_key[7],aes128_key[8],aes128_key[9],aes128_key[10],aes128_key[11],aes128_key[12],aes128_key[13],aes128_key[14],aes128_key[15]);
	AES128_ECB_encrypt(buf,aes128_key,buffer);
	bkss_dbg_print(MOD_ENG, "%s() %d buffer =%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",__func__,__LINE__
		,buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15]);
	if(wechat_operate)
	{
		pack_send_data((uint8_t *)buffer,true);
		bkss_send_request(rep_send_data,rep_send_data_len,BKSS_WECHAT_READ_INDI_UUID);
		memset(rep_send_data,0,ATT_MAX_VALUE_LEN);
		rep_send_data_len = 0;
		return 0;
	}

       
    if(!mmi_IsListEmpty(&g_bkss_cntx.conn_list))
    {
        while ((ListEntry *)pConnNode != &g_bkss_cntx.conn_list)
        {
            pConnNext = (BkssConnCntx *)mmi_GetNextNode(&pConnNode->conn_node);
            
            if(pConnNode->conn_status == BKSS_STATUS_CONNECTED) //CONNECTED
            {
                gatt_conn.reg_ctx = g_bkss_cntx.reg_ctx;
                gatt_conn.conn_ctx = pConnNode->conn_ctx;

                while((ListEntry *)svc_list != &g_bkss_cntx.srv_list.srvlist)
                {          
                    if(svc_list->type == TYPE_CHARACTERISTIC)
                    {
                        type = bkss_find_uuid_by_handle(svc_list->handle, &svc_uuid, &chr_uuid,&desc_uuid);   
                        if(type != TYPE_CHARACTERISTIC)
                        {
                            break;                          
                        }
                         
                        svc_uuid16 = convert_array_to_uuid16(svc_uuid);
                        chr_uuid16 = convert_array_to_uuid16(chr_uuid);
                        
                        bkss_dbg_print(MOD_ENG, "[BKSS] bkss_send_data, svc_uuid16:0x%x, chr_uuid16:0x%x \n", svc_uuid16, chr_uuid16);
                        if (chr_uuid16 == BKSS_READ_UUID)
                        {  
                            send_size = len;
                            if(len > BKSS_MAX_VALUE_LEN)
                            {
                                len = BKSS_MAX_VALUE_LEN;
                            }
                             
                            att_value = (ATT_VALUE *)get_ctrl_buffer(sizeof(ATT_VALUE));
                            memcpy(att_value->value, buffer, len);
                            att_value->len = len;
                            srv_gatts_send_indication(&gatt_conn, svc_list->handle, FALSE, att_value);
                            free_ctrl_buffer(att_value);
                            att_value = NULL;
                            break;
                        }
                    }
                    
                    svc_list = (gatts_service_decl_struct *)mmi_GetNextNode(&svc_list->declnode);
                }
            }
            
            pConnNode = pConnNext;
        }
    }
    
    bkss_dbg_print(MOD_ENG, "[BKSS] bkss_send_data <==, send_size:%d \n", send_size);
    return  send_size;
#else //MMI_BLE_IAS_ALERT_STATUS_CHAR
    return  0;
#endif //MMI_BLE_IAS_ALERT_STATUS_CHAR
}



void bkss_request_write_callback(gatt_conn_struct *conn, U16 trans_id, BD_ADDR *bd_addr,
                                     ATT_HANDLE attr_handle, ATT_VALUE *value, U16 offset,
                                     BOOL need_rsp, BOOL is_prep)
{
	BkssConnCntx    *pConnNode = (BkssConnCntx *)mmi_GetHeadList(&g_bkss_cntx.conn_list);
	ATT_UUID         svc_uuid;
	ATT_UUID         chr_uuid;
	ATT_UUID         desc_uuid;
	int              type = bkss_find_uuid_by_handle(attr_handle, &svc_uuid, &chr_uuid,&desc_uuid);
	U16              svc_uuid16 = convert_array_to_uuid16(svc_uuid);
	U16              chr_uuid16 = convert_array_to_uuid16(chr_uuid);
	U16              desc_uuid16 = convert_array_to_uuid16(desc_uuid);
    S32 ret_val = 0;
	ATT_VALUE        noti_value;
	int              notify_status = 0;  //0, no need , 1 notify, 2 inditcaion
	int i = 0;
	

	if (mmi_IsListEmpty(&g_bkss_cntx.conn_list))
	{
		return;
	}

	while ((ListEntry *)pConnNode != &g_bkss_cntx.conn_list)
	{
		if (memcmp(bd_addr->addr, pConnNode->bdaddr, BD_ADDR_SIZE) == 0)
		{
			if (svc_uuid16 == BKSS_SERVICE && chr_uuid16 == BKSS_WRITE_UUID)
			{
				if (g_bkss_cntx.callback)
				{                   
					ret_val = g_bkss_cntx.callback->data_to_read_notify_cb(bd_addr->addr, &(value->value[offset]), value->len);
					if (ret_val !=0)
					{
					    // error happen, should send error response
					    srv_gatts_send_response(conn, trans_id, OS_STATUS_FAILED, attr_handle, value); 
					}
					else
					{
					    // success, should send success response
					    srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, value); 
					}
					switch(notify_status)
					{
						case 2:
							srv_gatts_send_indication(conn, attr_handle, TRUE, &noti_value);
							break;
						default:
							break;
					}

				}
			}
																			
			else if ((svc_uuid16 == BKSS_SERVICE) && (chr_uuid16 == BKSS_WECHAT_READ_INDI_UUID)
				&& (desc_uuid16 == BKSS_CHAR_CONFIG_UUID))
			{
				bkss_dbg_print(MOD_ENG,"%s() %d notify_status = %d,need_rsp=%d",__func__,__LINE__,notify_status,need_rsp);
			    memset(&noti_value, 0, sizeof(ATT_VALUE));
				if ((value->value[0] == 1) || (value->value[0] == 3))
				{
					if (value->len >= BKSS_MAX_VALUE_LEN)
					{
						memcpy(pConnNode->dec.dec_value.value, value->value, BKSS_MAX_VALUE_LEN);
						pConnNode->dec.dec_value.len = BKSS_MAX_VALUE_LEN;
					}
					else
					{
						memcpy(pConnNode->dec.dec_value.value, value->value, sizeof(value->value));
						pConnNode->dec.dec_value.len = value->len;
					}
					pConnNode->dec.is_dec_write = TRUE;
					pConnNode->dec.handle = attr_handle;
					notify_status = 1;
				}

				if (need_rsp)
				{
					srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &noti_value);
				}
				switch(notify_status)
				{
					case 2:
						srv_gatts_send_indication(conn, attr_handle, TRUE, &noti_value);
						break;
					case 1:
					//	mmi_frm_cb_reg_event(EVT_ID_SRV_CHARBAT_NOTIFY, bas_battery_change_handler, NULL);
					//	StartTimer(BAS_SEND_NOTIFICATION_TIMER,BAS_SEND_NOTIFICATION_TIME,(FuncPtr)bas_send_notification);
						break;
					case 0:
					default:
						break;
				}
				bkss_start_auth_request();
			}
			else if ((svc_uuid16 == BKSS_SERVICE) && (chr_uuid16 == BKSS_WECHAT_WRITE_UUID))
			{
				kal_bool send_indicate = KAL_FALSE;
				memset(&noti_value, 0, sizeof(ATT_VALUE));
				if ((value->value[0] == 0xfe)&&(value->value[1] == 0x01))
				{
				bkss_dbg_print(MOD_ENG,"%s() %d notify_status = %d,need_rsp=%d",__func__,__LINE__,notify_status,need_rsp);
					resp_send_data_len = value->len;
					memset(resp_send_data, 0x00, ATT_MAX_VALUE_LEN);
					memcpy(resp_send_data,value->value,resp_send_data_len);
					if(resp_send_data_len > ATT_MAX_VALUE_LEN)
					{
						bkss_dbg_print(MOD_ENG,"%s() %d send_indicate = %d,resp_send_data[5]=%x",__func__,__LINE__,send_indicate,resp_send_data[5]);
						memset(resp_send_data, 0x00, ATT_MAX_VALUE_LEN);
						resp_send_data_len = 0;
						srv_gatts_send_response(conn, trans_id, OS_STATUS_FAILED, attr_handle, &noti_value);
					}
					else if(resp_send_data_len==resp_send_data[3])
					{
						send_indicate = KAL_TRUE;
					}
					else
						srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &noti_value);
				}
				else if((resp_send_data_len +value->len )==(resp_send_data[2]*256 + resp_send_data[3]))
				{
				bkss_dbg_print(MOD_ENG,"%s() %d notify_status = %d,need_rsp=%d",__func__,__LINE__,notify_status,need_rsp);
					memcpy(resp_send_data+resp_send_data_len,value->value,value->len);
					resp_send_data_len += value->len;
					send_indicate = KAL_TRUE;
				}
				else if(resp_send_data_len != 0)
				{
				bkss_dbg_print(MOD_ENG,"%s() %d notify_status = %d,need_rsp=%d",__func__,__LINE__,notify_status,need_rsp);
					memcpy(resp_send_data+resp_send_data_len,value->value,value->len);
					resp_send_data_len += value->len;
					if (need_rsp)
					{
						srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &noti_value);
					}
				}
				else
				{
					bkss_dbg_print(MOD_ENG,"%s() %d send_indicate = %d,resp_send_data[5]=%x",__func__,__LINE__,send_indicate,resp_send_data[5]);
					memset(resp_send_data, 0x00, ATT_MAX_VALUE_LEN);
					resp_send_data_len = 0;
					srv_gatts_send_response(conn, trans_id, OS_STATUS_FAILED, attr_handle, &noti_value);
				}
				for(i = 0;i<value->len;i++)
				{
					bkss_dbg_print(MOD_ENG,"%s() %d %x",__func__,__LINE__,value->value[i]);
				}
				bkss_dbg_print(MOD_ENG,"%s() %d send_indicate = %d,resp_send_data[5]=%x",__func__,__LINE__,send_indicate,resp_send_data[5]);
				if(send_indicate)
				{
					if ((resp_send_data[4] == 0x4e)&&(resp_send_data[5] == 0x21))
					{
						if (need_rsp)
						{
							srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &noti_value);
						}
						wechat_operate = KAL_TRUE;
						if (resp_send_data_len == 14)
							bkss_start_init_request();
					}
					else if(((resp_send_data[4] == 0x4e)&&(resp_send_data[5] == 0x22))
						||((resp_send_data[4] == 0x75)&&((resp_send_data[5] == 0x31)||(resp_send_data[5] == 0x32))))
					{
						srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, value); 
						memset(wechat_data,0,ATT_MAX_VALUE_LEN);
						wechat_data_len = 0;
						unpack_resp_send_data(resp_send_data,&resp_send_data_len);
						bkss_dbg_print(MOD_ENG,"%s() %d %s %d",__func__,__LINE__,wechat_data,wechat_data_len);
						ret_val = g_bkss_cntx.callback->wechatdata_to_read_notify_cb(bd_addr->addr, wechat_data, wechat_data_len);
					}
					else if((resp_send_data[4] == 0x4e)&&(resp_send_data[5] == 0x23))
					{
						if (need_rsp)
						{
							srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &noti_value);
						}
						bkss_start_send_data();
					}
					else
					{
						if (need_rsp)
						{
							srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, &noti_value);
						}
						switch(notify_status)
						{
							case 2:
								srv_gatts_send_indication(conn, attr_handle, TRUE, &noti_value);
								break;
							default:
								break;
						}
					}
					memset(resp_send_data, 0x00, ATT_MAX_VALUE_LEN);
					resp_send_data_len = 0;
				}

			}
			else
			{
			    // fail, not support read
			    value->len = 1;
	            value->value[offset] = GATT_ERROR_REQUEST_NOT_SUPPORT;
				bkss_dbg_print(MOD_ENG, "[BKSS]write callback:value.len %d, value->value[offset]:%d!", 
			        value->len, value->value[offset]);			
			  
			    if (need_rsp)
				{
					srv_gatts_send_response(conn, trans_id, OS_STATUS_SUCCESS, attr_handle, value); 
				}
			}
			return;
		}
		pConnNode = (BkssConnCntx *)mmi_GetNextNode(&pConnNode->conn_node);
	}
}

void bkss_service_started_callback(OS_STATUS status, void *reg_ctx,
                                         ATT_HANDLE srvc_handle)
{
	if (status == OS_STATUS_SUCCESS)
	{
		srv_gatts_listen(reg_ctx, TRUE);
	}
}


void bkss_add_services_callback(OS_STATUS status, void *reg_ctx)
{
	gatts_service_decl_struct *svc_list = (gatts_service_decl_struct *)mmi_GetHeadList(&g_bkss_cntx.srv_list.srvlist);
	if (g_bkss_cntx.reg_ctx == reg_ctx)
	{
		if (g_bkss_cntx.state == BKSS_STATUS_ENABLING)
		{	
			if (status == OS_STATUS_SUCCESS)
			{
				if (g_bkss_cntx.op_flag == BKSS_OP_ADD_SERVICE)
				{
					if (mmi_IsListEmpty(&g_bkss_cntx.srv_list.srvlist))
					{
						return;
					}
					while ((ListEntry *)svc_list != &g_bkss_cntx.srv_list.srvlist)
					{
						if (svc_list->type == TYPE_SERVICE)
						{
							#ifndef BKSS_UT
							srv_gatts_start_service(reg_ctx, svc_list->handle, GATT_TRANSPORT_LE);
							#endif
						}
						svc_list = (gatts_service_decl_struct *)mmi_GetNextNode(&svc_list->declnode);
					}
					g_bkss_cntx.state = BKSS_STATUS_ENABLED;
				}
			}
			else
			{
				bkss_deinit();
			}
		}	
		
	}
	//bkss_dbg_print(MOD_ENG, "[BKSS] bkss_add_services_callback -!\n");
}


S32 bkss_init(bkss_callback_t *cb)
{
	BtStatus status = BT_STATUS_SUCCESS;
	mmi_frm_cb_reg_event(SRV_LE_CM_EVENT_DISCONNECT_REQ, bkss_recv_event_handler, NULL);
	if (g_bkss_cntx.state == BKSS_STATUS_DISABLED)
	{
		g_bkss_cntx.state = BKSS_STATUS_ENABLING;
		g_bkss_cntx.op_flag = BKSS_OP_INIT;
		g_bkss_cntx.callback = cb;
		memcpy(g_bkss_cntx.uid.uu, &g_bkss_uuid, sizeof(bt_uuid_struct));
		mmi_InitializeListHead(&g_bkss_cntx.conn_list);
		#ifndef BKSS_UT
		srv_gatts_profile_factory_register(&g_bkss_cntx.uid, &g_bkss_cb, &g_bkss_factory_cb);
		#endif
	}
	else if (g_bkss_cntx.state == BKSS_STATUS_DISABLING)
	{
		if(g_bkss_cntx.op_flag == BKSS_OP_DEINIT)
		{
			g_bkss_cntx.op_flag = BKSS_OP_INIT;
		}
		g_bkss_cntx.callback = cb;
		memcpy(g_bkss_cntx.uid.uu, &g_bkss_uuid, sizeof(bt_uuid_struct));
	}
	//bkss_dbg_print(MOD_ENG, "[BKSS] bkss_init -!\n");
	return status;
}

S32 bkss_deinit()
{
	gatt_conn_struct gatt_conn;
	BD_ADDR          bd_addr;
	BkssConnCntx    *pConnCur = (BkssConnCntx *)mmi_GetHeadList(&g_bkss_cntx.conn_list);
	BkssConnCntx    *pConnNext = NULL;
	if ((g_bkss_cntx.state == BKSS_STATUS_DISABLED)
		|| (g_bkss_cntx.state == BKSS_STATUS_DISABLING))
		return BT_STATUS_SUCCESS;
	#ifndef BKSS_UT
	//srv_gatts_listen(g_bkss_cntx.reg_ctx, FALSE);
	#endif
	if (g_bkss_cntx.state == BKSS_STATUS_ENABLED)
	{
		if (mmi_IsListEmpty(&g_bkss_cntx.conn_list))
		{
			goto do_deinit;
		}
		
        while ((ListEntry *)pConnCur != &g_bkss_cntx.conn_list)
		{
			pConnNext = (BkssConnCntx *)mmi_GetNextNode(&pConnCur->conn_node);
			gatt_conn.reg_ctx = g_bkss_cntx.reg_ctx;
			gatt_conn.conn_ctx = pConnCur->conn_ctx;
			if((pConnCur->conn_status == BKSS_STATUS_CONNECTED) 
				|| (pConnCur->conn_status == BKSS_STATUS_CONNECTING))
			{
				memcpy(bd_addr.addr, pConnCur->bdaddr, BD_ADDR_SIZE);
				#ifndef BKSS_UT
				//srv_gatts_disconnect(&gatt_conn, &bd_addr);
				#endif

                if(g_bkss_cntx.callback != NULL)
					g_bkss_cntx.callback->connection_state_cb(bd_addr.addr, BKSS_STATUS_DISCONNECTED);
			}
			mmi_RemoveEntryList(&pConnCur->conn_node);
			free_ctrl_buffer(pConnCur);
			pConnCur = pConnNext;
		}
	}
	
do_deinit:
	#ifndef BKSS_UT
	srv_gatts_profile_factory_delete_services(g_bkss_cntx.reg_ctx, &g_bkss_cntx.srv_list);
	srv_gatts_profile_factory_clear_services(g_bkss_cntx.reg_ctx, &g_bkss_cntx.srv_list);
	srv_gatts_profile_factory_deregister(g_bkss_cntx.reg_ctx);
	#endif
	mmi_frm_cb_dereg_event(SRV_LE_CM_EVENT_DISCONNECT_REQ, bkss_recv_event_handler, NULL);
	g_bkss_cntx.state = BKSS_STATUS_DISABLED;
	g_bkss_cntx.op_flag = BKSS_OP_DEINIT;
	//bkss_dbg_print(MOD_ENG, "[BKSS] bkss_deinit -!\n");
	return BT_STATUS_SUCCESS;
}

S32 bkss_send_data(char* bdaddr,void* buf, U32 len, S32 *err_code)
{
    gatts_service_decl_struct *svc_list = (gatts_service_decl_struct *)mmi_GetHeadList(&g_bkss_cntx.srv_list.srvlist);
    BkssConnCntx    *pConnNode = (BkssConnCntx *)mmi_GetHeadList(&g_bkss_cntx.conn_list);
    BkssConnCntx    *pConnNext = NULL;
    gatt_conn_struct gatt_conn;
    ATT_VALUE *att_value = NULL;
    ATT_UUID         svc_uuid;
    ATT_UUID         chr_uuid;
	ATT_UUID         desc_uuid;
    int type;
    U16  svc_uuid16;
    U16  chr_uuid16;
    U32 send_size = 0; 
    
    //bkss_dbg_print(MOD_ENG, "[BKSS] bkss_send_data buf:%x, size:%d !\n", buf, len);

    if (buf == NULL)
    {
        *err_code = -1;
        return 0;
    }
       
    if (!mmi_IsListEmpty(&g_bkss_cntx.conn_list))
    {
        while ((ListEntry *)pConnNode != &g_bkss_cntx.conn_list)
        {
            pConnNext = (BkssConnCntx *)mmi_GetNextNode(&pConnNode->conn_node);
            if (pConnNode->conn_status == BKSS_STATUS_CONNECTED )
            {
                //bkss_dbg_print(MOD_ENG, "[BKSS] find in list!\n");
                gatt_conn.reg_ctx = g_bkss_cntx.reg_ctx;
                gatt_conn.conn_ctx = pConnNode->conn_ctx;

                while ((ListEntry *)svc_list != &g_bkss_cntx.srv_list.srvlist)
                {          

                    //bkss_dbg_print(MOD_ENG, "[BKSS] find in list!\n");
                    if (svc_list->type == TYPE_CHARACTERISTIC)
                    {
                         
                        type = bkss_find_uuid_by_handle(svc_list->handle, &svc_uuid, &chr_uuid,&desc_uuid);   
                        if (type != TYPE_CHARACTERISTIC)
                        {
                            *err_code = -3;
                            break;                          
                        }
                         
                        svc_uuid16 = convert_array_to_uuid16(svc_uuid);
                        chr_uuid16 = convert_array_to_uuid16(chr_uuid);
                        if (chr_uuid16 == BKSS_WRITE_UUID)
                        {  
                            send_size = len;
                            if (len > BKSS_MAX_VALUE_LEN)
                            {
                                len = BKSS_MAX_VALUE_LEN;
                            }
                             
                            att_value = (ATT_VALUE *)get_ctrl_buffer(sizeof(ATT_VALUE));
                            memcpy(att_value->value, buf, len);
                            att_value->len = len;
                            srv_gatts_send_indication(&gatt_conn, svc_list->handle,FALSE, att_value);
                            free_ctrl_buffer(att_value);
                            att_value = NULL;
                            *err_code = 0;
                            break;
                        }
                    }
                    svc_list = (gatts_service_decl_struct *)mmi_GetNextNode(&svc_list->declnode);
                }
                
                return send_size;
            }
            pConnNode = pConnNext;
        }

        *err_code = -4;
        return 0;
    }
    else
    {
        // no connecton 
        *err_code = -5;
        return  0;
    }
}            
BtStatus bkss_disconnect_event_handler(BD_ADDR *bdaddr)
{
	gatt_conn_struct gatt_conn;
	BD_ADDR          bd_addr;
	BkssConnCntx    *pConnCur = (BkssConnCntx *)mmi_GetHeadList(&g_bkss_cntx.conn_list);

	if ((g_bkss_cntx.state == BKSS_STATUS_DISABLED)
		|| (g_bkss_cntx.state == BKSS_STATUS_DISABLING))
		return BT_STATUS_FAILED;

    while ((ListEntry *)pConnCur != &g_bkss_cntx.conn_list)
	{
	    if (memcmp(bdaddr->addr, pConnCur->bdaddr, BD_ADDR_SIZE) == 0)
	    {
			gatt_conn.reg_ctx = g_bkss_cntx.reg_ctx;
			gatt_conn.conn_ctx = pConnCur->conn_ctx;
			 if ((pConnCur->conn_status == BKSS_STATUS_DISCONNECTED) 
				|| (pConnCur->conn_status == BKSS_STATUS_DISCONNECTING))
			{
				if(g_bkss_cntx.callback != NULL)
					g_bkss_cntx.callback->connection_state_cb(pConnCur->bdaddr, BKSS_STATUS_DISCONNECTED);
				return BT_STATUS_SUCCESS;
			}			
			break;	
		}
		pConnCur = (BkssConnCntx *)mmi_GetNextNode(&pConnCur->conn_node);
	}
	return BT_STATUS_FAILED;
}

void bkss_get_bd_addr(BD_ADDR *dst,const srv_bt_cm_bt_addr *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memcpy((U8 *)&dst->addr[0], (U8 *)&src->lap, 3);
    dst->addr[3] = src->uap;
    memcpy((U8 *)&dst->addr[4], (U8 *)&src->nap, 2);

}

mmi_ret bkss_recv_event_handler(mmi_event_struct *evt)
{
    srv_le_cm_evt_disconnect_struct *disconn_data = (srv_le_cm_evt_disconnect_struct *)evt;
	bkss_dbg_print(MOD_ENG, "[BKSS] bkss_recv_event_handler evt->evt_id %d!\n",evt->evt_id);
	    
    if ( evt->evt_id == SRV_LE_CM_EVENT_DISCONNECT_REQ ) 
    {
        BD_ADDR remote_addr;
		bkss_get_bd_addr(&remote_addr,disconn_data->dev_addr);
		bkss_disconnect_event_handler(&remote_addr);
		bkss_dbg_print(MOD_ENG, "[BKSS] bkss_recv_event_handler remote_addr %s!\n",remote_addr.addr);
    }
    return MMI_RET_OK;
}
#endif  /* __IOT_BLE_BKSS_SUPPORT__ */
