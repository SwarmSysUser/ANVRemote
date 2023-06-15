/****************************************************************************
 * File:   hwSCI3.h
 * Author: Adam Fullerton
 * Design: Adam Fullerton
 * Date:   05/04/2004
 *
 * $Workfile: $
 * $Revision: $
 * $Project:  $ 
 * $Compiler: $
 * $Target:   $
 * $State:    $
 * ('Exp'erimental/'Stab'le/'Rel'eased)
 * $Date      $
 *
 * Description : SCI3 control functions
 *
 * (C) ARDWare ltd. 2004-2008. All Rights Reserved.
 * Rosedene, The Dene, Hindon, Salisbury, Wiltshire SP3 6EE.
 * 
 * The copyright to the computer program(s) herein is the property of
 * ARDWare ltd. The program(s) may be used and/or copied only with the
 * written permission of ARDWare ltd. or in accordance with the terms and
 * conditions stipulated in the agreement/contract under which the
 * program(s) have been supplied.
 * This copyright notice must not be removed.
 ****************************************************************************/

#ifndef HWSCI3_H_INCLUDED
#define HWSCI3_H_INCLUDED

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> System Includes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> User Includes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

#include "Types.h"

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Defines <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

                                    /* See table 13.2 in hardware manual for
                                       baud rate equation and settings for
                                       3.6864MHz external clock */
#define SCI3_19200_BRR_SETTING      5
#define SCI3_19200_CKS_SETTING      0
                                    /* The 1 bit delay count value */
#define SCI3_BIT_DELAY_COUNT        192

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Constant Macros <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*
 * The flow control signals
 **************************
 *
 */
                                    /* The RTS INPUT pin */
#define ANVR_SCI3_RTS               IO.PDR2.BIT.B0
                                    /* The CTS OUTPUT pin */
#define ANVR_SCI3_CTS               IO.PDR8.BIT.B4

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Function Macros <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
 
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Typedefs <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
 
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Enumerated Types <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>> Constant Data  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
 
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Function Prototypes <<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> External Variables  <<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
  
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Global Variables  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Public Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Description: Function to write to the seral port
 *
 * Parameters:
 *              IN  pbySrc - Pointer to the data to transmit
 *              IN  stLength - The length of data to reansmit
 *              IN  pbfCancel - Pointer to a flag set TRUE to cancel
 *
 * Return value: TRUE for success otherwise FALSE
 *
 *****************************************************************************/

extern  BOOL WriteSCI3(PBYTE pbySrc, size_t stLength, PBOOL pbyCancel);

/*****************************************************************************
 * Description: Function to read from the serial port
 *
 * Parameters:
 *              OUT pbyDest - Pointer to the destination memory
 *              IN  stLength - The length of data to read
 *              IN  pbfCancel - Pointer to a flag set TRUE to cancel
 *
 * Return value: 0 for success otherwise error code
 *
 *****************************************************************************/

extern  BOOL ReadSCI3(PBYTE pbyDest, size_t stLength, PBOOL pbfCancel);

#ifdef NA28

/*****************************************************************************
 * Description: Function to open SCI3
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

extern  VOID hwOpenSCI3(VOID);

/*****************************************************************************
 * Description: Function to read a byte from the serial port
 *
 * Parameters:
 *              IN  pbyData - Pointer to the data read
 *
 * Return value: TRUE if a byte was read
 *
 *****************************************************************************/

extern  BOOL hwReadSCI3(PBYTE pbyData);

/*****************************************************************************
 * Description: Function to write a buffer to the serial port
 *
 * Parameters:
 *              IN  pbySrc - Pointer to the data to transmit
 *              IN  stLength - The length of data to reansmit
 *              IN  pbfCancel - Pointer to a flag set TRUE to cancel
 *
 * Return value: TRUE for success otherwise FALSE
 *
 *****************************************************************************/

extern  BOOL hwWriteSCI3(PBYTE pbySrc, size_t stLength, PBOOL pbyCancel);

/*****************************************************************************
 * Description: Function to close SCI3
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

extern  VOID hwCloseSCI3(VOID);

#endif

#ifdef __cplusplus
}
#endif

#endif                              /* HWSCI3_H_INCLUDED */

/****************************************************************************
* Source Safe History:
* $History: $
*
* End of file $Workfile: $
*****************************************************************************/
/****************************************************************************
 * Software Supplied By:                                  www.ardware.co.uk *
 * ARDWare Ltd                                     Tel: +44 (0)1747 820 536 *
 * The Dene                                        Fax: +44 (0)1747 820 669 *
 * Hindon                               Adam Fullerton:  adam@ardware.co.uk *
 * Salisbury                             Rufus Sidford: rufus@ardware.co.uk *
 * Wiltshire                            Douglas Toates:  doug@ardware.co.uk *
 * SP3 6EE                                   Tom Lacey:   tom@ardware.co.uk *
 * Company Registration Number:4157440. Support charged at commercial rates *
 ****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> End  Of File <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/