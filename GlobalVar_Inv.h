#ifndef _GLOBALVAR_INV_
#define _GLOBALVAR_INV_

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include <math.h>
#include "pwmControl.h"
 
/**************************************************************************************/
//#define pi 3.1415926535897932
#define DEG60  1.04720 
#define COS2PI_3 -0.5
#define SIN2PI_3 0.866
#define pi3 9.424777960769
#define DEG30  0.52360
#define TESTTABLEVALUE
#define AQ_NO_ACTION 0
#define AQ_CLEAR     2
#define AQ_SET		 1
#define AQ_TOGGLE    3


#define ET_CTR_PRD	1
#define ET_CTR_ZRO	0
#define VOLTAGE_REATED     60.0         /*V*/
#define SPEED_RATED        50.0         /*hz */
#define VFConst      6.22               /*311v--50Hz*/
#define spd_Up_Rate  50.0/60.0  /* 50Hz */
#define spd_Dn_Rate  50.0/120.0 /*  50Hz */
#define spdEQLimt spd_Dn_Rate * 0.3 + 0.0000000001
#define SRAM_Base_Adress	0x200000
#define DA1 (*(unsigned extern short extern int *)0x200000)
#define DA2 (*(unsigned extern short extern int *)0x200001)
#define DA3 (*(unsigned extern short extern int *)0x200002)
#define DA4 (*(unsigned extern short extern int *)0x200003)
#define AD  (*(unsigned extern short extern int *)0x200009)
#define PT1 (*(unsigned extern short extern int *)0x200004)
#define PT2 (*(unsigned extern short extern int *)0x200005)
#define ChopperPWM1 (*(unsigned extern short extern int *)0x200006)
#define ChopperPWM2 (*(unsigned extern short extern int *)0x200007)
#define TXCPUDRAM (*(struct txStruct *)0x100080)
#define DRAM (unsigned extern int *)0x100000
#define RXCPUDRAM (*(struct rxStruct *)0x100000)
#define MaxVdcDevi 2  /*20  20100325*/
#define lim_uz 0.9
#define DisTime 6000  /*for SCR discharge*/
#define Bw   50
#define DisChargeFinishVdc 10
#define SYSINT_TABLE_SZ	256
#define ACCELERATE_PHASE 0.00314
//#define PI	3.1415926
#define N_PI	-3.1415926 
#define ET_CTR_PRD  1
#define Halfpi 1.5707963267948966
#define MP  2000//2250  /* 25MHz,100us *//*used in SVPWM_inv*/

#define TsSVM 0.0016666666666667/*600hz SVPWM三角载波周期*/
#define TsSHE 2.777778e-4 /*3600hz SHEPWM三角载波周期*/
#define T1PRHalfSVM 20833  /*600hz SVPWM三角载波周期对应PWM板的周期的一半，定时器增减计数，该板定时器时钟频率25Mhz*/
#define T1PRHalfSHE 3472/*3600hz SVPWM三角载波周期对应PWM板的周期寄存器的一半，该板定时器时钟频率25Mhz*/  
#define T1PRSVM 41666/*41668 */ /* 25MHz,600Hz*/
#define T1PRSHE 6944/*13888 */ /* 50MHz,3600Hz*/
#define SVMClkFreq 25000000
#define SHEClkFreq /*50000000*/ 25000000

#define MinPulseDuration 0.0001
#define DeadTimeCnt 1000  /* 20us ，50MHz  -  1000*/
/* #define DeadTimeCnt 0 */  /* 20us ，50MHz*/
#define Limit_I 70   /*    */
#define Limit_Irec 70  /*   */
#define Limit_U 250
#define UINT16 unsigned short
#define MinP 2000//2250 /*90us,25MHz,2250, 20180510*/ /*used in SHEPWM*/ 
#define VdMotorLim 170
#define VqMotorLim 170

#define MinPulseCheck 1000  //1000// 20us,50MHz   
#define DeadZone  1000     //1000//20us,1000,50MHz,20180510

