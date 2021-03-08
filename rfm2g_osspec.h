/* rfm2g_osspec.h - RFM2g VxWorks OS Specific Header File */

/* 
===============================================================================
                            COPYRIGHT NOTICE

    Copyright (C) 2001, 2006 GE Fanuc Embedded Systems, Inc.
    International Copyright Secured.  All Rights Reserved.

-------------------------------------------------------------------------------

modification history
--------------------
$Workfile: rfm2g_osspec.h $
$Revision: 17 $
$Modtime: 8/10/06 3:13p $
*/

/*
DESCRIPTION  This file 
 1) define STDRFM2GCALL which is used for functions that return RFM2G_STATUS 
 2) define RFM2GHANDLE
 3) prototypes for driver specific functions
 4) include other header files as necessary.
 5) include common API definitions header file rfm2g_defs.h
 6) Include header file that defines RFM2G types, rfm2g_types.h 

INCLUDE FILES: 
*/

#ifndef RFM2GAPI_VXW_H
#define RFM2GAPI_VXW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "rfm2g_types.h"
#include "rfm2g_defs.h"  
#include "rfm2g_regs.h"

/* Define a MACRO for the standard calling pattern */
#define STDRFM2GCALL RFM2G_STATUS

/* RFM2g Handle value created during RFM2gOpen */
#define RFM2GHANDLE void*

STDRFM2GCALL RFM2gClearOwnData(RFM2GHANDLE  rh, RFM2G_BOOL * state);

#ifdef __cplusplus
}
#endif

#endif
