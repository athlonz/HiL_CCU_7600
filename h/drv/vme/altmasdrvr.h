/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : altmasdrvr.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations needed by the ALTHEA master interface
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
 * $Log: altmasdrvr.h,v $
 * Revision 1.1.1.1  2015/10/13 12:55:36  ioxos
 * imported ALTHEA sources
 *
 *  
 *=============================< end file header >============================*/

#include <def.h>

#ifdef VXW
	#include <semLib.h>
#endif

#ifndef _ALTMASDRVR
#define _ALTMASDRVR

#define ALTMAS_DEV_COUNT               16    /* Maximum number of althea7910 devices  */
#define ALTMAS_NAME               "altmas"   /* Name of the althea7910 device        */
#define ALTMAS_MINOR_START              0    /* First minor number                    */
#define ALTMAS_IVEC_COUNT             256    /* VME IVE 8 bit encoded                 */

struct altmas_device
{
  void *alt;                        /* handle to access ALTHEA control device   */
#ifdef LIN
  struct mutex mutex;               /* Mutex for locking ALTHEA master device   */
#endif
#ifdef VXW
  SEM_ID mutex;                     /* Mutex for locking ALTHEA master device   */
#endif  
  int use_cnt;                      /* Device use counter                       */
  struct alt_ioctl_mas_map *map_p;  /* Device mapping control structure         */
  int done  ;                       /* 0 if mapping not doneelse 1              */
  int offset;                       /* Mapping offset (-1 if mapping not done)  */
  int ivec;                         /* Interrupt vector assigned to device      */
  int ip_cnt;                       /* Interrupt pending counter                */
  wait_queue_head_t queue;          /* wait queue to synchronize with VME IRQs  */
};

struct altmas
{
  struct cdev cdev;
  dev_t dev_id;
  void *alt;                        /* handle to access ALTHEA control device */
  struct altmas_device **mas;       /* pointer to the list of minor devices   */
};

struct altmas_ivec_tbl
{
  struct altmas_device *mas[ALTMAS_IVEC_COUNT];
};
#endif /*  _H_ALTMASDRVR */

/*================================< end file >================================*/
