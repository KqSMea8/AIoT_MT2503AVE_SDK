#include "adups_typedef.h"
#include "adups_debug.h"
#include "MMIDataType.h"
#include "MMI_include.h"
#include <soc_api.h>
#include "cbm_api.h"
#include "adups_net.h"

#include "nbr_public_struct.h"


#ifdef __ADUPS_FOTA_SOC__
static adups_int8 g_tcp_socket_id = -1;
ADUPS_SOCKET_MGR_STRUCT adups_socket_mgr={NULL,NULL,NULL,NULL,0,0,-1};

extern void adups_get_new_version(void);
extern void adups_upgrade_thread_on(void);
extern ADUPS_BOOL  adups_fota_running(void);
extern void adups_register(void);


adups_uint32 adups_encode_dataaccount_id(void)
{

	static adups_uint32 account_id=0;
	#if 0
	cbm_app_info_struct app_info;
	static adups_uint8 app_id=0;
	memset(&app_info, 0, sizeof(app_info));
	app_info.app_type = DTCNT_APPTYPE_BRW_HTTP | DTCNT_APPTYPE_SKIP_WIFI;
	cbm_register_app_id_with_app_info(&app_info, &app_id);
	account_id = cbm_encode_data_account_id(CBM_DEFAULT_ACCT_ID, CBM_SIM_ID_SIM1, app_id, KAL_FALSE);
	#endif
	account_id = accid_get_by_iot();
	return account_id;
}

adups_extern void adups_save_download_ip(adups_uint8 addr1,adups_uint8 addr2,adups_uint8 addr3,adups_uint8 addr4);
ADUPS_BOOL adups_GetHostByName_Notify(void* inMsg)
{
	adups_int32 i;
    	app_soc_get_host_by_name_ind_struct* dns_ind = (app_soc_get_host_by_name_ind_struct*)inMsg;
	adups_uint32 addr_buf;
	adups_uint8 addr_len;
	adups_uint16 port;
	adups_int32 result;

	adups_DebugPrint_Ext("[ADUPS_DNS]:","adups_GetHostByName_Notify dns_ind->result=%d", dns_ind->result);
	adups_DebugPrint_Ext("[ADUPS_DNS]:","adups_GetHostByName_Notify dns_ind->addr_len=%d", dns_ind->addr_len);
	
	for(i=0;i<dns_ind->addr_len;i++){
	adups_DebugPrint_Ext("[ADUPS_DNS]:", "adups_GetHostByName_Notify dns_ind->addr=%d", dns_ind->addr[i]);
	}

	if(dns_ind->addr_len==4)
		adups_save_download_ip(dns_ind->addr[0],dns_ind->addr[1],dns_ind->addr[2],dns_ind->addr[3]);

     if(dns_ind)
     {
         adups_DebugPrint("adups_GetHostByName_Notify dns_ind->request_id=%d, g_request_id=%d", dns_ind->request_id, adups_socket_mgr.adups_request_id);
		 
         if(dns_ind->request_id != adups_socket_mgr.adups_request_id)
         {
             return ADUPS_FALSE;
         }
     }
	
    if (dns_ind && dns_ind->result != FALSE)
    {
    	adups_net_set_state(NET_STAT_CONN_REQ);
        memcpy(&addr_buf, dns_ind->addr, dns_ind->addr_len);
        addr_len = dns_ind->addr_len;
        port = 80;
	result=adups_connect(dns_ind->request_id,addr_buf,addr_len,port);

	if(result!=E_ADUPS_SUCCESS)
	{
	adups_DebugPrint_Ext("[ADUPS_DNS]:","adups_GetHostByName_Notify result!=E_ADUPS_SUCCESS");
		adups_soc_callback_init(&adups_socket_mgr,ADUPS_ERROR_NOTIFY);

		adups_socket_mgr.adups_net_error_notify(dns_ind->request_id);
			adups_net_set_state(NET_STAT_CONN_ERR);

	}
		
		adups_net_set_state(NET_STAT_CONN_SUCC);	
	return ADUPS_TRUE;
    }
    else                                // dns analyse error.
    {
	adups_DebugPrint_Ext("[ADUPS_DNS]:","adups_GetHostByName_Notify dns_ind->dns analyse error"); 
	    adups_soc_callback_init(&adups_socket_mgr,ADUPS_ERROR_NOTIFY);
	    adups_socket_mgr.adups_net_error_notify(dns_ind->request_id);
		adups_net_set_state(NET_STAT_DNS_ERR);
	    //display dns error
    }

    return ADUPS_FALSE;
}