#define UdcUpDownDiff 40
/*
#define MOTOR_CURRENTLOOP_KI 0.2
#define MOTOR_CURRENTLOOP_KP 1
#define MOTOR_SPEEDLOOP_KI 0.002
#define MOTOR_SPEEDLOOP_KP 0.5
*/
#define MOTOR_CURRENTLOOP_KI 0.2
#define MOTOR_CURRENTLOOP_KP 1

#define MOTOR_SPEEDLOOP_KI 0.002
#define MOTOR_SPEEDLOOP_KP 0.5

/**************************************************************************************/
// 
//struct PWMdataStruct
//{
// /*input variables*/
//	double vvvf1;
//	double vvvf1Interpolation;
//	double Kv;
//	double KvPre;
//	int KvIncreFlag;
//	double VFGama ;
//	double VFGamaOrigin;
//	double VFGama_Pre;
//	unsigned int Reg_UpDown;
//	unsigned int IntFlag;
//	unsigned int invPwmMode;
//	unsigned int SHEShutFlag;
//	unsigned int Gama_TotalNum_Pre;
//	unsigned int HalfGama_TotalNum;
//	
//	unsigned int Gama_TotalNum;
//	
//	/*middle variables*/
//	double VFGama6[6];
//	double VFGama_Pre6[6];
//	unsigned int gama_Num[6]; 
//	unsigned int gama_Num_pre[6];
//
//	int Epwm1_timeconti;
//	int Epwm2_timeconti;
//	int Epwm3_timeconti;
//	int Epwm4_timeconti;
//	int Epwm5_timeconti;
//	int Epwm6_timeconti;
//	int Epwm_g_i_timeconti;
//	
//	unsigned int EPwm1PRDPre ;
//	unsigned int EPwm1CADPre ;
//	unsigned int EPwm1ZROPre ;
//	unsigned int EPwm1CAUPre ;
//
//	unsigned int EPwm2PRDPre ;
//	unsigned int EPwm2CADPre ;
//	unsigned int EPwm2ZROPre ;
//	unsigned int EPwm2CAUPre ;
//
//	unsigned int EPwm3PRDPre ;
//	unsigned int EPwm3CADPre ;
//	unsigned int EPwm3ZROPre ;
//	unsigned int EPwm3CAUPre ;
//
//	unsigned int EPwm4PRDPre ;
//	unsigned int EPwm4CADPre ;
//	unsigned int EPwm4ZROPre ;
//	unsigned int EPwm4CAUPre ;
//
//	unsigned int EPwm5PRDPre ;
//	unsigned int EPwm5CADPre ;
//	unsigned int EPwm5ZROPre ;
//	unsigned int EPwm5CAUPre ;
//
//	unsigned int EPwm6PRDPre ;
//	unsigned int EPwm6CADPre ;
//	unsigned int EPwm6ZROPre ;
//	unsigned int EPwm6CAUPre ;
//	 
//	unsigned int EPwm1EndState ;
//	unsigned int EPwm2EndState ;
//	unsigned int EPwm3EndState ;
//	unsigned int EPwm4EndState ;
//	unsigned int EPwm5EndState ;
//	unsigned int EPwm6EndState ;
//	
//	unsigned int EPwmEndState ; 
//	
//	/*output variables*/
//	unsigned int ACQ_ZRO;
//	unsigned int ACQ_CAU;
//	unsigned int ACQ_PRD;
//	unsigned int ACQ_CAD;
//	double CMPA[6];
//	
//	int FirstEnPWMFlag;
//	int vvvf1IncreFlag;
//    int InvModulationFlag;   
//	int PWM4Cnt;
//	double T1PR;
//	double T1PRHalf;
//	double Ts;
//	int Change_SL;
//	int SVMModeSwitchUp;
//	int ChangeSHENPJumpTest;
//	int ModeFlag;
//	int ChangeSHENPJumpFlag;
//	int SVMModeSwitchDn;
//	int SVPWMType;
//	char* PWMTransLocalAddr;
//	int SHEReady;
//	int MinPulseOK;
//	int SwitchSpeedDnOnce;
//	int SwitchSpeedUpOnce;
//	
//	double Ia;
//	double Ib;
//	double Ic;
//	double Ua;
//	double Ub;
//	double Uc;
//	double Theta_SecdFwd;  /*PLL*/
//	double KiPLL;
//	double KpPLL;
//	double Delta;
//	double GridFOC_Gama;  /*锁相角*/
//	double sum_usq;
//	
//	double Udc_up_filter;/* neutral point balancing*/
//	double Udc_down_filter;
//	double NeuVoltCtl_sum;
//	double Ki_uzRec;  /*0*/
//	double Kp_uzRec;  /*0.03*/
//	
//	double vref;	
//	double VKi;
//	double vsum;
//	double VKp;
//	double vdsum;
//	double Idq_Ki;
//	double Idq_Kp;
//	double Idref;
//	double Iqref;
//	double vqsum;
//	
//	int TransWriteFlag;
//	int MinPulseFlag;
//	
//	unsigned int ZROPreTest;
//	unsigned int CAUPreTest;
//	unsigned int PRDPreTest;
//	unsigned int CADPreTest;
//	
//	unsigned int BranchTest[6];
//	
//} ; 
/*************************函数*************************************************************/

 
extern void resetSHE(struct PWMdataStruct *PWMdata); 
extern void SHEPWM(struct PWMdataStruct *PWMdata);    /* SHEPWM_Core.c */
extern void MinPulseSetAse(struct PWMdataStruct *PWMdata);
extern void MinPulseSetDes(struct PWMdataStruct *PWMdata);
extern void ChangeTsSHE(struct PWMdataStruct *PWMdata);	
extern void SHEMinPulseCheck(struct PWMdataStruct *PWMdata);


