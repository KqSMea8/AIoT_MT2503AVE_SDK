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
 *   fs_bl_main.c
 *
 * Project:
 * --------
 *   Maui
 *
 * Description:
 * ------------
 *   This file contains FS for Boot loader.
 *
 * Author:
 * -------
 * -------
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
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
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
#include "fs_utility.h"
#include "rtfiles.h"
#include "fs_kal.h"
#include "rtfex.h"
#include "rtfbuf.h"
#include "bl_init.h"

typedef enum
{
    FS_BL_PROC_NULL,
    FS_BL_PROC_INIT,
    FS_BL_PROC_READY,
    
    FS_BL_PROC_INVALID
}fs_bl_proc_enum;

static fs_bl_proc_enum fs_bl_proc_state;

extern int        TLSBlockIndex;
int sanitycheckcount=0;
// REVIEW: remove State
// REVISE: Done
static RTFDrive *fs_bl_locate_drive(int DriveIndex)
{
    RTFDrive  *volatile Drive = NULL;
    kal_int32 volatile result = FS_NO_ERROR;
	   kal_uint8 LocateSysDrv = KAL_FALSE;
    
    if (fs_bl_proc_state == FS_BL_PROC_NULL)
    {
    	#if defined(__FS_FUNET_ENABLE__)
    	if(sanitycheckcount==0)
		#endif
        RTFInit();

        fs_bl_proc_state = FS_BL_PROC_INIT;
    }

#if defined(__FS_FUNET_ENABLE__)


   LocateSysDrv = fs_conf_query_if_sys_drv((WCHAR)(DriveIndex + MT_BASE_DRIVE_LETTER), (WCHAR*)&DriveIndex);

   // now DriveIndex is "drive letter", let it become "index"
   DriveIndex = DriveIndex - MT_BASE_DRIVE_LETTER;

   if ((DriveIndex < 0) || (DriveIndex >= FS_MAX_DRIVES))
   {

      XRAISE(RTF_DRIVE_NOT_FOUND);
   }

   Drive = gFS_Data.DriveTable + DriveIndex;

   if (Drive->Dev == NULL)
   {
       XRAISE(RTF_DRIVE_NOT_FOUND);
   }
#else 
    Drive = gFS_Data.DriveTable;
#endif
    if (fs_bl_proc_state == FS_BL_PROC_INIT)
    {

        // REVIEW: need to check why it isn't empty 
        if (Drive->Dev == NULL) 
        {
            XRAISE(FS_DRIVE_NOT_FOUND);
        }

        // REVIEW: remove State
        // REVISE: Done
        XTRY
        {
            case XCODE:
				#if !defined(__FS_FUNET_ENABLE__)
                result= MountRemovableDevice(Drive->Dev);
				#endif
                break;
        // REVIEW: add default
        // REVISE: Done
            default:
               result = XVALUE; 
               break;
            
            case XFINALLY:
                if (result != FS_NO_ERROR)
                    Drive = NULL;
                break;
        }
        XENDX
        fs_bl_proc_state = FS_BL_PROC_READY;
    }


    return Drive;
}