adups_int32 adups_GetHostByName( adups_int32 request_id, const adups_char* domain, adups_uint32* addr_buf,adups_uint16* addr_len,adups_uint32 nwk_account_id)

{

	adups_int32 ret;
	adups_int32 result;


	adups_socket_mgr.adups_request_id= request_id;
	adups_net_set_state(NET_STAT_DNS_REQ);

	ret = soc_gethostbyname(KAL_FALSE,
					 MOD_ADUPS, //MOD_MMI
					 request_id, 
					 domain, 
					 (kal_uint8 *)addr_buf, 
					 (kal_uint8 *)addr_len,
					 0,
					 nwk_account_id);

	adups_DebugPrint_Ext("[ADUPS_DNS]:","adups_config_fota_addr soc_gethostbyname return =%d", ret);
	
    if (ret >= SOC_SUCCESS)             // success
    {
		adups_DebugPrint_Ext("[ADUPS_DNS]:","adups_config_fota_addr soc_gethostbyname SOC_SUCCESS");
		adups_net_set_state(NET_STAT_DNS_SUCC);

	result=E_ADUPS_SUCCESS;
    } 
	else if (ret == SOC_WOULDBLOCK)         // block
    {
		adups_DebugPrint_Ext("[ADUPS_DNS]:","adups_config_fota_addr soc_gethostbyname SOC_WOULDBLOCK");
		adups_net_set_state(NET_STAT_DNS_GETTING);

	 result= E_ADUPS_WOULDBLOCK;
    }
    else                                    // error
    {
		adups_DebugPrint_Ext("[ADUPS_DNS]:","adups_config_fota_addr soc_gethostbyname ERROR");
		adups_net_set_state(NET_STAT_DNS_ERR);
	result= E_ADUPS_FAILURE;

    }

	return result;
}


adups_int8 adups_open_socket(void)
{
       adups_int8 soc_id;
       adups_int8 option;
       adups_int8 result;
       adups_uint32 account_id;

	account_id=adups_encode_dataaccount_id();

	adups_net_set_state(NET_STAT_NEWSOC_REQ);
	soc_id = soc_create(SOC_PF_INET, SOC_SOCK_STREAM, 0, MOD_ADUPS, account_id);//MOD_MMI

	if(soc_id < 0)
	{
		adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_open_socket: soc_create failed, sockid = %d", soc_id);
		
		#ifdef ADUPS_REPORT_DL
			adups_memset(adups_report_dl,0,384);
			adups_strncpy(adups_report_dl,"adups_open_socket err",22);
			
			adups_DebugPrint(  "fota_soc_send	SOC_ERROR adups_report_dl is %s ",adups_report_dl); 
		#endif

			
		adups_net_set_state(NET_STAT_NEWSOC_ERR);
		return NULL;
	}
       adups_socket_mgr.adups_tcp_socket_id= soc_id;
	adups_DebugPrint_Ext("[ADUPS_SOC]:","connect_tcp   soc_id = %d ", soc_id);
	option = SOC_READ | SOC_WRITE | SOC_ACCEPT | SOC_CONNECT | SOC_CLOSE;
   	result =  soc_setsockopt(soc_id, SOC_ASYNC, &option, sizeof(adups_int8));
   	option = KAL_TRUE;
   	result = soc_setsockopt(soc_id, SOC_NBIO, &option, sizeof(option));
	adups_net_set_state(NET_STAT_NEWSOC_SUCC);

	return soc_id;
	
}


