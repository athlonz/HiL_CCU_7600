/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : altslvdrvr.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations needed by the ALTHEA slave interface
 *    device driver
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
 * $Log: altslvdrvr.h,v $
 * Revision 1.1.1.1  2015/10/13 12:55:36  ioxos
 * imported ALTHEA sources
 *
 *  
 *=============================< end file header >============================*/

#ifdef VXW
	#include <semLib.h>
#endif

#ifndef _ALTSLVDRVR
#define _ALTSLVDRVR

#define ALTSLV_DEV_COUNT               16    /* Maximum number of althea7910 devices  */
#define ALTSLV_NAME               "altslv"   /* Name of the althea7910 device        */
#define ALTSLV_MINOR_START              0    /* First minor number                    */

struct altslv_device
{
  void *alt;                        /* handle to access ALTHEA control device    */
  #ifdef LIN
  	  struct mutex mutex;           /* Mutex for locking ALTHEA slave device     */	  
  #endif
  #ifdef VXW
  	SEM_ID mutex;                   /* Mutex for locking ALTHEA slave device     */
  #endif
  int use_cnt;                      /* Device use counter                        */
  struct alt_ioctl_map_req *map_r;  /* Device mapping control structure          */
  struct alt_ioctl_kbuf_req *kbuf_r;/* exchange buffer control structure         */
  int done  ;                       /* 0 if mapping not done else 1              */
  int offset;                       /* Mapping offset (-1 if mapping not done)   */
  int sg_id;                        /* mapper identifier                         */
};

struct altslv
{
  struct cdev cdev;
  dev_t dev_id;
  void *alt;                        /* handle to access ALTHEA control device */
  struct altslv_device **slv;       /* pointer to the list of minor devices   */
};

#endif /*  _H_ALTSLVDRVR */

/*================================< end file >================================*/