//extern void SVPWM(struct PWMdataStruct *PWMdata);    /* SVM_Core.c */
extern void SVPWMH(struct PWMdataStruct *PWMdata);
extern void SVPWML(struct PWMdataStruct *PWMdata);
extern void JpStart_NPC_SVM_SetAse(struct PWMdataStruct *PWMdata);
extern void JpStart_NPC_SVM_SetDes(struct PWMdataStruct *PWMdata);
extern void NPC_SVM_SetDes(void);
extern void NPC_SVM_SetAse(void);
extern void ChangeTsSVM(struct PWMdataStruct *PWMdata);
extern void Minimum_Pulse_TLC(struct PWMdataStruct *PWMdata);
//extern void InvPwmModeModulation(struct PWMdataStruct *PWMdata);

extern void NP_JUMP_AVOIDy2(struct PWMdataStruct *PWMdata); 
extern void CheckSwich2SHE(struct PWMdataStruct *PWMdata);
/*extern void SVPWM_VSR(struct PWMdataStruct *PWMdata);*/
//extern void NPC_SVM_SetAseRec(void)  ;
//extern void NPC_SVM_SetADesRec(void);
/*extern void Minimum_Pulse_TLCRec(struct PWMdataStruct *PWMdata); svmtlc   in1--MP_testRec  in2--VBC_test  in3--NPChangeRec  in4--MP这个数值是必改的。in5--T1PRHalfSVM_Rec  in6--MPhalf必改！ */


/*******************************变量*******************************************************/
/* extern short   IntFlag;  */
extern short   IntFlagHF,IntFlagHFInv;

extern int count_test;
extern int EvenOdd[6];
extern int SHEShutFlag_YN ;
extern int PsblupDateCMPA[6];

extern int MinPulse;/*=T1PRHalf;*/

extern double Ts;
/* extern double T1PR; */
/* extern double T1PRHalf;  */

extern int rxVdc1,rxVdc2;  /* for VBC ,originally rxCpuData.Vdc1*/
extern int rxia_vsi,rxib_vsi,rxic_vsi;

