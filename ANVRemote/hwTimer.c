/****************************************************************************
 * File:   hwTimer.c
 * Author: Adam Fullerton
 * Design: Adam Fullerton
 * Date:   01/04/2004
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
 * Description : 8 bit time V is used as a generic software timer
 *               using the generic linked list timer functions.
 *               The timer is configured for a 1mS interval.
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

#include <machine.h>

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> User Includes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

#include "IODefine.h"
#include "ANVRemote.h"

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

static VOID hwStartTimerV(VOID);

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
 * Description: Function to start a timer running
 *
 * Parameters:
 *              IN  pTimer - Pointer to the timer to start
 *              IN  pTimerFunction - Pointer to the timer call back function
 *              IN  pvParameter - Pointer to the call back parameter
 *              IN  dwTime - The time in mS
 *              IN  Type - The timer type, SINGLE_SHOT_TIMER 
 *                         or PERIODIC_TIMER
 *
 * Return value: N/A
 *
 *****************************************************************************/

VOID StartTimer(PTIMER  pTimer,
                PCFNTMR pTimerFunction,
                PVOID   pvParameter,
                DWORD   dwTime,
                TMRTYPE Type)
{
    INT iMask = get_imask_ccr();
                                    /* Mask interrupts to prevent list break */
    if (!iMask) {
        set_imask_ccr(TRUE);
    }
                                    /* Remove from the list if its 
                                       already in use */
    llTimerRemove(pTimer);
                                    /* Initialise the variables */
    pTimer->pTimerFunction = (PFNTMR)pTimerFunction;
    pTimer->pvParameter = pvParameter;
    pTimer->dwTime = dwTime;
    pTimer->Type = Type;
                                    /* Insert into linked list */
    llTimerInsert(pTimer);
                                    /* enable interrupts */
    if (!iMask) {
        set_imask_ccr(FALSE);
    }
                                    /* If the timer module is stopped */
    if (MSTCR1.BIT.MSTTV) {
                                    /* Initialise the timer hardware */
        hwStartTimerV();
    }
}

/*****************************************************************************
 * Description: Function to stop a timer
 *
 * Parameters:
 *              IN  pTimer - Pointer to the timer to stop
 *
 * Return value: TRUE for success otherwise the timer was not found
 *
 *****************************************************************************/

BOOL StopTimer(PTIMER pTimer)
{
    INT     iMask = get_imask_ccr();
    BOOL    bfReturn;
                                    /* Mask interrupts to prevent list break */
    if (!iMask) {
        set_imask_ccr(TRUE);
    }
                                    /* Insert into linked list */
    bfReturn = llTimerRemove(pTimer);
                                    /* enable interrupts */
    if (!iMask) {
        set_imask_ccr(FALSE);
    }

    return bfReturn;
}

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Private Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*****************************************************************************
 * Description: Function to start the timer V running
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

static VOID hwStartTimerV(VOID)
{
                                    /* Allow module operation */
    MSTCR1.BIT.MSTTV = FALSE;
                                    /* (XTAL/FS_PRE_SCALAR = /128) */
    TV.TCRV0.BIT.CKS = ANVR_CKS_SETTING;
    TV.TCRV1.BIT.ICKS = ANVR_ICKS_SETTING;
                                    /* Clear counter on compare/match A */
    TV.TCRV0.BIT.CCLR = ANVR_CCLR_SETTING;
                                    /* Set match A value */
    TV.TCORA = ANVR_TCORA_1MS_SETTING;
                                    /* Enable interrupts */
    TV.TCRV0.BIT.CMIEA = TRUE;
}

/*****************************************************************************
 * Description: Function to stop the V timer from running
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

static VOID hwStopTimerV(VOID)
{
                                    /* Disable the interrupt */
    TV.TCRV0.BIT.CMIEA = FALSE;
                                    /* Stop the module */
    MSTCR1.BIT.MSTTV = TRUE;
}

/*****************************************************************************
 * Description: The timer ISR itself
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

__interrupt(vect=22) void INT_TimerV(void)
{
                                    /* Test the timers */
    if (!llTimerIsr()) {
                                    /* If there are none left then stop the
                                       timer hardware from running */
        hwStopTimerV();
    }
                                    /* Clear the compare and match flag */
    TV.TCSRV.BIT.CMFA = FALSE;
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