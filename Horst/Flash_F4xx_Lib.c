/**
  * ############################################################################
  * @file     Flash_F4xx_Lib.c
  * @brief    New Vario
  * @author   Horst Rupp
  * @brief    This library supports programming the flash memory
  *           provides the routines to jump to
  *           * application
  *           * coldstart
  *           * system boot loader
  *
  * ############################################################################
  */
//
// Includes
//
#include "Flash_F4xx_Lib.h"
#if ( defined BUILD_AD57_FE )
  #include "Translator_Tables.h"   // for txt_BLhex
#endif

#if 0  //<-------- copied from h.file  -------- just as info and  memeo help
  typedef struct
  {
    uint32_t    first_addr_of_sector;
    uint32_t    last_addr_of_sector;
    uint32_t    size_of_sector;
    uint32_t    available_flash_space;
  } sector_descr_t;
#endif

//
//  Sector description table for STM32F4xx
//
ROM sector_descr_t Sector_Table[12] =
{ //first addr   last addr  sec_size  remaining_flash_size                secno
  { 0x08000000, 0x08003FFF, 16*1024,  7*128*1024 + 64*1024 + 4*16*1024 }, //  0
  { 0x08004000, 0x08007FFF, 16*1024,  7*128*1024 + 64*1024 + 3*16*1024 }, //  1
  { 0x08008000, 0x0800BFFF, 16*1024,  7*128*1024 + 64*1024 + 2*16*1024 }, //  2
  { 0x0800C000, 0x0800FFFF, 16*1024,  7*128*1024 + 64*1024 + 16*1024   }, //  3
  { 0x08010000, 0x0801FFFF, 64*1024,  7*128*1024 + 64*1024             }, //  4
  { 0x08020000, 0x0803FFFF, 128*1024, 7*128*1024                       }, //  5
  { 0x08040000, 0x0805FFFF, 128*1024, 6*128*1024                       }, //  6
  { 0x08060000, 0x0807FFFF, 128*1024, 5*128*1024                       }, //  7
  { 0x08080000, 0x0809FFFF, 128*1024, 4*128*1024                       }, //  8
  { 0x080A0000, 0x080BFFFF, 128*1024, 3*128*1024                       }, //  9
  { 0x080C0000, 0x080DFFFF, 128*1024, 2*128*1024                       }, //  10
  { 0x080E0000, 0x080FFFFF, 128*1024, 128*1024                         }  //  11
};


  COMMON  uint8_t  My_SectorNum;
  COMMON  uint32_t My_CurrentFlashWritePosition;
  COMMON  uint32_t My_CurrentFlashReadPosition;

  #define   c_min_lower_addr_space_sector    0
  #define   c_max_lower_addr_space_sector    7
  #define   c_min_upper_addr_space_sector    8
  #define   c_max_upper_addr_space_sector   11

  #define   c_blink_alive_limit     50
  #define   c_buffersize_inbytes    256
  #define   c_buffersize_in32bit    ( c_buffersize_inbytes / 4 )

  //#define   c_APP_ADDR      0x08080000

//
// =============================================================================
//
//  1. Set Sector Adress
//
//  Setup internal addressing
//  and return available flash space
//
uint32_t  Flash_F4xx_SetSectorAddrs ( uint8_t secno )
{
  My_SectorNum                  = secno;
  My_CurrentFlashWritePosition  = Sector_Table[secno].first_addr_of_sector;
  My_CurrentFlashReadPosition   = Sector_Table[secno].first_addr_of_sector;

  return Sector_Table[secno].available_flash_space;
}

//
// =============================================================================
//  2. Erase Sector
//
//  This function is unaware of the sector size
//  yet erases the sector appropriately
//
void Flash_F4xx_EraseSector ( uint8_t secno )
{
  HAL_FLASH_Unlock();
  FLASH_Erase_Sector ( secno, FLASH_VOLTAGE_RANGE_3);
  HAL_FLASH_Lock();
}

//
// =============================================================================
//  3. Write Flash
//
//  Writes a buffer of bytes to flash
//
void Flash_F4xx_Write ( void * wrBuf, uint32_t size_in_chunks_of_32bit )
{
  HAL_StatusTypeDef l_ret_code;
  uint16_t          l_flash_repeat_count;
  HAL_FLASH_Unlock();

  for ( uint32_t i = 0; i < size_in_chunks_of_32bit; i++ )
  {
    l_flash_repeat_count  = 0;
    l_ret_code            = HAL_ERROR;
    while ( ( l_ret_code != HAL_OK ) && ( l_flash_repeat_count++ < 100 ) )
      l_ret_code = HAL_FLASH_Program ( FLASH_TYPEPROGRAM_WORD,
                                       My_CurrentFlashWritePosition,
                                       ((uint32_t *)wrBuf)[i]
                                     );
    My_CurrentFlashWritePosition += 4;
  }
  HAL_FLASH_Lock();
}

//
// =============================================================================
//
//  4. Read Flash
//
//  Reads a buffer of bytes from flash
//
void Flash_F4xx_Read ( void * rdBuf, uint32_t size_in_chunks_of_32bit )
{
  uint32_t i;
  HAL_FLASH_Unlock();

  for (i = 0; i < size_in_chunks_of_32bit; i++ )
  {
    *((uint32_t *)rdBuf + i) = *(uint32_t *)My_CurrentFlashReadPosition;
    My_CurrentFlashReadPosition += 4;
  }

  HAL_FLASH_Lock();
}

// ****************************************************************************
//
// 5a. SHORT Test Application Health
//
//    Check for g_MutableAppSignature.sMagicNumber   == c_Magic_Number
//    Direct addressing of block which is expected to contain the signature
//    Read this buffer ...
//    Then check for l_buffer.usign.sMagicNumber == c_Magic_Number
//    Then check for l_buffer.usign.sC_R_C       == g_MutableAppSignature.sC_R_C
//    Then check for l_buffer.usign.sSizeInBytes == g_MutableAppSignature.sSizeInBytes
//    Then check for l_buffer.usign.sVersion     == g_MutableAppSignature.sVersion
//
uint8_t AD57_EEProm_Only_Health_Check ( void )
{
  #if ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL )
  {
    buffer_t    l_buffer;
    uint32_t    l_bytesread;
    uint8_t     l_health = c_nok;
    uint32_t    l_max_block_count;


    //  Obtain EEProm signature
    //  Check EEProm application data block
    //
    Read_Appl_Sign_Data_from_EEProm ( );

    //
    //  Check if magic number ok
    //
    if ( g_MutableAppSignature.sMagicNumber == c_Magic_Number )
    {
      //  Set flasher module internal addresses
      //
      //uint32_t  l_flashable_space =
                     Flash_F4xx_SetSectorAddrs ( c_min_upper_addr_space_sector );

      //  Size of image counted in buffers
      //
      //  NOTE: sSizeInBytes is guaranteed to be an EXACT multiple of
      //  c_buffer_size -- it is written by Generic_Signature_SelfSign()
      //  (or AfterBurner.py) as the exact byte offset of the .signature
      //  block from APP_ADDRESS, and the .signature section is placed by
      //  the linker at ALIGN(256). Do NOT add "+ 1" here: sSizeInBytes
      //  already IS the block offset of the signature -- adding one more
      //  block jumps one block PAST it and the magic number is never
      //  found there.
      //
      l_max_block_count = g_MutableAppSignature.sSizeInBytes / c_buffer_size;
      //
      //  Instead of walking through the file, here we go to the respective
      //  block by direct addressing
      //
      My_CurrentFlashReadPosition += l_max_block_count * c_buffer_size;




      //  read bytewise the last chunk of flash data
      //
      l_bytesread = c_buffer_size;
      acquire_privileges();
      Flash_F4xx_Read ( l_buffer.u32, l_bytesread/4 );        // read uint32_t
      drop_privileges();

      //  Check if magic number resides in this block of flash data
      //
      if  (
            ( l_buffer.usign.sMagicNumber == c_Magic_Number )
          &&
            ( l_buffer.usign.sSizeInBytes == g_MutableAppSignature.sSizeInBytes )
          &&
            ( l_buffer.usign.sC_R_C       == g_MutableAppSignature.sC_R_C )
          &&
            ( l_buffer.usign.sVersion     == g_MutableAppSignature.sVersion )
          )
        l_health = c_ok;

    }
    return l_health;
  }
  #endif
  return c_nok;  // Should never occur
}

