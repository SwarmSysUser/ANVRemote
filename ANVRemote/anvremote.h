/****************************************************************************
 * File:   ANVRemote.h
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
 * Description : Header file for hardware access.
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

#ifndef ANVREMOTE_H_INCLUDED
#define ANVREMOTE_H_INCLUDED

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> System Includes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> User Includes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

#include "IODefine.h"
#include "Types.h"
#include "hwTimer.h"
#include "hwSCI3.h"

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Defines <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

#define ANVR_XTAL_FREQUENCY         3686400UL

#define ANVR_TRANSMIT_TIME_OUT      1000UL
#define ANVR_TRANSMIT_TIME_OUT2     1000UL
#define ANVR_TRIES                  4
                                    /* Remove this define to send the pause
                                       un-pause message */
#define ANVR_RECORD_START_STOP
                                    /* Remove this define to un-reverse the 
                                       remote button swap */
#define ANVR_REVERSE_REMOTE_BUTTONS

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Constant Macros <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
 
/*
 * Serial port
 *************
 *
 */
                                    /* The settings for port 2 */
#define ANVR_SERIALPORT_PCR_SETTING (BYTE)0

/*
 * Switches
 **********
 *
 */
                                    /* Switch port data register */
#define ANVR_SWITCH_PORT            IO.PDR1.BYTE
                                    /* Updates requested ref ASS300704LET */
#ifdef  ANVR_REVERSE_REMOTE_BUTTONS
                                    /* The start switch mask */
#define ANVR_SWITCH_START_MASK      (BYTE)(BIT_0 | BIT_5)
                                    /* The stop switch mask */
#define ANVR_SWITCH_STOP_MASK       (BYTE)(BIT_2 | BIT_6)
#else
                                    /* The start switch mask */
#define ANVR_SWITCH_START_MASK      (BYTE)(BIT_0 | BIT_6)
                                    /* The stop switch mask */
#define ANVR_SWITCH_STOP_MASK       (BYTE)(BIT_2 | BIT_5)
#endif
                                    /* The mask for the bits that are used */
#define ANVR_SWITCH_PORT_MASK       (BYTE)(BIT_0 | BIT_2 | BIT_5 | BIT_6)
                                    /* All port pins are inputs */
#define ANVR_SWITCH_PCR_SETTING     (BYTE)0
                                    /* TXD pin used */
#define ANVR_SWITCH_PMR_SETTING     (BYTE)BIT_1
                                    /* No pullups on port 1 */
#define ANVR_SWITCH_PUCR_SETTING    (BYTE)0

/*
 * LEDs
 ******
 *
 */
                                    /* The LED port data register */
#define ANVR_LED_PORT               IO.PDR8.BYTE
                                    /* The bits to conrtol the start LED */
#define ANVR_LED_START_MASK         (BYTE)(BIT_0 | BIT_1)
                                    /* The bits to control the stop LED */
#define ANVR_LED_STOP_MASK          (BYTE)(BIT_2 | BIT_3)
                                    /* The control register setting to make
                                       the pins associated with both the 
                                       LEDs and the CTS flow control signal
                                       to OUTPUTS */
#define ANVR_LED_PCR_SETTING        (BYTE)(BIT_0 | BIT_1 | BIT_2 | BIT_3 |\
                                    BIT_4) /* CTS pin BIT_4 */

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Function Macros <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

                                    /* Macros to switch the LED states */
#define ANVR_START_LED_ON           ANVR_LED_PORT &= ~ANVR_LED_START_MASK

#define ANVR_START_LED_OFF          ANVR_LED_PORT |= ANVR_LED_START_MASK

#define ANVR_STOP_LED_ON            ANVR_LED_PORT &= ~ANVR_LED_STOP_MASK

#define ANVR_STOP_LED_OFF           ANVR_LED_PORT |= ANVR_LED_STOP_MASK

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

#endif                              /* ANVREMOTE_H_INCLUDED */

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