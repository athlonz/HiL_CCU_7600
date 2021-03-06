/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : sflashlib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : July 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    sflashlib.c.
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

#ifndef _H_SFLASHLIB
#define _H_SFLASHLIB

#define SFLASH_CMD_DUMMY  0xA5
#define SFLASH_CMD_WREN   0x06
#define SFLASH_CMD_WRDI   0x04
#define SFLASH_CMD_RDSR   0x05
#define SFLASH_CMD_RDSR2  0x35
#define SFLASH_CMD_WRSR   0x01
#define SFLASH_CMD_READ   0x03
#define SFLASH_CMD_FREAD  0x0B
#define SFLASH_CMD_PP     0x02
#define SFLASH_CMD_SE     0xD8
#define SFLASH_CMD_BE     0xC7
#define SFLASH_CMD_RDID   0x9F

#define SFLASH_SR_BUSY    0x01
#define SFLASH_SR_WEL     0x02
#define SFLASH_SR_BP0     0x04
#define SFLASH_SR_BP1     0x08
#define SFLASH_SR_BP2     0x10
#define SFLASH_SR_TB      0x20
#define SFLASH_SR_SEC     0x40
#define SFLASH_SR_SRP0    0x80
#define SFLASH_SR_SRP1   0x100
#define SFLASH_SR_QE     0x200
#define SFLASH_SR_LB0    0x400
#define SFLASH_SR_LB1    0x800
#define SFLASH_SR_LB2   0x1000
#define SFLASH_SR_LB3   0x2000
#define SFLASH_SR_CMP   0x4000
#define SFLASH_SR_SUS   0x8000

#define SFLASH_SR2_SRP1   0x01
#define SFLASH_SR2_QE     0x02
#define SFLASH_SR2_LB0    0x04
#define SFLASH_SR2_LB1    0x08
#define SFLASH_SR2_LB2    0x10
#define SFLASH_SR2_LB3    0x20
#define SFLASH_SR2_CMP    0x40
#define SFLASH_SR2_SUS    0x80

#define SFLASH_ID_INV     0xff

struct sflash_para {
	int device_size;
	int block_size;
	int sector_size;
	int page_size;
	int erase_device_tmo;
	int erase_sector_tmo;
	int erase_block_tmo;
};

struct sflash_ctl {
	void __iomem *sflash_ptr;
	char id[4];
	struct sflash_para *para;
};

int alt_sflash_conf(struct althea7910_device *alt, struct sflash_para *fp);
int alt_sflash_read_ID(struct althea7910_device *alt, char *id);

#endif 
