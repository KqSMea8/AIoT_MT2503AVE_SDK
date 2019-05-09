#include "adups_typedef.h"
#include "adups_debug.h"
#include "adups_define.h"
#include "MMI_include.h"
#include "mmi_rp_app_adups_def.h"


void adups_display_notify(adups_int32 message)
{
#ifndef ADUPS_LCD_NONE
	switch(message)
	{
		case E_ADUPS_TOKEN_WORNG:
			DisplayPopup((PU8)GetString(STR_ID_AUDPS_CK_ERROR_TOKEN), IMG_GLOBAL_ERROR, 5000, 0);			
            	break;
				
		case E_ADUPS_PROJECT_WORONG:
			DisplayPopup((PU8)GetString(STR_ID_AUDPS_CK_ERROR_PROJECT), IMG_GLOBAL_ERROR, 5000, 0);			
            	break;

		case E_ADUPS_PARAM_WORONG:
			DisplayPopup((PU8)GetString(STR_ID_AUDPS_CK_ERROR_PARAM), IMG_GLOBAL_ERROR, 5000, 0);			
            	break;

		case E_ADUPS_LAST_VERSION:
			DisplayPopup((PU8)GetString(STR_ID_AUDPS_CK_ERROR_LATEST), IMG_GLOBAL_ERROR, 5000, 0);			
            	break;

		case E_ADUPS_SUCCESS:
			DisplayPopup((PU8)GetString(STR_GLOBAL_DONE), IMG_GLOBAL_SUCCESS, 5000, 0);			
            	break;

		case E_ADUPS_SIM_NOTEXIST:
			DisplayPopup((PU8)GetString(STR_GLOBAL_UNAVAILABLE_SIM), IMG_GLOBAL_ERROR, 5000, 0);			
            	break;
				
		case E_ADUPS_MID_ERROR:
			DisplayPopup((PU8)GetString(STR_ID_AUDPS_IMEI_ERROR), IMG_GLOBAL_ERROR, 5000, 0);			
            	break;

		case E_ADUPS_NOMEMORY:
			DisplayPopup((PU8)GetString(STR_ID_AUDPS_DL_ERROR), IMG_GLOBAL_ERROR, 5000, 0);			
            	break;

		default:
			DisplayPopup((PU8)GetString(STR_GLOBAL_FAILED), IMG_GLOBAL_ERROR, 5000, 0);			
            	break;			
		
	}
#endif
}


static void ADUPS_Entry_Progress_Scr_exit(void)
{
	mmi_frm_scrn_close(GRP_ID_ADUPS_MAIN, SCR_ADUPS_PROGRESS);
}

void ADUPS_Entry_Progress_Scr(U16 strid)
{
#ifndef ADUPS_LCD_NONE
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	mmi_frm_group_create(GRP_ID_ROOT, GRP_ID_ADUPS_MAIN, NULL, NULL);
	mmi_frm_group_enter(GRP_ID_ADUPS_MAIN, MMI_FRM_NODE_SMART_CLOSE_FLAG);

	if(!mmi_frm_scrn_enter(GRP_ID_ADUPS_MAIN, SCR_ADUPS_PROGRESS, ADUPS_Entry_Progress_Scr_exit, ADUPS_Entry_Progress_Scr, MMI_FRM_FULL_SCRN))
	{
		return;
	}

    adups_DebugPrint("ADUPS_Entry_Progress_Scr");
	
    ShowCategory63Screen(
        (U8*)GetString(strid),
        mmi_get_event_based_image(MMI_EVENT_PROGRESS),
        NULL);
	
#endif

}

void adups_display_progress(adups_int32 message)
{
#ifndef ADUPS_LCD_NONE

	adups_uint16 strid;
	
	if(message==E_ADUPS_NET_CONNECTING)
		strid=STR_GLOBAL_CONNECTING;
	else if(message==E_ADUPS_DOWNLOAD_CONNECTING)
		strid=STR_GLOBAL_DOWNLOADING;	

	ADUPS_Entry_Progress_Scr(strid);
#endif
}


void adups_stop_progress(void)
{
#ifndef ADUPS_LCD_NONE
    //mmi_frm_scrn_close_active_id();
	//mmi_frm_scrn_close(GRP_ID_ADUPS_MAIN, SCR_ADUPS_PROGRESS);
#ifdef __MMI_FRM_HISTORY__
    //DeleteScreenIfPresent(SCR_ADUPS_PROGRESS);
#endif
#endif
}


extern void adups_get_new_version(void);
void AdupsHighlightCV(void)
{
#ifndef ADUPS_LCD_NONE
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Change left soft key icon and label */
    ChangeLeftSoftkey(STR_GLOBAL_OK, 0);
    /* Change right soft key icon and label */
    ChangeRightSoftkey(STR_GLOBAL_BACK, 0);
    /* Register function for left/right soft key */
adups_DebugSwitch(1);
    SetLeftSoftkeyFunction(adups_get_new_version, KEY_EVENT_UP);
    SetRightSoftkeyFunction(GoBackHistory, KEY_EVENT_UP);
    return;
#endif	
}

adups_extern adups_int32 ADUPS_file_size(void);
adups_extern adups_int32 ADUPS_recvd_size(void);


float ADUPS_get_download_percent(void)
{
	float DownloadPercent = 0.00;
	adups_int32 FileSize = 0,RecvdSize = 0;
	FileSize = ADUPS_file_size();
	RecvdSize = ADUPS_recvd_size();
	if(FileSize = 0)
		return 0;
	DownloadPercent = RecvdSize/FileSize;
	return DownloadPercent;
}

ADUPS_BOOL adups_auto_patch(void)
{
	return ADUPS_TRUE;
}



