/**
  * ############################################################################
  * @file     Generic_Signature.h
  * @brief    New Vario
  * @author   Horst Rupp
  * @brief    This file contains
  * @brief    This
  * ############################################################################
  */
//******************************************************************************
//
// Includes
//
#include  "Generic_Types.h"

#ifndef __Generic_Signature_H
  #define __Generic_Signature_H

// ***************************************************************************
  //
  // signature
  //
  typedef struct
  {

    uint32_t   sMagicNumber;
    uint32_t   sVersion;
    uint32_t   sSizeInBytes;
    uint32_t   sC_R_C;

  } AppSignature_t;

  #define c_Size_Signature  sizeof (AppSignature_t)

#endif  // #ifndef __Generic_Signature_H
//
// ******************************************************************************************
//  The End
// ******************************************************************************************

