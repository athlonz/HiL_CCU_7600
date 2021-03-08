/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : timerlib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Nov 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    timerlib.c.
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
 *=============================< end file header >============================*/

#ifndef _H_TIMERLIB
#define _H_TIMERLIB

int alt_timer_irq(struct althea7910_device *alt, int src, void *arg);
int timer_init(struct althea7910_device *alt);
int alt_timer_start(struct althea7910_device *alt, int start);
int alt_timer_stop(struct althea7910_device *alt);
int alt_timer_read(struct althea7910_device *alt, struct alt_time *tm);

#endif
