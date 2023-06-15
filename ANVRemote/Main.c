/****************************************************************************
 * File:   ANVRemote.c
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

/*
 * A structure to define how a LED should be flashed
 ***************************************************
 *
 */

typedef struct _LEDCTL
{
    DWORD   dwOnTime;               /* The time in mS that the LED is ON 
                                       -1 forever */
    DWORD   dwOffTime;              /* The time in mS that the LED is off 
                                       which defines the flash pulse width
                                       and frequecy */
} LEDCTL,
*PLEDCTL,
*const PCLEDCTL;
                                    /* Define a pointer to constant values */
typedef const PLEDCTL CPLEDCTL;

                                    /* Define a structure that holds the
                                       LED map and the number of times
                                       to flash the LED */
typedef struct _LEDINFO
{
    PLEDCTL     pLedCtl;            /* Pointer to the led control */
    BYTE        byLED;              /* The LED map to flash */
    DWORD       dwRepeat;           /* The number of times to repeat. 
                                        -1 represents repeat for ever. 
                                       N/A when dwOnTime is set to -1 */
    TIMER       Timer;              /* A timer for the LED */

} LEDINFO,
*PLEDINFO;

/*
 * Define the start message codes
 ********************************
 *
 *  Response to SER130 not storing:
 *  [0x02][0x01][0x06][0x03][0x06][0x0D][0x0A]
 *  when storing:
 *  [0x02][0x01][0x15]0003[0x03][0x16][0x0D][0x0A]
 *  Response to SER? when storing:
 *  [0x02][0x01]A130[0x03]s[0x0D][0x0A]
 *  Response to STO? when storing:
 *  [0x02][0x01]A1[0x03]p[0x0D][0x0A]
 *  Response to SRS0 and 1
 *  [0x02][0x01][0x06][0x03][0x06][0x0D][0x0A]
 * 
 *  Block Check Codes
 *
 */

const BYTE gcStartMessage[] = 
{
    0x02, 0x00,
#ifdef ANVR_RECORD_START_STOP
    (BYTE)'C', (BYTE)'S', (BYTE)'R', (BYTE)'S', (BYTE)'1',
#else
    (BYTE)'C', (BYTE)'P', (BYTE)'S', (BYTE)'E', (BYTE)'0',
#endif
    0x03, 0x00, 0x0D, 0x0A
};

const BYTE gcStopMessage[] = 
{
    0x02, 0x00,
#ifdef ANVR_RECORD_START_STOP
    (BYTE)'C', (BYTE)'S', (BYTE)'R', (BYTE)'S', (BYTE)'0',
#else
    (BYTE)'C', (BYTE)'P', (BYTE)'S', (BYTE)'E', (BYTE)'1',
#endif
    0x03, 0x00, 0x0D, 0x0A
};

const BYTE gcpbyResponseOK[] =
{
    0x02, 0x01, 0x06, 0x03, 0x06, 0x0D, 0x0A
};

const BYTE gcThresholdCheckMessage[] =
{
    0x02, 0x01,
    (BYTE)'C', (BYTE)'S', (BYTE)'E', (BYTE)'R', (BYTE)'?',
    0x03, 0x38, 0x0D, 0x0A
};

const BYTE gcpbyThresholdResponseMessage[] = 
{
    0x02, 0x01, 
    (BYTE)'A', (BYTE)'1', (BYTE)'3', (BYTE)'0',
    0x03, (BYTE)'s', 0x0D, 0x0A
};

const BYTE gcStoreCheckMessage[] = 
{
    0x02, 0x01,
    (BYTE)'C', (BYTE)'S', (BYTE)'T', (BYTE)'O', (BYTE)'?',
    0x03, 0x34, 0x0D, 0x0A
};

const BYTE gcpbyStoreResponseMessage[] = 
{
    0x02, 0x01,
    (BYTE)'A', (BYTE)'1',
    0x03, (BYTE)'p', 0x0D, 0x0A
};

const BYTE gcReturnResponseMessage[] = 
{
    0x02, 0x01,
    (BYTE)'C', (BYTE)'R', (BYTE)'E', (BYTE)'T', (BYTE)'1',
    0x03, 0x31, 0x0D, 0x0A
};


