/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : vme.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Oct 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains declarations related to the VME standard.
 *
 *----------------------------------------------------------------------------
 * Copyright (c) 2016 IOxOS Technologies SA <ioxos@ioxos.ch>
 *
 *    Copyright and all other rights in this document are reserved by 
 *    IOxOS Technologies SA. This document contains proprietary information    
 *    and is supplied on express condition that it may not be disclosed, 
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of  
 *    IOxOS Technologies SA                                                          
 *
 *----------------------------------------------------------------------------
 *  Change History
 *
 *  
 *=============================< end file header >============================*/

#ifndef _H_VME
#define _H_VME 

/* VME address modifiers */
#define VME_AM_MASK  	 0x3f
#define VME_XAM_MASK 	 0xff

#define VME_AM_MBLT  	 0x00
#define VME_AM_DATA  	 0x01
#define VME_AM_PROG  	 0x02
#define VME_AM_BLT   	 0x03
#define VME_AM_SUPER  	 0x04
#define VME_AM_A32   	 0x08
#define VME_AM_A16   	 0x28
#define VME_AM_A24   	 0x38

#define VME_AM_USR   	 0x10
#define VME_AM_USR_1 	 0x10
#define VME_AM_USR_2 	 0x14
#define VME_AM_USR_3 	 0x18
#define VME_AM_USR_4 	 0x1c

#define VME_AM_USR_SLT   0x00
#define VME_AM_USR_BLT   0x40
#define VME_AM_USR_MBLT  0x80

#define VME_AM_2eVME     0x0e0
#define VME_AM_2eSST160  0x020
#define VME_AM_2eSST233  0x120
#define VME_AM_2eSST320  0x220
#define VME_AM_2eSST400  0x320

#define VME_AM_CRCSR     0x2f
#define VME_AM_IACK      0x3f

#endif 
