/****************************************************************************
 * File:   llTimer.h
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
 * NOTE:
 *      DO NOT USE THIS TIMER ON THE STACK! Because this timer uses a linked
 *      list it can be 'broken' if a timer is started from the stack and the
 *      function returns before the timer has expired.
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

#ifndef LLTIMER_H_INCLUDED
#define LLTIMER_H_INCLUDED

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

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Constant Macros <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
 
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Function Macros <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
 
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Typedefs <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*
 * The timer call back function
 ******************************
 *
 */

typedef VOID (*PFNTMR)(PVOID);
typedef VOID (*const PCFNTMR)(PVOID);

/*
 * Define the types of timer available
 *************************************
 *
 */

typedef enum 
{
    FREE_TIMER_RESOURCE = 0,
    SINGLE_SHOT_TIMER,              /* Runs once and stops */
    PERIODIC_TIMER                  /* Runs until stopped */

} TMRTYPE,
*PTRTYPE;

/*
 * The structure of a timer
 **************************
 *
 */

typedef struct _TIMER
{
    struct _TIMER *pNext;           /* Pointer to the next timer on the list */
    DWORD       dwCount;            /* The current count */
    PFNTMR      pTimerFunction;     /* Pointer to the function to call */
    PVOID       pvParameter;        /* A generic parameter to pass to the 
                                       timer function */
    DWORD       dwTime;             /* The time in mS must be greater than 0 */
    TMRTYPE     Type;               /* The timer type SINGLE_SHOT or PERIODIC */

} TIMER,
*PTIMER;

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

extern  BOOL llTimerRemove(PTIMER pTimer);

/*****************************************************************************
 * Description: Function to insert a new timer to the timer list
 *
 * Parameters:
 *              IN  pNewTimer - Pointer to the timer to add
 *
 * Return value: N/A
 *
 * NOTE:        The caller is responsible for disabling interrupts. 
 *              An interrupt during this procedure will damage the list and
 *              cause malfunction.
 *
 *****************************************************************************/

extern  VOID llTimerInsert(PTIMER pNewTimer);

/*****************************************************************************
 * Description: The Timer IRQ handler
 *
 * Parameters:
 *              N/A
 *
* Return value: TRUE if there are still timers on the list
 *
 *****************************************************************************/

extern  BOOL llTimerIsr(VOID);

#ifdef __cplusplus
}
#endif

#endif                              /* LLTIMER_H_INCLUDED */

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