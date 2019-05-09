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
 *   fs_bl_func.c
 *
 * Project:
 * --------
 *   Maui
 *
 * Description:
 * ------------
 *   This file contains APIs for Boot loader.
 *
 * Author:
 * -------
 * -------
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "kal_release.h"
#include "fat_fs.h"
#include "rtfiles.h"
#if !defined(__FS_FUNET_ENABLE__)
extern kal_int32 fs_bl_open(const kal_wchar *FileName);
#else
extern kal_int32 fs_bl_open(const kal_wchar *FileName,kal_uint32 Flag);
#endif
extern kal_int32 fs_bl_close(FS_HANDLE FileHandle);
extern int fs_bl_read(FS_HANDLE FileHandle, void * DataPtr, UINT Length, UINT * Read);
extern int fs_bl_seek(FS_HANDLE FileHandle, int Offset, int Whence);
extern kal_int32 fs_bl_get_size(FS_HANDLE FileHandle, UINT * Size);
extern int fs_bl_write(FS_HANDLE FileHandle, void * DataPtr, UINT Length, UINT * Written);
extern int fs_bl_delete(const WCHAR * FileName);
extern int fs_bl_sanitycheck(void);

kal_int32 FS_Open(const kal_wchar *FileName, kal_uint32 Flag)
{
 #if !defined(__FS_FUNET_ENABLE__)
    return fs_bl_open(FileName);
 #else
	return fs_bl_open(FileName,Flag);
 #endif
}
     

int FS_Close(FS_HANDLE FileHandle)
{
    return fs_bl_close(FileHandle);
}
     
    
int FS_Read(FS_HANDLE FileHandle, void * DataPtr, UINT Length, UINT * Read)
{

    if (!Read)
        return FS_PARAM_ERROR;
    
    return fs_bl_read(FileHandle, DataPtr, Length, Read);
}
//Jeffrey Add for FOTA_IOT     

int FS_Write(FS_HANDLE FileHandle, void * DataPtr, UINT Length, UINT * Written)
{

    if (!Written)
        return FS_PARAM_ERROR;
    
    return fs_bl_write(FileHandle, DataPtr, Length, Written);
}    

int FS_Delete(const WCHAR * FileName)
{
    
    return fs_bl_delete(FileName);
} 


int FS_SanityCheck(void)
{
	return fs_bl_sanitycheck();
}
int FS_CreateBootSector(void * BootSector, const FS_PartitionRecord * Partition, kal_uint8 MediaDescriptor, kal_uint32 MinSectorsPerCluster, kal_uint32 Flags)
{
   int Result = RTF_NO_ERROR;

   Result = RTFCreateBootSector(BootSector, (RTFPartitionRecord *)Partition, MediaDescriptor, MinSectorsPerCluster, Flags);

  // fs_util_check_stack();

   return Result;
}

int FS_CreateMasterBootRecord(void * SectorBuffer, const FS_PartitionRecord * DiskGeometry)
{
   int Result = RTF_NO_ERROR;

   Result = RTFCreateMasterBootRecord(SectorBuffer, (RTFPartitionRecord *)DiskGeometry);

 //  fs_util_check_stack();

   return Result;
}

int FS_FormatGetFatType(void * BootSector, kal_uint32 *RootDirSectors, kal_uint32 *FATSectors, kal_uint32 *Clusters)
{
    return RTFFormatGetFatType(BootSector, RootDirSectors, FATSectors, Clusters);
}

int FS_FormatInitFatSector(kal_uint8 MediaDescriptor, kal_int32 FATType, kal_uint8 *Sector)
{
    RTFFormatInitFatSector(MediaDescriptor, FATType, Sector);
    return FS_NO_ERROR;
}

int FS_SplitPartition(void * MasterBootRecord, kal_uint32 Sectors)
{
   int Result;

   Result = RTFSplitPartition(MasterBootRecord, Sectors);

 //  fs_util_check_stack();

   return Result;
}
int FS_Seek(FS_HANDLE FileHandle, int Offset, int Whence)
{
    if (Whence != FS_FILE_BEGIN &&
        Whence != FS_FILE_CURRENT &&
        Whence != FS_FILE_END)
    {
        return FS_PARAM_ERROR;
    }
    
    return fs_bl_seek(FileHandle, Offset, Whence);
}

int FS_GetFileSize(FS_HANDLE FileHandle, UINT * Size)
{
    int Result = FS_NO_ERROR;
    Result = fs_bl_get_size(FileHandle, Size);
    return Result;
}

