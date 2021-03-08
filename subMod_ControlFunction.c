/*  
* target: rectifier and invertor PWM call enterance   
* Copyright 2018,4,16 IEE, Inc. 
* description:   
* function: 用于闭环控制中各种基本函数的初始化、计算*
* debug: 
* modification history:
	1. V1.0 @ 2013-03-26 里面包含 PID控制器, PR控制器, PR锁相环, RMP斜坡函数，Clark变换， park变换， ipark变换，LPF低通滤波器, HPF高通滤波器, BPF带通滤波器
	20180426 by wxx
*/

#include "math.h" 
#include "subMod_ControlFunction.h"
#include "GlobalVar_Inv.h"
#include "variable_main.h"
/***************************************************************************************/
/******************定义结构体******************************************************/
struct  Trans_para iout,iout_ref,ucap_sum_com,uref_syn;
//电机控制
struct PIDREG Pid_iout_d={0,0,180,-180,0,1.25,0.0004,0,0,0,0,0,0,0,0,0};//2 0.0001
struct PIDREG Pid_iout_q={0,0,180,-180,0,1.25,0.0004,0,0,0,0,0,0,0,0,0};
struct PIDREG Pid_speed={0,0,25,-25,0,1,0.00007,0,0,0,0,0,0,0,0,0};
//电流定位
struct PIDREG Pid_iout_d_current_dir={0,0,50,-50,0,3,0.0003,0,0,0,0,0,0,0,0,0};
struct PIDREG Pid_iout_q_current_dir={0,0,50,-50,0,3,0.0003,0,0,0,0,0,0,0,0,0};
//整流闭环
struct filter filter_Id_rec1={15,1/7200.,0,0,0};
struct filter filter_Iq_rec1={15,1/7200.,0,0,0};
struct filter filter_Id_rec2={15,1/7200.,0,0,0};
struct filter filter_Iq_rec2={15,1/7200.,0,0,0};
struct filter filter_Udc1={5,1/7200.,0,0,0};
struct filter filter_Udc2={5,1/7200.,0,0,0};
struct filter filter_Udc1_2={15,1/7200.,0,0,0};
struct filter filter_Udc2_2={15,1/7200.,0,0,0};
struct filter Kvfilter={0.1,1/7200.,0,0,0};
//PR
struct PR_controlStruct PR_control_Ias_rec1;
struct PR_controlStruct PR_control_Ibs_rec1; 
struct PR_controlStruct PR_control_Ics_rec1; 
struct PR_controlStruct PR_control_Ias_rec2;
struct PR_controlStruct PR_control_Ibs_rec2; 
struct PR_controlStruct PR_control_Ics_rec2; 
//PR锁相
struct PR_viewStruct PR_view_Uas_rec1;
struct PR_viewStruct PR_view_Ubs_rec1; 
struct PR_viewStruct PR_view_Ucs_rec1; 
struct PR_viewStruct PR_view_Uas_rec2;
struct PR_viewStruct PR_view_Ubs_rec2; 
struct PR_viewStruct PR_view_Ucs_rec2; 
//未用
struct PIDREG Pid_Udc   ={0,0,500,-500,0,3,0.01,0,0,0,0,0,0,0,0,0};
struct PIDREG Pid_ialpha ={0,0,5000,-5000,0,5,0.01,0,0,0,0,0,0,0,0,0};
struct PIDREG Pid_ibeta  ={0,0,5000,-5000,0,5,0.01,0,0,0,0,0,0,0,0,0};
struct PIDREG Pid_theta  ={0,0,5000,-5000,0,1,0.01,0,0,0,0,0,0,0,0,0};
struct PIDREG Pid_mras  = {0,0,5000,-5000,0,0.005,0.0005,0,0,0,0,0,0,0,0,0};/*模型参考自适应PI*/
struct filter filter_theta={600,1/7200.,0,0,0};
struct filter filter_Id_inv={2,1/7200.,0,0,0};
struct filter filter_Iq_inv={2,1/7200.,0,0,0};

