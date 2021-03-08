/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : vmelib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : July 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    vmelib.c.
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

#ifndef _H_VMELIB
#define _H_VMELIB

#define VME_SET_NUM    16 /* Number of VME interrupt set */

#define VME_IRQ_STS_IDLE          0x00
#define VME_IRQ_STS_ALLOCATED     0x01
#define VME_IRQ_STS_ARMED         0x02
#define VME_IRQ_STS_WAITING       0x03
#define VME_IRQ_STS_INTERRUPTED   0x04

struct vme_time {
	uint time;
	uint utime;
};

struct vme_irq_ctl {
	uint set;
	uint vector;
#ifdef LIN
	struct semaphore sem; 	/* Semaphore to synchronize with VME interrput  */
#endif
#ifdef VXW	
	SEM_ID sem; 			/* Semaphore to synchronize with VME interrput  */
#endif	
	/*pid_t tgid;*/
	int state;
};
struct vme_ctl {
#ifdef LIN
	struct mutex vme_lock; 	/* Mutex to lock VME access                     */
#endif
#ifdef VXW
	SEM_ID vme_lock; 		/* Mutex to lock VME access                     */
#endif	
	int irq_set;
	uint idx[VME_SET_NUM];
	struct vme_irq_ctl irq_ctl[VME_SET_NUM];
};

void alt_vme_arb_set_conf(struct althea7910_device *alt, struct alt_ioctl_vme_arb_conf *conf);
void alt_vme_arb_get_conf(struct althea7910_device *alt, struct alt_ioctl_vme_arb_conf *conf);
void alt_vme_mas_set_conf(struct althea7910_device *alt, struct alt_ioctl_vme_mas_conf *conf);
void alt_vme_mas_get_conf(struct althea7910_device *alt, struct alt_ioctl_vme_mas_conf *conf);
void alt_vme_slv_set_conf(struct althea7910_device *alt, struct alt_ioctl_vme_slv_conf *conf);
void alt_vme_slv_get_conf(struct althea7910_device *alt, struct alt_ioctl_vme_slv_conf *conf);
int alt_vme_igen_set(struct althea7910_device *alt, struct alt_ioctl_vme_igen *igen);
void alt_vme_igen_get(struct althea7910_device *alt, struct alt_ioctl_vme_igen *igen);
void alt_vme_igen_clear(struct althea7910_device *alt);
int alt_vme_irq_alloc(struct althea7910_device *alt, struct alt_ioctl_vme_irq *irq);
int alt_vme_irq_arm(struct althea7910_device *alt, struct alt_ioctl_vme_irq *irq);
int alt_vme_irq_clear(struct althea7910_device *alt, struct alt_ioctl_vme_irq *irq);
int alt_vme_irq_wait(struct althea7910_device *alt, struct alt_ioctl_vme_irq *irq);
int alt_vme_irq_free(struct althea7910_device *alt, struct alt_ioctl_vme_irq *irq);
int alt_vme_irq_reset(struct althea7910_device *alt);

#endif 
