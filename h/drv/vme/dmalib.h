/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : dmalib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : July 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    dmalib.c.
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

#ifndef _H_DMALIB
#define _H_DMALIB

#define DMA_CTRL_NUM             2        /* Number of DMA controllers         */
#define DMA_CHAIN_NUM           32        /* Max number of descriptor in chain */

#define DMA_DESC_OFFSET     0x3000        /* Max number of descriptor in chain */
#define DMA_DESC_SIZE        0x800        /* Max size for desc chain (2*32*32) */
#define DMA_RING_OFFSET     0x1000        /* Offset in SHM for ring buffer     */
#define DMA_RING_SIZE       0x1000        /* Ring size 4k                      */

#define DMA_STS_IDLE          0x00
#define DMA_STS_ALLOCATED     0x01
#define DMA_STS_STARTED       0x02
#define DMA_STS_WAITING       0x03
#define DMA_STS_DONE          0x04

/* DMA chain hardware descriptor (8*32bit) */
struct dma_desc {
	uint ctl; 			/* Control register                */
	uint wcnt; 			/* Word counter (in bytes)         */
	uint shm_addr; 		/* Offset in shared memory         */
	uint next; 			/* Offset of next chain descriptor */
	uint rem_addr_l; 	/* Remote address bit  0:31        */
	uint rem_addr_h; 	/* Remote address bit 32:63        */
	uint status; 		/* Transfer status                 */
	uint time_msec; 	/* Msec timer                      */
};

struct dma_ctl
{
  int chan;                      /* DMA controller channel number                                   */
  int state;                     /* DMA channel state                                               */
  struct althea7910_device *alt;
  void __iomem *desc_ptr;        /* Kernel pointer to SHM allocated to DMA descriptor               */
  uint desc_offset;              /* PCI base address of SHM allocated to DMA descriptor             */
  uint ring_offset;              /* PCI base address of SHM allocated to DMA ring buffer            */
#ifdef LIN
	struct mutex dma_lock;     	 /* Mutex to lock DMA access                                        */
	struct semaphore sem;        /* Semaphore to synchronize with DMA interrputs                    */	
#endif
#ifdef VXW	
	SEM_ID dma_lock; 			 /* Mutex to lock DMA access                                        */
	SEM_ID sem; 			     /* Semaphore to synchronize with VME interrput                     */                               
#endif
  
  struct dma_desc rd_desc;
  struct dma_desc wr_desc;
  int status;                    /* DMA transfer status                                             */
  int irq;                       /* IRQs associated to the DMA channel                              */
};

void alt_dma_irq(struct althea7910_device *alt, int src, void *dma_ctl_p);
int dma_init(struct dma_ctl *dma);
int alt_dma_move(struct althea7910_device *alt, struct alt_ioctl_dma_req *dma_req_p);
int alt_dma_wait(struct althea7910_device *alt, struct alt_ioctl_dma_req *dma_req_p);
int alt_dma_status(struct althea7910_device *alt, struct alt_ioctl_dma_sts *dma_sts_p);
int alt_dma_alloc(struct althea7910_device *alt, struct alt_ioctl_dma *dma_p);
int alt_dma_free(struct althea7910_device *alt, struct alt_ioctl_dma *dma_p);
int alt_dma_clear(struct althea7910_device *alt, struct alt_ioctl_dma *dma_p);

#endif 
