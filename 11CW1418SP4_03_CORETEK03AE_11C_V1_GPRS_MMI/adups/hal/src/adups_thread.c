#ifdef __ADUPS_FOTA_SOC__

#include "adups_typedef.h"
#include "adups_debug.h"
#include "MMIDataType.h"
#include "MMI_include.h"
#include <soc_api.h>
#include "cbm_api.h"
#include "adups_net.h"

#include "task_config.h"
#include "kal_public_api.h"
#include "stack_config.h"
#include "kal_common_defs.h"
#include "tst_sap.h"

static void adups_task_main(task_entry_struct *task_entry_ptr);
static void adups_task_entry(ilm_struct *ilm_ptr);
static ADUPS_BOOL adups_task_init(task_indx_type task_indx);
static ADUPS_BOOL adups_task_reset(task_indx_type task_indx);
static void adups_task_tst_inject_handler( ilm_struct *ilm_ptr );


kal_bool adups_task_create(comptask_handler_struct **handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    static const comptask_handler_struct adups_task_handler_info = 
    {
        adups_task_main,  /* task entry function */
        adups_task_init,       /* task initialization function */
        NULL,           /* task configuration function */
        adups_task_reset,      /* task reset handler */
        NULL,           /* task termination handler */
    };

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *handle = (comptask_handler_struct*) & adups_task_handler_info;

    return ADUPS_TRUE;
}


static void adups_task_main(task_entry_struct *task_entry_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct current_ilm;
    kal_uint32 my_index;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_get_my_task_index(&my_index);

    while (1)
    {
        receive_msg_ext_q_for_stack(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm);
        stack_set_active_module_id(my_index, current_ilm.dest_mod_id);

        adups_task_entry((void*)&current_ilm);

        free_ilm(&current_ilm);
    }

}

static void adups_task_entry(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	
	adups_DebugPrint("adups_task_entry: ilm_ptr->msg_id is %d",ilm_ptr->msg_id);
	if ( MSG_ID_APP_SOC_NOTIFY_IND == ilm_ptr->msg_id)
	{
		adups_soc_hdlr((void *)(ilm_ptr->local_para_ptr));
	}
	else if (MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND == ilm_ptr->msg_id)
	{
		adups_GetHostByName_Notify((void *)(ilm_ptr->local_para_ptr));
	}
	else if ((MSG_ID_L4C_NBR_CELL_INFO_REG_CNF == ilm_ptr->msg_id) ||(MSG_ID_L4C_NBR_CELL_INFO_IND == ilm_ptr->msg_id ))
	{
		adups_net_cell_rsp((void *)(ilm_ptr->local_para_ptr));
	}
	else if(MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND == ilm_ptr->msg_id)
	{
		//adups_soc_close(adups_int8 soc_id);
	}
#ifdef ADUPS_AUTO_TEST	
	else if (MSG_ID_NVRAM_WRITE_IMEI_CNF == ilm_ptr->msg_id)
	{
		//adups_get_new_version();
		adups_DebugPrint("adups: to enable new IMEI valid,reboot...");
		adups_device_start_reboot();
	}
#endif	
	else if (MSG_ID_TST_INJECT_STRING == ilm_ptr->msg_id)
	{
		adups_task_tst_inject_handler( ilm_ptr );
	}

}

static ADUPS_BOOL adups_task_init(task_indx_type task_indx)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (0)//(!aud_init())
    {
        return ADUPS_FALSE;
    }
	adups_boot_hal_init();

    return ADUPS_TRUE;
}

static ADUPS_BOOL adups_task_reset(task_indx_type task_indx)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return ADUPS_TRUE;
}

void adups_task_send_msg(module_type  dest_mod, sap_type sap_id, msg_type msg_id,void *local_param_ptr, kal_uint32 para_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct  *ilm_ptr;
    module_type src_mod;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    src_mod = MOD_ADUPS; //stack_get_active_module_id();
    ilm_ptr = allocate_ilm(src_mod);
    ilm_ptr->src_mod_id = src_mod;
    ilm_ptr->dest_mod_id = dest_mod;
    ilm_ptr->sap_id = sap_id;
    ilm_ptr->msg_id = msg_id;
    ilm_ptr->local_para_ptr = (local_para_struct*) local_param_ptr;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct*) NULL;
    msg_send_ext_queue(ilm_ptr);
}

void adups_task_free_msg( ilm_struct  *ilm_ptr)
{
	if(ilm_ptr)
		free_ilm(ilm_ptr);
}

//for test
static void adups_task_tst_inject_handler( ilm_struct *ilm_ptr )
{
    tst_module_string_inject_struct* tst_inject = (tst_module_string_inject_struct*)ilm_ptr->local_para_ptr;
	adups_char cmdstr[32] = {0};
	char cmdpara[5][16]={0};//donot change
	adups_uint8 numline = 5;//donot change
	adups_char *para = (adups_char *)(tst_inject->string);

	adups_itoa(tst_inject->index, &cmdstr,10);

	adups_fota_split_atcmd(para, ',', cmdpara, &numline);
	
	//if(numline>0 && numline<=5)
	{
		adups_fota_atcmd(cmdstr, cmdpara[0], cmdpara[1], cmdpara[2],0);
	}
	
}

#endif