//
// ****************************************************************************
//
//  5b. FULL Test Application Health
//
//    Check for g_MutableAppSignature.sMagicNumber   == c_Magic_Number
//    Walk through flash and read blocks until
//        l_buffer.usign.sMagicNumber != c_Magic_Number
//      Then check locally derived CRC             == l_buffer.usign.sC_R_C
//      Then check locally derived CRC             == g_MutableAppSignature.sC_R_C
//      Then check for l_buffer.usign.sC_R_C       == g_MutableAppSignature.sC_R_C
//      Then check for l_buffer.usign.sSizeInBytes == g_MutableAppSignature.sSizeInBytes
//      Then check for l_buffer.usign.sVersion     == g_MutableAppSignature.sVersion
//      Then check for l_size_of_flash_data        == l_buffer.usign.sSizeInBytes
//      Then check for l_size_of_flash_data        == g_MutableAppSignature.sSizeInBytes
//
uint8_t AD57_CRC_Plus_EEProm_Health_Check ( void )
{
  #if ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL )
  {
    uint16_t    l_q_result;
    buffer_t    l_buffer;
    uint32_t    l_bytesread;
    uint16_t    l_crc = 0;
    uint32_t    l_size_of_flash_data = 0;
    uint8_t     l_magic_number_found_in_flash = 0;
    uint8_t     l_health = c_nok;
    uint32_t    l_block_count = 0;
    uint32_t    l_max_block_count;

    //  Obtain EEProm signature
    //  Check EEProm application data block
    //
    Read_Appl_Sign_Data_from_EEProm ( );

    //
    //  Check if magic number ok
    //
    if ( g_MutableAppSignature.sMagicNumber == c_Magic_Number )
    {
      //  Set flasher module internal addresses
      //
      uint32_t l_flashable_space =
                    Flash_F4xx_SetSectorAddrs ( c_min_upper_addr_space_sector );

      //  Size of image counted in buffers
      //
      l_max_block_count = g_MutableAppSignature.sSizeInBytes / c_buffer_size + 1;


      //  Loop to read data from flash memory
      //
      l_bytesread = c_buffer_size;

      while ( ! l_magic_number_found_in_flash )
      {
        //  read bytewise in chunks of c_buffer_size
        //
        acquire_privileges();
        Flash_F4xx_Read ( l_buffer.u32, l_bytesread/4 );        // read uint32_t
        drop_privileges();
        l_block_count++;
        if ( l_block_count > l_max_block_count+1 )
          return l_health;

        //  Check if magic number resides in this block of flash data
        //
        if ( l_buffer.usign.sMagicNumber != c_Magic_Number )
        {
          //
          //  No, this is not the final block
          //  Continue with CRC
          //
          l_crc = CRC16_blockcheck_bytes( (uint8_t*) l_buffer.u8,
                                          l_crc,
                                          l_bytesread );
          l_size_of_flash_data += l_bytesread;
          //
          //  error condition
          //
          if ( l_size_of_flash_data > l_flashable_space )
            return l_health;

          Flash_LCtr_OutQItem.Status = c_verification;
          Flash_LCtr_OutQItem.ToDo = l_max_block_count;
          Flash_LCtr_OutQItem.Done = l_block_count;
          l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                                    &Flash_LCtr_OutQItem,
                                    portMAX_DELAY );
          ASSERT ( l_q_result == 1 );

          //vTaskDelay( 100 );
        }
        else
        {
          l_magic_number_found_in_flash = true;
          //
          //  Yes, this is the last chunk which holds the signature
          //  Special treatment
          //  Check the final CRC against EEProm
          //  Check size  against EEProm
          //
          l_crc = CRC16_blockcheck_bytes( (uint8_t*) l_buffer.u8,
                                          l_crc,
                                          sizeof ( AppSignature_t )-4 );
          l_size_of_flash_data += sizeof ( AppSignature_t );
          if  (
                ( l_crc == l_buffer.usign.sC_R_C )
              &&
                ( l_crc == g_MutableAppSignature.sC_R_C )
              &&
                ( l_size_of_flash_data == l_buffer.usign.sSizeInBytes )
              &&
                ( l_size_of_flash_data == g_MutableAppSignature.sSizeInBytes )
              )
            l_health = c_ok;
        }
      }
    }
    return l_health;
  }
  #endif
  return c_nok;  // Should never occur
}
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
//
//  6. BL on AD57, read from uSD, flash local application
//
void BL_on_AD57_Read_SD_Flash_Local_Appl ( uint16_t  dir_list_index )
{
  #if ( defined BUILD_AD57_BL )
  {
    uint16_t    l_sd_result;
    uint16_t    l_result = 0;
    uint16_t    l_q_result;
    uint8_t     l_filename[256];
    static FIL  hex_input_file;
    uint32_t    l_bytesread = 0;
    buffer_t    buffer1;
    buffer_t    l_buffer;
    uint8_t     l_feof;
    uint32_t    l_count_size = 0;
    uint16_t    l_crc = 0;
    uint32_t    l_filesize;

    //  Step 1 : Open the hex input file to be flashed
    //
    strcpy ( (char*) l_filename, (char*) g_DirListItems[dir_list_index] );
    l_sd_result = SD_Card_File_Open_4_Read ( &hex_input_file, (uint8_t*) l_filename );
    ASSERT ( l_sd_result == SD_OK );

    //  Step 2 : Retrieve size of hex input file to be flashed
    //
    l_filesize = SD_Card_File_Size ( &hex_input_file );

    //  Step 3 : Set flasher module internal addresses
    //
    uint32_t  l_flashable_space = Flash_F4xx_SetSectorAddrs ( c_min_upper_addr_space_sector );

    //  Step 4 : Check if hex file fits into flashable space
    //
    if ( l_filesize > l_flashable_space )
    {
      Flash_LCtr_OutQItem.Status = c_flash_not_enough_flash_space;
      l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                                &Flash_LCtr_OutQItem,
                                portMAX_DELAY );
      ASSERT ( l_q_result == 1 );
    }
    //  Step 5 : Loop to erase sectors
    //
    uint8_t j = c_max_upper_addr_space_sector - c_min_upper_addr_space_sector;

    Flash_LCtr_OutQItem.Status = c_erase_progress;
    Flash_LCtr_OutQItem.ToDo = j;
    Flash_LCtr_OutQItem.Done = 0;
    l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                              &Flash_LCtr_OutQItem,
                              portMAX_DELAY );
    ASSERT ( l_q_result == 1 );

    vTaskDelay( 100 );

    for (uint8_t  i = c_min_upper_addr_space_sector;
                  i <= c_max_upper_addr_space_sector;
                  i++ )
    {
      Flash_F4xx_EraseSector( i );
      Flash_LCtr_OutQItem.Status = c_erase_progress;
      Flash_LCtr_OutQItem.ToDo = j;
      Flash_LCtr_OutQItem.Done = i - c_min_upper_addr_space_sector;
      l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                                &Flash_LCtr_OutQItem,
                                portMAX_DELAY );
      ASSERT ( l_q_result == 1 );

      vTaskDelay( 100 );

    }

    //  Step 6 : Flash Programming
    //

    //  Loop to read data to be flashed
    //
    l_feof  = 0;

    while ( l_feof == 0 )
    {
      //
      //  Sequence is
      //    Read
      //    Flash
      //    Verify
      //
      //  read bytewise in chunks of c_buffer_size bytes
      //  ( 256 bytes == 64 uint32_t )
      //
      l_sd_result = SD_Card_File_Read ( &hex_input_file,
                                        buffer1.u8,
                                        c_buffer_size,
                                        &l_bytesread );
      ASSERT ( ( l_sd_result == SD_OK ) || ( l_sd_result == SD_EOF ) );
      if ( l_bytesread == 0 )
          l_feof = 1;
      else
      {
        //
        // and flash that chunk of data @ address <My_CurrentFlashWritePosition>
        // Address <My_CurrentFlashWritePosition> is incremented inside
        // Flash_F4xx_Write
        //
        acquire_privileges();
        Flash_F4xx_Write ( buffer1.u32, l_bytesread/4 );    // write 64 * uint32_t
        drop_privileges();

        l_count_size += l_bytesread;

        Flash_LCtr_OutQItem.Status = c_flash_progress;
        Flash_LCtr_OutQItem.ToDo = l_filesize;
        Flash_LCtr_OutQItem.Done = l_count_size;
        l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                                  &Flash_LCtr_OutQItem,
                                  portMAX_DELAY );
        ASSERT ( l_q_result == 1 );

        vTaskDelay( 1 );
        //
        // Then read that chunk of data again from flash and
        // compare with original input
        // Reading starts @ address <My_CurrentFlashReadPosition>
        // Address <My_CurrentFlashReadPosition> is incremented inside
        // Flash_F4xx_Read
        //
        acquire_privileges();
        Flash_F4xx_Read ( l_buffer.u32, l_bytesread/4 );        // read uint32_t
        drop_privileges();
        l_result = portable_memcmp (  (uint8_t *) buffer1.u8,
                                      (uint8_t *) l_buffer.u8,
                                      l_bytesread );
        if ( l_result != 0 )
        {
          //
          //  verification failure
          //
          Flash_LCtr_OutQItem.Status = c_flash_verification_failure;
          l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                                    &Flash_LCtr_OutQItem,
                                    portMAX_DELAY );
          ASSERT ( l_q_result == 1 );

          ASSERT ( l_result == 0 );
        }

        //
        //  check CRC
        //
        if ( l_bytesread != sizeof ( AppSignature_t ) )
        {
          //
          //  checking all other chunks but last (normal case)
          //
          l_crc = CRC16_blockcheck_bytes( (uint8_t*) l_buffer.u8,
                                          l_crc,
                                          l_bytesread );
        }
        else
        {
          //
          //  checking last chunk which holds the signature
          //
          l_crc = CRC16_blockcheck_bytes (  (uint8_t*) l_buffer.u8,
                                            l_crc,
                                            l_bytesread-4 );
          //l_bytesread = 23;    // left as a debug artifact in only one of
                                  // four project copies -- commented out for
                                  // consistency; confirm intent before re-enabling
          l_crc = l_crc - l_buffer.u32[3];
        }
      }
    }

    if (l_crc != 0 )
    {
      Flash_LCtr_OutQItem.Status = c_flash_CRC_failure;
      l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                                &Flash_LCtr_OutQItem,
                                portMAX_DELAY );
      ASSERT ( l_q_result == 1 );

      ASSERT ( l_crc == 0 );
    }

    /* Step 8 : Finalize Programming */
    l_sd_result = SD_Card_File_Close ( &hex_input_file );


  }
  #endif
}