union IO_in_REG_ANPC IO_in_ANPC;
union IO_out_REG_ANPC IO_out_ANPC;

/*PR参数初始化*/
void initPR_control(struct PR_controlStruct *PR_controldata)
{
	PR_controldata->PR_phai11=0.0;
	PR_controldata->PR_phai12=0.0;
	PR_controldata->PR_phai21=0.0;
	PR_controldata->PR_phai22=0.0;
	PR_controldata->PR_c1=0.0;
	PR_controldata->PR_c2=0.0;
	PR_controldata->PR_Ref=0.0;
	PR_controldata->PR_Fdb=0.0;
	PR_controldata->PR_Err=0.0;
	PR_controldata->PR_w1kp=0.0;
	PR_controldata->PR_w2kp=0.0;
	PR_controldata->PR_w1k=0.0;
	PR_controldata->PR_w2k=0.0;
	PR_controldata->PR_Kp=0.0;
	PR_controldata->PR_Kr=0.0; 
	PR_controldata->PR_OutMax=0.0;
	PR_controldata->PR_OutMin=0.0;
	PR_controldata->PR_Out=0.0;
}
/*PR控制*/
void calPR_control(struct PR_controlStruct *PR_controldata)
{
	PR_controldata->PR_Err=PR_controldata->PR_Ref-PR_controldata->PR_Fdb;
	PR_controldata->PR_w1kp=PR_controldata->PR_phai11*PR_controldata->PR_w1k+PR_controldata->PR_phai12*PR_controldata->PR_w2k+PR_controldata->PR_Kr*PR_controldata->PR_c1*PR_controldata->PR_Err;
	PR_controldata->PR_w2kp=PR_controldata->PR_phai21*PR_controldata->PR_w1k+PR_controldata->PR_phai22*PR_controldata->PR_w2k+PR_controldata->PR_Kr*PR_controldata->PR_c2*PR_controldata->PR_Err;
	PR_controldata->PR_w1k=PR_controldata->PR_w1kp;
	PR_controldata->PR_w2k=PR_controldata->PR_w2kp;
    if (PR_controldata->PR_w1k > PR_controldata->PR_OutMax)  
    {
    	PR_controldata->PR_w1k = PR_controldata->PR_OutMax;
    }
    if (PR_controldata->PR_w1k < PR_controldata->PR_OutMin)
    {
    	PR_controldata->PR_w1k = PR_controldata->PR_OutMin; 
    }
    PR_controldata->PR_Out=PR_controldata->PR_Kp*PR_controldata->PR_Err+PR_controldata->PR_w1k;  
}
/*PR锁相初始化*/
void initPR_view(struct PR_viewStruct *PR_viewdata)
{
	PR_viewdata->input_x=0.0;
	PR_viewdata->phai11=0.0;
	PR_viewdata->phai12=0.0;
	PR_viewdata->phai21=0.0;
	PR_viewdata->phai22=0.0;
	PR_viewdata->c1=0.0;
	PR_viewdata->c2=0.0;
	PR_viewdata->w1kp1=0.0;
	PR_viewdata->w2kp1=0.0;
	PR_viewdata->w1k=0.0;
	PR_viewdata->w2k=0.0;
	PR_viewdata->kf=0.0;
	PR_viewdata->lim_pr=0.0;
	PR_viewdata->peak_view=0.0;
	PR_viewdata->sin_view=0.0;
	PR_viewdata->cos_view=0.0;
}
/*PR锁相*/
void calPR_view(struct PR_viewStruct *PR_viewdata)
{
	PR_viewdata->w1kp1=PR_viewdata->phai11*PR_viewdata->w1k+PR_viewdata->phai12*PR_viewdata->w2k+PR_viewdata->kf*PR_viewdata->c1*(PR_viewdata->input_x-PR_viewdata->w1k);
	PR_viewdata->w2kp1=PR_viewdata->phai21*PR_viewdata->w1k+PR_viewdata->phai22*PR_viewdata->w2k+PR_viewdata->kf*PR_viewdata->c2*(PR_viewdata->input_x-PR_viewdata->w1k);
	PR_viewdata->w1k=PR_viewdata->w1kp1;
	PR_viewdata->w2k=PR_viewdata->w2kp1;
	if(PR_viewdata->w1k>PR_viewdata->lim_pr)
	{
		PR_viewdata->w1k=PR_viewdata->lim_pr;
	}
	if(PR_viewdata->w1k<(-1.0*PR_viewdata->lim_pr))
	{
		PR_viewdata->w1k=(-1.0*PR_viewdata->lim_pr);
	}
	PR_viewdata->peak_view=sqrt(PR_viewdata->w1k*PR_viewdata->w1k+PR_viewdata->w2k*PR_viewdata->w2k);
	PR_viewdata->sin_view=PR_viewdata->w1k/PR_viewdata->peak_view;
	PR_viewdata->cos_view=PR_viewdata->w2k/PR_viewdata->peak_view;  	
}
/*低通滤波器*/
void initLPF(LPF *lpf)
{
	lpf->temp = -2.0 * 3.141592654 * lpf->Hz * lpf->Ts;
	lpf->temp = 1.0 + lpf->temp + 0.5 * lpf->temp * lpf->temp;

	lpf->output    = 0.0;
	lpf->input     = 0.0;
	lpf->outputOld = 0.0;
}



void calcLPF(LPF *lpf)
{
	lpf->output    = (1.0 - lpf->temp) * lpf->input + lpf->temp * lpf->outputOld;
	lpf->outputOld = lpf->output;
}

void restartLPF(LPF *lpf)
{
	lpf->output    = 0.0;
	lpf->outputOld = 0.0;
}
/***************************************************************************************/
/*PI 控制器*/
void pid_reg3_restart(PIDREG3 *v)
{
	v->Ref  = 0;   	    	
	v->Fdb  = 0;   		   
	v->Err  = 0;		
	v->Up   = 0;			
	v->Ui   = 0;			
	v->Ud   = 0;			
	v->OutPreSat = 0;	   
	v->Out    = 0;   		
	v->SatErr = 0;		    
	v->Up1    = 0;			
}

void pid_reg3_calc(PIDREG3 *v)
{	
	v->Err = v->Ref - v->Fdb;

	v->Up = v->Kp*v->Err;

	v->Ui = v->Ui + v->Ki*v->Err + v->Kc*v->SatErr;

	if (v->Ui > v->OutMax)
	{
		v->Ui = v->OutMax;
	}
	if (v->Ui < v->OutMin)
	{
		v->Ui = v->OutMin;
	}

	v->Ud = v->Kd*(v->Up - v->Up1);

	v->OutPreSat = v->Up + v->Ui + v->Ud;     

	if (v->OutPreSat > v->OutMax) 
	{
		v->Out =  v->OutMax;
	}
	else if (v->OutPreSat < v->OutMin)
	{
		v->Out =  v->OutMin;
	}
	else
	{
		v->Out = v->OutPreSat;
	}

	v->SatErr = v->Out - v->OutPreSat;     
 
	v->Up1 = v->Up; 
}

/***************************************************************************************/
/*PR控制器*/
void PR_init(PRREG *v)
{
	v->An11 = cos(2 * PI * v->BaseFreq * v->Ts);
	v->An12 = -1.0 * sin(2 * PI * v->BaseFreq * v->Ts);
	v->An21 = sin(2 * PI * v->BaseFreq * v->Ts);
	v->An22 = cos(2 * PI * v->BaseFreq * v->Ts);
		
	v->C1 = v->An21;
	v->C2 = 1.0 - v->An11;
}

