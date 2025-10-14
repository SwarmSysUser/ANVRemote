/****************************************************************************
 * File:   Main_NL53.c
 * Author: Adam Fullerton
 * Design: Adam Fullerton
 * Date:   07/06/2011
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
 * (C) ARDWare ltd. 2004-2023. All Rights Reserved.
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

#include "ANVRemote.h"
#include "MatrixScan.h"


/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Defines <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/* Define the default post trigger time - Moved to configuration setting */
//#define ANV_POST_TRIGGER_TIME       (27990)

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
 * Define the message strings
 ****************************
 *
 */

const BYTE gcpbyMarker1On[] =
{
    "marker 1, on\r\n"
};

const BYTE gcpbyMarker1Off[] =
{
    "marker 1, off\r\n"
};

const BYTE gcpbyMarker2On[] =
{
    "marker 2, on\r\n"
};

const BYTE gcpbyMarker2Off[] =
{
    "marker 2, off\r\n"
};

const BYTE gcpbyPauseKeyPress[] =
{
    /* Dollar a special sign to make TX wait */
    "press key pause, keyon\r\n$press key pause, keyoff\r\n"
};

const BYTE gcpbyStoreCheckMessage[] =
{
    "measure?\r\n"
};

const BYTE gcpbyRecCheckMessage[] =
{
    "wave rec state?\r\n"
};

const BYTE gcpbyEchoOn[] = 
{
    "echo, on\r\n"
};

const BYTE gcpbLCDOn[] = 
{
    "lcd, on\r\n"
};

const BYTE gcpbBacklightOn[] = 
{
    "backlight, on\r\n"
};

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Enumerated Types <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

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

