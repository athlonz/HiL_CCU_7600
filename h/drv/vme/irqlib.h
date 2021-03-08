/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : irqlib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : July 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    irqlib.c.
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

#ifndef _H_IRQLIB
#define _H_IRQLIB

int alt_irq_register(struct althea7910_device *alt, int src,
		void (*func)(struct althea7910_device *, int, void *), void *arg);
void alt_irq_unregister(struct althea7910_device *alt, int src);
int alt_irq_check_busy(struct althea7910_device *alt, int src);
void alt_irq_spurious(struct althea7910_device *p, int src, void *arg);
int alt_irq_mask(struct althea7910_device *alt, int op, int src);

#endif