void PR_calc(PRREG *v)
{
	v->Err = v->Ref - v->Fed;
	
	v->W1kp1 = v->W1k * v->An11 + v->W2k * v->An12 + v->Err * v->C1 * v->Kr;
	v->W2kp1 = v->W1k * v->An21 + v->W2k * v->An22 + v->Err * v->C2 * v->Kr;

	if (v->W1kp1 > v->Max)
	{
		v->W1kp1 = v->Max;
	}
	else if (v->W1kp1 < v->Min)
	{
		v->W1kp1 = v->Min;
	}

	if (v->W2kp1 > v->Max)
	{
		v->W2kp1 = v->Max;
	}
	else if (v->W2kp1 < v->Min)
	{
		v->W2kp1 = v->Min;
	}
	
	v->Out = v->W1k + v->Kp * v->Err;		
	/*v->filterOut = v->Ref - v->W1k;*/

	v->W1k = v->W1kp1;
	v->W2k = v->W2kp1;

	if (v->Out > v->Max)
	{
		v->Out = v->Max;
	}
	else if (v->Out < v->Min)
	{
		v->Out = v->Min;
	}
}

void PR_restart(PRREG *v)
{
	v->Err   = 0.0;
	v->W1kp1 = 0.0;
	v->W2kp1 = 0.0;
	v->W1k   = 0.0;
	v->W2k   = 0.0;
	v->Out   = 0.0;
}
/***************************************************************************************/
/*Notch Filter*/
void NtFilter_init(NtFREG *v)
{
	v->An11 = cos(2 * PI * v->BaseFreq * v->Ts);
	v->An12 = -1.0 * sin(2 * PI * v->BaseFreq * v->Ts);
	v->An21 = sin(2 * PI * v->BaseFreq * v->Ts);
	v->An22 = cos(2 * PI * v->BaseFreq * v->Ts);

	v->C1 = v->An21;
	v->C2 = 1.0 - v->An11;
}

void NtFilter_calc(NtFREG *v)
{
	v->Err = v->Ref - v->Fed;

	v->W1kp1 = v->W1k * v->An11 + v->W2k * v->An12 + v->Err * v->C1 * v->Kr;
	v->W2kp1 = v->W1k * v->An21 + v->W2k * v->An22 + v->Err * v->C2 * v->Kr;

	v->Fed = v->W1k;	

	v->W1k = v->W1kp1;
	v->W2k = v->W2kp1;

	if (v->W1k > v->Max)
	{
		v->W1k = v->Max;
	}
	else if (v->W1k < v->Min)
	{
		v->W1k = v->Min;
	}
}

void NtFilter_restart(NtFREG *v)
{
	v->Err   = 0.0;
	v->W1kp1 = 0.0;
	v->W2kp1 = 0.0;
	v->W1k   = 0.0;
	v->W2k   = 0.0;
	v->Fed   = 0.0;
}
/***************************************************************************************/
/*斜坡函数*/
void rmp_cntl_restart(RMPCNTL *v)
{
	v->TargetValue    = 0; 	
	v->RampDelayCount = 0; 	
	v->SetpointValue  = 0;				 
	v->EqualFlag      = 0;		
}