//
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  7. Appl on AD57, read from uSD, flash local BL
//
void Appl_on_AD57_Read_SD_Flash_Local_BL ( void )
{
  #if ( defined BUILD_AD57_FE )
  {

    uint16_t    l_sd_result;
    uint16_t    l_result = 0;
    uint8_t     l_filename[256];
    static FIL  hex_input_file;
    uint32_t    l_bytesread = 0;
    buffer_t    buffer1;
    buffer_t    l_buffer;
    uint8_t     l_feof;
    uint32_t    l_count_size = 0;
    uint16_t    l_crc = 0;

    //  Step 1 : Open the hex input file to be flashed
    //
    strcpy ( (char*) l_filename, (char*) txt_BLhex );
    l_sd_result = SD_Card_File_Open_4_Read ( &hex_input_file, (uint8_t*) l_filename );
    ASSERT ( l_sd_result == SD_OK );

    //  Step 2 : Retrieve size of hex input file to be flashed
    //
    //uint32_t  l_filesize = SD_Card_File_Size ( &hex_input_file );

    //  Step 3 : Set flasher module internal addresses
    //
    //uint32_t  l_flashable_space =
        Flash_F4xx_SetSectorAddrs ( c_min_lower_addr_space_sector );

    //  Step 4 : Check if hex file fits into flashable space
    //
    //  if ( l_filesize > l_flashable_space )
    //  {
    //  }
    //  Step 5 : Loop to erase sectors
    //
    for (uint8_t i = c_min_lower_addr_space_sector; i <= c_max_lower_addr_space_sector; i++ )
    {
      Flash_F4xx_EraseSector( i );

      vTaskDelay( 1 );
    }

    //  Loop to read data to be flashed
    //
    l_feof  = 0;

    while ( l_feof == 0 )
    {
      //
      //  read bytewise in chunks of c_buffer_size bytes
      //
      l_sd_result = SD_Card_File_Read ( &hex_input_file,
                                        buffer1.u8,
                                        c_buffer_size,
                                        &l_bytesread );
      ASSERT ( ( l_sd_result == SD_OK ) || ( l_sd_result == SD_EOF ) );
      if ( l_bytesread == 0 )
          l_feof = 1;
      else
      {
        //
        //  and flash that chunk of data
        //
        acquire_privileges();
        Flash_F4xx_Write ( buffer1.u32, l_bytesread/4 );    // write uint32_t
        drop_privileges();

        l_count_size += l_bytesread;

        vTaskDelay( 1 );
        //
        //  Then read that chunk of data again from flash and compare with original input
        //
        acquire_privileges();
        Flash_F4xx_Read ( l_buffer.u32, l_bytesread/4 );        // read uint32_t
        drop_privileges();
        l_result = portable_memcmp (  (uint8_t *) buffer1.u8,
                                      (uint8_t *) l_buffer.u8,
                                      l_bytesread );
        if ( l_result != 0 )
        {
          //
          //  verification failure
          //
          ASSERT ( l_result == 0 );
        }

        //
        //  check CRC
        //
        if ( l_bytesread != sizeof ( AppSignature_t ) )
        {
          //
          //  checking all other chunks but last (normal case)
          //
          l_crc = CRC16_blockcheck_bytes( (uint8_t*) l_buffer.u8,
                                          l_crc,
                                          l_bytesread );
        }
        else
        {
          //
          //  checking last chunk which holds the signature
          //
          l_crc = CRC16_blockcheck_bytes (  (uint8_t*) l_buffer.u8,
                                            l_crc,
                                            l_bytesread-4 );
          //l_bytesread = 23;
          l_crc = l_crc - l_buffer.u32[3];
        }
      }
    }

    if (l_crc != 0 )
    {
      ASSERT ( l_crc == 0 );
    }

    /* Step 8 : Finalize Programming */
    l_sd_result = SD_Card_File_Close ( &hex_input_file );
  }
  #endif

}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  8.  BL on AD57, read from uSD, push to CAN, flash remote application
//
void BL_on_AD57_Read_SD_Push_CAN_2_Flash_Remote_Appl ( uint16_t p_id, uint16_t  dir_list_index )
{
  #if ( defined BUILD_AD57_BL )
  {
    uint16_t    l_sd_result;
    uint16_t    l_q_result;
    uint32_t    l_bytesread = 0;
    static FIL  hex_input_file;
    buffer_t    buffer1;
    uint32_t    l_count_size = 0;
    uint8_t     l_filename[256];
    uint32_t    l_filesize;
    //uint16_t    l_Cnt_of_Chunks_to_Push;
    uint16_t    l_No_Of_Current_Chunk;
    uint16_t    l_Magic_No_Found;
    //uint8_t     l_termination;

    //
    //  Step 0 : Get the attention of the resp. F4_Generic_BootLoader
    //            If the resp application is active,
    //            that application must relinquish control to the BL.
    //            This done by issuance of the CAN trigger
    //

    //
    //  Step 1 :  Open hex file of application to be transferred via CAN
    //            and flashed in remote system
    //
    strcpy ( (char*) l_filename, (char*) g_DirListItems[dir_list_index] );
    l_sd_result = SD_Card_File_Open_4_Read ( &hex_input_file, (uint8_t*) l_filename );
    ASSERT ( l_sd_result == SD_OK );

    //  Step 2 :  Retrieve size of hex input file to be transferred via CAN
    //            Calc no of 256-byte chunks to be transferred via CAN
    //            Align by 256 bytes
    //
    l_filesize  = SD_Card_File_Size ( &hex_input_file ) + ( c_buffer_size - 1 );

    //  Step 3 :  Initiate CAN transfer tunnel
    //
    Start_MDP_on_Remote_Board_via_CAN  ( p_id );

    Flash_LCtr_OutQItem.Status = c_wait_reboot;
    Flash_LCtr_OutQItem.ToDo = 0;
    Flash_LCtr_OutQItem.Done = 0;
    l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                              &Flash_LCtr_OutQItem,
                              portMAX_DELAY );
    ASSERT ( l_q_result == 1 );

#if ( defined BUILD_F4_GenBL ) || ( defined BUILD_F4_P_UTIL )
    vTaskDelay( 100 );
    vTaskDelay( 2000 );
#else
    vTaskDelay( 10 ); // Just to provoke a task switch
#endif

    //  Step 4 :  Loop to read data to be pushed via CAN
    //
    l_No_Of_Current_Chunk = 0;
    l_Magic_No_Found      = false;
    //l_termination         = false;
    while ( ! l_Magic_No_Found  )
    {
      //
      //  read bytewise in chunks of c_buffer_size bytes
      //
      portable_memset  ( (uint8_t*) buffer1.u8, 0xff, c_buffer_size );
      l_sd_result = SD_Card_File_Read ( &hex_input_file,
                                        buffer1.u8,
                                        c_buffer_size,
                                        &l_bytesread );
      ASSERT ( ( l_sd_result == SD_OK ) || ( l_sd_result == SD_EOF ) );


      //  Check if magic number resides in this block of flash data
      //
      if ( buffer1.usign.sMagicNumber == c_Magic_Number )
        l_Magic_No_Found = true;

      //
      //  and send that chunk of data to satellite
      //
      if ( ! g_CAN_Bus_Util_Data_Xfer_established )
      {
#if ( defined BUILD_F4_GenBL ) || ( defined BUILD_F4_P_UTIL )
        while ( ! g_CAN_Bus_Util_Data_Xfer_established )
          vTaskDelay( 50 );
        vTaskDelay( 1000 );
#else
        while ( ! g_CAN_Bus_Util_Data_Xfer_established )
          vTaskDelay( 10 );
#endif
      }

      //  Check if magic number resides in this block of flash data
      //
      if ( buffer1.usign.sMagicNumber == c_Magic_Number )
        l_Magic_No_Found = true;

      //
      // After <l_Magic_No_Found> there is only one more data xfer
      //
      MDP_Send_Data_via_CAN ( p_id + 0x10, buffer1.u8, c_buffer_size );
      //
      //  This bunch will xfer the magic number,
      //  so the receiving side does not expect any more data xfers after that.

      l_count_size += l_bytesread;

        Flash_LCtr_OutQItem.Status = c_flash_progress;
        Flash_LCtr_OutQItem.ToDo = l_filesize;
        Flash_LCtr_OutQItem.Done = l_count_size;
        l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                                  &Flash_LCtr_OutQItem,
                                  portMAX_DELAY );
        ASSERT ( l_q_result == 1 );

    }
    //
    //  Step 5 :  Terminate data transfer
    //
    buffer1.u32[0] = 0;

    //Send_MDP_Data_via_CAN ( p_id + 0x20, (uint8_t*)&buffer1.u32, 4 );

  }
  #endif
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  9.  BL on AD57, read from uSD, Push to CAN, flash remote BL
//
void BL_on_AD57_Read_SD_Push_CAN_2_Flash_Remote_BL ( uint16_t  dir_list_index )
{
  #if ( defined BUILD_AD57_BL )
  {
    uint16_t    l_sd_result, l_q_result;
    uint32_t    l_bytesread = 0;
    static FIL  hex_input_file;
    buffer_t    buffer1;
    uint8_t     l_feof;
    uint32_t    l_filesize;
    uint8_t     l_filename[256];
    uint16_t    l_count_size;

    //  Step 1 : Open the hex input file to be flashed
    //
    strcpy ( (char*) l_filename, (char*) "F4_BL.hex" );
    l_sd_result = SD_Card_File_Open_4_Read ( &hex_input_file, (uint8_t*) l_filename );
    ASSERT ( l_sd_result == SD_OK );

    //  Step 2 : Retrieve size of hex input file to be flashed
    //
    l_filesize = SD_Card_File_Size ( &hex_input_file ) + ( c_buffer_size - 1 );
    //uint16_t l_no_256_byte_chunks = l_filesize / c_buffer_size;

    MDP_Send_Data_via_CAN ( c_CID_AD57_Requests_Audio_CAN_MDP_Initialize, buffer1.u8, l_bytesread );



    //  Loop to read data to be pushed via CAN
    //
    l_feof  = 0;

    while ( l_feof == 0 )
    {
      //
      //  read bytewise in chunks of c_buffer_size bytes
      //
      l_sd_result = SD_Card_File_Read ( &hex_input_file,
                                        buffer1.u8,
                                        c_buffer_size,
                                        &l_bytesread );
      ASSERT ( ( l_sd_result == SD_OK ) || ( l_sd_result == SD_EOF ) );
      if ( l_bytesread == 0 )
        l_feof = 1;
      else
      {
        //
        //  and send that chunk of data to satellite
        //
        MDP_Send_Data_via_CAN ( c_CID_AD57_Requests_Audio_CAN_MDP_Initialize, buffer1.u8, l_bytesread );

        l_count_size += l_bytesread;

        Flash_LCtr_OutQItem.Status = c_flash_progress;
        Flash_LCtr_OutQItem.ToDo = l_filesize;
        Flash_LCtr_OutQItem.Done = l_count_size;
        l_q_result = xQueueSend ( Flash_LCtr_OutQId,
                                  &Flash_LCtr_OutQItem,
                                  portMAX_DELAY );
        ASSERT ( l_q_result == 1 );
      }
    }
  }
  #endif
}

