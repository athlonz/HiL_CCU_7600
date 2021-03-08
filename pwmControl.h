#ifndef _PWMCONTROL_
#define _PWMCONTROL_

#ifdef __cplusplus
extern "C" {
#endif
/*********************************************************************************************/
#include "vxWorks.h"
#include <math.h>
/*********************************************************************************************/
struct PWMdataStruct
{
 /*input variables*/
	double vvvf1;
	double vvvf1Interpolation;
	double Kv;
	double Kv1;
	double Kv2;
	double Kv3;
	double Kvave;
	double KvPreave;
	double KvPre;
	double KvPre1;
	double KvPre2;
	double KvPre3;
	int KvIncreFlag;
	double VFGama ;
	double VFGamaOrigin;
	double VFGama_Pre;
	unsigned int Reg_UpDown;
	unsigned int IntFlag;
	unsigned int invPwmMode;
	unsigned int SHEShutFlag;
	unsigned int Gama_TotalNum_Pre;
	unsigned int HalfGama_TotalNum;
	
	unsigned int Gama_TotalNum;
	
	/*middle variables*/
	double VFGama6[6];
	double VFGama_Pre6[6];
	unsigned int gama_Num[6]; 
	unsigned int gama_Num_pre[6];

	int Epwm1_timeconti;
	int Epwm2_timeconti;
	int Epwm3_timeconti;
	int Epwm4_timeconti;
	int Epwm5_timeconti;
	int Epwm6_timeconti;
	int Epwm_g_i_timeconti;
	
	unsigned int EPwm1PRDPre ;
	unsigned int EPwm1CADPre ;
	unsigned int EPwm1ZROPre ;
	unsigned int EPwm1CAUPre ;

	unsigned int EPwm2PRDPre ;
	unsigned int EPwm2CADPre ;
	unsigned int EPwm2ZROPre ;
	unsigned int EPwm2CAUPre ;

	unsigned int EPwm3PRDPre ;
	unsigned int EPwm3CADPre ;
	unsigned int EPwm3ZROPre ;
	unsigned int EPwm3CAUPre ;

	unsigned int EPwm4PRDPre ;
	unsigned int EPwm4CADPre ;
	unsigned int EPwm4ZROPre ;
	unsigned int EPwm4CAUPre ;

	unsigned int EPwm5PRDPre ;
	unsigned int EPwm5CADPre ;
	unsigned int EPwm5ZROPre ;
	unsigned int EPwm5CAUPre ;

	unsigned int EPwm6PRDPre ;
	unsigned int EPwm6CADPre ;
	unsigned int EPwm6ZROPre ;
	unsigned int EPwm6CAUPre ;
	 
	unsigned int EPwm1EndState ;
	unsigned int EPwm2EndState ;
	unsigned int EPwm3EndState ;
	unsigned int EPwm4EndState ;
	unsigned int EPwm5EndState ;
	unsigned int EPwm6EndState ;
	
	unsigned int EPwmEndState ; 
	
	/*output variables*/
	unsigned int ACQ_ZRO;
	unsigned int ACQ_CAU;
	unsigned int ACQ_PRD;
	unsigned int ACQ_CAD;
	double CMPA[6];
	
	int FirstEnPWMFlag;
	int vvvf1IncreFlag;
    int InvModulationFlag;   
	int PWM4Cnt;
	double T1PR;
	double T1PRHalf;
	double Ts;
	int Change_SL;
	int SVMModeSwitchUp;
	int ChangeSHENPJumpTest;
	int ModeFlag;
	int ChangeSHENPJumpFlag;
	int SVMModeSwitchDn;
	int SVPWMType;
	char* PWMTransLocalAddr;
	int SHEReady;
	int MinPulseOK;
	int SwitchSpeedDnOnce;
	int SwitchSpeedUpOnce;
	
	double Ia;
	double Ib;
	double Ic;
	double Ua;
	double Ub;
	double Uc;
	double Theta_SecdFwd;  /*PLL*/
	double KiPLL;
	double KpPLL;
	double Delta;
	double GridFOC_Gama;  /*锁相角*/
	double sum_usq;
	
	double Udc_up_filter;/* neutral point balancing*/
	double Udc_down_filter;
	double NeuVoltCtl_sum;
	double NeuVoltCtl_deltTRec;
	double Vdc1MinuVdc2Rec;
	double sign_iaRec;
	double sign_ibRec;
	double sign_icRec;
	double Gama_SVM_DeltTRec;
	double Ki_uzRec;  /*0*/
	double Kp_uzRec;  /*0.03*/
	
	double vref;	
	double VKi;
	double vsum;
	double VKp;
	double vdsum;
	double Idq_Ki;
	double Idq_Kp;
	double Idref;
	double Iqref;
	double vqsum;
	
	int TransWriteFlag;
	int MinPulseFlag;
	
	unsigned int ZROPreTest;
	unsigned int CAUPreTest;
	unsigned int PRDPreTest;
	unsigned int CADPreTest;
	
	unsigned int BranchTest[6];
	
} ; 
extern struct PWMdataStruct PWMdata_INV1; 
extern struct PWMdataStruct PWMdata_INV2;
extern struct PWMdataStruct PWMdata_REC1; 
extern struct PWMdataStruct PWMdata_REC2;


extern int 	SHERoute[6];
/******************************************************************************************/
extern void initPWMdata(struct PWMdataStruct *PWMdata);
extern void resetPWMdata(struct PWMdataStruct *PWMdata);
extern void InitINVPWM_Mode(struct PWMdataStruct *PWMdata,short InvModulationFlag);
extern void setPWMBoardReg_SRIO(struct PWMdataStruct *PWMdata,struct TX_PWM_SRIO_Struct *TxPwmBoardSrioData);
extern void resetPwmBoardReg_SRIO( struct TX_PWM_SRIO_Struct *TxPwmBoardSrioData, struct PWMdataStruct *PWMdata);
extern void enable_AllPWMOut( void);
extern void disable_AllPWMOut( void);
extern void enable_rec1PWM( void);
extern void enable_rec2PWM( void);
extern void enable_inv1PWM( void);
extern void enable_inv2PWM( void);
extern void disable_rec1PWM( void);
extern void disable_rec2PWM( void);
extern void disable_inv1PWM( void);
extern void disable_inv2PWM( void);
extern void recPWM_EntranceFunc(unsigned short index,struct PWMdataStruct *PWMdata,struct TX_PWM_SRIO_Struct *TxPwmBoardSrioData);
extern void invPWM_EntranceFunc(unsigned short index,struct PWMdataStruct *PWMdata,struct TX_PWM_SRIO_Struct *TxPwmBoardSrioData);
extern void invPwmSwitchOncePrinciple(struct PWMdataStruct *PWMdata);
extern void InvPwmModeModulation(struct PWMdataStruct *PWMdata); 
extern void SVPWM(struct PWMdataStruct *PWMdata);
/*****以下几个为老版的函数，暂时保留***********************************************/
extern void PWM_EntranceFunc(struct PWMdataStruct *PWMdata1,struct PWMdataStruct *PWMdata2,\
		struct PWMdataStruct *PWMdata3,struct PWMdataStruct *PWMdata4,int FreqBasedSw1_AutoSW0,\
		unsigned short index);
extern void SwitchOncePrinciple(struct PWMdataStruct *PWMdata1,struct PWMdataStruct *PWMdata2);
extern void resetPWMreg(struct PWMdataStruct *PWMdata);
extern void PWMBoardRegSet(struct PWMdataStruct *PWMdata); 
extern void GamaInterpolationNew(struct PWMdataStruct *PWMdata1,struct PWMdataStruct *PWMdata2,unsigned short index);

extern void Rec1_anpc1(void);
extern void Rec1_anpc2(void);
extern void Rec1_npc(void);

extern void Rec2_anpc1(void);
extern void Rec2_anpc2(void);
extern void Rec2_npc(void);

extern void Inv1_anpc1(void);
extern void Inv1_anpc2(void);
extern void Inv1_npc(void);

extern void Inv2_anpc1(void);
extern void Inv2_anpc2(void);
extern void Inv2_npc(void);

#ifdef __cplusplus
}
#endif

#endif