/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Enumerated Types <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*
 * Define the types of message sent by the device
 ************************************************
 *
 */

typedef enum _SCICMD
{
    SEND_START_MESSAGE = 0,         /* Code to send start command */
    SEND_STOP_MESSAGE               /* Code to send stop command */

} SCICMD,
*PSCICMD;

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>> Constant Data  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*
 * Define some settings for commonly used LED flashing
 *****************************************************
 *
 */

const LEDCTL GC_FLASH_RUNNING =
{
    10UL,                           /* On for 5% of the time to save power */
    90UL
};

const LEDCTL GC_FLASH_STOPPED =
{
    2000UL,
    1UL
};

const LEDCTL GC_FLASH_SENDING  =
{
    250UL,                          /* On for 1/4S */
    250UL                           /* Off for 1/4S */
};

const LEDCTL GC_ALWAYS_ON =
{
    (DWORD)-1L,
    0UL
};

const LEDCTL GC_ALWAYS_OFF =
{
    0UL,
    0UL
};

/*
 * The version structure
 ***********************
 *
 */

#pragma section VERSION
const struct _VERSION
{
    CHAR    pszVersion[32];
    CHAR    pszDate[16];
    CHAR    pszTime[16];

} gVersion = {

    "ANV Remote V1.6 for H8/3672",
    __DATE__,
    __TIME__
};
#pragma section

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Function Prototypes <<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

static void SetStopFlag(void);
static void SwitchScan(void);
static void SetLedState(PLEDINFO  pLedInfo,
                        CPLEDCTL  pLedCtl,
                        BYTE      byLED,
                        DWORD     dwRepeat);
static void SetLedStateOn(PLEDINFO pLedInfo);
static BOOL SCI3_CheckOperatingMode(void);
static BOOL SCI3_SendCommand(SCICMD sciCommand);
static int memcmp(const void *pvMem1, const void *pvMem2, size_t stLength);

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
                                    /* Updates requested ref ASS300704LET */
                                    /* The record period is to be defined
                                       by a project define for 5 time periods */
TIMER       gRecordPeriodTimer;
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

/*
 * The global data associated with control of the LEDs
 *****************************************************
 *
 */

LEDINFO     gStartLED;
LEDINFO     gStopLED;

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
                                    /* Initialise the key scan debouncing */
    InitialiseMatrix(gpbyDebounceMapBase, 1, &gMtrxData);
                                    /* Start a 10mS timer to debounce the 
                                       switch inputs */
    StartTimer(&gSwitchScanTimer,
               (PCFNTMR)SwitchScan,
               PNULL,
               10UL,
               PERIODIC_TIMER);
                                    /* It is OK for the meter to send data 
                                       any time */
    ANVR_SCI3_CTS = FALSE;
                                    /* Loop forever */
    while (TRUE) {
                                    /* Check to see if the start button has
                                       been pressed */
        if (gbfStartButton) {
                                    /* Acknowledge the button press */
            gbfStartButton = FALSE;
                                    /* Set the LED to show that the start
                                       character will be sent */
            SetLedState(&gStartLED,
                        &GC_FLASH_SENDING,
                        ANVR_LED_START_MASK,
                        4UL);
                                    /* Attempt to send the start command */
            if ((SCI3_SendCommand(SEND_START_MESSAGE))
            &&  (SCI3_CheckOperatingMode())) {
                                    /* Stop activity on the stop LED */
                SetLedState(&gStopLED,
                            &GC_ALWAYS_OFF,
                            ANVR_LED_STOP_MASK,
                            0UL);
                                    /* Show the recording state forever */
                SetLedState(&gStartLED,
                            &GC_FLASH_RUNNING,
                            ANVR_LED_START_MASK,
                            (DWORD)-1L);
            }
                                    /* Updates requested ref ASS300704LET */
                                    /* Build settings define if the record
                                       timer is included */
#ifdef ANVR_RECORD_PERIOD_IN_MS
                                    /* Start the record period timer */
            StartTimer(&gRecordPeriodTimer,
                       (PCFNTMR)SetStopFlag,
                       PNULL,
                       ANVR_RECORD_PERIOD_IN_MS,
                       SINGLE_SHOT_TIMER);
#endif
        }
                                    /* Check to see if the stop button has
                                       been pressed */
        if (gbfStopButton) {
                                    /* Updates requested ref ASS300704LET */
                                    /* Build settings define if the record
                                       timer is included */
#ifdef ANVR_RECORD_PERIOD_IN_MS
                                    /* Stop the record period timer */
            StopTimer(&gRecordPeriodTimer);
            /* NOTE: Stop timer is called before the flag is re-set
               to ensure that two stop events are not processed if 
               the timer ISR occurs between the test and the flag
               acknowlegde */
#endif
                                    /* Acknowledge the button press */
            gbfStopButton = FALSE;
                                    /* Set the LED to show that the stop
                                       character will be sent */
            SetLedState(&gStopLED,
                        &GC_FLASH_SENDING,
                        ANVR_LED_STOP_MASK,
                        4UL);
                                    /* Attempt to send the stop command */
            if (SCI3_SendCommand(SEND_STOP_MESSAGE)) {
                                    /* Stop activity on the start LED */
                SetLedState(&gStartLED,
                            &GC_ALWAYS_OFF,
                            ANVR_LED_START_MASK,
                            0UL);
                                    /* Indicated the stop condition */
                SetLedState(&gStopLED,
                            &GC_FLASH_STOPPED,
                            ANVR_LED_STOP_MASK,
                            1UL);
            }
        }
    }
}

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Private Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*****************************************************************************
 * Description: Timer call-back function to set the stop flag
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