//
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  10.  BL on F4, read from CAN, flash local application
//
void BL_on_F4_Read_CAN_Flash_Local_Appl ( void )
{
  #if ( defined BUILD_F4_GenBL )
  {
    uint16_t    l_result = 0;
    uint32_t    l_bytesread = 0;
    buffer_t    buffer1;
    buffer_t    l_buffer;
    uint16_t    l_crc_1 = 0;
    uint16_t    l_crc_2 = 0;
    uint16_t    l_No_Of_Current_Chunk;

    uint16_t    l_Magic_No_Found = false;

    //  ----------------- Preamble -------------------------------------------------------
    //
    //  1.  Licht aus
    //
    HAL_GPIO_WritePin ( LED_Port, LEDgreen_left_Pin, GPIO_PIN_RESET );

    //
    //  2.  Set flasher module internal write and read addresses to low boundary of upper address space
    //      <<My_CurrentFlashWritePosition> -- <<My_CurrentFlashReadPosition>>
    //
    //uint32_t  l_flashable_space = Flash_F4xx_SetSectorAddrs ( c_min_upper_addr_space_sector );
    Flash_F4xx_SetSectorAddrs ( c_min_upper_addr_space_sector );

    //
    //  3.  Perform erasing of upper address space
    //
    for (uint8_t i = c_min_upper_addr_space_sector; i <= c_max_upper_addr_space_sector; i++ )
    {
      Flash_F4xx_EraseSector( i );
    }

    //  ---------------- Loop -------------------------------------------------
    //
    //  4.  Loop to receive and flash data
    //
    l_No_Of_Current_Chunk = 0;        // no check on chunk numbers !!!!!!
    l_Magic_No_Found      = false;
    while ( ! l_Magic_No_Found  )
    {
      //  .................. Fireworks ........................................
      //
      //  4a.  Green left LED flashing indicates CAN data reception
      //
      if ( l_No_Of_Current_Chunk / 8 * 8 != l_No_Of_Current_Chunk )
      {
        HAL_GPIO_WritePin ( LED_Port, LEDgreen_left_Pin, GPIO_PIN_SET );
      }
      else
      {
        HAL_GPIO_WritePin ( LED_Port, LEDgreen_left_Pin, GPIO_PIN_RESET );
      }

      //  ........ Receive data ...............................................
      //
      //  4b.   Collect data for one 256 byte chunk
      //    First data block should be pending already !!!!
      //
      #ifdef MDP_TIMER_LIB
        Timer_Set ( cc_timer_3 );
      #endif

      if ( ! MDP_Receive_Data_via_CAN ( (uint8_t*) buffer1.u8,
                                        c_buffersize_inbytes ) )
        my_break_point;

      l_No_Of_Current_Chunk++;

      //  ........ Flash data .................................................
      //
      //  4c. Flash this chunk of data to upper address space at
      //      <My_CurrentFlashWritePosition>
      //
      // write 64 * uint32_t
      //
      #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
        acquire_privileges();
      #endif
      Flash_F4xx_Write ( buffer1.u32, c_buffersize_in32bit );    // write 64 * uint32_t
      #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
        drop_privileges();
      #endif

      //  ........ Verify data by mem compare..................................
      //
      //  4d. Then read that chunk of data again from flash at
      //      <My_CurrentFlashReadPosition> and compare with original input
      //

      // read uint32_t
      //
      #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
        acquire_privileges();
      #endif
      Flash_F4xx_Read ( l_buffer.u32, c_buffersize_in32bit );
      #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
        drop_privileges();
      #endif

      //  ........... mem compare .............................................
      //
      l_result = portable_memcmp (  (uint8_t *) buffer1.u8,
                                    (uint8_t *) l_buffer.u8,
                                    l_bytesread );
      if ( l_result != 0 )
      {
        //  mem compare verification failure
        //
        my_break_point;         // TODO
      }

      //  ........ Verify data by CRC .........................................
      //
      //
      //  4e. CRC on flashed input data
      //
      l_crc_1 = CRC16_blockcheck_bytes( (uint8_t*) buffer1.u8,
                                      l_crc_1,
                                      l_bytesread );

      //
      //  4f. CRC on re-read data
      //
      l_crc_2 = CRC16_blockcheck_bytes( (uint8_t*) l_buffer.u8,
                                      l_crc_2,
                                      l_bytesread );

      //
      //  4g. compare crc verification failure
      //
      if ( l_crc_1 != l_crc_2 )
      {
        my_break_point;
      }

      #ifdef MDP_TIMER_LIB
        Timer_Get_Current_Value ( cc_timer_3 );
      #endif

      //  ........ Magic Number ??? ...........................................
      //
      //  4h. Check if magic number resides in this block of flash data
      //
      if ( buffer1.usign.sMagicNumber == c_Magic_Number )
      {
        l_Magic_No_Found = true;
        //
        // After <l_Magic_No_Found> no more data xfer is expected
        //
      }

    }

    //  ........... End of loop  //  Licht aus ................................
    //
    HAL_GPIO_WritePin ( LED_Port, LEDgreen_left_Pin, GPIO_PIN_RESET );

    //  .......................................................................
    //
    //  buffer1 contains signature data
    //  Store that buffer into last flash region of lower address space
    //
    F4_Write_Signature_Sector_7 ( (AppSignature_t *) l_buffer.u32 );

  }
  #endif
}

