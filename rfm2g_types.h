/* rfm2g_types.h - RFM2g VxWorks RFM2g Type definition file */

/* 
===============================================================================
                            COPYRIGHT NOTICE

    Copyright (C) 2001, 2006 GE Fanuc Embedded Systems, Inc.
    International Copyright Secured.  All Rights Reserved.

-------------------------------------------------------------------------------

modification history
--------------------
$Workfile: rfm2g_types.h $
$Revision: 10 $
$Modtime: 7/24/08 1:54p $
*/

/*
DESCRIPTION


INCLUDE FILES: 
*/

#ifndef RFM2GTYPES_VXW_H
#define RFM2GTYPES_VXW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"

/*--------------------------------------------------------
 * Define RFM2g INT Values
 --------------------------------------------------------*/
#ifndef RFM2G_INT8
#define RFM2G_INT8 INT8
#endif
#ifndef RFM2G_INT16
#define RFM2G_INT16 INT16
#endif
#ifndef RFM2G_INT32
#define RFM2G_INT32 INT32
#endif
#ifndef RFM2G_INT64
#define RFM2G_INT64 INT64
#endif
#ifndef RFM2G_UINT8
#define RFM2G_UINT8 UINT8
#endif
#ifndef RFM2G_UINT16
#define RFM2G_UINT16 UINT16
#endif
#ifndef RFM2G_UINT32
#define RFM2G_UINT32 UINT32
#endif
#ifndef RFM2G_UINT64
#define RFM2G_UINT64 unsigned long long
#endif

#ifndef RFM2G_BOOL
# define RFM2G_BOOL RFM2G_UINT8
#endif

#ifndef RFM2G_NODE
# define RFM2G_NODE RFM2G_UINT16
#endif

#ifndef RFM2G_CHAR
#define RFM2G_CHAR char
#endif

#ifndef RFM2G_ADDR
#define RFM2G_ADDR RFM2G_UINT32
#endif

/* Add base constants for the Boolean */
#define RFM2G_TRUE   1
#define RFM2G_FALSE  0

/* Define symbols to specify data width */
typedef enum RFM2GDataWidth
{
  RFM2G_BYTE      = 1,
  RFM2G_WORD      = 2,
  RFM2G_LONG      = 4,
  RFM2G_LONGLONG  = 8
} RFM2GDATAWIDTH;

#ifdef __cplusplus
}
#endif

#endif
