/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mapmaslib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : July 2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    mapmaslib.c.
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

#ifndef _H_MAPMASLIB
#define _H_MAPMASLIB

short alt_map_mas_set_mode( struct althea7910_device *alt, struct alt_ioctl_map_mode *m);
int alt_map_mas_set_sg( struct althea7910_device *alt, struct map_ctl *map_ctl_p, int offset);
int alt_map_mas_clear_sg( struct althea7910_device *alt, struct map_ctl *map_ctl_p, int offset, int npg);
int alt_map_mas_alloc( struct althea7910_device *alt, struct alt_ioctl_map_win *w);
ulong alt_map_mas_modify( struct althea7910_device *alt, struct alt_ioctl_map_win *w);
int alt_map_mas_free( struct althea7910_device *alt, int sg_id, uint offset);
int alt_map_mas_get( struct althea7910_device *alt, struct alt_ioctl_map_win *w);

#endif 
