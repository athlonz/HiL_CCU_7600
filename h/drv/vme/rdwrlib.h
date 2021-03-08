/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdwrlib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : July 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    rdwrlib.c.
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

#ifndef _H_RDWRLIB
#define _H_RDWRLIB

struct rdwr_ctl {
	void __iomem *rdwr_ptr; /* Base Address to access SMEM RAM     */
	int offset;             /* Address offset allocated by mapper  */
};

int alt_rdwr_init( struct althea7910_device *alt);
void alt_rdwr_exit( struct althea7910_device *alt);
int alt_rem_read( struct althea7910_device *alt, struct alt_ioctl_rdwr *rw);
int alt_rem_write( struct althea7910_device *alt, struct alt_ioctl_rdwr *rw);

#endif