const LEDCTL GC_FLASH_POST_TRIGGER  =
{
    465UL,                          /* On for 1/2S */
    465UL                           /* Off for 1/2S */
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

    "NL-53 Remote V1.1 for H8/3672",
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
static void SetLedStateAlt(PLEDINFO  pLedInfo,
                           CPLEDCTL  pLedCtl,
                           BYTE      byLED,
                           DWORD     dwRepeat);
static void SetLedStateOn(PLEDINFO pLedInfo);
static BOOL StartManualRecord(void);
static BOOL RecordIsRuning(void);
static BOOL TurnLCDOn(void);
static BOOL StopManualRecord(void);
static void StartPostTriggerTimer(DWORD dwTimeOut);
static BOOL Marker1On(void);
static void Marker1Off(void);
static BOOL Marker2On(void);
static void Marker2Off(void);
static void DropMarker2(void);
static int memcmp(const void *pvMem1, const void *pvMem2, size_t stLength);
static char *strstr(const char *pszString, const char *pszSubString);
static BOOL SCI3_SendCommandCheckResponse(PBYTE     pbyCommand,
                                          size_t    stCommandLength,
                                          char     *pszResponse);

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
                                    /* Timer for marker off message */
TIMER       gMarker1OffTimer;
                                    /* Timer for post trigger timer */
TIMER       gPostTriggerTimer;
                                    /* Timer for mark on / off */
TIMER       gSleepTimer;
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
                                    /* Flag set TRUE when timer started */
BOOL        gbfTimerStarted = FALSE;
                                    /* Flag set TRUE when post trigger timer is complete */
BOOL        gbfPostTriggerDone = FALSE;
                                    /* Flag set TRUE when the post trigger timer is running */
BOOL        gbfPostTriggerTime = FALSE;
                                    /* Flag set TRUE when marker on / off timer is complete */
BOOL        gbfMarkerSleepTimer = FALSE;

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

/*
 * The global buffer for the responses from the instrument
 *********************************************************
 *
 */

BYTE        gpbyResponseBuffer[128];

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
        if (gbfStartButton)
        {
                                    /* Stop the post trigger time if running */
            StopTimer(&gPostTriggerTimer);
            gbfPostTriggerTime = FALSE;
                                    /* Show to the user that the button press is being processed */
            ANVR_START_LED_ON;
                                    /* Attempt to send the start command */
            if (StartManualRecord())
            {
                                    /* Start the record period timer */
#ifdef ANVR_RECORD_PERIOD_IN_MS
                StartTimer(&gRecordPeriodTimer,
                           (PCFNTMR)SetStopFlag,
                           PNULL,
                           ANVR_RECORD_PERIOD_IN_MS,
                           SINGLE_SHOT_TIMER);
#endif
               gbfTimerStarted = TRUE;
            }
            else
            {
                                    /* Set the LEDs to alternate flashing */
                SetLedState(&gStartLED,
                            &GC_FLASH_SENDING,
                            ANVR_LED_START_MASK,
                            4UL);
                SetLedStateAlt(&gStopLED,
                               &GC_FLASH_SENDING,
                               ANVR_LED_STOP_MASK,
                               4UL);
            }
                                    /* Acknowledge the button press */
            gbfStartButton = FALSE;
        }
                                    /* Check to see if the stop button has
                                       been pressed */
        if (gbfStopButton) {
                                    /* Switch the stop button LED on */
            ANVR_STOP_LED_ON;
            ANVR_START_LED_OFF;
                                    /* Always drop a marker when the button is pressed */
            if (RecordIsRuning())
            {
                TurnLCDOn();
                DropMarker2();
                                    /* If the post trigger time is not already running */
                if ((!gbfPostTriggerTime)
                &&  (gbfTimerStarted))
                {
                                    /* Set the LED to show that the 
                                       post record timer has been started */
                    SetLedState(&gStopLED,
                                &GC_FLASH_POST_TRIGGER,
                                ANVR_LED_STOP_MASK,
                                ANV_POST_TRIGGER_TIME / 465);
                                    /* Start the post trigger timer */
                    StartPostTriggerTimer(ANV_POST_TRIGGER_TIME);
                                    /* Stop activity on the start LED */
                    SetLedState(&gStartLED,
                                &GC_ALWAYS_OFF,
                                ANVR_LED_START_MASK,
                                0UL);
                }
            }
            else
            {
                                    /* Set the LEDs to alternate flashing */
                SetLedState(&gStartLED,
                            &GC_FLASH_SENDING,
                            ANVR_LED_START_MASK,
                            4UL);
                SetLedStateAlt(&gStopLED,
                               &GC_FLASH_SENDING,
                               ANVR_LED_STOP_MASK,
                               4UL);
            }
                                    /* Stop the record period timer */
#ifdef ANVR_RECORD_PERIOD_IN_MS
            StopTimer(&gRecordPeriodTimer);
            /* NOTE: Stop timer is called before the flag is re-set
               to ensure that two stop events are not processed if 
               the timer ISR occurs between the test and the flag
               acknowlegde */
            gbfTimerStarted = FALSE;
#endif
                                    /* Acknowledge the button press */
            gbfStopButton = FALSE;
        }
        if (gbfPostTriggerDone)
        {
            gbfPostTriggerDone = FALSE;
                                    /* Attempt to send the stop command */
            if (StopManualRecord())
            {
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
 * Description: Timer call-back function to set the post trigger done flag
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

static void SetPostTriggerDoneFlag(void)
{
    gbfPostTriggerDone = TRUE;
    gbfPostTriggerTime = FALSE;
}

/*****************************************************************************
 * Description: Timer call-back function to set the sleep flag
 *
 * Parameters:
 *              N/A
 *
 * Return value: N/A
 *
 *****************************************************************************/

static void SetSleepFlag(void)
{
    gbfMarkerSleepTimer = TRUE;
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
    set_imask_ccr(TRUE);
    StartTimer(&gCanceTransmission,
               (PCFNTMR)SetCancelFlag,
               PNULL,
               dwTimeOut,
               SINGLE_SHOT_TIMER);
    gbfCancel = FALSE;
    set_imask_ccr(FALSE);
}

/*****************************************************************************
 * Description: Function to start a timer for the post trigger delay
 *
 * Parameters:
 *              IN  dwTimeOut - The time period to wait before cancel 
 *
 * Return value: N/A
 *
 *****************************************************************************/

static void StartPostTriggerTimer(DWORD dwTimeOut)
{
    gbfPostTriggerDone = FALSE;
    gbfPostTriggerTime = TRUE;
    StartTimer(&gPostTriggerTimer,
               (PCFNTMR)SetPostTriggerDoneFlag,
               PNULL,
               dwTimeOut,
               SINGLE_SHOT_TIMER);
}

/*****************************************************************************
 * Description: Function to send a message and check the response
 *
 * Parameters:
 *              IN  pbyCommand - Pointer to the command
 *              IN  stLength - The length of the command
 *              IN  pszResponse - Pointer to the expected response string
 *
 * Return value: TRUE if the expected response was found
 *
 *****************************************************************************/

static BOOL SCI3_SendCommandCheckResponse(PBYTE     pbyCommand,
                                          size_t    stCommandLength,
                                          char     *pszResponse)
{
    PBYTE   pbyZero = gpbyResponseBuffer;
    PBYTE   pbyEnd = pbyZero + sizeof(gpbyResponseBuffer);
    while (pbyZero < pbyEnd)
    {
        *pbyZero++ = 0;
    }
    /* Start the cancel timer */
    StartCancelTimer(ANVR_TRANSMIT_TIME_OUT2);
    /* Write the command */
    if (WriteSCI3(pbyCommand, stCommandLength, &gbfCancel))
    {
        /* Read the response */
        ReadSCI3(gpbyResponseBuffer,
                 sizeof(gpbyResponseBuffer),
                 &gbfCancel);
        if (strstr((char*)gpbyResponseBuffer, (char*)pszResponse))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************
 * Description: Function to switch the LCD on
 *
 * Parameters:
 *              N/A
 *
 * Return value: true if the NL-53 is responding to commands
 *
 *****************************************************************************/

static BOOL TurnLCDOn(void)
{
    /* Turn the LCD on */
    if (SCI3_SendCommandCheckResponse(gcpbLCDOn,
                                      sizeof(gcpbLCDOn),
                                      "R+0000"))
    {
        /* Turn the backlight on */
        if (SCI3_SendCommandCheckResponse(gcpbBacklightOn,
                                          sizeof(gcpbBacklightOn),
                                          "R+0000"))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************
 * Description: Function to send the menu key-press message
 *
 * Parameters:
 *              N/A
 *
 * Return value: TRUE if manual recording started
 *
 *****************************************************************************/

static BOOL SendPauseKeyPress(void)
{
    /* Send on/off key-press */
    if (SCI3_SendCommandCheckResponse(gcpbyPauseKeyPress,
                                      sizeof(gcpbyPauseKeyPress),
                                      "R+0000"))
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 * Description: Function to check if wave recording is in progress
 *
 * Parameters:
 *              N/A
 *
 * Return value: TRUE if recording
 *
 *****************************************************************************/

static BOOL RecordIsRuning(void)
{
    /* Check to see if recording is stopped */
    if (SCI3_SendCommandCheckResponse(gcpbyRecCheckMessage,
                                      sizeof(gcpbyRecCheckMessage),
                                      "Stop"))
    {
        return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
 * Description: Function to start manual recording
 *
 * Parameters:
 *              N/A
 *
 * Return value: TRUE if manual recording started
 *
 *****************************************************************************/

static BOOL StartManualRecord_Try(void)
{
    /* This is so the commands and responses can be monitored */
    SCI3_SendCommandCheckResponse(gcpbyEchoOn, sizeof(gcpbyEchoOn), "R+0000");
    /* Check to see if the machine is measuring */
    if (SCI3_SendCommandCheckResponse(gcpbyStoreCheckMessage,
                                      sizeof(gcpbyStoreCheckMessage),
                                      "Start"))
    {
        /* Switch the LCD on */
        TurnLCDOn();
        /* Check to see if recording is stopped */
        if (SCI3_SendCommandCheckResponse(gcpbyRecCheckMessage,
                                          sizeof(gcpbyRecCheckMessage),
                                          "Stop"))
        {
            /* Start the recording */
            if (SendPauseKeyPress())
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************
 * Description: Function to drop marker 1 on
 *
 * Parameters:
 *              N/A
 *
 * Return value: none
 *
 *****************************************************************************/

static BOOL Marker1On(void)
{
    int iCount = ANVR_TRIES;
    while (iCount--)
    {
                            /* Drop a marker */
        if (SCI3_SendCommandCheckResponse(gcpbyMarker1On,
                                          sizeof(gcpbyMarker1On),
                                          "R+0000"))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************
 * Description: Function to drop marker 1 off
 *
 * Parameters:
 *              N/A
 *
 * Return value: none
 *
 *****************************************************************************/

static void Marker1Off(void)
{
    int iCount = ANVR_TRIES;
    while (iCount--)
    {
                            /* Drop a marker */
        if (SCI3_SendCommandCheckResponse(gcpbyMarker1Off,
                                          sizeof(gcpbyMarker1Off),
                                          "R+0000"))
        {
            break;
        }
    }
}

/*****************************************************************************
 * Description: Function to drop marker 2 on
 *
 * Parameters:
 *              N/A
 *
 * Return value: none
 *
 *****************************************************************************/

static BOOL Marker2On(void)
{
    int iCount = ANVR_TRIES;
    while (iCount--)
    {
                            /* Drop a marker */
        if (SCI3_SendCommandCheckResponse(gcpbyMarker2On,
                                          sizeof(gcpbyMarker2On),
                                          "R+0000"))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************
 * Description: Function to drop marker 2 off
 *
 * Parameters:
 *              N/A
 *
 * Return value: none
 *
 *****************************************************************************/

static void Marker2Off(void)
{
    int iCount = ANVR_TRIES;
    while (iCount--)
    {
                            /* Drop a marker */
        if (SCI3_SendCommandCheckResponse(gcpbyMarker2Off,
                                          sizeof(gcpbyMarker2Off),
                                          "R+0000"))
        {
            break;
        }
    }
}

/*****************************************************************************
 * Description: Function to drop marker 2 on and off
 *
 * Parameters:
 *              N/A
 *
 * Return value: none
 *
 *****************************************************************************/

static void DropMarker2(void)
{
    gbfMarkerSleepTimer = FALSE;
    Marker2On();
                                    /* Stop activity on the stop LED */
    SetLedState(&gStopLED,
                &GC_ALWAYS_OFF,
                ANVR_LED_STOP_MASK,
                0UL);
                                    /* Sleep 1 second */
    StartTimer(&gPostTriggerTimer,
               (PCFNTMR)SetSleepFlag,
               PNULL,
               993UL,
               SINGLE_SHOT_TIMER);
                                    /* Wait for timer to set flag in call-back */
    while (FALSE == gbfMarkerSleepTimer)
    {
        /* Wait here */
    }
    Marker2Off();
}

/*****************************************************************************
 * Description: Function to start manual recording
 *
 * Parameters:
 *              N/A
 *
 * Return value: TRUE if manual recording started
 *
 *****************************************************************************/

static BOOL StartManualRecord(void)
{
    int iCount = ANVR_TRIES;
    while (iCount--)
    {
        if (StartManualRecord_Try())
        {
            gbfMarkerSleepTimer = FALSE;
            Marker1On();
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
                                    /* Sleep 1 second */
            StartTimer(&gPostTriggerTimer,
                       (PCFNTMR)SetSleepFlag,
                       PNULL,
                       993UL,
                       SINGLE_SHOT_TIMER);
            /* Wait for timer to set flag in call-back */
            while (FALSE == gbfMarkerSleepTimer)
            {
                /* Wait here */
            }
            Marker1Off();
            return TRUE;
        }
    }
                                    /* Drop a marker */
    Marker1On();
    return FALSE;
}

/*****************************************************************************
 * Description: Function to stop manual recording
 *
 * Parameters:
 *              N/A
 *
 * Return value: TRUE if manual recording was stopped
 *
 *****************************************************************************/

static BOOL StopManualRecord_Try(void)
{
    /* This is so the commands and responses can be monitored */
    SCI3_SendCommandCheckResponse(gcpbyEchoOn, sizeof(gcpbyEchoOn), "R+0000");
                                    /* Check to see if recording is started */
    if (SCI3_SendCommandCheckResponse(gcpbyRecCheckMessage,
                                      sizeof(gcpbyRecCheckMessage),
                                      "Manual"))
    {
                                    /* Stop the recording */
        SendPauseKeyPress();
                                    /* Check that recording has been stopped */
        if (SCI3_SendCommandCheckResponse(gcpbyRecCheckMessage,
                                          sizeof(gcpbyRecCheckMessage),
                                          "Stop"))
        {
            return TRUE;
        }
    }
    else
    {
        if (strstr((char*)gpbyResponseBuffer, "Stop"))
        {
                                    /* Recording has already been stopped */
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    return FALSE;
}

/*****************************************************************************
 * Description: Function to stop manual recording
 *
 * Parameters:
 *              N/A
 *
 * Return value: TRUE if manual recording was stopped
 *
 *****************************************************************************/

static BOOL StopManualRecord(void)
{
    int iCount = ANVR_TRIES;
    while (iCount--)
    {
        if (StopManualRecord_Try())
        {
            return TRUE;
        }
    }
    return FALSE;
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

static void SetLedStateAlt(PLEDINFO  pLedInfo,
                           CPLEDCTL  pLedCtl,
                           BYTE      byLED,
                           DWORD     dwRepeat)
{
                                    /* Set the LED info variables */
    pLedInfo->pLedCtl = pLedCtl;
    pLedInfo->byLED = byLED;
    pLedInfo->dwRepeat = dwRepeat;
                                    /* Set the LED on */
    SetLedStateOff(pLedInfo);
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

/*****************************************************************************
 * Description: Function to search a string for a sub string
 *
 * Parameters:
 *              IN  pszString - Pointer to string to search
 *              IN  pszSubString - Pointer to the string to look for
 *
 * Return value: Pointer to the start of the sub string or NULL if not found
 *
 *****************************************************************************/

static char *strstr(const char *pszString, const char *pszSubString)
{
    int iIndex;
    if (*pszSubString)
    {
        while (*pszString)
        {
            for (iIndex = 0; *(pszString + iIndex) == *(pszSubString + iIndex); iIndex++)
            {
                if (!*(pszSubString + iIndex + 1))
                {
                    return pszString;
                }
            }
            pszString++;
        }
        return NULL;
    }
    else
    {
        return pszString;
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