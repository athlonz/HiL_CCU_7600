/* RFM2gDrv.c - RFM2g PCI Driver */

/* 
===============================================================================
                            COPYRIGHT NOTICE

    Copyright (C) 2001, 2006 GE Fanuc Embedded Systems, Inc.
    International Copyright Secured.  All Rights Reserved.

-------------------------------------------------------------------------------
 
modification history
--------------------
$Workfile: RFM2gDrv.h $
$Revision: 53 $
$Modtime: 11/05/07 11:00a $
*/

/*
DESCRIPTION:  Header file for RFM2gDrv.c


INCLUDE FILES: 
*/

#ifndef RFM2GDRV_VXW_H
#define RFM2GDRV_VXW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "rfm2g_types.h"

/* Driver Initialization data */
#define RFM2G_REG_SPACE   0x1000  
#define RFM2G_IO_SIZE     0       /* Not used 0x100 */
#define RFM2G_MAX_UNITS   4       /* Number of devices supported */

#if (CPU_FAMILY == PPC)

    /* 
     * The code below was tested on MV2400, MV2700 and MV5100 BSP's.
     * The user is responsible for modifying as necessary to 
     * support their BSP.  
     */

    #ifdef BSPX_COMPILE_TRACE
    #warning RFM2gDrvPci : Compiling For PPC 
    #endif

    #define RFM2G_MEM_BEGIN    0x50000000  /* <-- add to sysPhysMemDesc[] - above VME A32 */

    #if defined(PMC_INT_LVL1)                /* MV2700, MV2400, etc */

        #define RFM2G_INT_LVL1 PMC_INT_LVL1  /* lower PMC slot */
        #define RFM2G_INT_LVL2 PMC_INT_LVL4  /* upper PMC slot */
        #define RFM2G_INT_LVL3 PMC_INT_LVL1  /* not supported  */
        #define RFM2G_INT_LVL4 PMC_INT_LVL1  /* not supported  */

    #elif defined(PCI_INT_LVL1)              /* MV5100, etc. */

        #define RFM2G_INT_LVL1 PCI_INT_LVL1  /* lower PMC slot */
        #define RFM2G_INT_LVL2 PCI_INT_LVL4  /* upper PMC slot */
        #define RFM2G_INT_LVL3 PCI_INT_LVL1  /* not supported  */
        #define RFM2G_INT_LVL4 PCI_INT_LVL1  /* not supported  */

    #else /* Uses PCI Autoconfiguration */

        #define RFM2G_INT_LVL1 -1  /* PMC#1 (upper) slot */
        #define RFM2G_INT_LVL2 -1  /* PMC#2 (lower) slot */
        #define RFM2G_INT_LVL3 -1  /* not supported  */
        #define RFM2G_INT_LVL4 -1  /* not supported  */

        /* see above for example */    
        #warning BSP did not define int level used for PMC sites, assuming PCI auto configuration
    
    #endif

    #define RFM2G_DEVICE_NUM0 0x10 /* lower slot IDSEL-to-MPIC interrupt routing */
    #define RFM2G_DEVICE_NUM1 0x11 /* upper slot IDSEL-to-MPIC interrupt routing */
    #define RFM2G_DEVICE_NUM2 -1   /* not supported IDSEL-to-MPIC interrupt routing */
    #define RFM2G_DEVICE_NUM3 -1   /* not supported IDSEL-to-MPIC interrupt routing */

#else /* x86 */

/**(for x86 VMIC)********************************************************************
 * 1st RFM Base -  (0xD0000000 - 0xD7FFFFFF) or (0xD0000000 - 0xD3FFFFFF)
 * 2nd RFM Base -  (0xD8000000 - 0xDFFFFFFF) or (0xD8000000 - 0xDBFFFFFF)
 * 3rd RFM Base -  (0xE0000000 - 0xE7FFFFFF) or (0xE0000000 - 0xE3FFFFFF)
 * 4th RFM Base -  (0xE8000000 - 0xEFFFFFFF) or (0xE8000000 - 0xEBFFFFFF)
 *
 * 1st Local Mem Base -  (0xF0000000 - 0xF0000FFF)
 * 1st CtrlSatus Base -  (0xF0001000 - 0xF0001FFF)
 * 2nd Local Mem Base -  (0xF0002000 - 0xF0002FFF)
 * 2nd CtrlSatus Base -  (0xF0003000 - 0xF0003FFF)
 * 3rd Local Mem Base -  (0xF0004000 - 0xF0004FFF)
 * 3rd CtrlSatus Base -  (0xF0005000 - 0xF0005FFF)
 * 4th Local Mem Base -  (0xF0006000 - 0xF0006FFF)
 * 4th CtrlSatus Base -  (0xF0007000 - 0xF0007FFF)
 * *********************************************************************************/

/* 
 * By default the x86 build uses the values programmed by BIOS.  To override the BIOS 
 * settings, add the following to RFM2g_Drv.c
 *
 * #define RFM2G_PCI_CFG_TYPE PCI_CFG_FORCE
 *
 * You may need to modify the univ232.h.  Reduce the VME_A32_BOUND_ADDRESS definition. 
 *
 * Example:
 *
 * #define VME_A32_BOUND_ADDRESS    0x20000000
 */

    #ifdef BSPX_COMPILE_TRACE
    #warning RFM2gDrvPci : Compiling For X86
    #endif

    #define RFM2G_MEM_BEGIN  0xD0000000  /*  */
    #define RFM2G_IO_BEGIN   -1          /* Not for X86 */
    #define RFM2G_INT_LVL1   -1          /* Not for X86 */
    #define RFM2G_INT_LVL2   -1          /* Not for X86 */
    #define RFM2G_INT_LVL3   -1          /* Not for X86 */
    #define RFM2G_INT_LVL4   -1          /* Not for X86 */
    #define RFM2G_DEVICE_NUM0 -1         /* Not for X86 */
    #define RFM2G_DEVICE_NUM1 -1         /* Not for X86 */
    #define RFM2G_DEVICE_NUM2 -1         /* Not for X86 */
    #define RFM2G_DEVICE_NUM3 -1         /* Not for X86 */