adups_int32 adups_connect(adups_int32 request_id,adups_uint32 addr_buf,adups_uint16 addr_len,adups_uint16 port)
{
        sockaddr_struct addr;
        adups_int8 soc_id;
        adups_int8 result;



	 memset(&addr,0x00,sizeof(sockaddr_struct));

	 memcpy((void *)addr.addr,&addr_buf,sizeof(addr_buf));
	adups_DebugPrint(  "adups_connect :addr buf=%d.%d,%d,%d,addr_len=%d ",addr.addr[0],addr.addr[1],addr.addr[2],addr.addr[3],addr_len);
	 addr.addr_len=addr_len;
	 addr.port=port;
	
	 soc_id=adups_open_socket();

	adups_net_set_state(NET_STAT_CONN_REQ);
   	result = soc_connect(soc_id, &addr);
	
	adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_connect return %d ", result);
	 if (result >= SOC_SUCCESS)
    	{
	adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_connect   SOC_SUCCESS ");
		adups_net_set_state(NET_STAT_CONN_SUCC);
		adups_soc_callback_init(&adups_socket_mgr,ADUPS_WRITE_NOTIFY);
          	adups_socket_mgr.adups_write_notify(soc_id);	
    	}
	 else if(result == SOC_WOULDBLOCK)
    	{
		adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_connect   SOC_WOULDBLOCK ");
		adups_net_set_state(NET_STAT_CONNECTING);
			adups_socket_mgr.adups_soc_state=SOC_CONNECT;
    	}
    	else
   	 {
	adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_connect   SOC_ERROR "); 
		adups_net_set_state(NET_STAT_CONN_ERR);
		
		#ifdef ADUPS_REPORT_DL
		adups_memset(adups_report_dl,0,384);
		adups_strncpy(adups_report_dl,addr_buf,sizeof(addr_buf));
		
		adups_DebugPrint(  "fota connect	SOC_ERROR adups_report_dl is %s ",adups_report_dl); 
		#endif
		
		adups_soc_close(soc_id);
		adups_soc_callback_init(&adups_socket_mgr,ADUPS_CLOSE_NOTIFY);
          	adups_socket_mgr.adups_close_notify(soc_id);	
   	 }

	return E_ADUPS_SUCCESS;
}

void adups_soc_close(adups_int8 soc_id)
{
	adups_int32 result = 0;
       adups_DebugPrint("adups_soc_close   soc_id=%d,  g_tcp_socket_id=%d", soc_id, adups_socket_mgr.adups_tcp_socket_id);
       if(soc_id != adups_socket_mgr.adups_tcp_socket_id ) 
	   	return;
	   
	adups_net_set_state(NET_STAT_CLOSE_REQ);
       result = soc_close(soc_id);
	adups_DebugPrint("adups_soc_close	result is %d! ",result);
	if (result != SOC_SUCCESS)
    	{
		adups_DebugPrint("adups_soc_close   unsuccess! ");
		adups_socket_mgr.adups_soc_state=SOC_CLOSE;
		adups_net_set_state(NET_STAT_CLOSE_ERR);
    	}
	adups_net_set_state(NET_STAT_CLOSE_SUCC);
}

