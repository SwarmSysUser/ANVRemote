/****************************************************************************
 * File:   IntVect.c
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
 * Description : The unused interrupt vectors
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

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>> Private Functions  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/

#pragma section IntPRG

/* vector 1 Reserved */

/* vector 2 Reserved */

/* vector 3 Reserved */

/* vector 4 Reserved */

/* vector 5 Reserved  */

/* vector 6 Reserved */

/* vector 7 NMI */
__interrupt(vect=7) void INT_NMI(void)
{
    //sleep();
}

/* vector 8 TRAP */
__interrupt(vect=8) void INT_TRAP1(void)
{
    sleep();
}

/* vector 9 TRAP */
__interrupt(vect=9) void INT_TRAP2(void)
{
    sleep();
}

/* vector 10 TRAP */
__interrupt(vect=10) void INT_TRAP3(void)
{
    sleep();
}

/* vector 11 TRAP */
__interrupt(vect=11) void INT_TRAP4(void)
{
    sleep();
}

/* vector 12 Address break */
__interrupt(vect=12) void INT_ABRK(void)
{
    sleep();
}

/* vector 13 SLEEP */
__interrupt(vect=13) void INT_SLEEP(void)
{
    sleep();
}

/* vector 14 IRQ0 */
__interrupt(vect=14) void INT_IRQ0(void)
{
    sleep();
}

/* vector 15 IRQ1 */

/* vector 16 IRQ2 */

/* vector 17 IRQ3 */
__interrupt(vect=17) void INT_IRQ3(void)
{
    sleep();
}

/* vector 18 WKP */
__interrupt(vect=18) void INT_WKP(void)
{
    sleep();
}

/* vector 19 Timer A Overflow */

/* vector 20 Reserved  */

/* vector 21 Timer W */
__interrupt(vect=21) void INT_TimerW(void)
{
    sleep();
}

/* vector 22 Timer V */
#ifdef  SKIP                        /* In hwTimer.c */
__interrupt(vect=22) void INT_TimerV(void)
{
    sleep();
}
#endif
/* vector 23 SCI3 */
__interrupt(vect=23) void INT_SCI3(void)
{
    sleep();
}

/* vector 24 IIC */
__interrupt(vect=24) void INT_IIC(void)
{
    sleep();
}

/* vector 25 ADI */
__interrupt(vect=25) void INT_ADI(void)
{
    sleep();
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