void rmp_cntl_calc(RMPCNTL *v)
{	
	if (v->TargetValue != v->SetpointValue)
	{
		v->RampDelayCount += 1.0;
		if (v->RampDelayCount >= v->RampDelayMax)
		{
			if (v->TargetValue >= v->SetpointValue)
			{
				v->SetpointValue += 0.00025;          
				if (v->SetpointValue > v->RampHighLimit)
					v->SetpointValue = v->RampHighLimit;           
				v->RampDelayCount = 0;
			}
			else
			{
				v->SetpointValue -= 0.00025;          
				if (v->SetpointValue < v->RampLowLimit)
					v->SetpointValue = v->RampLowLimit;           
				v->RampDelayCount = 0;
			}
		}
	}
	else  v->EqualFlag = 1; 
}
/***************************************************************************************/
/*线电压转相电压*/
void calcLineToPhase(LINE_TO_PHASE *v)
{
	v->Ua = 0.3333333 * (v->Uab - v->Uca);
	v->Ub = 0.3333333 * (v->Ubc - v->Uab);
	v->Uc = 0.3333333 * (v->Uca - v->Ubc);
}
/***************************************************************************************/
/*ABC 线电压 转换为 DQ*/
void calc_RST_DQ(RST_TO_DQ *v)
{
	float Sine, Cose, Sine_60, Cose_60, theta_60;

	theta_60 = v->theta - 1.0472;

	if (theta_60 < 0.0)
	{
		theta_60 += 6.283185;
	}

	Sine = sin(v->theta);
	Cose = cos(v->theta);
	Sine_60 = sin(theta_60);
	Cose_60 = cos(theta_60);


	v->Ds = 0.6666667 * (Cose * v->Uab + Cose_60 * v->Ubc);
	v->Qs = 0.6666667 * (-Sine * v->Uab - Sine_60 * v->Ubc);
}
/***************************************************************************************/
/*ABC 电压 转换为 DQ*/
void calc_ABC_DQ(ABC_TO_DQ *v)
{	
	v->Ds = 0.6666667 * (v->Cose * v->Ua + v->Cose_120 * v->Ub + v->Cose_240 * v->Uc);
	v->Qs = 0.6666667 * (-v->Sine * v->Ua - v->Sine_120 * v->Ub - v->Sine_240 * v->Uc);
}
/***************************************************************************************/
void calc2ndLPF(LPF2nd *v)
{
	v->outputOld2 = v->outputOld;
	v->outputOld = v->out;	

	v->out = -v->a1 * v->outputOld - v->a2 * v->outputOld2 + v->b0 * v->input + v->b1 * v->inputOld + v->b2 * v->inputOld2;

	v->inputOld2 = v->inputOld;
	v->inputOld = v->input;
}

void restart2ndLPF(LPF2nd *v)
{
	v->out    = 0.0;
	v->outputOld = 0.0;
	v->outputOld2 = 0.0;
	v->inputOld = 0.0;
	v->inputOld2 = 0.0;
}

void calcAvrLpf(AVR_LPF *v)
{
	v->output = (v->input + v->inputOld) * 0.5;

	v->inputOld = v->input;
}

void restartAvrLpf(AVR_LPF *v)
{
	v->input = 0;
	v->inputOld = 0;
	v->output = 0;
}
/***************************************************************************************/

/////////新添加闭环函数，满足电机闭环控制要求//////////////////
float AngleLimit(float angle)
{
	float temp;
	temp=angle;
	while(temp<0) temp=temp+2*PI;
	while(temp>2*PI) temp=temp-2*PI;
	return (temp);
}
/********************滤波函数***********/
void filter_function(struct filter *filter)
{ 
	float temp=0;
	temp=-1*2*PI*filter->fc*filter->Ts;
	temp=1+temp+0.5*temp*temp;
	filter->filter_out=(1.0-temp)*filter->filter_input+temp*filter->filter_old;
	filter->filter_old=filter->filter_out;
}