ADUPS_BOOL adups_soc_hdlr(void *msg_ptr)
{
    app_soc_notify_ind_struct *soc_notify = (app_soc_notify_ind_struct*) msg_ptr;

	
	adups_DebugPrint("adups_soc_hdlr soc_notify->event_type=%d", soc_notify->event_type);
	adups_DebugPrint("adups_soc_hdlr soc_id=%d", soc_notify->socket_id);
	
	if(soc_notify->event_type != adups_socket_mgr.adups_soc_state){
		return ADUPS_FALSE;
	}
      adups_DebugPrint(  "fota_soc_hdr   soc_id=%d,  g_tcp_socket_id=%d", soc_notify->socket_id, adups_socket_mgr.adups_tcp_socket_id);
	  
       if(soc_notify->socket_id != adups_socket_mgr.adups_tcp_socket_id )
       {
            return ADUPS_FALSE;
       }

    switch (soc_notify->event_type)
    {
        case SOC_WRITE:	
		adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_soc_hdlr SOC_WRITE");

		adups_soc_callback_init(&adups_socket_mgr,ADUPS_WRITE_NOTIFY);
          	adups_socket_mgr.adups_write_notify(soc_notify->socket_id);	
           break;
            
        case SOC_READ:
		adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_soc_hdlr SOC_READ");
		adups_soc_callback_init(&adups_socket_mgr,ADUPS_READ_NOTIFY);
		adups_socket_mgr.adups_read_notify(soc_notify->socket_id);
           break;

        case SOC_CONNECT:
	adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_soc_hdlr SOC_CONNECT");
		adups_net_set_state(NET_STAT_CONN_SUCC);
		adups_soc_callback_init(&adups_socket_mgr,ADUPS_WRITE_NOTIFY);
          	adups_socket_mgr.adups_write_notify(soc_notify->socket_id);	
           break;
		   
        case SOC_CLOSE:
	adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_soc_hdlr SOC_CLOSE");
		adups_soc_callback_init(&adups_socket_mgr,ADUPS_CLOSE_NOTIFY);
          	adups_socket_mgr.adups_close_notify(soc_notify->socket_id);		

           break;
    }

    return ADUPS_FALSE;
	
}


adups_int32  adups_soc_send(adups_int8 soc_id,Request* req )
{
   adups_int32 result;
   

   adups_DebugPrint(  "adups_soc_send  soc_id = %d,g_tcp_socket_id:%d", soc_id,adups_socket_mgr.adups_tcp_socket_id);
   
	adups_net_set_state(NET_STAT_SEND_REQ);
   result = soc_send(soc_id, req->header.data, strlen(req->header.data),0);
   
	adups_DebugPrint(  " adups_soc_send -------------  strlen(req->header.data) =%d",  strlen(req->header.data));
	adups_DebugPrint(  " adups_soc_send ------------- header->data =%s", req->header.data);
	adups_DebugPrint(  " %s", req->header.data+70);
	adups_DebugPrint(  " %s", req->header.data+200);
	if(strlen(req->header.data) > 300)
		adups_DebugPrint(  " %s", req->header.data+300);
	adups_DebugPrint_Ext("[ADUPS_SOC]:","adups_soc_send num of byte sent = %d", result);

	 if (result >= SOC_SUCCESS)
    	 {
		adups_DebugPrint(  "adups_soc_send   SOC_SUCCESS ");
		adups_net_set_state(NET_STAT_SEND_SUCC);
   		return E_ADUPS_SUCCESS;
    	 }
	 else if(result == SOC_WOULDBLOCK)
  	 {
		adups_DebugPrint(  "adups_soc_send   SOC_WOULDBLOCK ");
		adups_net_set_state(NET_STAT_SENDING);
		adups_socket_mgr.adups_soc_state=SOC_WRITE;
		return E_ADUPS_WOULDBLOCK;
  	 }
	 else
	 {
	       adups_DebugPrint(  "adups_soc_send   SOC_ERROR "); 
		adups_net_set_state(NET_STAT_SEND_ERR);

		return E_ADUPS_FAILURE;		   
	  }
}



adups_int32 adups_soc_recv(adups_int8 soc_id,adups_char* buf,adups_int32 len,adups_uint32 flag)
{
	adups_int32 result = -1;	   

	adups_DebugPrint(  "adups_soc_rev  soc_recv------>");	
	adups_net_set_state(NET_STAT_RECV_REQ);
	result = soc_recv(soc_id, buf, len, flag);
	
	adups_DebugPrint(  "recv buf=%s,result=%d",buf+70,result);

	if (result >SOC_SUCCESS)
	{
		adups_net_set_state(NET_STAT_RECV_SUCC);
		return 	result;	
	}
	else if(result == SOC_WOULDBLOCK)
	{
		adups_DebugPrint(  "adups_soc_rev   SOC_WOULDBLOCK ");
		adups_net_set_state(NET_STAT_RECVING);
		adups_socket_mgr.adups_soc_state=SOC_READ;
		adups_DebugPrint(  "adups_soc_rev   SOC_WOULDBLOCK ----------->1 ");
		return E_ADUPS_WOULDBLOCK;
	}
	else if(result ==SOC_SUCCESS)
	{
		adups_net_set_state(NET_STAT_RECV_SUCC2);
		return E_ADUPS_SUCCESS;
		
	}
	else
	{
		adups_net_set_state(NET_STAT_RECV_ERR);
		return E_ADUPS_FAILURE;
	}

}

