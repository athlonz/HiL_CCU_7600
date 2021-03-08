
/* vxbSdMmcXbd.h - Generic SD/MMC XBD block driver header */

/*
 * Copyright (c) 2009, 2012 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01b,06apr12,sye  fixed compile issue when included by a CPP file. (WIND00342562)
01a,18aug09,b_m  written.
*/

#ifndef __INCvxbSdMmcXbdh
#define __INCvxbSdMmcXbdh

/* includes */

#include <drv/xbd/xbd.h>

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

/* SD/MMC XBD service task name */

#define SDMMC_XBD_SVC_TASK_NAME     "sdMmcXbdSvc"

/* SD/MMC XBD service task priority */

#define SDMMC_XBD_SVC_TASK_PRI      50

/* SD/MMC XBD service task stack size */

#define SDMMC_XBD_SVC_TASK_STACK    8192

#define SDMMC_CLK_FREQ_400KHZ       400000
#define SDMMC_CLK_FREQ_25MHZ        25000000
#define SDMMC_CLK_FREQ_26MHZ        26000000
#define SDMMC_CLK_FREQ_50MHZ        50000000
#define SDMMC_CLK_FREQ_52MHZ		52000000
/* SD/MMC XBD block device */

typedef struct sdmmcXbdDev
    {
    XBD             xbd;                /* xbd device */
    devname_t       devName;            /* name of xbd device */
    int             svcTaskId;          /* service task id */
    SEM_ID          bioReady;           /* wake up bio service task */
    SEM_ID          bioMutex;           /* lock bio queue */
    struct bio *    bioHead;            /* head of bio queue */
    struct bio *    bioTail;            /* tail of bio queue */
    SEM_ID          xbdSemId;           /* used for xbdBlkDevCreateSync() */
    BOOL            xbdInserted;        /* inserted or removed */
    BOOL            xbdInstantiated;    /* TRUE if stack init is complete */
    void *          card;               /* pointer to SD/MMC card */
    FUNCPTR         blkRd;              /* block read routine */
    FUNCPTR         blkWt;              /* block write routine */
    } SDMMC_XBD_DEV;

/* forward declarations */

device_t sdMmcXbdDevCreate (void * sdMmc, char * devName);
STATUS sdMmcXbdDevDelete (void * sdMmc);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCvxbSdMmcXbdh */
