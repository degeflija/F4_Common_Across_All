/**
 * @file  Flash_F4xx_Lib.h
 * @brief  New Vario
 * @author  Horst Rupp
 */

#ifndef __Flash_F4xx_Lib_H
  #define __Flash_F4xx_Lib_H


  //
  // ***************************************************************************
  //

  #include "Generic_Includes.h"
  #include "stm32f4xx_hal.h"

  #include  "Generic_Common.h"
  #include  "Generic_Signature.h"
  #include  "MyCRC.h"
  #if ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL )
    #include  "Generic_Signature_Lib.h"   // only needed by the EEPROM health-check functions
  #endif
  #include  "Portable_Utils.h"
  #if ( defined BUILD_F4_GenBL ) || ( defined BUILD_AD57_BL )
    #include  "CAN_MDP_Lib.h"
  #endif
  #if ( defined BUILD_AD57_FE ) || ( defined BUILD_AD57_BL )
    #include  "SD_Card_Lib.h"
  #endif
  #include  "can.h"
  #include  "Generic_CAN_Ids.h"
  #include  "FreeRTOS_wrapper.h"

  //
  // ***************************************************************************
  //
  // Typedefs
  //
  #define   c_buffer_size   256

  typedef struct
  {
    uint32_t    first_addr_of_sector;
    uint32_t    last_addr_of_sector;
    uint32_t    size_of_sector;
    uint32_t    available_flash_space;
  } sector_descr_t;


  //1. data size
  typedef enum
  {
    DATA_TYPE_8=0,
    DATA_TYPE_16,
    DATA_TYPE_32,
  }DataTypeDef;

  typedef union
  {
    uint32_t        u32 [c_buffer_size/4];
    uint8_t         u8[c_buffer_size];
    AppSignature_t  usign;
  } buffer_t;

  typedef enum reset_cause_e
  {
      RESET_CAUSE_UNKNOWN = 0,
      RESET_CAUSE_LOW_POWER_RESET,
      RESET_CAUSE_WINDOW_WATCHDOG_RESET,
      RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
      RESET_CAUSE_SOFTWARE_RESET,
      RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
      RESET_CAUSE_EXTERNAL_RESET_PIN_RESET,
      RESET_CAUSE_BROWNOUT_RESET,
  } reset_cause_t;


  // ***************************************************************************
  // ***************************************************************************
  //
  /** Start address of application space in flash */
  #define APP_ADDRESS (uint32_t)0x08080000

  /** Address of System Memory (ST Bootloader) */
  #define ST_BOOTLOADER_ADDRESS (uint32_t)0x1FFF0000

  /** Address of Own Bootloader Address == Cold Start */
  #define COLD_START_ADDRESS (uint32_t)0x08000000

  typedef int ( *FPTR)( void); // declare void* -> int function pointer

  //function prototypes

  //1. Set Sector Adress
  uint32_t  Flash_F4xx_SetSectorAddrs ( uint8_t sector );

  //2. Erase Sector
  void Flash_F4xx_EraseSector ( uint8_t secno );

  //3. Write Flash
  void Flash_F4xx_Write ( void * wrBuf, uint32_t sizein32bit );

  //4. Read Flash
  void Flash_F4xx_Read ( void * rdBuf, uint32_t sizein32bit );

  // ***************************************************************************

  //5a. SHORT Test Application Health
  //
  uint8_t AD57_EEProm_Only_Health_Check ( void );

  //5b. FULL Test Application Health
  //
  uint8_t AD57_CRC_Plus_EEProm_Health_Check ( void );

  //  6.  BL on AD57, read from uSD, flash local application
  //
  void BL_on_AD57_Read_SD_Flash_Local_Appl ( uint16_t  dir_list_index );

  //  7.  Appl on AD57, read from uSD, flash local BL
  //
  void Appl_on_AD57_Read_SD_Flash_Local_BL ( void );

  //  8.  BL on AD57, read from uSD, push to CAN, flash remote application
  //
  void BL_on_AD57_Read_SD_Push_CAN_2_Flash_Remote_Appl ( uint16_t p_id, uint16_t  dir_list_index );

  //  9.  BL on AD57, read from uSD, push to CAN, flash remote BL
  //
  void BL_on_AD57_Read_SD_Push_CAN_2_Flash_Remote_BL ( uint16_t  dir_list_index );

  //  10.	BL on F4, read from CAN, flash local application
  //
  void BL_on_F4_Read_CAN_Flash_Local_Appl ( void );

  //  **************************************************************************
  //
  //  11.  Appl on F4, read from CAN, flash local BL
  //
  void Appl_on_F4_Read_CAN_Flash_Local_BL ( void );   // TODO

  //  **************************************************************************
  //
  //  12. Jump to Application
  //
  void Bootloader_JumpToApplication(void);

  //  13. Jump to ColdStart
  //
  void Bootloader_JumpToColdStart(void);

  //  **************************************************************************
  //
  //  16. Check Signature
  //
  uint16_t F4_Check_Signature_Sector_7 ( void );

  //  17.  BL on F4, write Signature into last sector of lower address space
  //
  void F4_Write_Signature_Sector_7 ( AppSignature_t * Signature );

  //  17b. On F4, recompute a real CRC over the resident app image and
  //       self-sign sector 7 with it -- for debugger-flashed images that
  //       never went through AfterBurner.py
  //
  void Generic_Signature_SelfSign ( void );

  //  18.  BL on F4, write Signature into last sector of lower address space
  //
  void F4_Clear_Signature_Sector_7 ( void );

  //  nn. Check upper address space for proper app
  //
  //    Walk through flash and read blocks until
  //        l_buffer.usign.sMagicNumber != c_Magic_Number
  //      Then check locally derived CRC             == l_buffer.usign.sC_R_C
  //      Then check for l_size_of_flash_data        == l_buffer.usign.sSizeInBytes
  //
  uint8_t Check_Application_In_Upper_Address_Space ( void );

  //  nn.  Get cause of reset
  //
  reset_cause_t reset_cause_get(void);

  //  nn.  Get cause of reset by name
  //
  const char * reset_cause_get_name(reset_cause_t reset_cause);

#endif

  //
// *****************************************************************************
// ************************ EOF ************************************************
// *****************************************************************************
//
