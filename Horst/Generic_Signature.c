/**
  * ############################################################################
  * @file     Generic_Signature.c
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
#include "Generic_Signature.h"
#include "memory.h"
#include "common.h"
#include "version.h"

  //******************************************************************************
  //
  __attribute__((__section__(".signature"))) uint32_t ThisApplicationsSignature[4] =
          {
            c_Magic_Number,
            VERSION_TXT6,
            0xffffffff,
            0xffffffff
          };

//
// ******************************************************************************************
//  The End
// ******************************************************************************************