extern double spdRef;
extern double spdCom;
extern int ExacTim;

//extern double  Udc; 

/* extern double Freq; */ /* fundamental frequency*/

extern int updateCMPA;

 
extern union IO_OUT_REG IO_OUT; 
extern union IO_IN_REG IO_IN;
extern unsigned short Trans_TimerEnable;
extern unsigned short Trans_timerPeriod;
extern unsigned short Trans_clkDivide;
extern unsigned short Trans_deadtime;
extern unsigned short Trans_bus_pwmen;
extern unsigned short Trans_err_state;
extern unsigned short Trans_pwm_en;
extern unsigned short Trans_bus_err,Inv_PWMA1,Inv_PWMA2,Inv_PWMA3,Inv_PWMA4,Inv_PWMB1,Inv_PWMB2,Inv_PWMC1;

extern unsigned short InvDriveErr, Pwm_en;
extern unsigned short RecDriveErr;
extern unsigned short RecDriveErrLocked;
extern unsigned short InvDriveErrLocked;
extern unsigned short BusErr;

extern int adin[16];
extern double Udc_up,Udc_down,Iu,Iv,Iw,Uu,Uv,Uw,RUdc_up,RUdc_down,IuOld,IvOld,IwOld;
extern double Rec_Ia,Rec_Ib,Rec_Ic,Rec_Ua,Rec_Ub,Rec_Uc,Udc_down_filter,Udc_up_filter, Rec_Iu,Rec_Iv,Rec_Iw;
extern double Rec_IaTemp,Rec_IbTemp,Rec_IcTemp,Rec_UaTemp,Rec_UbTemp,Rec_UcTemp,Udc_down_filterTemp,Udc_up_filterTemp, Rec_IuTemp,Rec_IvTemp,Rec_IwTemp;
extern struct  Trans_para iout,iout_ref,icom,icom2,icom2_ref,icom4,delturef,deltu2,deltu4,ucap_sum_com;//,uref;

extern double UsRef;
extern double Kv;
extern double VVVF_Gama;
/* extern int Reg_UpDown;  */
extern double Gama_SVM;
extern int MP_test;

extern double pt2t1,pt3t2,pt3t1;
extern double pt1,pt2,pt3;
extern int MP_Debug;
extern int Ct1t2t3,Bbit,Sbit,Gbit;
extern int MPhalf;

/* extern short  ModeFlag;  */

extern int Sector_Num;
extern int Region_Num;
extern int Sector_Num_Pre;
extern int Region_Num_Pre;
extern double u_alph;  
extern double u_beta; 
extern double epwm1cmpa,epwm2cmpa,epwm3cmpa,epwm4cmpa,epwm5cmpa,epwm6cmpa;
extern double epwm1cmpaPre,epwm2cmpaPre,epwm3cmpaPre,epwm4cmpaPre,epwm5cmpaPre,epwm6cmpaPre;

extern int Min_Pulse;

extern int EPwm_g_i_PRDPre;
extern int EPwm_g_i_CAUPre; 
extern int EPwm_g_i_ZROPre; 
extern int EPwm_g_i_CADPre;

extern double Vdc1S[5];
extern double Vdc2S[5];
extern int j;
extern double Vdc1Av;
extern double Vdc2Av;
extern long lj;
/* extern double ia,ib,ic;  */

//extern short  runCmd;

extern unsigned  int ClearCmd;
extern unsigned  int faultCode1,faultCode2,faultCode3,faultCode4;

extern unsigned  int rxCpuLiveCntOld;
extern unsigned  int SVMSHECmd;   

extern int temp;
extern int temp11,temp22,temp33,temp44;
//extern unsigned int runState;
extern unsigned int      VMEConnectFlag;
extern unsigned int	  VMEConnectErrCnt;
extern unsigned  int VMEConnectOKCnt;

/*tb*/
extern double t1,t2,t3;
extern void PWM_Prepare(void);
/*extern void VdcDevi2deltT(struct PWMdataStruct *PWMdata);*/
extern double t2N; /*test  t2NS,t2NB,*/
extern double ta0,tb0,tc0,ta,tb,tc;

