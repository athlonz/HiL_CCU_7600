/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : ioctllib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : July 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    ioctllib.c.
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

#ifndef _H_IOCTLLIB
#define _H_IOCTLLIB

int ioctl_csr(struct althea7910_device *alt, unsigned int cmd,
		unsigned long arg);
int ioctl_map(struct althea7910_device *alt, unsigned int cmd,
		unsigned long arg);
int ioctl_rdwr(struct althea7910_device *alt, unsigned int cmd,
		unsigned long arg);
int ioctl_vme(struct althea7910_device *alt, unsigned int cmd,
		unsigned long arg);
int ioctl_dma(struct althea7910_device *alt, unsigned int cmd,
		unsigned long arg);
int ioctl_kbuf(struct althea7910_device *alt, unsigned int cmd,
		unsigned long arg);

#endif 
