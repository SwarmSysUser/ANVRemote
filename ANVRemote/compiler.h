/****************************************************************************
 * File:   compiler.h
 * Author: Adam Fullerton
 * Design: Adam Fullerton
 * Date:   01/08/02
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
 * Description : C/C++ Compiler adjustment macros
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

#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

/*
 * Embedded CPU data type definitions
 ************************************
 *
 */
                                    /* Set a few #defines for potential 
                                       compilers used */                
#define                 MCS      0  /* Hitachi */
#define                 GNU      1  /* Hitachi + many other devices */
#define                 IAR      2  /* Hitachi + some other devices */
#define                 MSV      3  /* Microsoft Visual C */
  
                                    /* Test the compiler intrinisic defs */
#ifdef __GNUC__                     /* GNU compiler - C mode   */
    #define COMPILER    GNU
#elif defined(__GNUG__)             /* GNU compiler - C++ mode */
    #define COMPILER    GNU
#elif defined __IAR_SYSTEMS_ICC
    #define COMPILER    IAR         /* IAR compiler            */           
#elif defined _MSC_VER    
    #define COMPILER    MSV         /* Microsoft c compiler    */
#else    
    #define COMPILER    MCS         /* MCS compiler            */
#endif

/****************************************************************************
 *>>>>>>>>>>>>>>>>>>>>>>>>>> Pragma Macros <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 ****************************************************************************/
                   
                                    /* Visual Cpp */
#if COMPILER == MSV
#define PACK1                       pack(1)
#define UNPACK                      pack()
#else
                                    /* SH series */
#if defined _SH4 | defined _SH3E | defined _SH3 | defined _SH2E | defined _SH2 | defined _SH1
#define PACK1                       # ## pragma pack1
#define UNPACK                      # ## pragma unpack
#else
                                    /* H8S series */         
#define PACK1                       # ## pragma pack 1
#define UNPACK                      # ## pragma unpack
#endif

#endif

#endif                              /* COMPILER_H_INCLUDED */

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