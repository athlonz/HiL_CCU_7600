/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : altioctl.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : June 2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *  This file contains the declarations for IOCTL commands supported by the
 * 	ALTHEA7910 control driver.
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

#ifndef _H_ALTIOCTL
#define _H_ALTIOCTL

#ifdef _cplusplus
extern "C" {
#endif


#include "def.h"
#include "althea7910.h"



#define ALT_IOCTL_OP_MASK    0xffff0000

#define ALT_IOCTL_ID              0x00000000
#define ALT_IOCTL_ID_NAME         (ALT_IOCTL_ID | 0x01)
#define ALT_IOCTL_ID_VERSION      (ALT_IOCTL_ID | 0x02)

#define ALT_IOCTL_CSR             0x00010000
#define ALT_IOCTL_CSR_RD          (ALT_IOCTL_CSR | 0x01)
#define ALT_IOCTL_CSR_WR          (ALT_IOCTL_CSR | 0x02)
#define ALT_IOCTL_CSR_OR          (ALT_IOCTL_CSR | 0x03)
#define ALT_IOCTL_CSR_XOR         (ALT_IOCTL_CSR | 0x04)
#define ALT_IOCTL_CSR_AND         (ALT_IOCTL_CSR | 0x05)
#define ALT_IOCTL_CSR_RDm         (ALT_IOCTL_CSR | 0x11)
#define ALT_IOCTL_CSR_WRm         (ALT_IOCTL_CSR | 0x12)
#define ALT_IOCTL_CSR_ORm         (ALT_IOCTL_CSR | 0x13)
#define ALT_IOCTL_CSR_XORm        (ALT_IOCTL_CSR | 0x14)
#define ALT_IOCTL_CSR_ANDm        (ALT_IOCTL_CSR | 0x15)
#define ALT_IOCTL_CSR_SMON        (ALT_IOCTL_CSR | 0x100)
#define ALT_IOCTL_CSR_SMON_RD     (ALT_IOCTL_CSR | 0x101)
#define ALT_IOCTL_CSR_SMON_WR     (ALT_IOCTL_CSR | 0x102)
#define ALT_IOCTL_CSR_PCIEP       (ALT_IOCTL_CSR | 0x200)
#define ALT_IOCTL_CSR_PCIEP_RD    (ALT_IOCTL_CSR | 0x201)
#define ALT_IOCTL_CSR_PCIEP_WR    (ALT_IOCTL_CSR | 0x202)

struct alt_ioctl_csr_op {
	int operation;
	int offset;
	int data;
	int mask;
};

#define ALT_IOCTL_MAP                 0x00020000
#define ALT_IOCTL_MAP_READ            (ALT_IOCTL_MAP | 0x1000)
#define ALT_IOCTL_MAP_MAS             (ALT_IOCTL_MAP | 0x2000)
#define ALT_IOCTL_MAP_SLV             (ALT_IOCTL_MAP | 0x3000)
#define ALT_IOCTL_MAP_CLEAR           (ALT_IOCTL_MAP | 0x4000)
#define ALT_IOCTL_MAP_OP_MSK          (ALT_IOCTL_MAP | 0xf000)
#define ALT_IOCTL_MAP_MAS_ALLOC       (ALT_IOCTL_MAP_MAS | 0x1)
#define ALT_IOCTL_MAP_MAS_FREE        (ALT_IOCTL_MAP_MAS | 0x2)
#define ALT_IOCTL_MAP_MAS_MODIFY      (ALT_IOCTL_MAP_MAS | 0x3)
#define ALT_IOCTL_MAP_MAS_GET         (ALT_IOCTL_MAP_MAS | 0x4)
#define ALT_IOCTL_MAP_SLV_ALLOC       (ALT_IOCTL_MAP_SLV | 0x1)
#define ALT_IOCTL_MAP_SLV_FREE        (ALT_IOCTL_MAP_SLV | 0x2)
#define ALT_IOCTL_MAP_SLV_MODIFY      (ALT_IOCTL_MAP_SLV | 0x3)
#define ALT_IOCTL_MAP_SLV_GET         (ALT_IOCTL_MAP_SLV | 0x4)

#define MAP_ID_INVALID         0
#define MAP_ID_MAS_PCIE_MEM    1
#define MAP_ID_MAS_PCIE_PMEM   2
#define MAP_ID_SLV_VME         4
#define MAP_ID_SLV_VME_A32     4
#define MAP_ID_SLV_VME_A24     5
#define MAP_ID_SLV_VME_A16     6
#define MAP_ID_SLV_VME_BRC     7

#define MAP_SPACE_INVALID     -1
#define MAP_SPACE_PCIE         0  
#define MAP_SPACE_VME          1  
#define MAP_SPACE_SHM          2  
#define MAP_SPACE_USR          3  

#define MAP_FLAG_FREE          0
#define MAP_FLAG_BUSY          1
#define MAP_FLAG_PRIVATE       2
#define MAP_FLAG_FORCE         4
#define MAP_FLAG_LOCKED        8

#define MAP_REM_ADDR_AUTO     -1  
#define MAP_LOC_ADDR_AUTO     -1  
#define MAP_IDX_INV 		  -1
#define MAP_SWAP_NO 		 0x0
#define MAP_SWAP_AUTO 		 0x1
#define MAP_SWAP_DW 		 0x2
#define MAP_SWAP_QW 		 0x3	
#define MAP_SWAP_MASK 	 	 0x3

struct alt_ioctl_map_ctl {
	struct alt_map_blk {
		char flag;
		char usr;
		short npg;
		uint mode;
		ulong rem_addr;
		/*uint tgid;*/
	}*map_p;
	char rsv;
	char sg_id;
	short pg_num;
	int pg_size;
	ulong win_base;
	ulong win_size;
};

struct alt_ioctl_map_mode {
	char sg_id;
	char flags;
	short hw;
	char ads;
	char space;
	char swap;
	char am;
};

struct alt_ioctl_map_req {
	ulong rem_addr;
	ulong loc_addr;
	uint size;
	struct alt_ioctl_map_mode mode;
};

struct alt_ioctl_map_sts {
	ulong rem_base;
	ulong loc_base;
	uint size;
	struct alt_ioctl_map_mode mode;
};

struct alt_ioctl_map_win {
	int pg_idx;
	union {
		struct alt_ioctl_map_sts sts;
		struct alt_ioctl_map_req req;
	}xxx;
};

#define ALT_IOCTL_ITC            0x00050000
#define ALT_IOCTL_ITC_MSK_SET    (ALT_IOCTL_ITC | 0x1)
#define ALT_IOCTL_ITC_MSK_CLEAR  (ALT_IOCTL_ITC | 0x2)

/* ALTHEA interrupt sources */
#define ITC_SRC_ILOC_ALM0         0x00
#define ITC_SRC_VME_SYSFAIL       0x10
#define ITC_SRC_VME_IRQ1          0x11
#define ITC_SRC_VME_IRQ2          0x12
#define ITC_SRC_VME_IRQ3          0x13
#define ITC_SRC_VME_IRQ4          0x14
#define ITC_SRC_VME_IRQ5          0x15
#define ITC_SRC_VME_IRQ6          0x16
#define ITC_SRC_VME_IRQ7          0x17
#define ITC_SRC_VME_ACFAIL        0x18
#define ITC_SRC_VME_MASERR        0x19
#define ITC_SRC_VME_IACK          0x1a
#define ITC_SRC_VME_GTM           0x1b
#define ITC_SRC_VME_LM0           0x1c
#define ITC_SRC_VME_LM1           0x1d
#define ITC_SRC_VME_LM2           0x1e
#define ITC_SRC_VME_LM3           0x1f
#define ITC_SRC_DMA_RD0_END       0x20
#define ITC_SRC_DMA_RD0_ERR       0x21
#define ITC_SRC_DMA_RD1_END       0x22
#define ITC_SRC_DMA_RD1_ERR       0x23
#define ITC_SRC_DMA_WR0_END       0x24
#define ITC_SRC_DMA_WR0_ERR       0x25
#define ITC_SRC_DMA_WR1_END       0x26
#define ITC_SRC_DMA_WR1_ERR       0x27
#define ITC_SRC_VME_MBX0          0x28
#define ITC_SRC_VME_MBX1          0x29
#define ITC_SRC_VME_MBX2          0x2a
#define ITC_SRC_VME_MBX3          0x2b
#define ITC_SRC_VME_MBX4          0x2c
#define ITC_SRC_VME_MBX5          0x2d
#define ITC_SRC_VME_MBX6          0x2e
#define ITC_SRC_VME_MBX7          0x2f
#define ITC_SRC_VME_FIF0          0x28
#define ITC_SRC_VME_FIF1          0x29
#define ITC_SRC_VME_FIF2          0x2a
#define ITC_SRC_VME_FIF3          0x2b
#define ITC_SRC_VME_FIF4          0x2c
#define ITC_SRC_VME_FIF5          0x2d
#define ITC_SRC_VME_FIF6          0x2e
#define ITC_SRC_VME_FIF7          0x2f

#define ITC_IP(src)      (1<<(src&0xf)) /* Interrupt pending bit-field from source id    */

#define ITC_CTL_ILOC                  0
#define ITC_CTL_VME                   1
#define ITC_CTL_DMA                   2
#define ITC_CTL_USR                   3
#define ITC_CTL(src)       ((src>>4)&3) /* Interrupt control id from interrupt source id */

#define ITC_IACK_VEC(iack)      (iack&0xff)  /* Extract vector from iack register        */
#define ITC_IACK_SRC(iack) ((iack>>8)&0x3f)  /* Extract source id from iack register     */
#define ITC_IACK_CTL(iack) ((iack>>12)&0x3)  /* Extract controller id from iack register */

#define ALT_IOCTL_RDWR             0x00060000
#define ALT_IOCTL_RDWR_READ        (ALT_IOCTL_RDWR | 0x01)
#define ALT_IOCTL_RDWR_WRITE       (ALT_IOCTL_RDWR | 0x02)

#define RDWR_SPACE_PCIE   0
#define RDWR_SPACE_VME    1
#define RDWR_SPACE_SHM    2
#define RDWR_SPACE_USR    3
#define RDWR_SPACE_KBUF   8
#define RDWR_SPACE_MASK 0xf

#define RDWR_SWAP_NO      0
#define RDWR_SWAP_AUTO    1
#define RDWR_SWAP_DS      2
#define RDWR_SWAP_QS      3

#define RDWR_SIZE_BYTE    1
#define RDWR_SIZE_SHORT   2
#define RDWR_SIZE_INT     4
#define RDWR_SIZE_DBL     8

#define RDWR_MODE_SET_DS(m, ds)   ((m & 0xf0) | (ds & 0xf))
#define RDWR_MODE_GET_DS(m)       (m & 0xf) 
#define RDWR_MODE_SET_AS(m, as)   ((m & 0xf) | ((as<<4) & 0xf0))
#define RDWR_MODE_GET_AS(m)       ((m & 0xf0) >> 4) 
#define RDWR_MODE_SET_AM(m, am)   ((m & ~VME_AM_MASK) | (am & VME_AM_MASK))
#define RDWR_MODE_GET_AM(m)       (m & VME_AM_MASK) 
#define RDWR_MODE_SET_SWAP(m, sw) ((m & 0xf) | ((sw<<4) & 0xf0))
#define RDWR_MODE_GET_SWAP(m)     ((m & 0xf0) >> 4) 

#define RDWR_MODE_A8    1
#define RDWR_MODE_A16   2
#define RDWR_MODE_A24   3
#define RDWR_MODE_A32   4
#define RDWR_MODE_A64   8
#define RDWR_MODE_AS(x) (x<<4)

#define RDWR_MODE_SET( ads, space, am)   (((ads&0xff)<<24) | ((space&0xff)<<16) | (am&0xffff))

#define RDWR_SWAP_DATA   0x80

struct alt_ioctl_rdwr {
	ulong rem_addr;
	char *buf;
	int len;
	union {
		uint mode;
		struct rdwr_mode {
			char ads;
			char space;
			char swap;
			char am;
		} m;
	}yyy;
};

#define ALT_IOCTL_VME                0x00070000
#define ALT_IOCTL_VME_OP_MSK         (ALT_IOCTL_VME | 0xf000)
#define ALT_IOCTL_VME_ARB            (ALT_IOCTL_VME | 0x1000)
#define ALT_IOCTL_VME_ARB_GET        (ALT_IOCTL_VME_ARB | 0x1)
#define ALT_IOCTL_VME_ARB_SET        (ALT_IOCTL_VME_ARB | 0x2)
#define ALT_IOCTL_VME_ARB_CLEAR      (ALT_IOCTL_VME_ARB | 0x3)
#define ALT_IOCTL_VME_MAS            (ALT_IOCTL_VME | 0x2000)
#define ALT_IOCTL_VME_MAS_GET        (ALT_IOCTL_VME_MAS | 0x1)
#define ALT_IOCTL_VME_MAS_SET        (ALT_IOCTL_VME_MAS | 0x2)
#define ALT_IOCTL_VME_MAS_CLEAR      (ALT_IOCTL_VME_MAS | 0x3)
#define ALT_IOCTL_VME_SLV            (ALT_IOCTL_VME | 0x3000)
#define ALT_IOCTL_VME_SLV_GET        (ALT_IOCTL_VME_SLV | 0x1)
#define ALT_IOCTL_VME_SLV_SET        (ALT_IOCTL_VME_SLV | 0x2)
#define ALT_IOCTL_VME_SLV_CLEAR      (ALT_IOCTL_VME_SLV | 0x3)
#define ALT_IOCTL_VME_CRCSR          (ALT_IOCTL_VME | 0x4000)
#define ALT_IOCTL_VME_CRCSR_GET      (ALT_IOCTL_VME_CRCSR | 0x1)
#define ALT_IOCTL_VME_CRCSR_SET      (ALT_IOCTL_VME_CRCSR | 0x2)
#define ALT_IOCTL_VME_CRCSR_CLEAR    (ALT_IOCTL_VME_CRCSR | 0x3)
#define ALT_IOCTL_VME_IGEN           (ALT_IOCTL_VME | 0x5000)
#define ALT_IOCTL_VME_IGEN_GET       (ALT_IOCTL_VME_IGEN | 0x1)
#define ALT_IOCTL_VME_IGEN_SET       (ALT_IOCTL_VME_IGEN | 0x2)
#define ALT_IOCTL_VME_IGEN_CLEAR     (ALT_IOCTL_VME_IGEN | 0x3)
#define ALT_IOCTL_VME_IRQ            (ALT_IOCTL_VME | 0x6000)
#define ALT_IOCTL_VME_IRQ_ALLOC      (ALT_IOCTL_VME_IRQ | 0x1)
#define ALT_IOCTL_VME_IRQ_FREE       (ALT_IOCTL_VME_IRQ | 0x2)
#define ALT_IOCTL_VME_IRQ_ARM        (ALT_IOCTL_VME_IRQ | 0x3)
#define ALT_IOCTL_VME_IRQ_WAIT       (ALT_IOCTL_VME_IRQ | 0x4)
#define ALT_IOCTL_VME_IRQ_ARMWAIT    (ALT_IOCTL_VME_IRQ | 0x5)
#define ALT_IOCTL_VME_IRQ_CLEAR      (ALT_IOCTL_VME_IRQ | 0x6)
#define ALT_IOCTL_VME_IRQ_RESET      (ALT_IOCTL_VME_IRQ | 0x7)

#define VME_ARB_PRI					 0
#define VME_ARB_RRS                	 1
#define VME_ARB_PRI_PP            	 2
#define VME_ARB_RRS_PP            	 3
#define VME_ARB_ERR                 -1

struct alt_ioctl_vme_arb_conf {
	uint csr;
	uint arb_mode;
	uint bus_tmo;
};

#define VME_REQ_RWD               	 0
#define VME_REQ_ROR               	 1
#define VME_REQ_FAIR              	 2
#define VME_REQ_NOREL             	 3
#define VME_REQ_ERR                 -1

struct alt_ioctl_vme_mas_conf {
	uint csr;
	uint req_mode;
	uint req_level;
};

struct alt_ioctl_vme_slv_conf {
	uint csr;
	uint base_A32;
	uint size_A32;
	uint base_A24;
	uint size_A24;
	uint base_A16;
	uint size_A16;
	uint base_2eBRC;
	uint size_2eBRC;
};

struct alt_ioctl_vme_igen {
	int csr;
	int vector;
	int level;
	int sts;
};

#define VME_IRQ_STARTED          0x01
#define VME_IRQ_WAITING          0x02
#define VME_IRQ_RECEIVED         0x04
#define VME_IRQ_ENDED            0x10
#define VME_IRQ_TMO              0x80

#define VME_IRQ_SYSFAIL          (1<<0)
#define VME_IRQ_1                (1<<1)
#define VME_IRQ_2                (1<<2)
#define VME_IRQ_3                (1<<3)
#define VME_IRQ_4                (1<<4)
#define VME_IRQ_5                (1<<5)
#define VME_IRQ_6                (1<<6)
#define VME_IRQ_7                (1<<7)
#define VME_IRQ_ACFAIL           (1<<8)
#define VME_IRQ_ERROR            (1<<9)
#define VME_IRQ_IACK             (1<<10)
#define VME_IRQ_TIMER            (1<<11)
#define VME_IRQ_LOCMON_0         (1<<12)
#define VME_IRQ_LOCMON_1         (1<<13)
#define VME_IRQ_LOCMON_2         (1<<14)
#define VME_IRQ_LOCMON_3         (1<<15)
#define VME_IRQ_LOCMON_MASK      (0xf<<12)
#define VME_IRQ_LOCMON(i)        (1<<(12+i))

struct alt_ioctl_vme_irq {
	uint status;
	uint irq;
	uint vector;
	uint tmo;
	uint umask;
};

#define ALT_IOCTL_DMA                0x00080000
#define ALT_IOCTL_DMA_MOVE         (ALT_IOCTL_DMA | 0x1)
#define ALT_IOCTL_DMA_STATUS       (ALT_IOCTL_DMA | 0x2)
#define ALT_IOCTL_DMA_WAIT         (ALT_IOCTL_DMA | 0x3)
#define ALT_IOCTL_DMA_CLEAR        (ALT_IOCTL_DMA | 0x4)
#define ALT_IOCTL_DMA_ALLOC        (ALT_IOCTL_DMA | 0x5)
#define ALT_IOCTL_DMA_FREE         (ALT_IOCTL_DMA | 0x6)

#define DMA_CHAN_NUM    2        /* Number of DMA channels         */
#define DMA_CHAN_0      0        /* DMA channel #0                 */
#define DMA_CHAN_1      1        /* DMA channel #1                 */

struct alt_ioctl_dma_req {
	ulong src_addr;
	ulong des_addr;
	uint size;
	unsigned char src_space;
	unsigned char src_mode;
	unsigned char des_space;
	unsigned char des_mode;
	unsigned char start_mode;
	unsigned char end_mode;
	unsigned char intr_mode;
	unsigned char wait_mode;
	uint dma_status;
};

struct alt_ioctl_dma {
	short status;
	char state;
	char chan;
};

struct alt_ioctl_dma_sts {
	struct alt_ioctl_dma dma;
	uint rd_csr;
	uint rd_ndes;
	uint rd_cdes;
	uint rd_cnt;
	uint wr_csr;
	uint wr_ndes;
	uint wr_cdes;
	uint wr_cnt;
};

#define DMA_SPACE_PCIE       0x00
#define DMA_SPACE_VME        0x01
#define DMA_SPACE_SHM        0x02
#define DMA_SPACE_USR        0x03
#define DMA_SPACE_KBUF       0x08
#define DMA_SPACE_MASK       0x07

#define DMA_VME_A16          0x10
#define DMA_VME_A24          0x20
#define DMA_VME_A32          0x30
#define DMA_VME_SGL          0x30
#define DMA_VME_BLT          0x40
#define DMA_VME_MBLT         0x50
#define DMA_VME_2eVME        0x60
#define DMA_VME_2eFAST       0x70
#define DMA_VME_2e160        0x80
#define DMA_VME_2e233        0x90
#define DMA_VME_2e320        0xa0
#define DMA_START_PIPE_NO    0x00
#define DMA_START_PIPE       0x01
#define DMA_START_CHAN(chan) ((chan & 1) << 4)
#define DMA_VME_SWAP      	 0x40  /* Automatic VME byte swapping */  
#define DMA_SPACE_WS     	 0x10
#define DMA_SPACE_DS      	 0x20
#define DMA_SPACE_QS     	 0x30

#define DMA_INTR_ENA     	 0x01

#define DMA_WAIT_INTR    	 0x01
#define DMA_WAIT_1MS     	 0x02
#define DMA_WAIT_10MS    	 0x04
#define DMA_WAIT_100MS   	 0x06
#define DMA_WAIT_1S      	 0x08
#define DMA_WAIT_10S     	 0x0a
#define DMA_WAIT_100S    	 0x0c

#define DMA_PCIE_TC0         0x00  /* Traffic Class 0 */
#define DMA_PCIE_TC1      	 0x01  /* Traffic Class 1 */
#define DMA_PCIE_TC2      	 0x02  /* Traffic Class 2 */
#define DMA_PCIE_TC3     	 0x03  /* Traffic Class 3 */
#define DMA_PCIE_TC4      	 0x04  /* Traffic Class 4 */
#define DMA_PCIE_TC5     	 0x05  /* Traffic Class 5 */
#define DMA_PCIE_TC6     	 0x06  /* Traffic Class 6 */
#define DMA_PCIE_TC7      	 0x07  /* Traffic Class 7 */
#define DMA_PCIE_RR1     	 0x00  /* 1 outstanding read request */
#define DMA_PCIE_RR2     	 0x10  /* 2 outstanding read request */
#define DMA_PCIE_RR3     	 0x20  /* 3 outstanding read request */

#define DMA_SIZE_PKT_128  	 0x00000000 
#define DMA_SIZE_PKT_256  	 0x40000000 
#define DMA_SIZE_PKT_512 	 0x80000000 
#define DMA_SIZE_PKT_1K   	 0xc0000000 

#define DMA_STATE_IDLE       0x00
#define DMA_STATE_ALLOCATED  0x01
#define DMA_STATE_STARTED    0x02
#define DMA_STATE_WAITING    0x03
#define DMA_STATE_DONE       0x04

#define DMA_STATUS_RUN_RD0   0x01
#define DMA_STATUS_RUN_RD1   0x02
#define DMA_STATUS_RUN_WR0   0x04
#define DMA_STATUS_RUN_WR1   0x08
#define DMA_STATUS_DONE      0x10
#define DMA_STATUS_WAITING   0x000
#define DMA_STATUS_ENDED     0x100
#define DMA_STATUS_TMO       0x80
#define DMA_STATUS_ERR       0x40
#define DMA_STATUS_BUSY      0x20

#define ALT_IOCTL_KBUF       0x00090000
#define ALT_IOCTL_KBUF_ALLOC (ALT_IOCTL_KBUF | 0x1)
#define ALT_IOCTL_KBUF_FREE  (ALT_IOCTL_KBUF | 0x2)
#define ALT_IOCTL_KBUF_READ  (ALT_IOCTL_KBUF | 0x3)
#define ALT_IOCTL_KBUF_WRITE (ALT_IOCTL_KBUF | 0x4)

struct alt_ioctl_kbuf_req {
	uint size;
	void *k_base;
	long long *b_base;
	void *u_base;
};

struct alt_ioctl_kbuf_rw {
	uint size;
	void *k_addr;
	void *buf;
};

#define ALT_IOCTL_TIMER                0x000a0000
#define ALT_IOCTL_TIMER_START         (ALT_IOCTL_TIMER | 0x1)
#define ALT_IOCTL_TIMER_STOP          (ALT_IOCTL_TIMER | 0x2)
#define ALT_IOCTL_TIMER_READ          (ALT_IOCTL_TIMER | 0x3)

struct alt_ioctl_timer {
	int operation;
	int mode;
	struct alt_time {
		int msec;
		int usec;
	} time;
};

#define TIMER_ENA          0x80000000  /* Timer global enable                    */
#define TIMER_1MHZ         0x00000000  /* Timer frequency 1 MHz                  */
#define TIMER_5MHZ         0x00000001  /* Timer frequency 5 MHz                  */
#define TIMER_25MHZ        0x00000002  /* Timer frequency 25 MHz                 */
#define TIMER_100MHZ       0x00000003  /* Timer frequency 100 MHz                */
#define TIMER_BASE_1000    0x00000000  /* Timer period 1000 usec                 */
#define TIMER_BASE_1024    0x00000008  /* Timer period 1024 usec                 */
#define TIMER_SYNC_LOC     0x00000000  /* Timer synchronization local            */
#define TIMER_SYNC_USR1    0x00000010  /* Timer synchronization user signal #1   */
#define TIMER_SYNC_USR2    0x00000020  /* Timer synchronization user signal #2   */
#define TIMER_SYNC_SYSFAIL 0x00000040  /* Timer synchronization VME sysfail      */
#define TIMER_SYNC_IRQ1    0x00000050  /* Timer synchronization VME IRQ#1        */
#define TIMER_SYNC_IRQ2    0x00000060  /* Timer synchronization VME IRQ#2        */
#define TIMER_SYNC_ENA     0x00000080  /* Timer synchronization enable           */
#define TIMER_OUT_SYSFAIL  0x00000100  /* Issue sync signal on VME sysfail       */
#define TIMER_OUT_IRQ1     0x00000200  /* Issue sync signal on VME IRQ#1         */
#define TIMER_OUT_IRQ2     0x00000300  /* Issue sync signal on VME IRQ#2         */
#define TIMER_SYNC_ERR     0x00010000  /* Timer synchronization error            */

#ifdef _cplusplus
}
#endif

#endif