/***************************************************************************************/
void Pid(struct PIDREG *Pid)
{ 
	float uitem=0;
	Pid->err_reg=Pid->ref_reg-Pid->fdb_reg; 
	Pid->up_reg=Pid->Kp*Pid->err_reg;     
	Pid->up_reg1=Pid->up_reg;    
	/*   caculate the integral regulator;*/
	/*   Save ui(k) as ui(k+1) for next pid;  */     
	uitem=Pid->Kc*(Pid->out_reg-Pid->uprsat_reg);
	Pid->ui_reg=Pid->ui_reg1+Pid->Ki*Pid->err_reg+uitem;
	if(Pid->ui_reg<=Pid->out_min)
	{
		Pid->ui_reg=Pid->out_min;
	}
	if(Pid->ui_reg>=Pid->out_max)
	{
		Pid->ui_reg=Pid->out_max;
	}
	Pid->ui_reg1=Pid->ui_reg;
	/*  caculate the differential regulator;  */
	Pid->ud_reg=Pid->Kd*(Pid->up_reg-Pid->up_reg1);    
	Pid->uprsat_reg=Pid->up_reg+Pid->ui_reg+Pid->ud_reg;
	/*  output saturation limited;  */
	if (Pid->uprsat_reg>=Pid->out_max)
		Pid->out_reg=Pid->out_max;
	else 
	{ if(Pid->uprsat_reg<=Pid->out_min)
	Pid->out_reg=Pid->out_min;
	else
		Pid->out_reg=Pid->uprsat_reg;
	}   

}
/***************************************************************************************/
void Clarke(struct Trans_para *clarke)
{
	float temp;  
	clarke->alpha=0.66666667*clarke->a-0.3333333*clarke->b-0.3333333*clarke->c;
	temp=clarke->b-clarke->c;
	clarke->beta=temp*SQRT3_3;  
}
/***************************************************************************************/
void IClarke(struct Trans_para *iclarke)
{
	iclarke->a=iclarke->alpha;
	iclarke->b=-0.5*iclarke->a+SQRT3_2*iclarke->beta;
	iclarke->c=-iclarke->a-iclarke->b;
}
/***************************************************************************************/
void Park(struct Trans_para *park,float park_gamma)
{   
	float temp;
	temp=park_gamma;    
	park->d=park->alpha*cos(temp) +park->beta*sin(temp);                                        
	park->q=-park->alpha*sin(temp)+park->beta*cos(temp); 


}
/***************************************************************************************/
void IPark(struct Trans_para *ipark,float ipark_gamma)
{
	float temp;
	temp=ipark_gamma;
	ipark->alpha=ipark->d * cos(temp)-ipark->q * sin(temp);
	ipark->beta=ipark->d * sin(temp)+ipark->q * cos(temp);
}

void VA(struct Trans_para *va)
{
	va->m=sqrt(va->d*va->d+va->q*va->q);
	if(va->m != 0) va->delta=acos(va->d/va->m);
	if(va->q<0) va->delta=-va->delta;
}

/***************************************************************************************/
void Pid_Restart(struct PIDREG *v)
{
	v->ref_reg = 0;     
	v->fdb_reg = 0;      
	v->out_max = 2000;      
	v->out_min = -2000;      
	v->out_reg = 0;     
	v->Kp = 20;               
	v->Ki = 1;               
	v->Kd = 0;               
	v-> Kc = 0;               
	v->err_reg = 0;          
	v->uprsat_reg = 0;      
	v->up_reg = 0;           
	v->up_reg1 = 0;         
	v->ui_reg = 0;          
	v->ui_reg1 = 0;          
	v->ud_reg = 0;          
};
/***************************************************************************************/
 
void calc_ABC_DQ2(float a,float b,float c,float theta,float * d,float * q)
{	
  *d = 0.6666667 * ( cos(theta) *  a +  cos(theta-PI*2/3) * b +  cos(theta+PI*2/3) * c);
  *q = 0.6666667 * (-sin(theta) * a -  sin(theta-PI*2/3) * b -  sin(theta-PI*2/3) * c);

};

void Cal_ABC_DQ(ABC2DQ *v)
{	
	v->D = 0.6666667 * ( cos(v->Theta) * v->A +  cos(v->Theta-PI*2/3) * v->B +  cos(v->Theta+PI*2/3) * v->C);
    v->Q = 0.6666667 * (-sin(v->Theta) * v->A -  sin(v->Theta-PI*2/3) * v->B -  sin(v->Theta+PI*2/3) * v->C);

};
 