// REVIEW: remove Flag
// REVISE: Done
#if !defined(__FS_FUNET_ENABLE__)
kal_int32 fs_bl_open(const kal_wchar *FileName)
#else
kal_int32 fs_bl_open(const kal_wchar *FileName,kal_uint32 Flags)
#endif
{
   int UL;
   int     volatile          Result;
   RTFile* volatile          f              = NULL;
   
   WacthDogRestart();
   
    XTRY
    {
        case XCODE:
        {
            f = gFS_Data.FileTable;

            /* ignore any spaces ahead of FileName */
            while (compASCII(FileName, 0, ' '))
            {
                FileName += 2;
            }

            UL = fs_util_dchar_strlen((char *)FileName);

            if (((UL == 12) && fs_util_comp_uni_ascii((char *)FileName, "\\\\.\\", 4) && compASCII(FileName, 5, ':')) ||
                ((UL == 36) && fs_util_comp_uni_ascii((char *)FileName, "\\\\.\\PHYSICALDRIVE", 17)) ||
                ((UL >= 90) && (fs_util_comp_uni_ascii((char *)FileName, "\\\\.\\FMapF\\", 10))) ||
                (UL== 0))
            {
                XRAISE(FS_INVALID_FILENAME);
            }
            else
            {

                /*
                * see if the dir name contains a drive letter
                * Case UL < 2: Name has only 1 char!
                */
                f->Drive = fs_bl_locate_drive(fs_util_toupper(toASCII(FileName, 0)) - MT_BASE_DRIVE_LETTER);

                if (!f->Drive)
                    XRAISE(FS_DRIVE_NOT_FOUND);
                
                f->Dev = f->Drive->Dev;
                f->SpecialKind = NormalFile;
            }

            // REVIEW: remove it
            // REVIEW: Done

		#if !defined(__FS_FUNET_ENABLE__)
            f->Flags = (FS_OPEN_NO_DIR | FS_READ_ONLY);
		#else
			f->Flags = Flags;
		#endif

            if (SearchFile(f, SEARCH_FILES, (char *)FileName, NULL)) // it does exist
            {
                // if this is a folder
                if ((f->DirEntry.Dir.Attributes & FS_ATTR_DIR) != 0)
                {
                    XRAISE(FS_FILE_NOT_FOUND);
                }
			#if defined(__FS_FUNET_ENABLE__)
				if (Flags & RTF_CREATE_ALWAYS)
	            {
	               // Read-only, directory and volume file is not allowed to be deleted
	               if (f->DirEntry.Dir.Attributes & (RTF_ATTR_READ_ONLY | RTF_ATTR_DIR | RTF_ATTR_VOLUME))
	               {
	                //  fs_util_trace_err_noinfo(MT_READ_ONLY_ERROR);

	                  XRAISE(MT_READ_ONLY_ERROR);
	               }

	               // Opened file is not allowed to be deleted (W09.26)
	               CheckNotOpen(f);

	               if (FIRST_FILE_CLUSTER(f->DirEntry.Dir) != 0)
	               {
	                  if (GetClusterValue(f->Drive, FIRST_FILE_CLUSTER(f->DirEntry.Dir), 0) != 0)   // do not delete FAT chains if it is a dangling FirstCluster
	                  {
	                      //quota
	                      FATDelete(f->Drive, FIRST_FILE_CLUSTER(f->DirEntry.Dir), f);

	                      if (Flags & RTF_COMMITTED)
	                      {
	                         FlushFAT(f->Dev);
	                      }
	                  }

	                  SET_FIRST_FILE_CLUSTER(f->DirEntry.Dir, 0);
	               }

	               f->DirEntry.Dir.Attributes = Flags & RTF_ATTR_ANY;

	               if (f->DirEntry.Dir.FileSize != 0)
	               {
	                  RTFSYSGetDateTime(&f->DirEntry.Dir.DateTime);
	                  f->DirEntry.Dir.FileSize = 0;
	               }

	               /*
	                * Use CREATE_ALWAYS for an existing file does not need to do anything
	                * to internal hints because the location of this file is not changed.
	                */

	               UpdateDirEntry(f);
	            }
			#endif
            }
            else // it does not exist
            {
            	 if (Flags & (RTF_CREATE_ALWAYS | RTF_CREATE))
	            {
	              // RTFileCheck_WriteProtect(f);

	               if (Flags & RTF_READ_ONLY)
	               {
	                //  fs_util_trace_err_noinfo(MT_READ_ONLY_ERROR);

	                  XRAISE(MT_READ_ONLY_ERROR);
	               }

	               //MTCheckSharingWithSpecialKindFh(f);

	               //CheckValidFileName2((WCHAR *)f->FullName, KAL_FALSE);
	               kal_mem_set((void*)&f->DirEntry.Dir, 0, sizeof(f->DirEntry.Dir));
	               f->DirEntry.Dir.Attributes = Flags & RTF_ATTR_ANY;
	               RTFSYSGetDateTime(&f->DirEntry.Dir.DateTime);
	               f->DirEntry.Dir.CreateDateTime = f->DirEntry.Dir.DateTime;


	               CreateNewDirEntry(f, NULL);

	               if (Flags & RTF_COMMITTED)
	               {
	                  CommitDirEntry(f);
	                  FlushFAT(f->Dev);
	               }
	               
	            }
				 else
				 {
                	XRAISE(FS_FILE_NOT_FOUND);
				 }
            }

            InitFilePointer(f);

            Result = MakeNewFileHandle(f);

            break;
        }

        default:
            Result = XVALUE;
            break;

        case XFINALLY:
        {
            if (f)
            {
                if (Result < FS_NO_ERROR)
                    FreeFTSlot(f);
            }
            
            break;
        }
    } 
   XEND_API
    
   return Result;
}

     

