/****************************************************************************
 * File:   llTimer.c
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
 * Description : Generic linked list timer for small systems
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

#include "llTimer.h"

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> User Includes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

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

static PTIMER *llTimerFind(PTIMER *ppTimerList, PTIMER pTimer);

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> External Variables  <<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
  
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Global Variables  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

PTIMER  gpTimerList;

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Public Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*****************************************************************************
 * Description: Function to remove a timer from the list
 *
 * Parameters:
 *              IN  pNewTimer - Pointer to the timer to remove
 *
 * Return value: TRUE for success
 *
 * NOTE:        The caller is responsible for disabling interrupts. 
 *              An interrupt during this procedure will damage the list and
 *              cause malfunction.
 *
 *****************************************************************************/

BOOL llTimerRemove(PTIMER pTimer)
{
    PTIMER *ppTimerList = llTimerFind(&gpTimerList, pTimer);
    
    if (*ppTimerList) {
        *ppTimerList = pTimer->pNext;
        if (*ppTimerList) {
            (*ppTimerList)->dwCount += pTimer->dwCount;
        }
        return TRUE;
    }
    
    return FALSE;
}

/*****************************************************************************
 * Description: Function to insert a new timer to the timer list
 *
 * Parameters:
 *              IN  ppTimerList - Pointer to the top of the list pointer
 *              IN  pNewTimer - Pointer to the timer to add
 *
 * Return value: N/A
 *
 * NOTE:        The caller is responsible for disabling interrupts. 
 *              An interrupt during this procedure will damage the list and
 *              cause malfunction.
 *
 *****************************************************************************/

VOID llTimerInsert(PTIMER pNewTimer)
{
    PTIMER *ppTimerList = &gpTimerList;
                                    /* Set the count to the length */
    pNewTimer->dwCount = pNewTimer->dwTime;
                                    /* Search the list for the insertion point */
    while ((*ppTimerList != NULL)
    &&     (pNewTimer->dwCount >= (*ppTimerList)->dwCount)) {
                                    /* Subtract the time as we go */
        pNewTimer->dwCount -= (*ppTimerList)->dwCount;
        ppTimerList = &(*ppTimerList)->pNext;
    }
                                    /* Insert timer before next in the list */
    pNewTimer->pNext = *ppTimerList;
    *ppTimerList = pNewTimer;
                                    /* If there is a next timer */
    if (pNewTimer->pNext != NULL) {
                                    /* Adjust the count */
        pNewTimer->pNext->dwCount -= pNewTimer->dwCount;
    } 
}

/*****************************************************************************
 * Description: The Timer IRQ handler
 *
 * Parameters:
 *              N/A
 *
 * Return value: TRUE if there are still timers on the list
 *
 *****************************************************************************/

BOOL llTimerIsr(VOID)
{
    PTIMER *ppTimerList = &gpTimerList;

    if (*ppTimerList) {
                                    /* Decrement the clock count */
        (*ppTimerList)->dwCount -= 1;
                                    /* For all the timers that have
                                       expired set the timer event */
        while ((*ppTimerList != NULL)
        &&     ((*ppTimerList)->dwCount == 0)) {
            PTIMER pExpired = *ppTimerList;
                                    /* Put the next timer at the top */
            *ppTimerList = (*ppTimerList)->pNext;
                                    /* If it is periodic then add it
                                       to the list again now */
            if (pExpired->Type == PERIODIC_TIMER) {
                llTimerInsert(pExpired);
            } else {
                pExpired->Type = FREE_TIMER_RESOURCE;
            }
                                    /* Call the timer function */
            pExpired->pTimerFunction(pExpired->pvParameter);
        }
    }
                                    /* Test to see if there are still
                                       timers running */
    if (gpTimerList) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Private Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*****************************************************************************
 * Description: Function to find a timer in the list
 *
 * Parameters:
 *              IN  ppTimerList - Pointer to the top of the list pointer
 *              IN  pNewTimer - Pointer to the timer to remove
 *
 * Return value: pointer to the timer pointer in the list, pointer to NULL
 *
 *****************************************************************************/

static PTIMER *llTimerFind(PTIMER *ppTimerList, PTIMER pTimer)
{
                                    /* Search the list for the timer */
    while ((*ppTimerList != NULL)
    &&     (*ppTimerList != pTimer)) {
        ppTimerList = &(*ppTimerList)->pNext;
    }
    return ppTimerList;
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