//
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  11.  Appl on F4, read from CAN, flash local BL
//
void Appl_on_F4_Read_CAN_Flash_Local_BL ( void )
{
  #if ( defined BUILD_F4_P_UTIL_ )    // TODO
  {
    uint16_t    l_sd_result;
    uint16_t    l_result = 0;
    uint32_t    l_bytesread = 0;
    buffer_t    buffer1;
    buffer_t    l_buffer;
    uint8_t     l_feof;
    uint32_t    l_count_size = 0;
    uint16_t    l_crc = 0;
    uint32_t    l_filesize;

    //  Step 1 : Open the hex input file to be flashed
    //
    strcpy ( (char*) l_filename, (char*) txt_BLhex );
    l_sd_result = SD_Card_File_Open_4_Read ( &hex_input_file, (uint8_t*) l_filename );
    ASSERT ( l_sd_result == SD_OK );

    //  Step 2 : Retrieve size of hex input file to be flashed
    //
    uint32_t  l_filesize = SD_Card_File_Size ( &hex_input_file );

    //  Step 3 : Set flasher module internal addresses
    //
    uint32_t  l_flashable_space = Flash_F4xx_SetSectorAddrs ( c_min_lower_addr_space_sector );

    //  Step 4 : Check if hex file fits into flashable space
    //
    if ( l_filesize > l_flashable_space )
    {
    }
    //  Step 5 : Loop to erase sectors
    //
    for (uint8_t i = c_min_lower_addr_space_sector; i <= c_max_lower_addr_space_sector; i++ )
    {
      Flash_F4xx_EraseSector( i );

      vTaskDelay( 1 );
    }

    //  Step 6 : Flash Programming
    //
    //vTaskDelay( 10 );

    //  Loop to read data to be flashed
    //
    l_feof  = 0;

    while ( l_feof == 0 )
    {
      //
      //  read bytewise in chunks of c_buffer_size bytes
      //
      l_sd_result = SD_Card_File_Read ( &hex_input_file,
                                        buffer1.u8,
                                        c_buffer_size,
                                        &l_bytesread );
      ASSERT ( ( l_sd_result == SD_OK ) || ( l_sd_result == SD_EOF ) );
      if ( l_bytesread == 0 )
          l_feof = 1;
      else
      {
        //
        //  and flash that chunk of data
        //
        acquire_privileges();
        Flash_F4xx_Write ( buffer1.u32, l_bytesread/4 );    // write uint32_t
        drop_privileges();

        l_count_size += l_bytesread;

        vTaskDelay( 1 );
        //
        //  Then read that chunk of data again from flash and compare with original input
        //
        acquire_privileges();
        Flash_F4xx_Read ( l_buffer.u32, l_bytesread/4 );        // read uint32_t
        drop_privileges();
        l_result = portable_memcmp (  (uint8_t *) buffer1.u8,
                                      (uint8_t *) l_buffer.u8,
                                      l_bytesread );
        if ( l_result != 0 )
        {
          //
          //  verification failure
          //
          ASSERT ( l_result == 0 );
        }

        //
        //  check CRC
        //
        if ( l_bytesread != sizeof ( AppSignature_t ) )
        {
          //
          //  checking all other chunks but last (normal case)
          //
          l_crc = CRC16_blockcheck_bytes( (uint8_t*) l_buffer.u8,
                                          l_crc,
                                          l_bytesread );
        }
        else
        {
          //
          //  checking last chunk which holds the signature
          //
          l_crc = CRC16_blockcheck_bytes (  (uint8_t*) l_buffer.u8,
                                            l_crc,
                                            l_bytesread-4 );
          //l_bytesread = 23;
          l_crc = l_crc - l_buffer.u32[3];
        }
      }
    }

    if (l_crc != 0 )
    {
      ASSERT ( l_crc == 0 );
    }

    /* Step 8 : Finalize Programming */
    l_sd_result = SD_Card_File_Close ( &hex_input_file );
  }
  #endif
}