kal_int32 fs_bl_close(FS_HANDLE FileHandle)
{
    FreeFTSlot(gFS_Data.FileTable);
    return FS_NO_ERROR;
}

     
    
int fs_bl_read(FS_HANDLE FileHandle, void * DataPtr, UINT Length, UINT * Read)
{
    RTFile* volatile f = NULL;
    UINT l1, l2, l3;
    DWORD volatile InitialFilePointer;
    int volatile Result = FS_NO_ERROR;

    *Read = 0;

    XTRY
    {
        case XCODE:
        {
            /* get device and system lock (check file's flag to decide use blocking or non-blocking mode) */
            f = gFS_Data.FileTable;

            InitialFilePointer = f->FilePointer;


            if (f->FilePointer > f->DirEntry.Dir.FileSize)
            {
               XRAISE(FS_INVALID_FILE_POS);
            }

            if ((f->FilePointer + Length) < f->FilePointer)
            {
                XRAISE(FS_FILE_TOO_LARGE);
            }


            Length = fs_min(Length, f->DirEntry.Dir.FileSize - f->FilePointer);

            /*
             * make 3 partitions:
             * 1. bytes up to next sector boundary
             * 2. complete sectors
             * 3. to end of area
             * if start and end of area are within the same sector,
             * l3 is used.
             */

            if ((f->FilePointer >> f->Dev->DevData.SectorShift) == ((f->FilePointer+Length) >> f->Dev->DevData.SectorShift))
            {
                l1 = 0;
                l2 = 0;
                l3 = Length;
            }
            else
            {
                l3 = (f->FilePointer+Length) - RTFRoundDown((f->FilePointer+Length), f->Dev->DevData.SectorSize);
                l1 = RTFRoundUp(f->FilePointer, f->Dev->DevData.SectorSize) - f->FilePointer;
                l2 = Length - l1 - l3;
            }
            
            /* Part 1 */
            if (l1 > 0)
            {
                BYTE * Data = GetBuffer(f->Dev, CLUSTER_TO_SECTOR_DIR_OFS(f->Drive, f->Cluster, f->Offset), 0);

                memcpy(DataPtr, Data + SECTOR_OFS(f->Drive, f->Offset), l1);


                *Read           += l1;
                DataPtr = (BYTE*) DataPtr + l1;
                MoveFilePointer(f, l1);
            }

            /* Part 2 */
            while (l2 > 0)
            {
                UINT       sec_length;      /* bytes in this segment */
                RTFSector  sec_amount;      /* number of sectors in segment; */
                RTFSector  sec_index;      /* sector to start writing at */

                sec_index = CLUSTER_TO_SECTOR_DIR_OFS(f->Drive, f->Cluster, f->Offset);
                sec_length = MoveFilePointerSegment(f, l2);
                sec_amount = sec_length >> f->Dev->DevData.SectorShift;

                rtf_core_read_sectors(f->Dev, DataPtr, sec_index, sec_amount, 0);

                *Read += sec_length;
                DataPtr = (BYTE*)DataPtr + sec_length;
                l2 -= sec_length;
            }

            /* Part 3 */
            if (l3 > 0)
            {
                BYTE * Data = GetBuffer(f->Dev, CLUSTER_TO_SECTOR_DIR_OFS(f->Drive, f->Cluster, f->Offset), 0);
                memcpy(DataPtr, Data + SECTOR_OFS(f->Drive, f->Offset), l3);
                MoveFilePointer(f, l3);
                *Read += l3;
            }

            
            break;
        }
        
        default:
        {
            Result = XVALUE;

            // REVIEW: add comment - for move file pointer failure
            // REVISE: Done
            if (Result != FS_FILE_TOO_LARGE &&
                f && ((InitialFilePointer + *Read) != f->FilePointer) )  /* for move file pointer failure */
            {
                XTRY
                {
                    case XCODE:
                        InitFilePointer(f);
                        MoveFilePointer(f, InitialFilePointer + *Read);
                        break;
                    default:
                        if (Result == FS_NO_ERROR)
                            Result = XVALUE;
                        XHANDLED;
                        break;
                    case XFINALLY:
                        break;
                }
                XENDX
            }
            
            break;
        }
        case XFINALLY:
        {
            break;
        }
    }
    XEND_API

return Result;
}

     
int fs_bl_write(FS_HANDLE FileHandle, void * DataPtr, UINT Length, UINT * Written)
{
   register RTFile * f;
   RTFile * volatile f1 = NULL;
   UINT l1, l2, l3;
   kal_uint32 volatile InitialFilePointer;
   int volatile Result = RTF_NO_ERROR;
   UINT DummyWritten;
   int fs_block_type = FS_BLOCK_ENUM; /* Fix multi-access bug, Karen Hsu, 2004/07/08, ADD */

   kal_bool volatile flush_chain_head = KAL_FALSE;
   kal_uint32        extend_file_options = 0;

   if (Written == NULL)
      Written = &DummyWritten;
  *Written = 0;

   XTRY
      case XCODE:
         f1 = f = ParseFileHandleEx(FileHandle, FS_NONBLOCK_MODE);



         InitialFilePointer = f->FilePointer;

         if(f->Valid == FS_FH_ABORTED)
         {
            //fs_util_trace_err_noinfo(MT_ABORTED_ERROR);

            XRAISE(MT_ABORTED_ERROR);
         }

         /* check RTF_READ_ONLY */
         if (f->Flags & RTF_READ_ONLY)
         {
          //  fs_util_trace_err_noinfo(MT_READ_ONLY_ERROR);

            XRAISE(MT_READ_ONLY_ERROR);
         }

         /* invalid file pointer, greater than file size */
         if ((f->SpecialKind == NormalFile) && (f->FilePointer > f->DirEntry.Dir.FileSize))
         {
          //  fs_util_trace_err_noinfo(RTF_INVALID_FILE_POS);
          //  fs_util_trace_info2(NULL, FS_ERR_RTFWRITE_FPTR_OUT_OF_RANGE, f->FilePointer, f->DirEntry.Dir.FileSize, NULL);

            XRAISE(RTF_INVALID_FILE_POS);
         }

         /* file pointer will wrap around, file is too large */
         if ((f->FilePointer + Length) < f->FilePointer)
         {
         //   fs_util_trace_err_noinfo(RTF_INVALID_FILE_POS);
         //   fs_util_trace_info2(NULL, FS_ERR_RTFWRITE_FPTR_WRAP_AROUND, f->FilePointer, Length, NULL);

            XRAISE(RTF_FILE_TOO_LARGE);
         }

         /* Save speical flag to TLS.
            Note. FS_NONBLOCK_MODE will overwrite FS_BLOCK_PROTECTION_ENUM, we don't support
            "non-blocking recoverable write" */

         if(f->Flags & FS_NONBLOCK_MODE)
            RTFSYSSetTLS(TLSBlockIndex,(_XData*)FS_NON_BLOCK_ENUM);
         else if (f->Flags & FS_PROTECTION_MODE)
            RTFSYSSetTLS(TLSBlockIndex,(_XData*)FS_BLOCK_PROTECTION_ENUM);

  

         // this file handle points to a NORMAL file

         // ROBUSTNESS: FAT table will be first flushed to disk for file
         //             without any cluster chain allocated before to avoid
         //             dangling FirstCluster in dir entry.         (W11.28)

         if (f->DirEntry.Dir.FileSize == 0 && f->Cluster == 0)
         {
            extend_file_options = FS_COMMITTED;
         }

         // make 3 partitions:
         // 1. bytes up to next sector boundary
         // 2. complete sectors
         // 3. to end of area
         // if start and end of area are within the same sector,
         // l3 is used.

         if ((f->Cluster == RTF_ROOT_DIR) && ((f->FilePointer + Length) > f->DirEntry.Dir.FileSize))
         {


            XRAISE(RTF_ROOT_DIR_FULL);
         }

         if ((f->FilePointer >> f->Dev->DevData.SectorShift) == ((f->FilePointer+Length) >> f->Dev->DevData.SectorShift))
         {
            l1 = 0;
            l2 = 0;
            l3 = Length;
         }
         else
         {
            l3 = (f->FilePointer+Length) - RTFRoundDown((f->FilePointer+Length), f->Dev->DevData.SectorSize);
            l1 = RTFRoundUp(f->FilePointer, f->Dev->DevData.SectorSize) - f->FilePointer;
            l2 = Length - l1 - l3;
         }

         /* get speical flag from TLS (nonblocking mode or recoverable write) */
         fs_block_type = (int)RTFSYSGetTLS(TLSBlockIndex); /* Remove RVCT warning, Karen Hsu, 2004/11/02, MOD*/

         if (l1 > 0)
         {
            BYTE * Data = GetBuffer(f->Dev, CLUSTER_TO_SECTOR_DIR_OFS(f->Drive, f->Cluster, f->Offset), 0);

            memcpy(Data + SECTOR_OFS(f->Drive, f->Offset), DataPtr, l1);

            SetDirty(f->Dev);
            SetRecoverableFlag(f->Dev, fs_block_type);

            /* if RTF_LAZY_DATA, RTF_CACHE_DATA must be set to keep data in buffer (done in RTFOpenFile) */
            if ((f->Flags & RTF_LAZY_DATA) == 0)
               CommitBuffer(f->Dev);

            if ((f->Flags & RTF_CACHE_DATA) == 0)
               DiscardBuffer(f->Dev); // discard committed buffer

            *Written        += l1;
            DataPtr = (BYTE*) DataPtr + l1;
            MoveFilePointer(f, l1);
         }
         while (l2 > 0)
         {
            UINT       n;      // number of bytes in segment
            RTFSector  S;      // sector to start writing at
            RTFSector  C;      // number of sectors in segment;



            if (f->Cluster == 0) // end of file, allocate more
            {
               ExtendFile(f, l2 + l3, extend_file_options);

               extend_file_options = 0; // reset extend_file_options to avoid repeatedly committing FAT tables in ExtendFile()
            }

            S = CLUSTER_TO_SECTOR_DIR_OFS(f->Drive, f->Cluster, f->Offset);
            n = MoveFilePointerSegment(f, l2);
            C = n >> f->Dev->DevData.SectorShift;

            // can write C sectors starting at S, but have to check
            // for any data in buffers

            DiscardBuffersRange(f->Dev, S, C, 0);   // discard without commit

            /* l2 writes will not involve cache */
            if (fs_block_type == FS_BLOCK_PROTECTION_ENUM)
               rtf_core_write_sectors(f->Dev, DataPtr, S, C, 0, RTF_RECOVERABLE_WRITE);
            else
               rtf_core_write_sectors(f->Dev, DataPtr, S, C, 0, RTF_NORMAL_WRITE);

            *Written        += n;
            DataPtr = (BYTE*) DataPtr + n;
            l2 -= n;
         }
         if (l3 > 0)
         {
            RTFSector S;
            UINT Offset = SECTOR_OFS(f->Drive, f->Offset);
            UINT NeedOldSector = (f->DirEntry.Dir.FileSize > f->FilePointer) || Offset;
            BYTE * Data;

            if (f->Cluster == 0)
            {
               ExtendFile(f, l3, extend_file_options);

               extend_file_options = 0; // reset extend_file_options to avoid repeatedly committing FAT tables in ExtendFile()
            }

            S = CLUSTER_TO_SECTOR_DIR_OFS(f->Drive, f->Cluster, f->Offset);
            MoveFilePointer(f, l3);

            Data = GetBuffer(f->Dev, S, NeedOldSector ? 0 : NO_LOAD);
            memcpy(Data + Offset, DataPtr, l3);
            SetDirty(f->Dev);
            SetRecoverableFlag(f->Dev, fs_block_type);

            *Written += l3;

            if (f->Flags & RTF_COMMITTED)
               CommitBuffer(f->Dev);
         }
         break;
      default:
         Result = XVALUE;
         if ((Result != RTF_FILE_TOO_LARGE) &&
             f1 && ((InitialFilePointer + *Written) != f1->FilePointer)
             || Result == MT_FLASH_ERASE_BUSY)
         XTRY
            case XCODE:
               InitFilePointer(f1);
               MoveFilePointer(f1, InitialFilePointer + *Written);
               break;
            default:
               XHANDLED;   // ignore further exceptions
               break;
            case XFINALLY:
               break;
         XENDX
         break;
      case XFINALLY:
         if (f1)
         {
            #ifdef __FS_TRACE_SUPPORT_OPEN_HINT__
            if (((InitialFilePointer + *Written) != f1->FilePointer))
                fs_err_internal_fatal(FS_ERR_HANDLE_04, NULL);
            #endif

            if (((f1->DirEntry.Dir.Attributes & RTF_ATTR_DIR) == 0)) // do not update dirs
            {
               if (f1->FilePointer > f1->DirEntry.Dir.FileSize)
               {
                  if (f1->DirEntry.Dir.FileSize == 0)
                  {
                     flush_chain_head = KAL_TRUE;
                  }

                  f1->DirEntry.Dir.FileSize = f1->FilePointer;

                  #ifdef __FS_SMART_FILE_SEEKER__
                  if (f1 && f1->usr_ptr_type == FS_TBL_USR_PTR_MILESTONE && f1->usr_ptr)
                  {
                     MTSH_ReCalHintData(f1);
                  }
                  #endif
               }

               f1->DirEntry.Dir.Attributes |= RTF_ATTR_ARCHIVE;

               #ifdef __FS_TRACE_SUPPORT_OPEN_HINT__
               #if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
               #endif
               #endif
            }

            XTRY
               case XCODE:
               	  // update write time
                  RTFSYSGetDateTime(&f1->DirEntry.Dir.DateTime);
                  UpdateDirEntry(f1);

                  // FAT flush is here (update only once after whole RTFWrite is done)
                  if (f1->Flags & RTF_COMMITTED)
                  {
                     FlushFAT(f1->Dev);
                  }

                  if (flush_chain_head)
                  {
                      /*
                       * For sudden power lose.
                       * Force to commit dir and first fat entry
                       */

                      FlushAllBuffersInGeometryOrder(f1->Dev);

                      flush_chain_head = KAL_FALSE;
                  }

                  break;
               default:
                  if (Result == RTF_NO_ERROR)
                      Result = XVALUE;
                  XHANDLED;
                  break;
               case XFINALLY:
                  // because TLS is task's view, we have to reset TLS to default: blocking mode.
                  if((f1->Flags & FS_NONBLOCK_MODE) || (f1->Flags & FS_PROTECTION_MODE))
                     RTFSYSSetTLS(TLSBlockIndex,(_XData*)FS_BLOCK_ENUM);
                  break;
            XENDX

         }
         break;
   XEND_API

   return Result;
}