#endif

/* BEGIN Driver Def's */

/* Driver Initialization data */
typedef struct _rfm2gResource
{
  RFM2G_UINT32 localMem;
  RFM2G_INT32  sizeLocalMem;
  RFM2G_UINT32 localIO;
  RFM2G_INT32  sizeLocalIO;
  RFM2G_UINT32 ctrlStatus;
  RFM2G_INT32  sizeCtrlStatus;
  RFM2G_UINT32 memBase;
  RFM2G_INT32  sizeMemBase;
  RFM2G_UINT8  intLvl;
  RFM2G_UINT32 stateMask;
  RFM2G_UINT32 state;
  RFM2G_INT32  bus;
  RFM2G_INT32  device;
  RFM2G_INT32  func;
} RFM2G_RESOURCE;

/* RFM Control and Status register offset values */
#define RFM2G_BRV    0x0000
#define RFM2G_BID    0x0001
#define RFM2G_BUILD_ID 0x0002
#define RFM2G_NID    0x0004
#define RFM2G_LCSR1  0x0008
#define RFM2G_RESV2  0x000c
#define RFM2G_LISR   0x0010
#define RFM2G_LIER   0x0014
#define RFM2G_NTD    0x0018
#define RFM2G_NTN    0x001c
#define RFM2G_NIC    0x001d
#define RFM2G_RESV3  0x001e
#define RFM2G_ISD1   0x0020
#define RFM2G_SID1   0x0024
#define RFM2G_RESV4  0x0025
#define RFM2G_ISD2   0x0028
#define RFM2G_SID2   0x002c
#define RFM2G_RESV5  0x002d
#define RFM2G_ISD3   0x0030
#define RFM2G_SID3   0x0034
#define RFM2G_RESV6  0x0035
#define RFM2G_INITD  0x0038
#define RFM2G_INITN  0x003C

/* ON/OFF definitions */
#define RFM2G_BIT_ON  1
#define RFM2G_BIT_OFF 0

/* Definition of the vrfm2g_nic (Network Interrupt Command) register */
#define RFM2G_NIC_RESET   (0x00)  /* Reset Int                   */
#define RFM2G_NIC_INT1    (0x01)  /* Net Int 1                   */
#define RFM2G_NIC_INT2    (0x02)  /* Net Int 2                   */
#define RFM2G_NIC_INT3    (0x03)  /* Net Int 3                   */
#define RFM2G_NIC_RSVD1   (0x04)  /* Reserved, used for OWN test */
#define RFM2G_NIC_INT4    (0x07)  /* Net Initialized Int         */
#define RFM2G_NIC_GLOBAL  (0x08)  /* Global Command bit          */

/* DMA */
#define RFM2G_DMA_SIZE_MASK 0x007FFFFF
#define RFM2G_DMA_VALID     0x80000000

/* BIT3 of DMADPR1 */
#define RFM2G_TO_PCI 1
#define PCI_TO_RFM2G 0

/* Default threshold used by the driver */
#define RFM2G_DEFAULT_DMA_THRESHOLD 32

/* Max DMA transfer size */
#define RFM2G_DEFAULT_TXFER_SIZE 256
#define RFM2G_MAX_TXFER_SIZE     0x007FFF80

/* Value of BIT20 and BIT21 of the LCSR1 register */
#define RFM2G_SIZE_64M  0x0
#define RFM2G_SIZE_128M 0x1
#define RFM2G_SIZE_256M 0x2

/* BIT MASK Definitions */
#define RFM2G_BIT0  0x00000001
#define RFM2G_BIT1  0x00000002
#define RFM2G_BIT2  0x00000004
#define RFM2G_BIT3  0x00000008
#define RFM2G_BIT4  0x00000010
#define RFM2G_BIT5  0x00000020
#define RFM2G_BIT6  0x00000040
#define RFM2G_BIT7  0x00000080
#define RFM2G_BIT8  0x00000100
#define RFM2G_BIT9  0x00000200
#define RFM2G_BIT10 0x00000400
#define RFM2G_BIT11 0x00000800
#define RFM2G_BIT12 0x00001000
#define RFM2G_BIT13 0x00002000
#define RFM2G_BIT14 0x00004000
#define RFM2G_BIT15 0x00008000
#define RFM2G_BIT16 0x00010000
#define RFM2G_BIT17 0x00020000
#define RFM2G_BIT18 0x00040000
#define RFM2G_BIT19 0x00080000
#define RFM2G_BIT20 0x00100000
#define RFM2G_BIT21 0x00200000
#define RFM2G_BIT22 0x00400000
#define RFM2G_BIT23 0x00800000
#define RFM2G_BIT24 0x01000000
#define RFM2G_BIT25 0x02000000
#define RFM2G_BIT26 0x04000000
#define RFM2G_BIT27 0x08000000
#define RFM2G_BIT28 0x10000000
#define RFM2G_BIT29 0x20000000
#define RFM2G_BIT30 0x40000000
#define RFM2G_BIT31 0x80000000

/* Declare interrupt types */
#define RFM2G_LISR_UNUSED      0x00004000
#define RFM2G_LISR_DISABLE     0xFFFF03F3

/* RFM2g VxWorks Driver Specific Functions */

STATUS RFM2gInit(void);   /* Function called in sysHwInit */

#ifdef __cplusplus
}
#endif

#endif
