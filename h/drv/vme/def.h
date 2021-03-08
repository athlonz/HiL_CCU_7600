/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : def.h
 *    author   : Xavier Paillard
 *    company  : IOxOS
 *    creation : October 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    Type definition for multiplatform.
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

#ifndef _H_DEF
#define _H_DEF

/* Choose the operating system */
/* ****************************** */
#undef WINno					/**/
#undef LINno					/**/
#define VXW						/**/
/* ****************************** */

/* Common useful type definition */
typedef unsigned long  ulong;
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef unsigned long  u64;
typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;


#undef DBG
/* Debug version selection */
#ifdef DBG
	#define DEBUGK(x) 
	#define LOGMSG(x) logMsg x
#else
	#define DEBUGK(x)
	#define LOGMSG(x)
#endif

/* Portable definition for VxWork operating system */

#include <vxWorks.h>

#define __iomem
#define IOREAD32(a, b) bswap32(vxbRead32(a, (UINT32 *) b))
#define IOWRITE32(a, b, c) vxbWrite32(a, (UINT32 *) c, bswap32(b))
#define EXPORTSYMBOL(x) 
#define task_tgid_nr(current) 0
#define MUTEX_INIT(a, b, c) a = semBCreate(b, c)
#define MUTEX_DESTROY(a) semDelete(a)
#define MUTEX_LOCK(a, b) semTake(a, b)
#define MUTEX_UNLOCK(a) semGive(a)
#define RANDOM() rand()
#define KMALLOC(a, b) malloc(a)
#define KFREE(a) free(a)
#define IOREMAP_NOCACHE(a, b, c) pmapGlobalMap(a, b, c)
#define IOUNMAP(a, b) pmapGlobalUnmap(a, b)
#define SEMA_INIT(a, b, c) a = semCCreate(c, b)
#define UP(a) semGive(a)
#define DOWN_TIMEOUT(a, b) semTake(a, (_Vx_ticks_t)b)	
#define DOWN_INTERRUPTIBLE(a, b) semTake(a, b)


#endif