/*All N-type*/
extern const  double * NPtimeTable[6][6][3]; 
/* All of this tables are corrected by TLC at 20090901 YZG ActTb is N-type, but the CMPR is not.               */
/*Region_Num：0--5*/
/*1--low   2--high --toggle 0--no action*/
/*ALL N-type*/
extern const  short ActTb_EPwm1[6][6][4];
extern const  short ActTb_EPwm2[6][6][4]; 
extern const  short ActTb_EPwm3[6][6][4];
extern const  short ActTb_EPwm4[6][6][4];
extern const  short ActTb_EPwm5[6][6][4];
extern const  short ActTb_EPwm6[6][6][4];

extern double Vdc1_1P2;
extern double Vdc2_1P2;
extern int ma_index;
/* extern int HalfGama_TotalNum;
extern int Gama_TotalNum;
extern int Gama_TotalNum_Pre;  */
//extern double 	f;
/* extern double vvvf1;  */
extern double vvvf1Pre;
extern double VFVolt,Ts_Pre;
extern double VFGama,VFGama_pre;
extern double vfk;
extern double Gama_SVM_Real;

extern double Gama_SVM_Used;
extern double jp_ta,jp_tb,jp_tc;
extern double t1C,t2C,t3C;
/* extern int PWM4Cnt;  */

extern const double * JpStart_NPtimeTable[6][3];
extern const double * JpStart_NPtimeTableC[6][3];
extern const short JapStart_ActTb_EPwm1[4][2];
extern const short JapStart_ActTb_EPwm2[4][2];

extern double jp_tbtemp;
extern int ZRO[6],CAU[6],PRD[6],CAD[6];

extern  double GamaTb;

extern int EPwm_g_i_CADPre;

extern double Prdf1,TsGama;

/* extern double VFGama_Pre ;  */
extern double CMPA[6]; 

/****************** Neutral Volt Control 20100321 ******************/              
extern double Gama_SVM_DeltT;
extern double NeuVoltCtl_sum;
extern double Kp_uz ;
extern double Ki_uz;
extern double Vdc1MinuVdc2;
extern double NeuVoltCtl_deltT;
extern int sign_ia, sign_ib, sign_ic;
extern int Nostep;/*VBC*/

/*Reg*/
/*0    1     2   3    4    5 */
extern const  short Tbt1t2t3[6][6];
extern int NPChange;
extern  int MP_test;
extern double Kvk1;
extern int Change_SL_Store;
extern double vvvf1Start;
extern double CNT_SCR;
extern double CNT_SCRRun;
extern int SCRDis;
extern int DSPSelfCheck;
extern double Ia_vsi, Ib_vsi, Ic_vsi, Ialpha_vsi, Ibelta_vsi, Id_vsi, Iq_vsi;
extern double Id_vsif, Iq_vsif, Id_vsifk1, Iq_vsifk1;
extern double P_vsif;
extern double Uref_vsi, Ualpha_vsi, Ubelta_vsi, Ud_vsi, Uq_vsi;
extern double Vdc, Kv_vsi, Gama_vsi;
extern double P_vsi;
extern int VBC_test;
/* extern int MinPulseOK;  */
extern int SHEShutFlag ;
extern int ShutSHEHappenedFlag;
 
 
extern int SHE2SVFLAG;
 
extern double MinPulseTime;/*150us*/
 
extern int SVM_PRD[6],SVM_CAD[6];
extern int SHEReadyFLAG[6];
extern double  VFGama6[6] ;
 
extern const  double * TbStartPointer;
extern double VFGama6_g_iC;
extern double VFGama_Pre6_g_iC;
extern double 	f1point;
extern double 	f2point;           
extern double 	f3point;             
extern double 	f4point;              
extern double 	f5point;            
extern double 	f6point;

