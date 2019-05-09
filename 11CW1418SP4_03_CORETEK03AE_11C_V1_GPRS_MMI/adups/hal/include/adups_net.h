#ifndef _ADUPS_NET_H_
#define _ADUPS_NET_H_

/*cell info structure*/
typedef struct{
    adups_uint16 arfcn;           /*ARFCN*/
    adups_uint8 bsic;              /*BSIC*/
    adups_uint8 rxlev;            /*Received signal level*/
    adups_uint16 mcc;            /*MCC*/
    adups_uint16 mnc;            /*MNC*/
    adups_uint16 lac;              /*LAC*/
    adups_uint16 ci;                /*CI*/
	adups_uint16  updatetimes;
}adups_cell_info_struct;

adups_extern adups_uint32 adups_encode_dataaccount_id(void);

adups_extern adups_int32 adups_get_rcvd_block_size(void);
adups_extern ADUPS_BOOL adups_config_addr_hdlr(void* inMsg);

adups_extern adups_int32 adups_GetHostByName( adups_int32 request_id, const adups_char* domain, adups_uint32* addr_buf,adups_uint16* addr_len,adups_uint32 nwk_account_id);

adups_extern adups_int8 adups_open_socket(void);

adups_extern adups_int32 adups_connect(adups_int32 request_id,adups_uint32 addr_buf,adups_uint16 addr_len,adups_uint16 port);

adups_extern void adups_soc_close(adups_int8 soc_id);

adups_extern ADUPS_BOOL adups_soc_hdlr(void *msg_ptr);

adups_extern adups_int32  adups_soc_send(adups_int8 soc_id,Request* req );

adups_extern adups_int32 adups_soc_recv(adups_int8 soc_id,adups_char* buf,adups_int32 len,adups_uint32 flag);

adups_extern void adups_soc_callback_init(ADUPS_SOCKET_MGR_STRUCT* socket_mgr, ADUPS_SOCMGR_USR adups_usr);

adups_extern void adups_net_set_state(adups_int32 netsm);

adups_extern adups_int32 adups_net_get_state(void);

adups_extern void adups_boot_hal_init(void);
adups_extern void adups_net_cell_req(void);
adups_extern void adups_net_cell_dereq(void);
#endif
