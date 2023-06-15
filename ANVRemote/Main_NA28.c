/****************************************************************************
 * File:   Main_NA28.c
 * Author: Adam Fullerton
 * Design: Adam Fullerton
 * Date:   05/01/2009
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
 * Description : Main entry point for the ANV remote firmware
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

#include "ANVRemote.h"
#include "MatrixScan.h"

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

/*
 * The version structure
 ***********************
 *
 */

#pragma section VERSION
const struct _VERSION
{
    CHAR    pszVersion[64];
    CHAR    pszDate[16];
    CHAR    pszTime[16];

} gVersion = {

    "ANV Remote for NA28 V1.0 for H8/3672",
    __DATE__,
    __TIME__
};
#pragma section

const char pszNA28[] = "NA28";

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Function Prototypes <<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

static void SetActiveSensingError(void);
static void SwitchScan(void);
static void SendCharacter(char chMessage);

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> External Variables  <<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
  
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Global Variables  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*
 * The global data associated with scanning the keys
 ***************************************************
 *
 */
                                    /* The switch scan timer */
TIMER       gSwitchScanTimer;
                                    /* The memory map required by the matrix
                                       scanning function. Although there is
                                       no key matrix on the product the 
                                       function is used with only 1 column */
BYTE        gpbyDebounceMapBase[MTXR_MAP_SIZE(1)];
                                    /* The data required by the key matrix
                                       debouncing object */
MTRXDATA    gMtrxData;
                                    /* Flag set TRUE when start button 
                                       pressed */
BOOL        gbfStartButton = FALSE;
                                    /* Flag set TRUE when stop button 
                                       pressed */
BOOL        gbfStopButton = FALSE;
                                    /* A timer to measure active sensing */
TIMER       gActiveSensing;

/*
 * The global data associated with the serial port
 *************************************************
 *
 */
                                    /* A flag to signal cancel transmission */
BOOL        gbfCancel;
                                    /* A timer to cancel the transmission */
TIMER       gCanceTransmission;

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Public Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*****************************************************************************
 * Description: The main entry point after hardware initialisation
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

void main(void)
{
                                    /* Open the serial port */
    hwOpenSCI3();
                                    /* Initialise the key scan debouncing */
    InitialiseMatrix(gpbyDebounceMapBase, 1, &gMtrxData);
                                    /* Start a 10mS timer to debounce the 
                                       switch inputs */
    StartTimer(&gSwitchScanTimer,
               (PCFNTMR)SwitchScan,
               PNULL,
               10UL,
               PERIODIC_TIMER);
                                    /* Start a 400mS timer to detect disconnection */
    StartTimer(&gActiveSensing,
               (PCFNTMR)SetActiveSensingError,
               PNULL,
               400UL,
               SINGLE_SHOT_TIMER);
                                    /* It is OK for the PC to send data 
                                       any time */
    ANVR_SCI3_CTS = FALSE;
                                    /* Loop forever */
    while (TRUE) {
        BYTE    byData;
                                    /* Check to see if the start button has
                                       been pressed */
        if (gbfStartButton) {
                                    /* Acknowledge the button press */
            gbfStartButton = FALSE;
            SendCharacter('1');
        }
                                    /* Check to see if the stop button has
                                       been pressed */
        if (gbfStopButton) {
                                    /* Acknowledge the button press */
            gbfStopButton = FALSE;
            SendCharacter('0');
        }
                                    /* See if a byte has been received */
        if (hwReadSCI3(&byData)) {
                                    /* Take appropriate action */
            switch (byData) {
                case 'X':
                {
                                    /* Start a 60S timer for manual test */
                    StartTimer(&gActiveSensing,
                               (PCFNTMR)SetActiveSensingError,
                               PNULL,
                               60000UL,
                               SINGLE_SHOT_TIMER);
                    SendCharacter('x');
                    break;
                }
                case 'A':
                {
                                    /* Start a 300mS timer to detect disconnection */
                    StartTimer(&gActiveSensing,
                               (PCFNTMR)SetActiveSensingError,
                               PNULL,
                               400UL,
                               SINGLE_SHOT_TIMER);
                                    /* Send a response so we know that the message
                                       has been received */
                    SendCharacter('a');
                    break;
                }
                case 'R':
                {
                    ANVR_STOP_LED_ON;
                    break;
                }
                case 'r':
                {
                    ANVR_STOP_LED_OFF;
                    break;
                }
                case 'G':
                {
                    ANVR_START_LED_ON;
                    break;
                }
                case 'g':
                {
                    ANVR_START_LED_OFF;
                    break;
                }
            }
        }
    }
                                    /* Theoretical.... */
    hwCloseSCI3();
}

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Private Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*****************************************************************************
 * Description: Timer call-back function to set the active sensing flag
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

static void SetActiveSensingError(void)
{
    ANVR_STOP_LED_ON;
    ANVR_START_LED_OFF;
}

/*****************************************************************************
 * Description: Timer call-back function to set the cancel flag for comms time-out
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

static void SetCancelFlag(void)
{
    gbfCancel = TRUE;
}

/*****************************************************************************
 * Description: Function to start a timer to cancel the transmission
 *
 * Parameters:
 *              IN  dwTimeOut - The time period to wait before cancel 
 *
 * Return value: N/A
 *
 *****************************************************************************/

static void StartCancelTimer(DWORD dwTimeOut)
{
    StartTimer(&gCanceTransmission,
               (PCFNTMR)SetCancelFlag,
               PNULL,
               dwTimeOut,
               SINGLE_SHOT_TIMER);
    gbfCancel = FALSE;
}

/*****************************************************************************
 * Description: Function to send a character with time-out
 *
 * Parameters:
 *              IN  chMessage - The character to send
 *
 * Return value: 
 *
 *****************************************************************************/

static void SendCharacter(char chMessage)
{
                                    /* Start the cancel timer - PC should assert
                                       RTS within time period */
    StartCancelTimer(250);
                                    /* Send the character when PC ready */
    hwWriteSCI3((PBYTE)&chMessage, sizeof(char), &gbfCancel);
}

/*****************************************************************************
 * Description: Function to scan the switch inputs
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 * NOTE: This function is run in the V timer ISR
 *
 *****************************************************************************/

static void SwitchScan(void)
{
    MTRXOUT ScanResult;
                                    /* Debounce the switches */
    DebounceMatrix(0,
                   (BYTE)~(ANVR_SWITCH_PORT & ANVR_SWITCH_PORT_MASK),
                   &ScanResult,
                   &gMtrxData);
                                    /* Test for stop switch press */
    if (ScanResult.byOn & ANVR_SWITCH_STOP_MASK) {
        gbfStopButton = TRUE;
    }
                                    /* Test for start switch press */
    if (ScanResult.byOn & ANVR_SWITCH_START_MASK) {
        gbfStartButton = TRUE;
    }
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