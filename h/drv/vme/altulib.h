/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : altulib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : June 2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    altulib.c.
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

#ifndef _H_ALTULIB
#define _H_ALTULIB

#include "def.h"
#include "altioctl.h"

int alt_init( void);
int alt_exit( void);
char *alt_get_lib_version( void);
char *alt_get_drv_version( void);
char *alt_get_drv_name( void);
int alt_csr_write( int idx, int *data_p);   /* Write to ALTHEA7910 CSR register   */
int alt_csr_read( int idx, int *data_p);    /* Read from ALTHEA7910 CSR register  */
int alt_smon_write( int idx, int *data_p);  /* Write to ARTIX7 SMON register      */
int alt_smon_read( int idx, int *data_p);   /* Read from ARTIX7 SMON register     */
int alt_pciep_write( int idx, int *data_p); /* Write to ARTIX7 PCIEP register     */
int alt_pciep_read( int idx, int *data_p);  /* Read from ARTIX7 PCIEP register    */
long long alt_swap_64( long long);
int alt_swap_32( int);
short alt_swap_16( short);
int ast_alt_write_blk( ulong addr, char *buf, int len, uint mode);
int ast_alt_read_blk( ulong addr, char *buf, int len, uint mode);
int ast_alt_write_sgl( ulong addr, char *buf, uint mode);
int ast_alt_read_sgl( ulong addr, char *buf, uint mode);
int alt_vme_write( uint vme_addr, char *buf, int len, int am, int ds, int swap);
int alt_vme_read( uint vme_addr, char *buf, int len, int am, int ds, int swap);
int ast_alt_vme_arb_get_conf(struct alt_ioctl_vme_arb_conf *a);
int ast_alt_vme_arb_set_conf(struct alt_ioctl_vme_arb_conf *a);
int ast_alt_vme_mas_get_conf(struct alt_ioctl_vme_mas_conf *m);
int ast_alt_vme_mas_set_conf(struct alt_ioctl_vme_mas_conf *m);
int ast_alt_vme_slv_get_conf(struct alt_ioctl_vme_slv_conf *s);
int ast_alt_vme_slv_set_conf(struct alt_ioctl_vme_slv_conf *s);
int ast_alt_vme_igen_get(struct alt_ioctl_vme_igen *i);
int alt_vme_igen_get_csr();
int ast_alt_vme_igen_set(int level, int vector);
int ast_alt_vme_igen_clear();
struct alt_ioctl_vme_irq *ast_alt_vme_irq_alloc( uint is);
int ast_alt_vme_irq_free(struct alt_ioctl_vme_irq *irq);
int ast_alt_vme_irq_arm(struct alt_ioctl_vme_irq *irq);
int ast_alt_vme_irq_clear(struct alt_ioctl_vme_irq *irq);
int ast_alt_vme_irq_wait(struct alt_ioctl_vme_irq *irq, uint tmo, uint *vector);
int alt_vme_irq_armwait( struct alt_ioctl_vme_irq *irq, uint tmo, uint *vector);
int ast_alt_vme_irq_reset();
int ast_alt_map_clear( struct alt_ioctl_map_ctl *m);
int ast_alt_kbuf_alloc(struct alt_ioctl_kbuf_req *kr_p);
int ast_alt_kbuf_free( struct alt_ioctl_kbuf_req *kr_p);
void *alt_kbuf_mmap( struct alt_ioctl_kbuf_req *kr_p);
int alt_kbuf_munmap( struct alt_ioctl_kbuf_req *kr_p);
int alt_kbuf_read( void *k_addr, char *buf, uint size);
int alt_kbuf_write( void *k_addr, char *buf, uint size);
int alt_map_alloc( struct alt_ioctl_map_win *w);
int alt_map_get( struct alt_ioctl_map_win *w);
int alt_map_free( struct alt_ioctl_map_win *w);
int alt_map_modify( struct alt_ioctl_map_win *w);
int ast_alt_map_read(struct alt_ioctl_map_ctl *m);
int ast_alt_dma_move(struct alt_ioctl_dma_req *dr_p);
int ast_alt_dma_wait(struct alt_ioctl_dma_req *dr_p);
int ast_alt_dma_status(struct alt_ioctl_dma_sts *ds_p);
int ast_alt_dma_alloc(int chan);
int ast_alt_dma_free(int chan);
int ast_alt_dma_clear(int chan);

#endif
