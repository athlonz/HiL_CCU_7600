#ifndef _GLOBALVAR_REC_
#define _GLOBALVAR_REC_

#ifdef __cplusplus
extern "C" {
#endif
/*********************************************************************************************/
#include "vxWorks.h"
#include <math.h>
/*********************************************************************************************/

#define T1PRHalfSVM_Rec 20834 
#define TsSVM 0.0016666666666667
#define T1PRSVM_Rec 41668  /* 25MHz,600Hz*/

#define DEG60  1.04720  
#define DEG30  0.52360
 
/* #define T1PR  41666
#define T1PRHalf 20832  */

#define MP_Rec 2000//750//2250 /*25Mhz,90us*/ 
#define PIVSR 3.1415926
#define INTPRD 2.777778e-4

/*TableVSR20150709*/
#define AQ_N 0
#define AQ_C 2
#define AQ_S 1
#define AQ_T 3
#define AQ_NO_ACTION 0
#define AQ_CLEAR     2
#define AQ_SET		 1
#define AQ_TOGGLE    3
#define ET_CTR_PRD	1
#define ET_CTR_ZRO	0
#define ET_CTR_ZERO	0

#define lim_uz 0.9
/*************************************************************************/
extern void SVPWM_VSR(struct PWMdataStruct *PWMdata);
extern void NPC_SVM_SetAseRec(void)  ;
extern void NPC_SVM_SetADesRec(void);
extern void Minimum_Pulse_TLCRec(struct PWMdataStruct *PWMdata);/* svmtlc   in1--MP_testRec  in2--VBC_test  in3--NPChangeRec  in4--MP这个数值是必改的。in5--T1PRHalfSVM_Rec  in6--MPhalf必改！ */

/***********************************************************************/
extern double usdFw,usqFw;
extern int Sector_Num, Sector_Num_k1;
extern int Region_Num, Region_Num_k1;
extern int Nostep,Nostep_k1;
extern double Gama_SVMRec,VFGamaRec;
extern double CMPARec[6];
extern short ZRORec[6],PRDRec[6],CAURec[6],CADRec[6]; 

extern int Region_Num_TB;  
extern double Ta,Tb,Tc;
/* extern double vvvf1;  */
extern double KvRec;
extern int IntFlagRec;
extern double ta0Rec,tb0Rec,tc0Rec,taRec,tbRec,tcRec,t1Rec,t2Rec,t3Rec;
extern int Reg_UpDownRec;
extern double Vdc1MinuVdc2Rec,Vdc1,Vdc2,MaxVdcDeviRec,NeuVoltCtl_sum;
extern double Ki_uzRec,Kp_uzRec;
extern int sign_iaRec,sign_ibRec,sign_icRec;
extern double Gama_SVM_DeltTRec;

extern double epwm1cmpa,epwm2cmpa,epwm3cmpa,epwm4cmpa,epwm5cmpa,epwm6cmpa;
extern double epwm1cmpa_k1,epwm2cmpa_k1,epwm3cmpa_k1,epwm4cmpa_k1,epwm5cmpa_k1,epwm6cmpa_k1;
extern int bigSectorNum,smallSectorNum;
extern const double * NPtimeTableRec[6][6][3];
extern const short 
ActTb_EPwm1Rec[6][6][4], 
ActTb_EPwm2Rec[6][6][4],
ActTb_EPwm3Rec[6][6][4],
ActTb_EPwm4Rec[6][6][4],
ActTb_EPwm5Rec[6][6][4],
ActTb_EPwm6Rec[6][6][4];

extern int NPChangeRec;

extern double e1,e2,e3;
extern double NeuVoltCtl_deltTRec,NeuVoltCtl_deltTRec1;
extern int DTtimeEx;/* DTime */

extern const short Tbt1t2t3Rec[6][6];
extern const short TbPt1t2t3Rec[6][6];/* 20100501 */

extern double pt2t1Rec,pt3t2Rec,pt3t1Rec;
extern double pt1Rec,pt2Rec,pt3Rec;
extern double pepwm1cmpa,pepwm3cmpa,pepwm5cmpa;
extern int MP_Debug;

extern int Ct1t2t3Rec,BbitRec,SbitRec,GbitRec;/* for MP */

extern int MPhalfRec;/* 0; */
extern int MPquart;/* MPDouble=3800;//0; */
extern int MPDT;

extern int MP_testRec, DT_test;



/* void Minimum_Pulse_TLC(void); */

extern unsigned int PtypeRec,NtypeRec;
extern unsigned int Ptypek1,Ntypek1;
extern int Tempbit;
extern unsigned int vsi_start;/* 20100429 */

extern int NPZROhead_A1, NPZROhead_A2, NPZROhead_B1, NPZROhead_B2, NPZROhead_C1, NPZROhead_C2;
extern int NPZROtail_A1, NPZROtail_A2, NPZROtail_B1, NPZROtail_B2, NPZROtail_C1, NPZROtail_C2;

extern int NPPRDhead_A1, NPPRDhead_A2, NPPRDhead_B1, NPPRDhead_B2, NPPRDhead_C1, NPPRDhead_C2;
extern int NPPRDtail_A1, NPPRDtail_A2, NPPRDtail_B1, NPPRDtail_B2, NPPRDtail_C1, NPPRDtail_C2;

extern int NPjumpARec, PNjumpARec, NPjumpBRec, PNjumpBRec, NPjumpCRec, PNjumpCRec;

extern unsigned int faultCode3;

extern int MP_Debug;
extern int Sector_NumRec,Region_NumRec;
//extern double Gama_SVMRec;
extern double epwm1cmpaRec,epwm2cmpaRec,epwm3cmpaRec,epwm4cmpaRec,epwm5cmpaRec,epwm6cmpaRec;
extern double ta0Rec,tb0Rec,tc0Rec,taRec,tbRec,tcRec;
 
 
#ifdef __cplusplus
}
#endif

#endif
