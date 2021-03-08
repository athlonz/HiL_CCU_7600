

#ifndef _SUBMOD_RFM2GH_
#define _SUBMOD_RFM2GH_

#ifdef __cplusplus
extern "C" {
#endif

/* includes */ 
#include <in.h>
#include "vxworks.h"
#include "logLib.h"
#include "stdio.h"
#include "intLib.h"
#include "iv.h"  
#include "sysLib.h"
#include "vme.h"
#include "math.h" 
#include "ioLib.h"
#include "wdLib.h"
#include "string.h"
#include <stdlib.h>
#include "rfm2g_api.h" 
 
 
/*********************************************************************************************/
 

 
/* globals */ 
 
 
/* forward declarations */ 
STATUS rfmSendCCU(void);
STATUS rfmInit(void);
STATUS rfmReceiveCCU(void);
 
STATUS rfmClose(void);
#ifdef __cplusplus
}
#endif

#endif