int fs_bl_delete(const WCHAR * FileName)
{
   RTFile *volatile 		f = NULL;
   volatile int 			start_fat_deletion = 0;
#ifdef __FS_OPEN_HINT__
   RTFDirLocation *volatile hint = NULL;
#endif

   XTRY
	  case XCODE:

		 f = ParseFileName((char *)FileName);



		 /**
		  * 1. Add RTF_OPEN_SHARED flag to prevent ACCESS DENIED issue if SearchFile()
		  * stays in some folders too long (W09.28)
		  *
		  * NOTE. Before delete entry, CheckNotOpen(f) will promise the deleted file
		  * is not opened.
		  *
		  * 2. Add RTF_COMMITTED to ensure deleted dir entry be flushed before FAT table. (W10.37)
		  */
		 f->Flags = RTF_READ_WRITE | RTF_OPEN_SHARED | RTF_COMMITTED;



     #ifdef __FS_OPEN_HINT__
		 hint = MTHintGet(FileName, MT_HINT_ACTION_FORCE_NO_NEW);
		 if (!SearchFile(f, SEARCH_FILES | SEARCH_RELEASE_LOCK, (char *)FileName, hint))
     #else
		 if (!SearchFile(f, SEARCH_FILES | SEARCH_RELEASE_LOCK, (char *)FileName, NULL))
     #endif /* __FS_OPEN_HINT__ */
		 {
		

			XRAISE(RTF_FILE_NOT_FOUND);
		 }

		 // After SearchFile() find this file, device lock should not be released to other tasks.
		 // Thus RTF_OPEN_SHARED flag does not have to be clear.
		 // f->Flags &= ~RTF_OPEN_SHARED;

		// CheckNotOpen(f);

		 if (f->DirEntry.Dir.Attributes & (RTF_ATTR_DIR | RTF_ATTR_VOLUME))
		 {
		

			XRAISE(MT_READ_ONLY_ERROR);
		 }

		 DeleteDirEntry(f);   // RTF_COMMITTED flag will ensure deleted SFN entry be flushed here!

     #ifdef __FS_OPEN_HINT__
		 /*
		  * internal hint does not support relative path currently,
		  * invalidate all possible hints if relative path is used.
		  */
		 if (FileName[1] != L':' || FileName[2] != L'\\')
			MTHintExpireByDrive(f->Drive);
		 else
		 {
			if (hint)
			{
           #ifdef __FS_TEST__
			   fs_assert_local(MTHintGetMatchType(hint) != MT_HINT_MATCH_TARGET_END);
           #endif

			   // delete perfect matched hint
			   if (MTHintGetMatchType(hint) == MT_HINT_MATCH_PERFECT)
			   {
				  MTHintDelete(MTHintGetIndex(hint));
			   }
			}
		 }
     #endif /* __FS_OPEN_HINT__ */

		 if (FIRST_FILE_CLUSTER(f->DirEntry.Dir) != 0)
		 //quota
		 {
			start_fat_deletion = 1;
			FATDelete(f->Drive, FIRST_FILE_CLUSTER(f->DirEntry.Dir), f);
		 }

		// if (((f->Dev->DeviceFlags & RTF_DEVICE_LAZY_WRITE) == 0) || (FileCount(f->Dev) <= 1))
			FlushAllBuffers(f->Dev);

		 break;
	  default:
		 if (start_fat_deletion && XVALUE == RTF_FAT_ALLOC_ERROR)
			XHANDLED;
		 break;
	  case XFINALLY:

		// if (f) FreeFTSlotAndDevice(f);

		 break;
   XEND_API
   return RTF_NO_ERROR;
}

