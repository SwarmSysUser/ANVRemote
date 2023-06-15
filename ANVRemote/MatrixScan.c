/****************************************************************************
 * File:   MatrixScan.c
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

#include "MatrixScan.h"

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

VOID InitialiseMatrix(PBYTE     pbyMap,
                      UINT      uiNumColumns,
                      PMTRXDATA pMtrxData)
{
    pMtrxData->pbyDebounceMapBase = pbyMap;
    pMtrxData->uiNumColumns = uiNumColumns;
                                /* Double the number to initialise */
    uiNumColumns <<= 1;
    while (uiNumColumns--) {
        *pbyMap++ = (BYTE)0xFF;
    }
}

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

VOID DebounceMatrix(UINT        uiColumn,
                    BYTE        byKeyData,
                    PMTRXOUT    pResult,
                    PMTRXDATA   pMtrxData)
{
    register BYTE    byRowMap, byChanges;
    register PBYTE   pbyMap1, pbyMap2;
                                    /* Set the map pointers */
    pbyMap1 = pMtrxData->pbyDebounceMapBase + uiColumn;
    pbyMap2 = pMtrxData->uiNumColumns + pbyMap1;
                                    /* Must scan keys twice to flag a key 
                                       press  - so logical OR with oldest map. 
                                       This is held in byRowMap because it is 
                                       replaced in the map map */
    byRowMap = *pbyMap2;
                                    /* Shift maps and logical or to debounce */
    *pbyMap2 = (BYTE)(*pbyMap1 | byKeyData); 
    *pbyMap1 = byKeyData;
                                    /* Spot changes */
    byChanges = (BYTE)(*pbyMap2 ^ byRowMap);
                                    /* Now figure out how many keys are still 
                                       pressed - Keys that are down are shown 
                                       as 1's */
    pResult->byDown = (BYTE)(byChanges | ~byRowMap);
                                    /* Now mask key offs from changes 
                                       and with new data to get 'Ons' */
    pResult->byOn = (BYTE)(byChanges & byRowMap);
                                    /* Now spot key offs - the difference 
                                       between changes and key ons */
    pResult->byOff = (BYTE)(byChanges ^ pResult->byOn);
}

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Private Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
 
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