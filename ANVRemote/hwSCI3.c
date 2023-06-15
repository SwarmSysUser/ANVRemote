/****************************************************************************
 * File:   hwSCI3.c
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

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> System Includes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> User Includes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

#include "hwSCI3.h"
#include "IODefine.h"

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Defines <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Constant Macros <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
 
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

static VOID hwOpenSCI3(VOID);
static BOOL hwReadSCI3(PBYTE pbyData);
static BOOL hwWriteSCI3(PBYTE pbySrc, size_t stLength, PBOOL pbyCancel);
static VOID hwCloseSCI3(VOID);

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> External Variables  <<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
  
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Global Variables  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Public Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*****************************************************************************
 * Description: Function to write to the seral port
 *
 * Parameters:
 *              IN  pbySrc - Pointer to the data to transmit
 *              IN  stLength - The length of data to transmit
 *              IN  pbfCancel - Pointer to a flag set TRUE to cancel
 *
 * Return value: TRUE for success otherwise FALSE
 *
 *****************************************************************************/

BOOL WriteSCI3(PBYTE pbySrc, size_t stLength, PBOOL pbyCancel)
{
    register BOOL    bfReturn;
                                    /* Open the serial port */
    hwOpenSCI3();
    bfReturn = hwWriteSCI3(pbySrc, stLength, pbyCancel);
                                    /* Close the serial port */
    hwCloseSCI3();

    return bfReturn;
}

/*****************************************************************************
 * Description: Function to read from the serial port
 *
 * Parameters:
 *              OUT pbyDest - Pointer to the destination memory
 *              IN  stLength - The length of data to read
 *              IN  pbfCancel - Pointer to a flag set TRUE to cancel
 *
 * Return value: TRUE for success otherwise FALSE
 *
 *****************************************************************************/

BOOL ReadSCI3(PBYTE pbyDest, size_t stLength, PBOOL pbfCancel)
{
                                    /* Open the serial port */
    hwOpenSCI3();
                                    /* Until the length of data is read OR
                                       the read is cancelled */
    while ((stLength)
    &&     (*pbfCancel == FALSE)) {
        BYTE    byData;
                                    /* See if there is a byte available */
        if (hwReadSCI3(&byData)) {
                                    /* Write it to the destination */
            *pbyDest++ = byData;
                                    /* Decrement the number of bytes to read */
            stLength--;
        }
    }
                                    /* Close the serial port */
    hwCloseSCI3();

    if ((stLength == 0)
    &&  (*pbfCancel == FALSE)) {
        return TRUE;
    }

    return FALSE;
}

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Private Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*****************************************************************************
 * Description: Function to open SCI3
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

static VOID hwOpenSCI3(VOID)
{
                                    /* From fig 13.4 of the hardware manual */
    DWORD   dwDelay = SCI3_BIT_DELAY_COUNT;
                                    /* Enable module operation */
    MSTCR1.BIT.MSTS3 = FALSE;
                                    /* Clear SCR3 */
    SCI3.SCR3.BYTE = 0;
                                    /* Clear the serial mode register.
                                       1 start bit, 8 bits data, 1 stop bit
                                       no parity, no MP comms */
    SCI3.SMR.BYTE = 0;
                                    /* Set the CKS setting */
    if (SCI3_19200_CKS_SETTING) {
        SCI3.SMR.BIT.CKS = SCI3_19200_CKS_SETTING;
    }
                                    /* Set the BRR */
    SCI3.BRR = SCI3_19200_BRR_SETTING;
                                    /* Wait 1 bit period */
    while (dwDelay--)
        /* Wait */;

    SCI3.SCR3.BIT.TE = 1;
    SCI3.SCR3.BIT.RE = 1;
    IO.PMR1.BIT.TXD = 1;
}

/*****************************************************************************
 * Description: Function to read a byte from the serial port
 *
 * Parameters:
 *              IN  pbyData - Pointer to the data read
 *
 * Return value: TRUE if a byte was read
 *
 *****************************************************************************/

static BOOL hwReadSCI3(PBYTE pbyData)
{
                                    /* Check and clear any error bits */
    if (SCI3.SSR.BIT.OER) {
        SCI3.SSR.BIT.OER = 0;
    }
    if (SCI3.SSR.BIT.FER) {
        SCI3.SSR.BIT.FER = 0;
    }
    if (SCI3.SSR.BIT.PER) {
        SCI3.SSR.BIT.PER = 0;
    }
                                    /* Check the receive data register full */
    if (SCI3.SSR.BIT.RDRF) {
                                    /* Read the receive data register */
        *pbyData = SCI3.RDR;
                                    /* Return TRUE to indicate a byte has been
                                       read */
        return TRUE;
    }
                                    /* Return FALSE to show that no data has
                                       been read */
    return FALSE;
}

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

static BOOL hwWriteSCI3(PBYTE pbySrc, size_t stLength, PBOOL pbyCancel)
{
                                    /* From Fig. 13.5 of the hardware manual */
    while (stLength--) {
                                    /* Wait while port is not ready */
        while ((!SCI3.SSR.BIT.TDRE) 
                                    /* or the mic is not ready to receive the 
                                       command */
           ||  (ANVR_SCI3_RTS)) {

                                    /* Test for operation cancel */
            if (*pbyCancel) {
                return FALSE;
            }
        }
                                    /* Write data to TDR */
        SCI3.TDR = *pbySrc++;
    }
                                    /* Wait for the data to be transmitted */
    while (!SCI3.SSR.BIT.TEND) {
                                    /* Test for operation cancel */
        if (*pbyCancel) {
            return FALSE;
        }
    }
                                    /* Success */
    return TRUE;
}

/*****************************************************************************
 * Description: Function to close SCI3
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

static VOID hwCloseSCI3(VOID)
{
    SCI3.SCR3.BIT.TE = 0;
    MSTCR1.BIT.MSTS3 = TRUE;
}
 
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