extern double 	f1pointB;
extern double 	f2pointB;     
extern double 	f3pointB;     
extern double 	f4pointB;         
extern double 	f5pointB;        
extern double 	f6pointB;
extern double   fpointWXX,fswitch;
extern double    VFGama_Pre6[6];

extern int Epwm_g_i_timeconti;

extern double deltGamaSHE;
extern int g_i;
/*110*1    20100604*/
extern const  double SHE_1[110];
/*101*3*/
extern const  double SHE_3[303];
/*102*5*/
extern const  double SHE_5[510];
/*101*7  */
extern const  double SHE_7[707];
/*101*9*/
extern const  double SHE_9[909];
/*101*1*/
extern const  double SHE_11[1111];
/*for vvvf1 Hysteris*/ 
/* extern int vvvf1IncreFlag;  */
extern int SVM_VME_Flag;
extern int SHE_VME_Flag;

extern double Freq_vsi;
extern double VFGamaStart;
extern int testflag1;
extern int changeTsSHEFLAG;
extern int testflag2;
extern double VFGamatemp;
extern double VFGamaSHEReady;
 
/* extern int FirstEnPWMFlag;  */
extern int NPCautFlag;
extern int NPZROhead_A1;
extern int NPZROtail_A1;
extern int NPZROhead_A2;
extern int NPZROtail_A2;
extern int NPZROhead_B1;/* = 0;*/
extern int NPZROtail_B1;/* = 0;*/
extern int NPZROhead_B2;/* = 0;*/
extern int NPZROtail_B2;/* = 0;*/
extern int NPZROhead_C1;/* = 0;*/
extern int NPZROtail_C1;/* = 0;*/
extern int NPZROhead_C2;/* = 0;*/
extern int NPZROtail_C2;/* = 0;*/
extern int NPPRDhead_A1;/* = 0;*/
extern int NPPRDtail_A1;/* = 0;*/
extern int NPPRDhead_A2;/* = 0;*/
extern int NPPRDtail_A2;/* = 0;*/
extern int NPPRDhead_B1;/* = 0;*/
extern int NPPRDtail_B1;/* = 0;*/
extern int NPPRDhead_B2;/* = 0;*/
extern int NPPRDtail_B2;/* = 0;*/
extern int NPPRDhead_C1;/* = 0;*/
extern int NPPRDtail_C1;/* = 0;*/
extern int NPPRDhead_C2;/* = 0;*/
extern int NPPRDtail_C2;/* = 0;*/

extern int NPjumpA;/* = 0;*/
extern int NPjumpB;/* = 0;*/
extern int NPjumpC;/* = 0;*/
extern int PNjumpA;/* = 0;*/
extern int PNjumpB;/* = 0;*/
extern int PNjumpC;/* = 0;*/
extern int EnablePWMPre;/* = 0;*/

extern int ErrFlag;


/* extern int ChangeSHENPJumpTest,ChangeSHENPJumpFlag;  */
extern double VFGamaStore;

extern int vrefDirFlag;

extern short InvIntFlag,InvIntFlag2;



extern double GamaInterDelta, VFGamaPre;

extern double IdTest, IqTest;

extern short IntFlagTest,PeriodTest,PeriodTest2,ZROTest,CAUTest,PRDTest,CADTest;
extern short CMPAU1Test,CMPAU2Test,CMPAU3Test,CMPAU4Test,CMPAU5Test,CMPAU6Test;
extern short CMPAD1Test,CMPAD2Test,CMPAD3Test,CMPAD4Test,CMPAD5Test,CMPAD6Test,SyncFlagTest;
int TotalNumStore; 



extern short errcnt;
extern int SHERoute[6];
extern int EnablePWM;

extern float rxFre,rxFre_pre;

//extern struct PWMdataStruct PWMdata_INV1; 
//extern struct PWMdataStruct PWMdata_INV2;
//extern struct PWMdataStruct PWMdata_REC1; 
//extern struct PWMdataStruct PWMdata_REC2;

#ifdef __cplusplus
}
#endif

#endif