// *****************************************************************************
//
//  12. Jump to Application
//
void Bootloader_JumpToApplication(void)
{
  //
  vTaskDelay ( 100 );

  //  Test application health
  //
  #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
    acquire_privileges();
  #endif

  uint32_t  JumpAddress = *(__IO uint32_t*)(APP_ADDRESS + 4);
  FPTR program_start = (FPTR)(JumpAddress);

  /*  Stop RTOS activity */
  portDISABLE_INTERRUPTS();

  /*
   *  A software jump is NOT a hardware reset: the NVIC's interrupt
   *  enable and pending bits survive untouched, unlike a real
   *  power-cycle where the core clears them as part of the reset
   *  sequence. If e.g. a CAN interrupt is still enabled and happens
   *  to be pending at the moment SCB->VTOR below switches to the
   *  target image's vector table, it fires straight into the target's
   *  ISR before that image has run its own peripheral init -- silent
   *  hard fault / hang, no crash dump, nothing to recover it.
   *
   *  Disable and clear everything so the target starts exactly as if
   *  it had come up from a real reset.
   */
  for ( uint8_t i = 0; i < 8; i++ )   // covers the full NVIC IRQ range on F4
  {
    NVIC->ICER[ i ] = 0xFFFFFFFF;   // disable all external interrupts
    NVIC->ICPR[ i ] = 0xFFFFFFFF;   // clear all pending flags
  }

  /*
   *  NVIC->ICER/ICPR above only covers external peripheral IRQs.
   *  PendSV and SysTick are core exceptions with their pending bits
   *  in SCB->ICSR, a completely separate mechanism. FreeRTOS routinely
   *  leaves PendSV pending to request the next context switch; if it's
   *  still pending when BASEPRI is dropped back to 0 below, it fires
   *  immediately -- mid-jump, against a scheduler that's being torn
   *  down. Clear both before unmasking.
   */
  SCB->ICSR |= ( SCB_ICSR_PENDSVCLR_Msk | SCB_ICSR_PENDSTCLR_Msk );
  __DSB();
  __ISB();

  /*
   *  Leave the BL's restricted core state behind before handing over.
   *  The BL runs under FreeRTOS-MPU: BASEPRI may be raised and the MPU
   *  regions in effect describe the BL's own layout (0x08000000 space),
   *  not the app's (APP_ADDRESS space). If left in place, the very first
   *  fetch/access after the jump faults with no handler left to catch it
   *  -> dark screen, nothing happens.
   */
  __set_BASEPRI(0);
  MPU->CTRL = 0;
  __DSB();
  __ISB();

  __set_CONTROL(0);   // privileged thread mode, MSP (leave BL's PSP/unprivileged mode)
  __ISB();

  /** stop I/O */
  HAL_RCC_DeInit();
  HAL_DeInit();

  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  SCB->VTOR = APP_ADDRESS;

  __set_MSP(*(__IO uint32_t*) APP_ADDRESS);
  __ISB();
  program_start();
}

//
// =============================================================================
//
//  13. Jump to ColdStart
//
void Bootloader_JumpToColdStart(void)
{
  #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
    acquire_privileges();
  #endif

  uint32_t  JumpAddress = *(__IO uint32_t*)(COLD_START_ADDRESS + 4);
  FPTR program_start = (FPTR)(JumpAddress);

  /*  Stop RTOS activity */
  portDISABLE_INTERRUPTS();

  /*
   *  See Bootloader_JumpToApplication() above: a software jump is not
   *  a hardware reset, so stale NVIC enable/pending state from this
   *  image survives into GenBL unless explicitly cleared here. GenBL
   *  defines its own CAN1_RX0/SCE handlers; a leftover pending CAN
   *  interrupt firing before GenBL's own CAN_Init() has run is a
   *  silent hard fault / hang with no diagnostics and no watchdog to
   *  recover it -- exactly the "doesn't come back until power-cycle"
   *  symptom this was chasing.
   */
  for ( uint8_t i = 0; i < 8; i++ )   // covers the full NVIC IRQ range on F4
  {
    NVIC->ICER[ i ] = 0xFFFFFFFF;   // disable all external interrupts
    NVIC->ICPR[ i ] = 0xFFFFFFFF;   // clear all pending flags
  }

  /*
   *  NVIC->ICER/ICPR above only covers external peripheral IRQs.
   *  PendSV and SysTick are core exceptions with their pending bits
   *  in SCB->ICSR, a completely separate mechanism. FreeRTOS routinely
   *  leaves PendSV pending to request the next context switch; if it's
   *  still pending when BASEPRI is dropped back to 0 below, it fires
   *  immediately -- mid-jump, against a scheduler that's being torn
   *  down. Clear both before unmasking.
   */
  SCB->ICSR |= ( SCB_ICSR_PENDSVCLR_Msk | SCB_ICSR_PENDSTCLR_Msk );
  __DSB();
  __ISB();

  __set_BASEPRI(0);
  MPU->CTRL = 0;
  __DSB();
  __ISB();

  __set_CONTROL(0);
  __ISB();

  /** stop I/O */
  HAL_RCC_DeInit();
  HAL_DeInit();

  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  SCB->VTOR = COLD_START_ADDRESS;

  __set_MSP(*(__IO uint32_t*) COLD_START_ADDRESS);
  __ISB();
  program_start();

}

//
// =============================================================================
//
//  16. Check Signature in upper-most sector of lower address space
//
uint16_t F4_Check_Signature_Sector_7 ( void )
{
  uint16_t    l_health = c_ok;

  #ifdef BUILD_F4_GenBL
  {
    //uint16_t    l_q_result;
    buffer_t    buffer1;
    buffer_t    l_buffer;
    uint32_t    l_bytesread;
    //uint16_t    l_crc = 0;
    //uint32_t    l_size_of_flash_data = 0;
    uint8_t     l_magic_number_found_in_flash = false;
    uint32_t    l_max_block_count;
    uint32_t    l_block_count = 0;

    //  Set flasher module internal addresses for application in upper space
    //
    uint32_t l_flashable_space = Flash_F4xx_SetSectorAddrs ( c_min_upper_addr_space_sector ); // 0x08080000

    //  Size in blocks
    //
    l_max_block_count = ( l_flashable_space / c_buffer_size );

    //  Loop to read data from upper memory flash until magic number is found
    //
    l_bytesread = c_buffer_size;
    while ( ! l_magic_number_found_in_flash )
    {
      //  read bytewise in chunks of c_buffer_size
      //
      #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ) )
        acquire_privileges();
      #endif

      Flash_F4xx_Read ( buffer1.u32, c_buffersize_in32bit );

      #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ) )
        drop_privileges();
      #endif

      // beware of overflow  // TODO
      //
      l_block_count++;
      if ( l_block_count >= l_max_block_count )
      {
        l_health = c_nok;
        return l_health;
      }

      //  Check if magic number resides in this block of flash data
      //
      if ( buffer1.usign.sMagicNumber == c_Magic_Number )
      {
        l_magic_number_found_in_flash = true;
      }
    }

    //  Set flasher module internal addresses in last segment of lower space
    //
    l_flashable_space = Flash_F4xx_SetSectorAddrs ( c_max_lower_addr_space_sector ); // TODO #2#2#2#2#2

    //  READ bytewise in chunks of c_buffer_size
    //
    #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
      acquire_privileges();
    #endif

    Flash_F4xx_Read ( l_buffer.u32, c_buffersize_in32bit );        // read uint32_t

    #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
      drop_privileges();
    #endif

    //  Check if magic number resides in this block of flash data
    //
    if ( l_buffer.usign.sMagicNumber != c_Magic_Number )
    {
      l_health = c_nok;
      return l_health;
    }

    //  Check if versions are alike
    //
    if ( buffer1.usign.sVersion != l_buffer.usign.sVersion  )
    {
      l_health = c_nok;
      return l_health;
    }
  }
  #endif
  return l_health;
}
//
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  17.  On F4, write Signature into last sector of lower address space
//
void F4_Write_Signature_Sector_7 ( AppSignature_t * Signature )
{
  #if ( defined BUILD_F4_GenBL ) || ( defined BUILD_F4_P_UTIL )
  {
    //
    //  Set sector addresses for
    //  last flash region of lower address space
    //  <<My_CurrentFlashWritePosition> -- <<My_CurrentFlashReadPosition>>
    //
    Flash_F4xx_SetSectorAddrs ( c_max_lower_addr_space_sector ); // TODO #1#1#1#1

    //  Perform erasing on that sector
    //
    Flash_F4xx_EraseSector( c_max_lower_addr_space_sector );

    // WRITE signature
    //
    #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
      acquire_privileges();
    #endif

    Flash_F4xx_Write ( Signature, c_Size_Signature );

    #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
      drop_privileges();
    #endif

    #ifdef FLASH_READ_BACK
      // READ the chunk containing signature again
      //
      #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
        acquire_privileges();
      #endif
      for ( uint8_t i = 0; i < c_Size_Signature; i++ )
      {
        Signature->sMagicNumber = 0;
        Signature->sVersion     = 0;
        Signature->sSizeInBytes = 0;
        Signature->sC_R_C       = 0;
      }

      Flash_F4xx_Read ( Signature, c_buffersize_in32bit );

      #if ( ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL ))
        drop_privileges();
      #endif
    #endif

    vTaskDelay( 1000 );
  }
  #endif
}