adups_int8 adups_net_socid(void)
{
	return adups_socket_mgr.adups_tcp_socket_id;
}
void adups_net_set_state(adups_int32 netsm)
{
	adups_DebugPrint("adups_net:set sm: %d", netsm);
	adups_socket_mgr.adups_net_sm = netsm;
}
adups_int32 adups_net_get_state(void)
{
	adups_DebugPrint("adups_net:get sm: %d", adups_socket_mgr.adups_net_sm);
	return adups_socket_mgr.adups_net_sm;
}

#define ADUPS_MAX_IMSI_LEN 16
#define ADUPS_LACCELL_UPDATE_MAXNUM 5

adups_cell_info_struct s_adups_sal_cur_cell_info={0};
adups_cell_info_struct s_adups_sal_nbr_cell_info[6];
adups_uint16 s_adups_sal_cell_nbr_num = 0;
adups_uint8 s_adups_imsi[ADUPS_MAX_IMSI_LEN+1] = {0};


void adups_net_cell_rsp(l4c_nbr_cell_info_ind_struct *msg_ptr)
{
	adups_uint16 i=0;
	gas_nbr_cell_info_struct cell_info;

	adups_stop_timer(adups_net_cell_req);

	if(msg_ptr)
	{
		if (TRUE == msg_ptr->is_nbr_info_valid)
		{
			memcpy((void *)&cell_info, (void *)(&(msg_ptr->ps_nbr_cell_info_union.gas_nbr_cell_info)), sizeof(gas_nbr_cell_info_struct));
		}
		else
		{
			memset((void *)&cell_info, 0, sizeof(gas_nbr_cell_info_struct));	
		}
	
		s_adups_sal_cur_cell_info.arfcn = cell_info.nbr_meas_rslt.nbr_cells[cell_info.serv_info.nbr_meas_rslt_index].arfcn;
		s_adups_sal_cur_cell_info.bsic = cell_info.nbr_meas_rslt.nbr_cells[cell_info.serv_info.nbr_meas_rslt_index].bsic;
		s_adups_sal_cur_cell_info.rxlev = cell_info.nbr_meas_rslt.nbr_cells[cell_info.serv_info.nbr_meas_rslt_index].rxlev;
		s_adups_sal_cur_cell_info.mcc = cell_info.serv_info.gci.mcc;
		s_adups_sal_cur_cell_info.mnc = cell_info.serv_info.gci.mnc;
		s_adups_sal_cur_cell_info.lac = cell_info.serv_info.gci.lac;
		s_adups_sal_cur_cell_info.ci = cell_info.serv_info.gci.ci;
		s_adups_sal_cell_nbr_num = cell_info.nbr_cell_num;
		
		s_adups_sal_cur_cell_info.updatetimes++;
#if 1		
			adups_DebugPrint_Ext("[ADUPS_CELL]:","adups_net_cell_rsp <curr>: arfcn: %d, bsic:%d, rxlev:%d, mcc:%d, mnc:%d, lac:%d, ci:%d, nbr_num:%d", \
			s_adups_sal_cur_cell_info.arfcn, s_adups_sal_cur_cell_info.bsic, s_adups_sal_cur_cell_info.rxlev, \
			s_adups_sal_cur_cell_info.mcc, s_adups_sal_cur_cell_info.mnc, s_adups_sal_cur_cell_info.lac, s_adups_sal_cur_cell_info.ci, \
			s_adups_sal_cell_nbr_num);
#endif
		for(i = 0; i < cell_info.nbr_cell_num; i++)
		{
			s_adups_sal_nbr_cell_info[i].arfcn = cell_info.nbr_meas_rslt.nbr_cells[cell_info.nbr_cell_info[i].nbr_meas_rslt_index].arfcn;
			s_adups_sal_nbr_cell_info[i].bsic = cell_info.nbr_meas_rslt.nbr_cells[cell_info.nbr_cell_info[i].nbr_meas_rslt_index].bsic;
			s_adups_sal_nbr_cell_info[i].rxlev = cell_info.nbr_meas_rslt.nbr_cells[cell_info.nbr_cell_info[i].nbr_meas_rslt_index].rxlev;
			s_adups_sal_nbr_cell_info[i].mcc = cell_info.nbr_cell_info[i].gci.mcc;
			s_adups_sal_nbr_cell_info[i].mnc = cell_info.nbr_cell_info[i].gci.mnc;
			s_adups_sal_nbr_cell_info[i].lac = cell_info.nbr_cell_info[i].gci.lac;
			s_adups_sal_nbr_cell_info[i].ci = cell_info.nbr_cell_info[i].gci.ci;
#if 0
			adups_DebugPrint("adups_net_cell_rsp <near>: idx:%d, arfcn: %d, bsic:%d, rxlev:%d, mcc:%d, mcc:%d, lac:%d, ci:%d, nbr_num:%d", \
				i, \
				s_adups_sal_nbr_cell_info[i].arfcn, s_adups_sal_nbr_cell_info[i].bsic, s_adups_sal_nbr_cell_info[i].rxlev, \
				s_adups_sal_nbr_cell_info[i].mcc, s_adups_sal_nbr_cell_info[i].mnc, s_adups_sal_nbr_cell_info[i].lac, s_adups_sal_nbr_cell_info[i].ci);
#endif			
		}

		if(s_adups_sal_cur_cell_info.updatetimes > ADUPS_LACCELL_UPDATE_MAXNUM)
			adups_net_cell_dereq();
		
	}	

	
}