int fs_bl_sanitycheck(void)
{
   int		   Result = RTF_NO_ERROR;
   int		   drvLetter;
   RTFDevice   *Dev = NULL;
   int		   fh, i;
   WCHAR	   path[4] = {'X', ':', '\\', 0};

   /* get the device that contains system drive */
   Dev = &gFS_DeviceList[gFS_DeviceNumOfSYSDRV - 1];

   if (sanitycheckcount ==0)
   /* RUN ONCE Block */
   {
	  RTFInit();

	  //--- Drive Remapping ---

	  drvLetter = MTGetDrive(MT_DRIVE_I_SYSTEM, 1, MT_NO_ALT_DRIVE);

	  /* make 'Z' drive letter as system drive alias */
	  if (drvLetter > 0)
	  {
		 MTMappingDrive(drvLetter, 90);
	  }

	  if (Dev->DevData.ErrorCondition < 0)		{
		  Result = Dev->DevData.ErrorCondition;
		  // Possible Errors are
		  // FS_FDM_USER_DRIVE_BROKEN
		  // FS_FDM_SYS_DRIVE_BROKEN
		  // FS_FDM_MULTIPLE_BROKEN
		  // FS_FDM_VERSION_MISMATCH
	  }

	  if (Result == RTF_NO_ERROR)
	  {
		// print_bootup_trace_enter(SST_INIT_RTFSANITY);
		// Result = RTFSanityCheck();
		// print_bootup_trace_exit(SST_INIT_RTFSANITY);

#ifdef __FS_QM_SUPPORT__
	//	 if (stack_query_boot_mode() != USBMS_BOOT)
	//	 {

			SweepDrive('Z');
	//	 }
#endif /* __FS_QM_SUPPORT__ */
	  }


	  /*
	   * Try to mount all non-removable drives to HasFileSystem stage by opening "root directory"
	   * NOTE: Do not use opening "volume file" because drive will be set as Unmounted after "volume file" is closed. (W09.52)
	   */
	  for (i = 0; i < FS_MAX_DRIVES; i++)
	  {
		 if (gFS_Data.DriveTable[i].Dev == NULL)
		 {
			break;
		 }

		 /*
		  * Previously drive should be mounted as Mounted in ScanPartitionTable()->InitLogicalDrive()
		  * System drive should be mounted as HasFileSystem in RTFCheckBootUpFlag()
		  */
		 if ((gFS_Data.DriveTable[i].Dev->DeviceFlags & (FS_DEVICE_NAND_FLASH | FS_DEVICE_NOR_FLASH)) &&
			 (gFS_Data.DriveTable[i].MountState < HasFileSystem))
		 {
			path[0] = i + MT_BASE_DRIVE_LETTER;

			fh = FS_Open(path, FS_READ_ONLY);	// open the root dir to mount drive

			if (fh > FS_NO_ERROR)
			{
			   FS_Close(fh);
			}
		 }
	  }
   }
   sanitycheckcount = 1;//just a symbol ,make the santiycheck just do once


   return Result;
}
int fs_bl_seek(FS_HANDLE FileHandle, int Offset, int Whence)
{
    RTFile *volatile f = NULL;
    DWORD            StartPos = 0;
    DWORD volatile   NewPos;
    int              Result = FS_NO_ERROR;

    XTRY
    {
        case XCODE:
        {
            f = gFS_Data.FileTable;

            if (f->FilePointer > f->DirEntry.Dir.FileSize)
            {
                XRAISE(FS_INVALID_FILE_POS);
            }


            switch (Whence)
            {
                case FS_FILE_BEGIN:
                {
                    if (f->SpecialKind == FileMapFile)
                    {
                    StartPos = f->DirEntry.ShortPos.Index; /* BIAS */
                    }
                    else
                    {
                    StartPos = 0;
                    }

                    break;
                }
                case FS_FILE_CURRENT:
                {
                    StartPos = f->FilePointer;
                    break;
                }
                case FS_FILE_END:
                {
                    StartPos = f->DirEntry.Dir.FileSize;
                    break;
                }
            }

            Offset = (kal_int64)StartPos + Offset;

            if (Offset < 0 || Offset >= ((kal_int64)2 << 32))  // new offset can't exceed 4 GB (FAT spec)
            {
                XRAISE(FS_INVALID_FILE_POS);
            }

            NewPos = (DWORD)Offset;

            if (NewPos > f->DirEntry.Dir.FileSize)
            {
                XRAISE(FS_INVALID_FILE_POS);
            }
            
            if (NewPos < f->FilePointer) /* Looking backward */
            {
                /* we need to go back at least one cluster */
                if ((f->FilePointer - NewPos) > f->Offset)
                {
                    /* clear f->FilePointer, Offset, Cluster and LastCluster */
                    InitFilePointer(f);

                    /* use traditional method to move file pointer */
                    MoveFilePointer(f, NewPos);
                }
                /* go back but still within the same cluster */
                else
                {
                    /* everything is simple, just substract f->Offset and FilePointer by offset */
                    f->Offset -= f->FilePointer - NewPos;
                    f->FilePointer -= f->FilePointer - NewPos;
                }
            }
            else if (NewPos > f->FilePointer)
            {
                DWORD NextClusterBoundary;
                DWORD l;


                NextClusterBoundary = RTFRoundUp(f->FilePointer, f->Drive->ClusterSize);

                // move to the next cluster boundary
                if (NextClusterBoundary > NewPos)
                    NextClusterBoundary = NewPos;

                l = NextClusterBoundary - f->FilePointer;

                if (l > 0)
                    MoveFilePointer(f, l);

                l = NewPos - f->FilePointer;

                while (l > 0)
                {
                    if (f->Cluster == 0)
                    {
                        XRAISE(FS_INVALID_FILE_POS);
                    }

                    MoveFilePointerSegment(f, l);
                    l = NewPos - f->FilePointer;
                }

            }

            break;
        }
        default:
        {
            Result = XVALUE;
            break;
        }
        case XFINALLY:
        {
            break;
        }
    }
    XEND_API

    if (Result < FS_NO_ERROR)
        return (kal_int64)Result;
    else
        return (kal_int64)NewPos;
}


kal_int32 fs_bl_get_size(FS_HANDLE FileHandle, UINT * Size)
{
    RTFile * f = gFS_Data.FileTable;

    *Size = f->DirEntry.Dir.FileSize;

    return FS_NO_ERROR;
}


