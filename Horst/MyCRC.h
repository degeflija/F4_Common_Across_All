/**
 * @file  MyCRC.h
 * @brief  New Vario
 * @author  Horst Rupp
 */

#include  "Generic_Includes.h"

#ifndef  __MyCRC_H
  #define  __MyCRC_H
  //
  // ***************************************************************************
  //
  uint16_t CRC16                  ( const uint16_t input, uint16_t crc );
  uint16_t CRC16_blockcheck_bytes ( const uint8_t *input, uint16_t crc_seed, uint16_t length );
  uint16_t CRC16_blockcheck       ( const uint8_t *input, uint16_t crc_seed, uint16_t length );

#endif  // #ifndef  __MyCRC_H
//
// *****************************************************************************
// The End
// *****************************************************************************




