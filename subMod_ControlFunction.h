

#ifndef _SUBMOD_CONTROL_FUNCTION_
#define _SUBMOD_CONTROL_FUNCTION_

#ifdef __cplusplus
extern "C" {
#endif
 
/*********************************************************************************************/
#include "vxWorks.h"
#include <math.h>
/*********************************************************************************************/


/* #define PI 3.141592654 */
#define SQRT3_3 0.5773503
#define SQRT3_2 0.8660254


struct Trans_para
{
 double a;                
 double b;               
 double c;
 double alpha;            
 double beta;     
  double alpha1;            
 double beta1; 
  double alpha2;            
 double beta2;  
 double d;           
 double q;  
 double d_rl;           
 double q_rl;  
  double d1;           
 double q1; 
  double d2;           
 double q2; 
 double m;
 double delta;
 double delta1;
 double delta2;
};
/*滤波器*/
struct filter
{
 double fc;     
 double Ts;      
 double filter_input;          
 double filter_out;               
 double filter_old;                    
};

/*PR*/
struct PR_controlStruct
{
	double PR_phai11;
	double PR_phai12;
	double PR_phai21;
	double PR_phai22;
	double PR_c1;
	double PR_c2;
	double PR_Ref;
	double PR_Fdb;
	double PR_Err;
	double PR_w1kp;
	double PR_w2kp;
	double PR_w1k;
	double PR_w2k;
	double PR_Kp;
	double PR_Kr; 
	double PR_OutMax;
	double PR_OutMin;
	double PR_Out;
};
/*PR锁相*/
struct PR_viewStruct
{
	double input_x;
	double phai11;
	double phai12;
	double phai21;
	double phai22;
	double c1;
	double c2;
	double w1kp1;
	double w2kp1;
	double w1k;
	double w2k;
	double kf;
	double lim_pr;
	double peak_view;
	double sin_view;
	double cos_view;
};




/*低通滤波器*/
typedef struct LPFK
{
	float  input;
	float  output;
	float  outputOld;

	float Ts;
	float Hz;

	float temp;
}LPF;

void initLPF(LPF *lpf);
void calcLPF(LPF *lpf);
void restartLPF(LPF *lpf);


//IO控制结构体
struct IO_in_ANPC_BITS_ANPC 
{	
UINT16 notused1:1;       
UINT16 notused2:1;	
UINT16 notused3:1;	
UINT16 notused4:1;

UINT16 notused5:1;
UINT16 notused6:1;
UINT16 notused7:1;
UINT16 main_switch:1;

UINT16 mainswitch_open:1;
UINT16 reset:1;
UINT16 discharge:1;
UINT16 stop_rec:1;

UINT16 stop_inv:1;
UINT16 start_inv:1;
UINT16 start_rec:1; 
UINT16 notused8:1;
 
};
struct IO_out_ANPC_BITS_ANPC 
{
	
/* used by ETOANPC*/
UINT16 notused1:1;       
UINT16 notused2:1;	
UINT16 notused3:1;	
UINT16 notused4:1;

UINT16 notused5:1;
UINT16 notused6:1;
UINT16 serial:1;
UINT16 main_switch_led:1;

UINT16 parallel:1;
UINT16 inv_overcurrent:1;
UINT16 Udc_Nobalance:1;
UINT16 DC_overvoltage:1;

UINT16 rec1_overcurrent:1;
UINT16 start_inv:1;
UINT16 start_rec:1; 
UINT16 precharge:1;
 
};
//IO控制共用体
union IO_in_REG_ANPC
{
struct IO_in_ANPC_BITS_ANPC bit;
UINT16 word;
};
union IO_out_REG_ANPC
{
struct IO_out_ANPC_BITS_ANPC bit;
UINT16 word;
};



/*  PID reguater function parameters : */
struct PIDREG
{
 double ref_reg;     
 double fdb_reg;      
 double out_max;      
 double out_min;      
 double out_reg;     
 double Kp;               
 double Ki;               
 double Kd;               
 double Kc;               
 double err_reg;          
 double uprsat_reg;      
 double up_reg;           
 double up_reg1;         
 double ui_reg;          
 double ui_reg1;          
 double ud_reg;          
};
/*PID控制器*/
typedef struct PIDK
{
	float  Ref;   		/* Input: Reference input */
	float  Fdb;   		/* Input: Feedback input */
	float  Err;			/* Variable: Error*/ 
	float  Kp;			/* Parameter: Proportional gain*/
	float  Up;			/* Variable: Proportional output */
	float  Ui;			/* Variable: Integral output */
	float Ud;			/* Variable: Derivative output	*/
	float  OutPreSat;	/* Variable: Pre-saturated output*/
	float  OutMax;		/* Parameter: Maximum output */
	float  OutMin;		/* Parameter: Minimum output*/
	float  Out;   		/* Output: PID output */
	float  SatErr;		/* Variable: Saturated difference */
	float  Ki;			/* Parameter: Integral gain */
	float  Kc;			/* Parameter: Integral correction gain */
	float  Kd; 			/* Parameter: Derivative gain */
	float  Up1;			/* History: Previous proportional output*/
}PIDREG3;

void pid_reg3_restart(PIDREG3 *v);
void pid_reg3_calc(PIDREG3 *v);

/*PR控制器*/
typedef struct {  	
	float Ref;
	float Fed;
	float Err;			/*Variable: Error */				  
	float Out;

	float Ts;
	float BaseFreq;

	float  An11;			/*Parameter: Proportional gain*/
	float  An12;			/* Variable: Proportional output */
	float  An21;			/* Variable: Integral output */
	float  An22;			/* Variable: Derivative output	*/
	float  C1;	        /* Variable: Pre-saturated output*/
	float  C2;		    /* Parameter: Maximum output */

	float  W1k;		    /* Parameter: Minimum output*/
	float  W2k;   	    /*Output: PID output */
	float  W1kp1;		/* Variable: Saturated difference*/
	float  W2kp1;	    /*Parameter: Integral gain*/

	/*float filterOut;*/		/* Notch Filter Output */

	float  Kr;			/*Parameter: Integral correction gain*/
	float  Kp;

	float  Max;			/*History: Previous proportional output*/
	float  Min;
} PRREG;	 

void PR_init(PRREG *v);
void PR_calc(PRREG *v);
void PR_restart(PRREG *v);

/*Notch Filter*/
typedef struct {  	
	float Ref;
	float Fed;
	float Err;			/*Variable: Error */				  
	
	float Ts;
	float BaseFreq;

	float  An11;			/*Parameter: Proportional gain*/
	float  An12;			/* Variable: Proportional output */
	float  An21;			/* Variable: Integral output */
	float  An22;			/* Variable: Derivative output	*/
	float  C1;	        /* Variable: Pre-saturated output*/
	float  C2;		    /* Parameter: Maximum output */

	float  W1k;		    /* Parameter: Minimum output*/
	float  W2k;   	    /*Output: PID output */
	float  W1kp1;		/* Variable: Saturated difference*/
	float  W2kp1;	    /*Parameter: Integral gain*/	

	float  Kr;			/*Parameter: Integral correction gain*/
	float  Kp;

	float  Max;			/*History: Previous proportional output*/
	float  Min;
} NtFREG;	 

void NtFilter_init(NtFREG *v);
void NtFilter_calc(NtFREG *v);
void NtFilter_restart(NtFREG *v);

/*斜坡函数*/
typedef struct { 
	float  TargetValue; 		/* Input: Target input (pu)*/
	float  RampDelayMax;		/* Parameter: Maximum delay rate (Q0)	*/	
	float  RampLowLimit;		/* Parameter: Minimum limit (pu)		*/		  
	float  RampHighLimit;	/* Parameter: Maximum limit (pu)*/
	float  RampDelayCount;  /* Variable: Incremental delay (Q0)	*/
	float  SetpointValue;	/*Output: Target output (pu)		*/		 
	float  EqualFlag;		/* Output: Flag output (Q0)*/
} RMPCNTL;	

void rmp_cntl_restart(RMPCNTL *v);
void rmp_cntl_calc(RMPCNTL *v);

/*线电压 转为 相电压*/
typedef struct
{
	float Uab;
	float Ubc;
	float Uca;

	float Ua;
	float Ub;
	float Uc;
}LINE_TO_PHASE;

void calcLineToPhase(LINE_TO_PHASE *v);

/*线电压 ABC 静止坐标系 to 旋转坐标系*/
typedef struct  
{
	float Ds;
	float Qs;

	float Uab;
	float Ubc;

	float theta;
}RST_TO_DQ;

void calc_RST_DQ(RST_TO_DQ *v);

/* ABC 静止坐标系 to 旋转坐标系*/
typedef struct  ABC_TO_DQ
{
  float D;
  float Q;
  float A;
  float B;
  float C;
  float Theta;

}ABC2DQ;
void Cal_ABC_DQ(ABC2DQ *v);



/* ABC 静止坐标系 to 旋转坐标系*/
typedef struct  
{
	float Ds;
	float Qs;

	float Ua;
	float Ub;
	float Uc;

	float Sine;
	float Cose;
	float Sine_120;
	float Cose_120;
	float Sine_240;
	float Cose_240;
	float theta_120;
	float theta_240;

}ABC_TO_DQ;

void calc_ABC_DQ(ABC_TO_DQ *v);

/*二阶低通滤波器*/
typedef struct LPFK2
{
	float  input;
	float  inputOld;
	float  inputOld2;
	float  out;
	float  outputOld;
	float  outputOld2;

	float b0;
	float b1;
	float b2;
	float a1;
	float a2;	

}LPF2nd;

void calc2ndLPF(LPF2nd *v);
void restart2ndLPF(LPF2nd *v);

typedef struct AVR_LPFK2
{
	float input;
	float inputOld;

	float output;
}AVR_LPF;

void calcAvrLpf(AVR_LPF *v);
void restartAvrLpf(AVR_LPF *v);
void calc_ABC_DQ2(float a,float b,float c,float theta,float * d,float * q);

extern struct  Trans_para iout,uref_syn,Upll,iout_rec,u_rec;
//电机控制
extern struct PIDREG Pid_iout_d;//2 0.0001
extern struct PIDREG Pid_iout_q;
extern struct PIDREG Pid_speed;
extern struct filter filter_theta_syn;
extern struct filter filter_infreq;
extern struct filter filter_Id_inv;
extern struct filter filter_Iq_inv;
extern struct filter filter_speed;
//电流定位
extern struct PIDREG Pid_iout_d_current_dir;
extern struct PIDREG Pid_iout_q_current_dir;
//整流闭环
extern struct filter filter_Id_rec1;
extern struct filter filter_Iq_rec1;
extern struct filter filter_Id_rec2;
extern struct filter filter_Iq_rec2;
extern struct filter filter_Ud_rec1;
extern struct filter filter_Uq_rec1;
extern struct filter filter_Ud_rec2;
extern struct filter filter_Uq_rec2;
extern struct filter filter_vd;
extern struct filter filter_vq;
extern struct filter filter_Udc1;
extern struct filter filter_Udc2;
extern struct filter filter_Udc1_2;
extern struct filter filter_Udc2_2;
extern struct filter Kvfilter;
extern struct filter filter_I_rec;
extern struct PIDREG Pid_Udc ;
extern struct PIDREG Pid_iout_d_rec;
extern struct PIDREG Pid_iout_q_rec;
extern struct PIDREG Pid_iout_d2_rec;
extern struct PIDREG Pid_iout_q2_rec;
//整流锁相
extern struct PIDREG Pid_PLL ;
//中点平衡
extern struct PIDREG Pid_Udcbanlance;
//PR控制
extern struct PR_controlStruct PR_control_Ias_rec1;
extern struct PR_controlStruct PR_control_Ibs_rec1; 
extern struct PR_controlStruct PR_control_Ics_rec1; 
extern struct PR_controlStruct PR_control_Ias_rec2;
extern struct PR_controlStruct PR_control_Ibs_rec2; 
extern struct PR_controlStruct PR_control_Ics_rec2; 
//PR锁相
extern struct PR_viewStruct PR_view_Uas_rec1;
extern struct PR_viewStruct PR_view_Ubs_rec1; 
extern struct PR_viewStruct PR_view_Ucs_rec1; 
extern struct PR_viewStruct PR_view_Uas_rec2;
extern struct PR_viewStruct PR_view_Ubs_rec2; 
extern struct PR_viewStruct PR_view_Ucs_rec2; 

extern union IO_in_REG_ANPC IO_in_ANPC;
extern union IO_out_REG_ANPC IO_out_ANPC;
extern void initPR_control(struct PR_controlStruct *PR_controldata);
extern void calPR_control(struct PR_controlStruct *PR_controldata);
extern void initPR_view(struct PR_viewStruct *PR_viewdata);
extern void calPR_view(struct PR_viewStruct *PR_viewdata);
extern void rec1_abc_dq(void);
extern void rec2_abc_dq(void);
extern void VFcontrol_Inv(void);
extern void VFcontrol_Rec(void);
extern void Grid_PLL(struct PWMdataStruct *PWMdata);  
extern void Grid_FOC(struct PWMdataStruct *PWMdata);
extern void GamaInterpolation(struct PWMdataStruct *PWMdata,unsigned short index);
extern void Grid_FOC_YY0(struct PWMdataStruct *PWMdata); 
extern void Grid_FOC_YD11(struct PWMdataStruct *PWMdata); 
extern void Grid_FOC_HighPower_REC1(struct PWMdataStruct *PWMdata);
extern void Grid_FOC_HighPower_REC2(struct PWMdataStruct *PWMdata);
extern void VdcDevi2deltT(struct PWMdataStruct *PWMdata);
extern void filter_function(struct filter *filter);
extern void Grid_FOC_YY0_PR(struct PWMdataStruct *PWMdata); 
extern void Control_init(void);

extern void Pid(struct PIDREG *Pid);
extern void Clarke(struct Trans_para *clarke);
extern void IClarke(struct Trans_para *iclarke);
extern void Park(struct Trans_para *park,float park_gamma);
extern void IPark(struct Trans_para *ipark,float ipark_gamma);




#ifdef __cplusplus
}
#endif

#endif