//
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  17b. On F4, self-sign the app's OWN .signature block with a REAL,
//       recomputed CRC + size -- no separate sector involved.
//
//  Unlike F4_Write_Signature_Sector_7() above (which writes an externally
//  supplied signature into sector 7 of the LOWER address space, for the
//  unrelated "locally flashed BL image" feature in
//  Appl_on_F4_Read_CAN_Flash_Local_BL()), this function patches the
//  sSizeInBytes/sC_R_C words that live INSIDE the app image itself, at
//  &ThisApplicationsSignature[2]/[3] in the UPPER address space
//  (APP_ADDRESS .. ). That is the one and only place GenBL's
//  Check_Application_In_Upper_Address_Space() ever looks when deciding
//  whether to auto-start the app -- it scans forward from APP_ADDRESS in
//  c_buffer_size chunks until it finds c_Magic_Number, then checks the
//  CRC/size that follow it. Writing anywhere else (e.g. sector 7) is
//  invisible to that check.
//
//  Algorithm (mirrors AfterBurner.py's own handling of the .bin file, and
//  AD57_CRC_Plus_EEProm_Health_Check()'s validation on the receiving end):
//    1. Chained CRC16_blockcheck_bytes() over the whole app image
//       (APP_ADDRESS .. start of the .signature block), in c_buffer_size
//       chunks.
//    2. CRC continued over the final 12 bytes: magic + version + size.
//
//  Flash-write constraint this relies on: sSizeInBytes/sC_R_C in
//  Generic_Signature.c must be initialised to 0xFFFFFFFF (erased state),
//  NOT an arbitrary placeholder such as the old 0xF1F1F1F1/0xE2E2E2E2.
//  STM32 flash can only program erased (all-1) words to an arbitrary
//  value without an erase cycle; a non-erased placeholder can't be
//  overwritten with a real CRC/size without erasing the whole sector,
//  which would destroy the app code living in the same sector. Because
//  AfterBurner.py patches the .bin file itself (before it's ever
//  flashed), it is unaffected by this and can still write real values
//  into that same 0xFFFFFFFF placeholder before flashing. If this
//  function finds the words already non-erased, it assumes the image was
//  already signed (by AfterBurner.py, or by this function on a previous
//  boot) and leaves it untouched rather than risk a doomed/half-written
//  program.
//
//  Use this in place of the old call
//      F4_Write_Signature_Sector_7 ( &ThisApplicationsSignature );
//  Replace with:
//      Generic_Signature_SelfSign();
//
//  ASSUMPTION guarded by the ASSERT below: the signature block sits on a
//  c_buffer_size (0x100) boundary, i.e. the app image size is always an
//  exact multiple of c_buffer_size -- guaranteed by the .signature
//  section's ALIGN(256) in the linker script, combined with APP_ADDRESS
//  (0x08080000) itself being 256-byte aligned.
//
extern uint32_t ThisApplicationsSignature[4];   // defined in Generic_Signature.c
                                                 // (remove this line if Generic_Signature.h
                                                 //  already declares it -- avoids a duplicate)

void Generic_Signature_SelfSign ( void )
{
  #if ( defined BUILD_F4_GenBL ) || ( defined BUILD_F4_P_UTIL )
  {
    buffer_t    l_buffer;     // re-used to read the app image, 256 bytes at a time
    buffer_t    l_sigbuf;     // holds the 12-byte header + recomputed CRC
    uint16_t    l_crc         = 0;
    uint32_t    l_bytes_total;
    uint32_t    l_remaining;

    //
    //  0.  If the size/CRC words are not in the erased (0xFFFFFFFF) state,
    //      this image has already been signed -- either by AfterBurner.py
    //      before flashing, or by this function on a previous boot.
    //      Programming over a non-erased word with a different value
    //      would silently corrupt it (flash can only clear bits, not set
    //      them, without an erase), so bail out instead.
    //
    if ( ( ThisApplicationsSignature[2] != 0xFFFFFFFF ) ||
         ( ThisApplicationsSignature[3] != 0xFFFFFFFF ) )
    {
      return;    // already signed -- nothing to do
    }

    //
    //  1.  Size of the app image == address of the signature block minus
    //      the app's actual start address in flash.
    //
    //  NOTE: ThisApplicationsSignature is declared as uint32_t[4] in
    //  Generic_Signature.c -- index [0]=magic, [1]=version,
    //  [2]=size placeholder, [3]=CRC placeholder.
    //
    //  APP_ADDRESS (0x08080000, from Flash_F4xx_Lib.h) is the real start
    //  of the app image for every active linker script in this project
    //  (STM32F407VGTX_FLASH.ld and both *_relocated_*.ld variants all set
    //  FLASH ORIGIN = 0x08080000; only the unused *_not_relocated.ld
    //  starts at 0x08000000).
    //
    l_bytes_total = ( (uint32_t) &ThisApplicationsSignature[0] ) - APP_ADDRESS;

    //
    //  2.  Recompute the running CRC over the whole app image, exactly as
    //      AfterBurner.py does over the .bin file, chunk by chunk.
    //      Sector 8 (c_min_upper_addr_space_sector) starts at APP_ADDRESS
    //      -- the same region GenBL's own boot-time check scans.
    //
    Flash_F4xx_SetSectorAddrs ( c_min_upper_addr_space_sector );
    // My_CurrentFlashReadPosition now points at APP_ADDRESS

    l_remaining = l_bytes_total;
    while ( l_remaining >= c_buffersize_inbytes )
    {
      Flash_F4xx_Read ( l_buffer.u32, c_buffersize_in32bit );
      l_crc        = CRC16_blockcheck_bytes ( l_buffer.u8, l_crc, c_buffersize_inbytes );
      l_remaining -= c_buffersize_inbytes;
    }

    //  l_remaining should be 0 here -- see 0x100-boundary assumption above.
    //
    ASSERT ( l_remaining == 0 );

    //
    //  3.  Build the signature header (magic + version + size) and
    //      continue the SAME running CRC over those 12 bytes -- mirrors
    //      AfterBurner.py's final-chunk handling.
    //
    l_sigbuf.u32[0] = c_Magic_Number;
    l_sigbuf.u32[1] = ThisApplicationsSignature[1];          // VERSION_TXT6, already correct
    l_sigbuf.u32[2] = l_bytes_total;

    l_crc = CRC16_blockcheck_bytes ( l_sigbuf.u8, l_crc, 12 );

    l_sigbuf.u32[3] = l_crc;

    //
    //  4.  Commit the real size + CRC in place, directly into the app's
    //      own .signature block. No erase: the words are confirmed
    //      erased (0xFFFFFFFF) by the check in step 0, so a plain word
    //      program is sufficient and the rest of the app image (which
    //      shares the same physical sector) is left untouched.
    //
    My_CurrentFlashWritePosition = (uint32_t) &ThisApplicationsSignature[2];
    Flash_F4xx_Write ( &l_sigbuf.u32[2], 2 );    // writes sSizeInBytes + sC_R_C only
  }
  #endif
}

