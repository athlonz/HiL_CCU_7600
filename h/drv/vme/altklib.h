/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : altklib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    altklib.c
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

#ifndef _H_ALTKLIB
#define _H_ALTKLIB

unsigned int alt_msb32(unsigned int x);
int alt_dev_init(struct althea7910_device *alt);
void alt_dev_exit(struct althea7910_device *alt);
int alt_irq_init(struct althea7910_device *alt);
void alt_irq_exit(struct althea7910_device *alt);
int alt_map_mas_init(struct althea7910_device *alt);
void alt_map_mas_exit(struct althea7910_device *alt);
int alt_map_slv_init(struct althea7910_device *alt);
void alt_map_slv_exit(struct althea7910_device *alt);
int alt_vme_mas_init(struct althea7910_device *alt);
int alt_map_read(struct althea7910_device *alt, struct alt_ioctl_map_ctl *m);
int alt_map_clear(struct althea7910_device *alt, struct alt_ioctl_map_ctl *m);
int alt_csr_op(struct althea7910_device *alt, struct alt_ioctl_csr_op *csr_op);
int alt_shm_init(struct althea7910_device *alt);
void alt_shm_exit(struct althea7910_device *alt);
int alt_kbuf_alloc(struct althea7910_device *alt, struct alt_ioctl_kbuf_req *r);
int alt_kbuf_free(struct althea7910_device *alt, struct alt_ioctl_kbuf_req *r);
int alt_vme_irq_init(struct althea7910_device *alt);
void alt_vme_irq_exit(struct althea7910_device *alt);
int alt_sflash_init( struct althea7910_device *alt);
int alt_dma_init(struct althea7910_device *alt);
void alt_dma_exit(struct althea7910_device *alt);
int alt_timer_init(struct althea7910_device *alt);

#include "irqlib.h"
#include "vmelib.h"
#include "maplib.h"
#include "mapmaslib.h"
#include "mapslvlib.h"
#include "rdwrlib.h"
#include "sflashlib.h"
#include "dmalib.h"
#include "timerlib.h"

#endif
