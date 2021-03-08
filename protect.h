#ifndef _PROTECT_
#define _PROTECT_

#ifdef __cplusplus
extern "C" {
#endif
/*********************************************************************************************/
#include "vxWorks.h"
#include <math.h>
/*********************************************************************************************/


extern void clr_fault_VME(void);
extern void fault_protect_VME(void);
extern void clr_fault_SRIO(void);
extern void fault_protect_SRIO(void);
#ifdef __cplusplus
}
#endif

#endif
