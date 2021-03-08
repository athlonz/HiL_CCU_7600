#ifndef _SLAVEBOARDRW_
#define _SLAVEBOARDRW_

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "math.h"  
 
/******************************************************************/

extern unsigned short rec1SrioID;
extern unsigned short rec1Srio_status	;//
extern unsigned short rec1Srio_error;//
extern unsigned short rec1Srio_igen_state;

extern unsigned short rec2SrioID;
extern unsigned short rec2Srio_status	;//
extern unsigned short rec2Srio_error;//
extern unsigned short rec2Srio_igen_state;

extern unsigned short inv1SrioID;
extern unsigned short inv1Srio_status	;//
extern unsigned short inv1Srio_error;//
extern unsigned short inv1Srio_igen_state;

extern unsigned short inv2SrioID;
extern unsigned short inv2Srio_status	;//
extern unsigned short inv2Srio_error;//
extern unsigned short inv2Srio_igen_state;

extern unsigned short ad1SrioID;
extern unsigned short ad1Srio_status	;//
extern unsigned short ad1Srio_error;//
extern unsigned short ad1Srio_igen_state;

extern unsigned short ad2SrioID;
extern unsigned short ad2Srio_status	;//
extern unsigned short ad2Srio_error;//
extern unsigned short ad2Srio_igen_state;

unsigned short clkSrioID;
unsigned short clkSrio_status	;//
unsigned short clkSrio_error;//
unsigned short clkSrio_igen_state;
/*************************************************************************/
 
extern void VMEAddrConvert(void);
extern void ClkIntInit_VME(void);
extern void DA4140Init_withVME(void);
extern void boardSelfCheck_withVME(void);
extern int  checkAst7700_VME(void);
extern int  IoBoardInit_VME(void);
extern void ast7600SRIO_Init(void);
extern void RecPwm1Init_withSrio(void); 
extern void RecPwm2Init_withSrio(void); 
extern void InvPwm1Init_withSrio(void); 
extern void InvPwm2Init_withSrio(void); 
extern void AD1Init_withSrio(void); 
extern void AD2Init_withSrio(void); 
extern void ClkInit_withSrio(void); 
 
extern void AD_Sample_VME(void);
extern void readGpsData_VME(void);
extern void read_7700_SrioData(void);
extern void write_7700_SrioData(void);
#ifdef __cplusplus
}
#endif

#endif
