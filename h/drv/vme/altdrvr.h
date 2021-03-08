/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : altdrvr.h
 *    author   : Xavier Paillard
 *    company  : IOxOS
 *    creation : October 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    VxWorks driver header.
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

#ifndef _ALTDRVR_H_
#define _ALTDRVR_H_

#include <vxWorks.h>
#include <intLib.h>
#include <logLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <wdLib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <hwif/vxbus/vxBus.h>
#include <hwif/vxbus/vxbPciLib.h>
/*#include <hwif/buslib/vxbPciMsi.h>*/
#include <hwif/util/vxbDmaBufLib.h>
#include <../src/hwif/h/vxbus/vxbAccess.h>


/*#include <drv/intrCtl/mpApic.h>*/
#include "pciDefines.h"
#include <iosLib.h>
#include "vxbResourceLib.h"
#include "altioctl.h"
/*#include <maplib.h>*/

#include "def.h"

#define ALTHEA7910_NAME "althea7910"    /* Name of the althea7910 device   */
#define DRIVER_VERSION  "1.00" 			/* Driver version tag              */

typedef struct altDevHandle {
	DEV_HDR hdr;
	VXB_DEVICE_ID pDev;
	struct althea7910_device *alt;
} ALT_DEV_HANDLE;

typedef struct altPciDrvCtrl {
	VXB_DEVICE_ID pDev; 				/* Pointer to the devices structure in VxBus */
	ULONG bar; 						/* Virtual start address of BAR              */
	void *handle;		 			/* Working pointer                           */
	size_t size; 					/* Size of mapped BAR                        */
	void *start; 					/* Physical start address                    */
	VXB_RESOURCE *intRes; 			/* Interrupt resource                        */
	VXB_RESOURCE_ADR *pResAdrPMEM; 	/* BAR0 prefetchable memory                  */
	VXB_RESOURCE_ADR *pResAdrMEM; 	/* BAR2 memory space                         */
	VXB_RESOURCE_ADR *pResAdrCSR; 	/* BAR3 CSR                                  */
} ALT_PCI_DRV_CTRL;

/* PCI Vendor / Device ID */
#define VendorID_PEV1100 0x7357 /* IOxOS  */
#define DeviceID_PEV1100 0x7910 /* ALTHEA */

struct shm_ctl {
	void __iomem *sram_ptr; /* Base Address to access SMEM RAM     */
	int sram_size; 			/* Size of SMEM RAM                    */
	int sram_offset; 		/* Address offset allocated by mapper  */
};


struct althea7910_irq_handler {
	void (*func)(struct althea7910_device *, int, void *); 	/* pointer to interrupt handler                  */
	void *arg; 												/* pointer to be passed when handler is executed */
	int cnt; 												/* interrupt counter                             */
	int busy;                                               /* busy flag                                     */  
};

struct althea7910_device {
	ALT_PCI_DRV_CTRL *pDrvCtrl; 			/* VXW device structure               */
	SEM_ID mutex_ctl; 						/* Mutex for locking control device   */
	SEM_ID csr_lock; 						/* Mutex for locking control device   */

	void __iomem *csr_ptr; 					/* Base Address of device registers   */
	struct althea7910_irq_handler *irq_tbl; /* Pointer to interrupt handler table */
	struct map_ctl *map_mas_pci_pmem; 		/* Master map PCI_PMEM BAR0/1         */
	struct map_ctl *map_mas_pci_mem; 		/* Master map PCI_PMEM BAR2           */
	struct map_ctl *map_slv_vme_a32; 		/* Slave map VME A32                  */
	struct map_ctl *map_slv_vme_a24; 		/* Slave map VME A24                  */
	struct map_ctl *map_slv_vme_a16; 		/* Slave map VME A16                  */
	struct map_ctl *map_slv_vme_2ebrc; 		/* Slave map 2eVME Broadcast          */
	struct shm_ctl *shm_ctl; 				/* Control structure for SHM          */
	struct rdwr_ctl *rdwr_ctl; 				/* Control structure for RDWR access  */
	struct sflash_ctl *sflash_ctl;          /* Control structure for SPI FLASH    */
	struct dma_ctl *dma_ctl[DMA_CHAN_NUM];  /* control structure for DMA          */
	struct vme_ctl *vme_ctl;                /* Control structure for RDWR access  */
};



#include "althea7910.h"
#include "ioctllib.h"
#include "altklib.h"

#endif