//
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  18.  On F4, clear Signature in last sector of lower address space
//
void F4_Clear_Signature_Sector_7 ( void )
{
  #if ( defined BUILD_F4_GenBL ) || ( defined BUILD_F4_P_UTIL )
  {
    //
    //  Set sector addresses for wipe clear function
    //  last flash region of lower address space
    //  <<My_CurrentFlashWritePosition> -- <<My_CurrentFlashReadPosition>>
    //
    Flash_F4xx_SetSectorAddrs ( c_max_lower_addr_space_sector );

    //  Perform erasing on that sector
    //
    Flash_F4xx_EraseSector( c_max_lower_addr_space_sector );

    vTaskDelay( 1000 );
  }
  #endif
}

//
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  nn. Check upper address space for app health
//
//    Walk through flash and read blocks until
//        l_buffer.usign.sMagicNumber != c_Magic_Number
//      Then check locally derived CRC             == l_buffer.usign.sC_R_C
//
uint8_t Check_Application_In_Upper_Address_Space ( void )
{
  #ifdef BUILD_F4_GenBL
  {
    buffer_t    l_buffer;
    uint32_t    l_bytesread;
    uint16_t    l_crc = 0;
    uint32_t    l_size_of_flash_data = 0;
    uint8_t     l_magic_number_found_in_flash = 0;
    uint8_t     l_health = c_nok;
    uint32_t    l_block_count = 0;
    uint32_t    l_max_block_count;

    //  Set flasher module internal addresses
    //
    uint32_t l_flashable_space =
                  Flash_F4xx_SetSectorAddrs ( c_min_upper_addr_space_sector );

    //  Size of image counted in buffers
    //
    l_max_block_count = l_flashable_space / c_buffer_size;

    //  Loop to read data from flash memory
    //
    l_bytesread = c_buffer_size;

    while ( ! l_magic_number_found_in_flash )
    {
      //  read bytewise in chunks of c_buffer_size
      //
      //  Bounds check BEFORE the read: on a blank/erased upper address
      //  space the magic number is never found, and reading past
      //  l_max_block_count walks off the physical end of flash -> BusFault.
      //  (l_max_block_count must NOT carry a stray +1 here either -- see
      //  the "GenBL BusFault startup issue" session.)
      //
      if ( l_block_count >= l_max_block_count )
        return l_health;

      //  NOTE: no acquire_privileges()/drop_privileges() here -- GenBL
      //  builds with configENABLE_MPU=0 (plain, non-MPU FreeRTOS port),
      //  so xPortRaisePrivilege()/portSWITCH_TO_USER_MODE() don't exist
      //  in this project and would fail to link. Every other caller of
      //  those macros in this file is guarded to AD57_FE/AD57_BL only --
      //  this function must follow the same rule.
      //
      Flash_F4xx_Read ( l_buffer.u32, l_bytesread/4 );        // read uint32_t
      l_block_count++;

      //  Check if magic number resides in this block of flash data
      //
      if ( l_buffer.usign.sMagicNumber != c_Magic_Number )
      {
        //
        //  No, this is not the final block
        //  Continue with CRC
        //
        l_crc = CRC16_blockcheck_bytes( (uint8_t*) l_buffer.u8,
                                        l_crc,
                                        l_bytesread );
        l_size_of_flash_data += l_bytesread;
        //
        //  error condition
        //
        if ( l_size_of_flash_data > l_flashable_space )
          return l_health;

      }
      else
      {
        l_magic_number_found_in_flash = true;
        //
        //  Yes, this is the last chunk which holds the signature
        //  Special treatment
        //  Check the final CRC and size against the signature that is
        //  actually embedded in the app image itself.
        //
        //  NOTE: no g_CurrEEPromData cross-check here -- that variable
        //  is only declared for BUILD_AD57_FE/BUILD_AD57_BL (it comes
        //  from Generic_Signature_Lib.h, which GenBL never includes).
        //  GenBL has no EEPROM-stored expectation to compare against;
        //  the signature written into flash by AfterBurner.py / the
        //  app's own self-sign step is the only source of truth here.
        //
        l_crc = CRC16_blockcheck_bytes( (uint8_t*) l_buffer.u8,
                                        l_crc,
                                        sizeof ( AppSignature_t )-4 );
        l_size_of_flash_data += sizeof ( AppSignature_t );
        if  (
              ( l_crc == l_buffer.usign.sC_R_C )
            &&
              ( l_size_of_flash_data == l_buffer.usign.sSizeInBytes )
            )
          l_health = c_ok;
      }
    }
    return l_health;
  }
  #endif
  return c_nok;  // Should never occur
}

//
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  nn.  Find cause of reset
//
reset_cause_t reset_cause_get(void)
{
  reset_cause_t reset_cause;

  if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
  {
    reset_cause = RESET_CAUSE_LOW_POWER_RESET;
  }
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
  {
    reset_cause = RESET_CAUSE_WINDOW_WATCHDOG_RESET;
  }
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
  {
    reset_cause = RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET;
  }
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
  {
    // This reset is induced by calling the ARM CMSIS
    // `NVIC_SystemReset()` function!
    reset_cause = RESET_CAUSE_SOFTWARE_RESET;
  }
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
  {
    reset_cause = RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
  }
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
  {
    reset_cause = RESET_CAUSE_EXTERNAL_RESET_PIN_RESET;
  }
  // Needs to come *after* checking the `RCC_FLAG_PORRST` flag in order to
  // ensure first that the reset cause is NOT a POR/PDR reset. See note
  // below.
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
  {
    reset_cause = RESET_CAUSE_BROWNOUT_RESET;
  }
  else
  {
    reset_cause = RESET_CAUSE_UNKNOWN;
  }

  // Clear all the reset flags or else they will remain set during future
  // resets until system power is fully removed.
  __HAL_RCC_CLEAR_RESET_FLAGS();

  return reset_cause;
}

//
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
//
//  nn.  Find cause of reset by name
//
const char * reset_cause_get_name(reset_cause_t reset_cause)
{
  const char * reset_cause_name = "TBD";

  switch (reset_cause)
  {
    case RESET_CAUSE_UNKNOWN:
      reset_cause_name = "UNKNOWN";
      break;
    case RESET_CAUSE_LOW_POWER_RESET:
      reset_cause_name = "LOW_POWER_RESET";
      break;
    case RESET_CAUSE_WINDOW_WATCHDOG_RESET:
      reset_cause_name = "WINDOW_WATCHDOG_RESET";
      break;
    case RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET:
      reset_cause_name = "INDEPENDENT_WATCHDOG_RESET";
      break;
    case RESET_CAUSE_SOFTWARE_RESET:
      reset_cause_name = "SOFTWARE_RESET";
      break;
    case RESET_CAUSE_POWER_ON_POWER_DOWN_RESET:
      reset_cause_name = "POWER-ON_RESET (POR) / POWER-DOWN_RESET (PDR)";
      break;
    case RESET_CAUSE_EXTERNAL_RESET_PIN_RESET:
      reset_cause_name = "EXTERNAL_RESET_PIN_RESET";
      break;
    case RESET_CAUSE_BROWNOUT_RESET:
      reset_cause_name = "BROWNOUT_RESET (BOR)";
      break;
  }

  return reset_cause_name;
}


//
// *****************************************************************************
// ************************ EOF ************************************************
// *****************************************************************************
//
