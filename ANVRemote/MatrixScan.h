/****************************************************************************
 * File:   MatrixScan.h
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
 * Description : Generic functions used to debounce switches and matrix
 *               scanned switches.
 *
 * There are three main things that some software may want to know:
 *
 * 1)  When a key has been pressed (Key byOn)
 * 2)  When a key has been released (Key byOff)
 * 3)  If a key is still down (Active byDown)
 *     
 * The keys should be mapped into a BYTE (byKeyData) and the action
 * of pressing a key takes the bit LOW (0).
 *
 * The function MaxtixDebounce reports the three main events for any
 * key byte, maintaining the MTRXDATA strucure.
 *
 * For RFI immunity the key must remain pressed for two consecutive scans, 
 * diferentiating between a spike and a key press. The entire matrix should
 * be scanned once every 10mS. 
 *
 * The scan lines are thought of as columns, and the key return lines are 
 * thought of as rows.
 * 
 * NOTE:
 * The function InitialiseMatrix must be called at before any calls to 
 * DebounceMatrix.The calling function must create array of BYTEs twice 
 * the number of columns. The define MTXR_MAP_SIZE can be used when 
 * dimensioning the array of bytes.
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

#ifndef MATRIXSCAN_H_INCLUDED
#define MATRIXSCAN_H_INCLUDED

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

#define MTXR_MAP_SIZE(NumColumns)   (NumColumns * 2)

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Function Macros <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
 
/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Typedefs <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

/*
 * Define the structure required to debounce a matrix beyboard
 *************************************************************
 *
 */

typedef struct _MTRXDATA
{
                                    /* Pointer to the debounce map base */
    PBYTE   pbyDebounceMapBase;
                                    /* The number of columns required in the
                                       matrix scan */
    UINT    uiNumColumns;

} MTRXDATA,
*PMTRXDATA;

/*
 * Define the structure of the output data
 *****************************************
 *
 */

typedef struct _MTRXOUT
{
    BYTE    byDown;                 /* Bit mapped key data bits set to 1 if
                                       the key is pressed (DOWN) */
    BYTE    byOn;                   /* Bit mapped key data bits set to 1
                                       if there is a new key presses */
    BYTE    byOff;                  /* Bit mapped key data bits set to 1
                                       if there is a new key released */

} MTRXOUT,
*PMTRXOUT;

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
 * Description: Function to initialise the matrix debounce data
 *
 * Parameters:
 *              IN  pbyMap - Pointer to the debounce map
 *              IN  uiNumColumns - The number of columns to debounce
 *              OUT pMtrxData - Pointer to the matrix data to initialise
 *
 * Return value: 0 for success otherwise error code
 *
 *****************************************************************************/

extern  VOID InitialiseMatrix(PBYTE     pbyMap,
                              UINT      uiNumColumns,
                              PMTRXDATA pMtrxData);

/*****************************************************************************
 * Description: Function to debounce bit mapped keys
 *
 * Parameters:
 *              IN  uiColumn - The key scan column index
 *              IN  byKeyData - The key scan data (0 for switch down)
 *              OUT pResult - Pointer to the result data
 *              IN/OUT pMtrxData - Pointer to the matrix data
 *
 * Return value: N/A
 *
 *****************************************************************************/

extern  VOID DebounceMatrix(UINT        uiColumn,
                            BYTE        byKeyData,
                            PMTRXOUT    pResult,
                            PMTRXDATA   pMtrxData);

#ifdef __cplusplus
}
#endif
#endif                              /* MATRIXSCAN_H_INCLUDED */

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