void adups_net_cell_req(void)
{
	adups_DebugPrint(  "adups_net_cell_req:  MSG_ID_L4C_NBR_CELL_INFO_REG_REQ");
	adups_task_send_msg(MOD_L4C, MMI_L4C_SAP, MSG_ID_L4C_NBR_CELL_INFO_REG_REQ, 0);
	adups_Debug_DeltaNew();//test
}

void adups_net_cell_dereq(void)
{
	adups_DebugPrint(  "adups_net_cell_dereq:  MSG_ID_L4C_NBR_CELL_INFO_DEREG_REQ");
	adups_task_send_msg(MOD_L4C, MMI_L4C_SAP, MSG_ID_L4C_NBR_CELL_INFO_DEREG_REQ, 0);
}

adups_int32 adups_get_rcvd_block_size(void)
{
	return RECV_BLOCK_SIZE;
}

#if 1//def ADUPS_AUTO_TEST

void adups_net_start_get_new_version(void)
{

		adups_get_new_version();
}
/*
void adups_net_start_change_imei(adups_uint16 seconds)
{
	//dynamic generate imei for test
	adups_start_timer(seconds*1000, adups_set_nvram_imei, ADUPS_FALSE); 
}
*/
#endif

void adups_boot_hal_init(void)
{
	adups_DebugSwitch(1);

	adups_DebugPrint_Ext("[ADUPS_COMMON]:","adups curr ver: %s", adups_get_device_version());

	
	adups_start_timer(30*1000, adups_register, ADUPS_FALSE); 
	adups_start_timer(5000, adups_net_cell_req, ADUPS_FALSE); 
	adups_start_timer(90*1000, adups_upgrade_thread_on, ADUPS_FALSE); //90
#if 0//def ADUPS_AUTO_TEST	
	adups_start_timer(140*1000, adups_net_start_get_new_version, ADUPS_FALSE); 
#endif
}

#endif