static void SetStopFlag(void)
{
    gbfStopButton = TRUE;
}

/*****************************************************************************
 * Description: Timer call-back function to set the cancel flag
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
 * Description: Function to send a message and check the response
 *
 * Parameters:
 *              
 *
 * Return value: 0 for success otherwise error code
 *
 *****************************************************************************/

static BOOL SCI3_SendCommandCheckResponse(PBYTE     pbyCommand,
                                          size_t    stCommandLength,
                                          PBYTE     pbyResponse,
                                          size_t    stResponseLength)
{
    BYTE    pbyResponseBuffer[sizeof(gcpbyThresholdResponseMessage)];
                                    /* Write the command */
    if (WriteSCI3(pbyCommand, stCommandLength, &gbfCancel)) {
                                    /* Read the response */
        if (ReadSCI3(pbyResponseBuffer, stResponseLength, &gbfCancel)) {
                                    /* Check that it matches the expected 
                                       response from the instrument */
            if (memcmp(pbyResponse, pbyResponseBuffer, stResponseLength) == 0) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*****************************************************************************
 * Description: Function to check that the NL-32 is set to the correct
 *              operating mode
 *
 * Parameters:
 *              N/A
 *
 * Return value: TRUE for success otherwise FALSE
 *
 *****************************************************************************/

static BOOL SCI3_CheckOperatingMode(void)
{
                                    /* Start the cancel timer */
    StartCancelTimer(ANVR_TRANSMIT_TIME_OUT);
                                    /* Check that the trigger mode is correct */
    if (SCI3_SendCommandCheckResponse(gcThresholdCheckMessage,
                                      sizeof(gcThresholdCheckMessage),
                                      gcpbyThresholdResponseMessage,
                                      sizeof(gcpbyThresholdResponseMessage))) {
                                    /* Check that the instrument is storing */
        if (SCI3_SendCommandCheckResponse(gcStoreCheckMessage,
                                          sizeof(gcStoreCheckMessage),
                                          gcpbyStoreResponseMessage,
                                          sizeof(gcpbyStoreResponseMessage))) {
            return TRUE;
        }
    }

    return FALSE;
}

/*****************************************************************************
 * Description: Function to send a command to the recorder
 *
 * Parameters:
 *              IN  sciCommand - The command to send
 *
 * Return value: TRUE for success otherwise FALSE
 *
 *****************************************************************************/

static BOOL SCI3_SendCommand(SCICMD sciCommand)
{
    PBYTE   pbySrc;
    size_t  stLength;

    switch (sciCommand) {
                                    /* 27/08/08 - mods to trigger video */
        case SEND_START_MESSAGE:
            IO.PDR5.BYTE = BIT_2 | BIT_3;
            pbySrc = gcStartMessage;
            stLength = sizeof(gcStartMessage);
            break;

        case SEND_STOP_MESSAGE:
            IO.PDR5.BYTE = 0;
            pbySrc = gcStopMessage;
            stLength = sizeof(gcStopMessage);
            break;

        default:
            return FALSE;
    }
                                    /* Start the cancel timer */
    StartCancelTimer(ANVR_TRANSMIT_TIME_OUT);
                                    /* Send the command and get the response */
    return WriteSCI3(pbySrc, stLength, &gbfCancel);
}

/*****************************************************************************
 * Description: Function to switch the LED off
 *
 * Parameters:
 *              IN  pLedCtl - Pointer to the LED information
 *
 * Return value: N/A
 *
 * NOTE: This function is run in the V timer ISR
 *
 *****************************************************************************/

static void SetLedStateOff(PLEDINFO pLedInfo)
{
                                    /* Switch the LED OFF */
    ANVR_LED_PORT |= pLedInfo->byLED;

    if (pLedInfo->dwRepeat) {
                                    /* Start a timer to switch ON */
        StartTimer(&pLedInfo->Timer,
                   (PCFNTMR)SetLedStateOn,
                   pLedInfo,
                   pLedInfo->pLedCtl->dwOffTime,
                   SINGLE_SHOT_TIMER);
    }
}

/*****************************************************************************
 * Description: Function to control a LED
 *
 * Parameters:
 *              OUT pLedInfo - Pointer to the LED information
 *              IN  pLedCtl - Pointer to the LED flash control data
 *              IN  byLED - The LED to control 
 *              IN  dwRepeat - The number of times to repeat
 *
 * Return value: N/A
 *
 *****************************************************************************/

static void SetLedState(PLEDINFO  pLedInfo,
                        CPLEDCTL  pLedCtl,
                        BYTE      byLED,
                        DWORD     dwRepeat)
{
                                    /* Set the LED info variables */
    pLedInfo->pLedCtl = pLedCtl;
    pLedInfo->byLED = byLED;
    pLedInfo->dwRepeat = dwRepeat;
                                    /* Set the LED on */
    SetLedStateOn(pLedInfo);
}

/*****************************************************************************
 * Description: Function to switch the LED on
 *
 * Parameters:
 *              IN  pLedInfo - Pointer to the LED information
 *
 * Return value: 0 for success otherwise error code
 *
 *
 * NOTE: This function is run in the V timer ISR and called by SetLedState
 *       at despatch level.
 *****************************************************************************/

static void SetLedStateOn(PLEDINFO pLedInfo)
{
                                    /* On time specified */
    if (pLedInfo->pLedCtl->dwOnTime) {
                                    /* Switch the LED ON */
        ANVR_LED_PORT &= (BYTE)(~pLedInfo->byLED);

        if ((pLedInfo->dwRepeat)
        &&  (pLedInfo->pLedCtl->dwOffTime)) {
                                    /* Decrement the repeat count */
            if (pLedInfo->dwRepeat != (DWORD)-1L) {
                pLedInfo->dwRepeat--;
            }
                                    /* Start the timer */
            StartTimer(&pLedInfo->Timer,
                       (PCFNTMR)SetLedStateOff,
                       pLedInfo,
                       pLedInfo->pLedCtl->dwOnTime,
                       SINGLE_SHOT_TIMER);
        }
    } else {
                                    /* Switch the LED OFF */
        ANVR_LED_PORT |= pLedInfo->byLED;
    }
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

/*****************************************************************************
 * Description: Function to compare two bits of memory
 *
 * Parameters:
 *              IN  pvMem1 - Pointer to the memory
 *              IN  pvMem2 - Pointer to the memory
 *              IN  stLength - The length of memory
 *
 * Return value: 0 = same otherwise different
 *
 *****************************************************************************/

static int memcmp(const void *pvMem1, const void *pvMem2, size_t stLength)
{
    while ((stLength)
    &&     (*(PCHAR)pvMem1 == *(PCHAR)pvMem2)) {
        pvMem1 = (PCHAR)pvMem1 + 1;
        pvMem2 = (PCHAR)pvMem2 + 1;
        stLength--;
    }
    return ((stLength) ? *(PBYTE)pvMem1 - *(PBYTE)pvMem2
                       : 0);
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