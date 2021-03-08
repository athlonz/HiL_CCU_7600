/*  
* target:close control function here   
* Copyright 2018,4,16 IEE, Inc. 
* description:  the goal is to use rectifier and invertor control funtion 
* function: 
* debug: 
* modification history:20180426 by wxx
*/
/********************************************************************************/
#include "vxWorks.h" 
#include "variable_main.h"
#include "GlobalVar_Rec.h"
#include "GlobalVar_Inv.h"
#include "slaveBoardRW.h"
#include "pwmControl.h"
#include "subMod_DataDisplayUDP.h"
#include "subMod_ModbusTcp.h"
#include "subMod_Rfm2g.h" 
#include "closeControl.h"
#include "ast7600Srio.h" 
#include "subMod_ControlFunction.h"
#include "ADRC.h"
/********************************************************************************/
#define Idmax 6000//30//400   30 for test  
#define Idmin -6000//-30//-400  
#define vdmax 6500
#define vdmin -6500
#define vqmax 6500
#define vqmin -6500

#define ADRC_CONTROLLER (1)

double Ki = 0.5;
double Kp = 5;
double err_usq = 0;
double sum_usq = 0;/* PI sum--initial */
double usq_out = 0;
double usq_feed = 0;
double Theta_Prim = 0;
double Theta_Prim_Sel=0;
double lim_usq =1000000; /* PLLtest //1000000; 20090722  //10 */
double Theta_SecdFwd = 0,DeltaPre = 0;
double Vdc1Mean = 1.0;
double Vdc2Mean = 1.0;
double verr = 0;
double Iderr = 0;
double Iqerr = 0;
double  vref = 70;

double  VKp = 100;/* 1.5; */  /* 0.04 -To- 1.5 */
double  VKi = 0.5;/*0.5;*/  /* 0 -To- 0.5 */

double vsum = 0.0;
double P_vsiCCU=0;
double Iqref = 0;
double vdsum = 0;
double vqsum = 0;

double Idq_Kp = 0.7;  /* 0.3 */
double Idq_Ki = 1.0;  /* 2 */


double MaxVdcDeviRec=5; /* 5 20. */
double Kp_uzRec = 0.03;
double Ki_uzRec = 0;

int REC_startflag=0; 

double Ud_Fwd;
short changestep;
short changestep_SST_Flag;

ABC2DQ ABC_Fb1;
ABC2DQ ABC_Fb2;

ABC2DQ ABC_Fb3;


double Iq_h, Iq_belta_01, Iq_belta_02, Iq_belta1, Iq_belta2, Iq_b, Iq_r;
double Id_h, Id_belta_01, Id_belta_02, Id_belta1, Id_belta2, Id_b, Id_r;
double Udc_h, Udc_belta_01, Udc_belta_02, Udc_belta1, Udc_belta2, Udc_b, Udc_r;

struct ADRCREG CurrentADRC_REC1_Id;
struct ADRCREG CurrentADRC_REC1_Iq;
struct ADRCREG CurrentADRC_REC2_Id;
struct ADRCREG CurrentADRC_REC2_Iq;
struct ADRCREG CurrentADRC_REC1_Udc;
/*********************************************************************/
void rec1_abc_dq(void)
{
    calc_ABC_DQ2(  recIa1,  recIb1,  recIc1,  recTheta1,  &recId1,&recIq1 ) ;  
}
/*********************************************************************/
void rec2_abc_dq(void)
{
    calc_ABC_DQ2(  recIa2,  recIb2,  recIc2,  recTheta2,  &recId2,&recIq2 ); 
 }
/*********************************************************************/
/*******************************逆变器VF比生成电压矢量*******************************************************************************************************/
/*Name		:VFcontrol_Inv
*Description：逆变VF比函数，生成频率、调制比、角度
*Input		:无
*Output		:无
*Return		:无
*History	:2018,by wxx
*/ 
void VFcontrol_Inv(void)
{
	 /*逆变频率在0.1hz和100hz之间来回升降，验证逆变PWM算法*/
   		if((vrefDirFlag == 0))  // 本地VF角度去掉注释
  		{
  			invfreq=invfreq+0.0005;
  			if(invfreq >150)
  				vrefDirFlag = 1;
  			PWMdata_INV1.vvvf1IncreFlag = 1;
  			PWMdata_INV2.vvvf1IncreFlag = 1;
  		}
  		else if((vrefDirFlag == 1))
  		{
  			invfreq=invfreq-0.0005;  
			if(invfreq <= 0.1)
				vrefDirFlag = 0;
			PWMdata_INV1.vvvf1IncreFlag = 0;
			PWMdata_INV2.vvvf1IncreFlag = 0;
  		} 
//   		invfreq=175;
//   		Kv_Inv=0.6;
/*   		Kv_Inv=0.95/95.0*fabs(invfreq);	 	
   		if(Kv_Inv<0.05) 
   			Kv_Inv=0.05;
   		if(Kv_Inv>0.95) 
			Kv_Inv=0.95;*/
   		Kv_Inv=0.95/300*fabs(invfreq);	
   		 	
   		   		if(Kv_Inv<0.01) 
   		   			Kv_Inv=0.01;
   		   		if(Kv_Inv>0.95) 
   					Kv_Inv=0.95;
   		
  	  Kv_Inv = 0.8;//可将逆变调制比固定
  	  invfreq=50.0;//可将逆变频率固定
  		DeltaAngle_Inv=1*PI*invfreq*TsSHE;  
  		VFGama_Inv = VFGama_Inv + DeltaAngle_Inv;
  		while(VFGama_Inv>=2*3.1415926) VFGama_Inv=VFGama_Inv-2*3.1415926;
  		while(VFGama_Inv<0) VFGama_Inv=VFGama_Inv+2*3.1415926;
  		
		PWMdata_INV1.vvvf1=invfreq;
		PWMdata_INV1.Kv=Kv_Inv;
		PWMdata_INV1.VFGama=VFGama_Inv;    
		
		PWMdata_INV2.vvvf1=invfreq; 
		PWMdata_INV2.Kv=Kv_Inv;
		PWMdata_INV2.VFGama=VFGama_Inv;
}  
 /*********************整流器VF方法生成电压矢量**************************************************************************************/
/*Name		:VFcontrol_Rec
*Description：整流VF比函数，频率50hz、调制比固定、角度变化
*Input		:无
*Output		:无
*Return		:无
*History	:2018,by wxx
*/ 
void VFcontrol_Rec(void)
{
	Kv_Rec = 0.8; //可将调制比固定
	VFGama_Rec = VFGama_Rec + 2.0*50.0*3.1415926*(TsSHE*0.5);
	while(VFGama_Rec>=2*3.1415926) VFGama_Rec=VFGama_Rec-2*3.1415926;
	while(VFGama_Rec<0) VFGama_Rec=VFGama_Rec+2*3.1415926;
	
	PWMdata_REC1.Kv = 0.8;
	PWMdata_REC2.Kv = 0.8;
	PWMdata_REC1.VFGama = VFGama_Rec;
	PWMdata_REC2.VFGama = VFGama_Rec;
}	 
/*************************整流器锁相环*************************************************************************************************************/
/*Name		:Grid_PLL
*Description：电网电压锁相，此处应简化程序，矢量变换、pi调节器应调用函数，以使程序简洁明了
*Input		:*PWMdata->ua,ub,uc
*Output		:PWMdata->GridFOC_Gama
*Return		:无
*History	:20150709,by cdd
*/ 
void Grid_PLL(struct PWMdataStruct *PWMdata)  
{
	ua = PWMdata->Ua;
	ub = PWMdata->Ub;
	uc = PWMdata->Uc; 
        

	Grid_u_alph = 0.666667 * (ua - 0.5 * (ub + uc));  
	Grid_u_beta = 0.577367 * (ub - uc);

	Grid_u_Ampli = Grid_u_alph * Grid_u_alph + Grid_u_beta * Grid_u_beta;  
	Grid_u_Ampli = sqrt(Grid_u_Ampli);
	GridU=Grid_u_Ampli;

	usd = Grid_u_alph * cos(PWMdata->Theta_SecdFwd) + Grid_u_beta * sin(PWMdata->Theta_SecdFwd);
	usq = Grid_u_beta * cos(PWMdata->Theta_SecdFwd) - Grid_u_alph * sin(PWMdata->Theta_SecdFwd);

/***********  PLL  ***********/
    err_usq = usq;
    PWMdata->sum_usq = PWMdata->sum_usq + PWMdata->KiPLL*err_usq*INTPRD/2;
    if( PWMdata->sum_usq > lim_usq)
        PWMdata->sum_usq = lim_usq;
    if(PWMdata->sum_usq < -lim_usq)
       PWMdata->sum_usq = -lim_usq;    
    usq_out = PWMdata->KpPLL*err_usq+PWMdata->sum_usq;        
    if(usq_out>lim_usq)
       usq_out=lim_usq;
    if(usq_out<-lim_usq)
       usq_out=-lim_usq;
       
    usq_feed=usq_out+100*PIVSR;/*100pi 电网电压频率*/

	DeltaPre = PWMdata->Delta;
	PWMdata->Delta=PWMdata->Delta+usq_feed*INTPRD/2;
	while(PWMdata->Delta>2*PIVSR) 
	{PWMdata->Delta = PWMdata->Delta - 2*PIVSR;}
	while(PWMdata->Delta<0) 
	{PWMdata->Delta = PWMdata->Delta + 2*PIVSR;}  /*limitor   0-2*PIVSR */

	Theta_Secd=PWMdata->Delta;          
	PWMdata->Theta_SecdFwd = 2*Theta_Secd - DeltaPre ;  /*预测补偿延时*/

	/* GridFOC_Gama = Theta_Secd + FilterDelay; */ 
	PWMdata->GridFOC_Gama = Theta_Secd;  
	/*limitor   0-2*PIVSR*/
	while(PWMdata->GridFOC_Gama>2*PIVSR) 
	{PWMdata->GridFOC_Gama = PWMdata->GridFOC_Gama - 2*PIVSR;}
	while(PWMdata->GridFOC_Gama<0) 
	{PWMdata->GridFOC_Gama = PWMdata->GridFOC_Gama + 2*PIVSR;}

	GridFOC_GamaTransf = PWMdata->GridFOC_Gama ;
	while(GridFOC_GamaTransf>2*PIVSR) 
	{GridFOC_GamaTransf = GridFOC_GamaTransf - 2*PIVSR;}
	while(GridFOC_GamaTransf<0) 
	{GridFOC_GamaTransf = GridFOC_GamaTransf + 2*PIVSR;}

	GridFOC_GamaPhaseShift = PWMdata->GridFOC_Gama;		/* For FeedForward*/
	while(GridFOC_GamaPhaseShift>2*PIVSR) 
	{GridFOC_GamaPhaseShift = GridFOC_GamaPhaseShift - 2*PIVSR;}
	while(GridFOC_GamaPhaseShift<0) 
	{GridFOC_GamaPhaseShift = GridFOC_GamaPhaseShift + 2*PIVSR;}
}

/*********************************逆变角度插值*****************************************************************************************************/
/*Name		:GamaInterpolation
*Description：逆变电角度预测插值，因为MCU下传到CPS数据是1200hz，而CPS是7200HZ定时，中间的周期的电角度需要预测插值
*Input		:*PWMdata->ua,ub,uc
*Output		:PWMdata->GridFOC_Gama
*Return		:无
*History	:20150709,by cdd
*/

void GamaInterpolation(struct PWMdataStruct *PWMdata,unsigned short index)
{
	if(index!=0 && index!=6)  // 修改成中断类型
	{
		DeltaAngle_Inv=1*PI*PWMdata->vvvf1Interpolation*TsSHE;
		PWMdata->VFGama = PWMdata->VFGama + DeltaAngle_Inv;
		
		while(PWMdata->VFGama>2*PI) PWMdata->VFGama=PWMdata->VFGama-2*PI;
		while(PWMdata->VFGama<0) PWMdata->VFGama=PWMdata->VFGama+2*PI;
		  
		InterpolationCnt++;
	}	
	GamaInterDelta = PWMdata->VFGama - VFGamaPre;
	VFGamaPre = PWMdata->VFGama;
}
/******************************定位程序************************************/
/* name:define_position()
 * descript:定位程序
 * OUTPUT: invfreq,Kv_Inv
 */
void define_position()
{
	if (dccmd<=7200*8) //8s时间定位
	{runcmd=3;  //runcmd=3，整流 逆变都启动
	enable_inv1PWM();	 //定位时允许逆变输出 
	enable_inv2PWM();
	syn_control_flag=0;//直流电，角度为0，代表与A相重合
	invfreq=0;//相当于通直流电
    Kv_Inv=12.0/UdcSet;//0.14;//0.08;//0.03;//0.05;//12/Udc1_rec2_filter;//0.07;//0.05;//0.08;//0.05;//0.3;//0.1;//0.25;
	if(Kv_Inv>0.15)
		Kv_Inv=0.15;
	
    dccmd=(dccmd+1);
	}
	else 
	{
	dccmd=(7200*8+10);
	runcmd=1;//runcmd=1 整流正常，逆变封脉冲
	disable_inv1PWM();	//定位结束封闭逆变侧脉冲  
	disable_inv2PWM();
	Kv_Inv=0;
	syn_control_flag=2;//令角度为uref_delta1
	}
}
/**********************共母线下变流器1 YY的控制********************/
/* name:Grid_FOC_HighPower_REC1
 * descript:共母线下对整流1的双闭环控制
 * OUTPUT: PWMdata->Kv/VFGama/
 */
void Grid_FOC_HighPower_REC1(struct PWMdataStruct *PWMdata)  /*双闭环控制系统，1200Hz*/
{
    VdcRec = PWMdata->Udc_down_filter + PWMdata->Udc_up_filter;  
	if (REC_startflag == 0)
	{
		Udc_ref = VdcRec+200;
		REC_startflag = 1;
	}

		if( PWMdata->vref < UdcSet )
			{
				Udc_ref = Udc_ref + vref_Up_Rate * 0.5 *  TsSVM * 2;
				PWMdata->vref = Udc_ref;
		    }
		else
			{PWMdata->vref=UdcSet;}

    ia = PWMdata->Ia;
	ib = PWMdata->Ib;
	ic = PWMdata->Ic;
	
	Is_alpha=0.666667*(ia-0.5*ib-0.5*ic);
	Is_belta=0.577350*(ib-ic);

	Isd=cos(PWMdata->GridFOC_Gama)*Is_alpha+sin(PWMdata->GridFOC_Gama)*Is_belta;
	Isq=-sin(PWMdata->GridFOC_Gama)*Is_alpha+cos(PWMdata->GridFOC_Gama)*Is_belta;
	
	Isd_filter=2*PI*10*(Isd-Isd_filter)*0.000138+Isd_filter;
	Isq_filter=2*PI*10*(Isq-Isq_filter)*0.000138+Isq_filter;

	Us_alpha=0.666667*(PWMdata->Ua-0.5*PWMdata->Ub-0.5*PWMdata->Uc);
	Us_belta=0.577350*(PWMdata->Ub-PWMdata->Uc);

	usdFw = (Us_alpha * cos(PWMdata->GridFOC_Gama) + Us_belta * sin(PWMdata->GridFOC_Gama)) * 1;
	usqFw = (Us_belta * cos(PWMdata->GridFOC_Gama) - Us_alpha * sin(PWMdata->GridFOC_Gama)) * 1;

	/*------电压环--------  */
#if ADRC_CONTROLLER == 1
	
	CurrentADRC_REC1_Udc.v=PWMdata->vref;
	CurrentADRC_REC1_Udc.y=VdcRec;
	CurrentADRC_REC1_Udc.b=Udc_b;
	CurrentADRC_REC1_Udc.h=0.000138888;
	CurrentADRC_REC1_Udc.r=Udc_r;
	CurrentADRC_REC1_Udc.beta_01=Udc_belta_01;
	CurrentADRC_REC1_Udc.beta_02=Udc_belta_02;
	CurrentADRC_REC1_Udc.beta1=Udc_belta1;
    ADRC(&CurrentADRC_REC1_Udc);
    Idref_udc=CurrentADRC_REC1_Udc.u+Ud_Fwd/usdFw;
    
#else
	verr=PWMdata->vref-VdcRec;/*test*/

	PWMdata->vsum=PWMdata->vsum + PWMdata->VKi*verr*TsCPU;
	if(PWMdata->vsum>Idmax) /*限幅*/
	   { PWMdata->vsum=Idmax;}
	if(PWMdata->vsum<Idmin)  /*brake force current*/
	   { PWMdata->vsum=Idmin;}
	Idref_udc = PWMdata->vsum + PWMdata->VKp*verr+Ud_Fwd/usdFw;		
#endif
	if(Idref_udc>Idmax) /*限幅*/
	   {Idref_udc=Idmax;}
	if(Idref_udc<Idmin)  /*brake force current*/
	   {Idref_udc=Idmin;}
	

	PWMdata->Idref=(Idref_udc+Id_pf)/2;  /*20100604 UPDATE*/
	Idref=PWMdata->Idref;
	
//	PWMdata->Idref=500;
	
	if(PWMdata->Idref>Idmax) /*限幅*/
	   {PWMdata->Idref=Idmax;}
	if(PWMdata->Idref<Idmin)  /*brake force current*/
	   {PWMdata->Idref=Idmin;}
	
    /***将电压环得到的idref分一半给整流2****/
	PWMdata_REC2.Idref = PWMdata->Idref;
	if(PWMdata_REC2.Idref>Idmax) /*限幅*/
	   {PWMdata_REC2.Idref=Idmax;}
	if(PWMdata_REC2.Idref<Idmin)  /*brake force current*/
	   {PWMdata_REC2.Idref=Idmin;}
	
/*----  电流环  ------*/
	#if ADRC_CONTROLLER == 1
	
	CurrentADRC_REC1_Id.v=PWMdata->Idref;
	CurrentADRC_REC1_Id.y=Isd_filter;
	CurrentADRC_REC1_Id.b=Id_b;
	CurrentADRC_REC1_Id.h=0.00013888;
	CurrentADRC_REC1_Id.r=Id_r;
	CurrentADRC_REC1_Id.beta_01=Id_belta_01;
	CurrentADRC_REC1_Id.beta_02=Id_belta_02;
	CurrentADRC_REC1_Id.beta1=Id_belta1;
    ADRC(&CurrentADRC_REC1_Id);
    vd=usdFw-CurrentADRC_REC1_Id.u;
    
	if(vd>vdmax) 
	 {vd=vdmax;}
	if(vd<vdmin)  /*brake force current*/
	 {vd=vdmin;}
	 
	CurrentADRC_REC1_Iq.v=PWMdata->Iqref;
	CurrentADRC_REC1_Iq.y=Isq_filter;
	CurrentADRC_REC1_Iq.b=Iq_b;
	CurrentADRC_REC1_Iq.h=0.00013888;
	CurrentADRC_REC1_Iq.r=Iq_r;
	CurrentADRC_REC1_Iq.beta_01=Iq_belta_01;
	CurrentADRC_REC1_Iq.beta_02=Iq_belta_02;
	CurrentADRC_REC1_Iq.beta1=Iq_belta1;
    ADRC(&CurrentADRC_REC1_Iq);
    vq=usqFw-CurrentADRC_REC1_Iq.u;
	if(vq>vqmax) /*限幅*/
	{vq=vqmax;}
	if(vq<vqmin) /*brake force current*/
	{vq=vqmin;}
	

	
	#else
	Iderr=PWMdata->Idref-Isd_filter;//_filter;/*Isdmean;//Isd;*/
	PWMdata->vdsum=PWMdata->vdsum + PWMdata->Idq_Ki*Iderr*TsCPU;
	if(PWMdata->vdsum>vdmax) /*限幅*/
	 {PWMdata->vdsum=vdmax;}
	if(PWMdata->vdsum<vdmin)  /*brake force current*/
	 {PWMdata->vdsum=vdmin;}
	/* vd=-(vdsum + Idq_Kp*Iderr - u_deAv);  *//* 20150810 */
	vdctrl=-(PWMdata->vdsum + PWMdata->Idq_Kp*Iderr);
	vd = vdctrl + usdFw ;//+ 0.222*Isq_filter;  /*20150811*/
	//vd_filter=2*PI*10*(vd-vd_filter)*0.000138+vd_filter; //20190307
	if(vd>vdmax) 
	 {vd=vdmax;}
	if(vd<vdmin)  /*brake force current*/
	 {vd=vdmin;}

	Iqerr= PWMdata->Iqref-Isq_filter;//_filter;/*Isqmean;//Isq;*/
	PWMdata->vqsum=PWMdata->vqsum + PWMdata->Idq_Ki*Iqerr*TsCPU; 
	if(PWMdata->vqsum>vqmax) /*限幅*/
	{PWMdata->vqsum=vqmax;}		 
	if(PWMdata->vqsum<vqmin) /*brake force current*/
	{PWMdata->vqsum=vqmin;}
	vqctrl=-(PWMdata->vqsum + PWMdata->Idq_Kp*Iqerr);//-(PWMdata->vqsum + PWMdata->Idq_Kp*Iqerr); 
	vq = vqctrl + usqFw ;//- 0.222*Isd_filter;   /*20150811*/
	//vq_filter=2*PI*10*(vq-vq_filter)*0.000138+vq_filter; //20190307
	if(vq>vqmax) /*限幅*/
	{vq=vqmax;}
	if(vq<vqmin) /*brake force current*/
	{vq=vqmin;}
	#endif
	
/*	GridFOC_FeedUGama=GridFOC_GamaTransf;										 
	while (GridFOC_FeedUGama>2*PIVSR)
	   {GridFOC_FeedUGama=GridFOC_FeedUGama-2*PIVSR;}
	while (GridFOC_FeedUGama<0)
	   {GridFOC_FeedUGama=GridFOC_FeedUGama+2*PIVSR;}
*/
	GridFOC_FeedUGama = PWMdata->GridFOC_Gama;	
	u_alph_Rect = cos(GridFOC_FeedUGama)*vd-sin(GridFOC_FeedUGama)*vq;	 /* feedForward 20150810 */
	u_beta_Rect = sin(GridFOC_FeedUGama)*vd+cos(GridFOC_FeedUGama)*vq;

	UsRefRec = sqrt(u_alph_Rect * u_alph_Rect + u_beta_Rect * u_beta_Rect);
	Gama_SVMRec = atan2(u_beta_Rect,u_alph_Rect);
	
	//if(Gama_SVMRec<0) /*limitor   0-2*PIVSR */
//	{Gama_SVMRec = Gama_SVMRec + 2*PIVSR;} 

	/********************** Calculate Modulation Index Kv *********************/
	 VdcRec = PWMdata->Udc_down_filter + PWMdata->Udc_up_filter;  
	
	/* 单独测试电流内环 
	VdcRec = 80;  */

	 if(VdcRec < 1.) VdcRec = 1.; /*VdcRec min = 1.*/
	//PWMdata->Kv=UsRefRec*1.732/5000;   /*Original Good*/
	 PWMdata->Kv=UsRefRec*1.732/5000;
	 if(PWMdata->Kv>=1) {PWMdata->Kv=1;}/*0.8;//1, 20091111*/
	 else if(PWMdata->Kv<0)
	 {PWMdata->Kv=0;}

	//UsRefRec = VdcRec * PWMdata->Kv * 0.57737;
	
	PWMdata->VFGama =Gama_SVMRec+100*PIVSR*TsSVM/8;//PWMdata->GridFOC_Gama;//-PIVSR/6;//+PIVSR; //Gama_SVMRec

    while(PWMdata->VFGama>2*PIVSR) 
	{PWMdata->VFGama = PWMdata->VFGama - 2*PIVSR;}
	while(PWMdata->VFGama<0) 
	{PWMdata->VFGama = PWMdata->VFGama + 2*PIVSR;}

	Ntypek1=NtypeRec;
	Ptypek1=PtypeRec;
	/***********************    For VBC    ************************/
	VdcDevi2deltT(PWMdata); 
}
/**********************共母线下变流器2 YD的控制************************************/
/* name:Grid_FOC_HighPower_REC2
 * descript:共母线下对整流2的双闭环控制
 * OUTPUT: PWMdata->Kv/VFGama/
 */
void Grid_FOC_HighPower_REC2(struct PWMdataStruct *PWMdata)  /*双闭环控制系统，1200Hz*/
{   
	double Gama;
	Gama = PWMdata->GridFOC_Gama+PIVSR/6;   //Y-D11  连接方式，需要加pi/6。  Y-D1  连接方式，需要减pi/6。

	while(Gama>2*PIVSR) 
	{Gama = Gama - 2*PIVSR;}
	while(Gama<0) 
	{Gama = Gama + 2*PIVSR;}

    ia = PWMdata->Ia;
	ib = PWMdata->Ib;
	ic = PWMdata->Ic;
	
	Is_alpha=0.666667*(ia-0.5*ib-0.5*ic);
	Is_belta=0.577350*(ib-ic);

	Isd2=cos(Gama)*Is_alpha+sin(Gama)*Is_belta;
	Isq2=-sin(Gama)*Is_alpha+cos(Gama)*Is_belta;
	
	Isd_filter2=2*PI*10*(Isd2-Isd_filter2)*0.000138+Isd_filter2;
	Isq_filter2=2*PI*10*(Isq2-Isq_filter2)*0.000138+Isq_filter2;

	Us_alpha=0.666667*(PWMdata->Ua-0.5*PWMdata->Ub-0.5*PWMdata->Uc);
	Us_belta=0.577350*(PWMdata->Ub-PWMdata->Uc);

	usdFw = (Us_alpha * cos(Gama) + Us_belta * sin(Gama)) * 1;
	usqFw = (Us_belta * cos(Gama) - Us_alpha * sin(Gama)) * 1;

/*单独测试整流2 的电流环  20180410**/
	
/*	PWMdata->Idref=680.331/2;

	if(PWMdata->Idref>Idmax) 
	   {PWMdata->Idref=Idmax;}
	if(PWMdata->Idref<Idmin)  //brake force current
	   {PWMdata->Idref=Idmin;}  */

/*----  电流环  ------*/
//	PWMdata->Idref = 500;
	
#if ADRC_CONTROLLER == 1

CurrentADRC_REC2_Id.v=PWMdata->Idref;
CurrentADRC_REC2_Id.y=Isd_filter2;
CurrentADRC_REC2_Id.b=Id_b;
CurrentADRC_REC2_Id.h=0.00013888;
CurrentADRC_REC2_Id.r=Id_r;
CurrentADRC_REC2_Id.beta_01=Id_belta_01;
CurrentADRC_REC2_Id.beta_02=Id_belta_02;
CurrentADRC_REC2_Id.beta1=Id_belta1;
ADRC(&CurrentADRC_REC2_Id);
vd=usdFw-CurrentADRC_REC2_Id.u;

if(vd>vdmax) 
 {vd=vdmax;}
if(vd<vdmin)  /*brake force current*/
 {vd=vdmin;}

CurrentADRC_REC2_Iq.v=PWMdata->Iqref;
CurrentADRC_REC2_Iq.y=Isq_filter2;
CurrentADRC_REC2_Iq.b=Iq_b;
CurrentADRC_REC2_Iq.h=0.00013888;
CurrentADRC_REC2_Iq.r=Iq_r;
CurrentADRC_REC2_Iq.beta_01=Iq_belta_01;
CurrentADRC_REC2_Iq.beta_02=Iq_belta_02;
CurrentADRC_REC2_Iq.beta1=Iq_belta1;
ADRC(&CurrentADRC_REC2_Iq);
vq=usqFw-CurrentADRC_REC2_Iq.u;
if(vq>vqmax) /*限幅*/
{vq=vqmax;}
if(vq<vqmin) /*brake force current*/
{vq=vqmin;}
#else
	Iderr=PWMdata->Idref-Isd_filter2;//_filter;/*Isdmean;//Isd;*/
	PWMdata->vdsum=PWMdata->vdsum + PWMdata->Idq_Ki*Iderr*TsCPU;
	if(PWMdata->vdsum>vdmax) /*限幅*/
	 {PWMdata->vdsum=vdmax;}
	if(PWMdata->vdsum<vdmin)  /*brake force current*/
	 {PWMdata->vdsum=vdmin;}
	/* vd=-(vdsum + Idq_Kp*Iderr - u_deAv);  *//* 20150810 */
	vdctrl=-(PWMdata->vdsum + PWMdata->Idq_Kp*Iderr);
	vd = vdctrl + usdFw ;//+ 0.222*Isq_filter2;  /*20150811*/
	//vd_filter=2*PI*10*(vd-vd_filter)*0.000138+vd_filter; //20190307
	if(vd>vdmax) 
	 {vd=vdmax;}
	if(vd<vdmin)  /*brake force current*/
	 {vd=vdmin;}

	Iqerr= PWMdata->Iqref-Isq_filter2;//_filter;/*Isqmean;//Isq;*/
	PWMdata->vqsum=PWMdata->vqsum + PWMdata->Idq_Ki*Iqerr*TsCPU; 
	if(PWMdata->vqsum>vqmax) /*限幅*/
	{PWMdata->vqsum=vqmax;}		 
	if(PWMdata->vqsum<vqmin) /*brake force current*/
	{PWMdata->vqsum=vqmin;}
	vqctrl=-(PWMdata->vqsum + PWMdata->Idq_Kp*Iqerr);//-(PWMdata->vqsum + PWMdata->Idq_Kp*Iqerr); 
	vq = vqctrl + usqFw ;//- 0.222*Isd_filter2;   /*20150811*/
	//vq_filter=2*PI*10*(vq-vq_filter)*0.000138+vq_filter; //20190307
	if(vq>vqmax) /*限幅*/
	{vq=vqmax;}
	if(vq<vqmin) /*brake force current*/
	{vq=vqmin;}
#endif
	u_alph_Rect = cos(Gama)*vd-sin(Gama)*vq;	 /* feedForward 20150810 */
	u_beta_Rect = sin(Gama)*vd+cos(Gama)*vq;

	UsRefRec = sqrt(u_alph_Rect * u_alph_Rect + u_beta_Rect * u_beta_Rect);
	Gama_SVMRec = atan2(u_beta_Rect,u_alph_Rect);


	/********************** Calculate Modulation Index Kv *********************/
	 VdcRec = PWMdata->Udc_down_filter + PWMdata->Udc_up_filter;  


	 if(VdcRec < 1.) VdcRec = 1.; /*VdcRec min = 1.*/
	 PWMdata->Kv=UsRefRec*1.732/5000;

	 if(PWMdata->Kv>=1) {PWMdata->Kv=1;}/*0.8;//1, 20091111*/
	 else if(PWMdata->Kv<0)
	 {PWMdata->Kv=0;}

	//UsRefRec = VdcRec * PWMdata->Kv * 0.57737;
	
	PWMdata->VFGama =Gama_SVMRec+100*PIVSR*TsSVM/8;//PWMdata->GridFOC_Gama;//-PIVSR/6;//+PIVSR; //Gama_SVMRec

    while(PWMdata->VFGama>2*PIVSR) 
	{PWMdata->VFGama = PWMdata->VFGama - 2*PIVSR;}
	while(PWMdata->VFGama<0) 
	{PWMdata->VFGama = PWMdata->VFGama + 2*PIVSR;}

	Ntypek1=NtypeRec;
	Ptypek1=PtypeRec;
	/***********************    For VBC    ************************/
	VdcDevi2deltT(PWMdata); 
}


/**********************整流侧中点电压平衡控制程序************************************/
/* name:VdcDevi2deltT
 * descript:整流侧中点电压平衡控制程序
 * OUTPUT: PWMdata->NeuVoltCtl_deltTRec
 */
void VdcDevi2deltT(struct PWMdataStruct *PWMdata)
{
	//根据电流方向和所处区间选择选择相应的冗余小矢量进行补偿，详细原理见笔记
    PWMdata->Vdc1MinuVdc2Rec = fabs(PWMdata->Udc_up_filter) - fabs(PWMdata->Udc_down_filter); 
	if (PWMdata->Ia>=0)
	   PWMdata->sign_iaRec=1;
	else
	   PWMdata->sign_iaRec=-1;

	if (PWMdata->Ib>=0)
	   PWMdata->sign_ibRec=1;
	else
	   PWMdata->sign_ibRec=-1;    

	if (PWMdata->Ic>=0)
	   PWMdata->sign_icRec=1;
	else
	   PWMdata->sign_icRec=-1;
     /**********   the regenerative voltage balance algorithms   **********/
    PWMdata->Gama_SVM_DeltTRec = PWMdata->VFGama;  /* Gama_SVM_DeltTRec is used for calc NeuVolt */
	
	    if (((PWMdata->Gama_SVM_DeltTRec>=11*PIVSR/6)&&(PWMdata->Gama_SVM_DeltTRec<=2*PIVSR))||((PWMdata->Gama_SVM_DeltTRec>=0)&&(PWMdata->Gama_SVM_DeltTRec<PIVSR/6)))
      { PWMdata->Vdc1MinuVdc2Rec=-PWMdata->Vdc1MinuVdc2Rec*PWMdata->sign_iaRec;
	  }   
    else if ((PWMdata->Gama_SVM_DeltTRec>=PIVSR/6)&&(PWMdata->Gama_SVM_DeltTRec<PIVSR/2))
      {PWMdata->Vdc1MinuVdc2Rec=PWMdata->Vdc1MinuVdc2Rec*PWMdata->sign_icRec;
	   }
    else if ((PWMdata->Gama_SVM_DeltTRec>=PIVSR/2)&&(PWMdata->Gama_SVM_DeltTRec<5*PIVSR/6))
      {PWMdata->Vdc1MinuVdc2Rec=-PWMdata->Vdc1MinuVdc2Rec*PWMdata->sign_ibRec;
	   }
    else if ((PWMdata->Gama_SVM_DeltTRec>=5*PIVSR/6)&&(PWMdata->Gama_SVM_DeltTRec<7*PIVSR/6))
      {PWMdata->Vdc1MinuVdc2Rec=PWMdata->Vdc1MinuVdc2Rec*PWMdata->sign_iaRec;
       }       
    else if ((PWMdata->Gama_SVM_DeltTRec>=7*PIVSR/6)&&(PWMdata->Gama_SVM_DeltTRec<9*PIVSR/6))
      {PWMdata->Vdc1MinuVdc2Rec=-PWMdata->Vdc1MinuVdc2Rec*PWMdata->sign_icRec;
	   Nostep=1;}
    else if ((PWMdata->Gama_SVM_DeltTRec>=9*PIVSR/6)&&(PWMdata->Gama_SVM_DeltTRec<11*PIVSR/6))
      {PWMdata->Vdc1MinuVdc2Rec=PWMdata->Vdc1MinuVdc2Rec*PWMdata->sign_ibRec;  
       }
	    
	   // PWMdata->Vdc1MinuVdc2Rec=-1*PWMdata->Vdc1MinuVdc2Rec;
    if( fabs(PWMdata->Vdc1MinuVdc2Rec) > (fabs(PWMdata->Udc_up_filter) +fabs(PWMdata->Udc_down_filter))*0.01)
	// if( fabs(PWMdata->Vdc1MinuVdc2Rec) > 5)
      {		
		//PWMdata->NeuVoltCtl_sum = PWMdata->NeuVoltCtl_sum + PWMdata->Ki_uzRec*PWMdata->Vdc1MinuVdc2Rec*TsSVM/2;
		  PWMdata->NeuVoltCtl_sum = PWMdata->NeuVoltCtl_sum + PWMdata->Ki_uzRec*PWMdata->Vdc1MinuVdc2Rec*TsSHE/2;
  	  	if (PWMdata->NeuVoltCtl_sum>lim_uz)
    	   PWMdata->NeuVoltCtl_sum=lim_uz;  	  
    	if (PWMdata->NeuVoltCtl_sum<-lim_uz)
      	   PWMdata->NeuVoltCtl_sum=-lim_uz;
        PWMdata->NeuVoltCtl_deltTRec = PWMdata->NeuVoltCtl_sum + PWMdata->Kp_uzRec*PWMdata->Vdc1MinuVdc2Rec;
  	  	if (PWMdata->NeuVoltCtl_deltTRec>lim_uz)
    	   PWMdata->NeuVoltCtl_deltTRec=lim_uz;  	  
    	if (PWMdata->NeuVoltCtl_deltTRec<-lim_uz)
      	   PWMdata->NeuVoltCtl_deltTRec=-lim_uz; 
     }
	else
		{
		PWMdata->NeuVoltCtl_deltTRec = 0;
		} 
}  
/**********************编码器测速程序************************************/
/* name:measure_speed()
 * descript:编码器测速程序
 * OUTPUT: theta_syn_filter ,theta_syn,speed,initial_phase
 */
void measure_speed()
{
//读取通过IO板传来的编码器数据
	if(selfCheck.bit.IOBoardOK==1) 
	{
	prd= *(short *)(IOLocalBaseAdrs+0xC2);////prd 分频后A信号两上升沿之间的高频脉冲个数/////
	prd1=*(short *)(IOLocalBaseAdrs+0xCC);////prd1 50MHz高频脉冲 A信号两上升沿之间的高频脉冲个数/////
	frecout= *(short *)(IOLocalBaseAdrs+0xC4);
	dir= *(short *)(IOLocalBaseAdrs+0xC6);
	phasedata=*(short *)(IOLocalBaseAdrs+0xC8);
	phasedata_z=*(short *)(IOLocalBaseAdrs+0x32);
	phasedata_z_initial=*(short *)(IOLocalBaseAdrs+0x36);
	phasedata_z_dir=*(short *)(IOLocalBaseAdrs+0x38);
	phasedata_z_initial_dir=*(short *)(IOLocalBaseAdrs+0x3A);
	phasedata_dir=*(short *)(IOLocalBaseAdrs+0x3C);
	z_flag    =*(short *)(IOLocalBaseAdrs+0x3E);
	}
  //由编码器脉冲数目计算出电机转速
 // T法速度计算公式：speed=60*f/(encoderLine*prd) 其中f为高频脉冲频率：f=1/(20ns*ioclockdivde) 
  if(prd==65535||prd==0)
	 speed1=0;
	else
	{   
		// speed1=60.0*50*1000*1000/encoderLine/prd/16;	
		speed1=60.0*50*1000*1000/encoderLine/prd/64;	
	 }
	 if(prd1==65535||prd1==0)
	 speed3=0;
	else
	{   
	   speed3=60.0*50*1000*1000/encoderLine/prd1;
	 }
// M法速度计算公式：speed=60*frecout/(encoderLine*倍频系数*tc)其中tc为30*0.138=4.166ms, 倍频系数为6
	 Timer_Counter++;
	if(Timer_Counter==29)  
	{
		m1=m2; 
		m2=frecout;
		Timer_Counter=0;
	}
	   m3=m2-m1;
	if (m3>=0)
	   speed2=60.0*m3/(encoderLine*0.004166*4);    
	else
	   speed2=60.0*(m3+65535)/(encoderLine*0.004166*4);
  //转速赋值
   if(speed1<500)
		speed=speed1;
   else speed=speed3;
  //转速方向判断
    if(dir==1)
   {
		speed=speed;
   }
	   else if(dir==0)
   {
	speed=(-1)*speed;
   }  
 //防止速度抖动
 //  if (firstspeed==0)
 //  {firstspeed=1;
  // if (speed >=10)
//	   speed_pre=speed;
 //  }
   if(speed_pre==0)
	   speed_pre=speed;
   if(abs(speed-speed_pre)>10)
	   speed=speed_pre;
   else
	   speed=speed;
       speed_pre=speed;

	//由编码器脉冲数目计算出电机角度theta_syn
	//借助z信号计算的电机实时角度phase_z_dir
	     phase_dir=phasedata_dir*2*PI/encoderLine/8*p;
	     phase_z_dir=phasedata_z_dir*2*PI/encoderLine/8*p;
		 phase_z_initial_dir=phasedata_z_initial_dir*2*PI/encoderLine/8*p;
	   while(phase_dir<0)      phase_dir=phase_dir+2*PI;
	   while(phase_dir>2*PI)   phase_dir=phase_dir-2*PI;
	   while(phase_z_dir<0)      phase_z_dir=phase_z_dir+2*PI;
	   while(phase_z_dir>2*PI)     phase_z_dir=phase_z_dir-2*PI;
	   while(phase_z_initial_dir<0)      phase_z_initial_dir=phase_z_initial_dir+2*PI;
	   while(phase_z_initial_dir>2*PI)     phase_z_initial_dir=phase_z_initial_dir-2*PI;
	   //定位初始角initial_phase
	   //定位程序启动，syn_control_flag=0，电机闭环控制，syn_control_flag=2，电机开环控制，syn_control_flag=1
	   if(syn_control_flag==0)
		   initial_phase=phase_z_dir;
	   else if((syn_control_flag==1)||(syn_control_flag==2))
		   initial_phase=initial_phase;
	   //电机调制角theta_syn
		theta_syn=phase_z_dir-initial_phase;
	  while(theta_syn<0)      theta_syn=theta_syn+2*PI;
	  while(theta_syn>2*PI)    theta_syn=theta_syn-2*PI;
	  //对调制角theta_syn滤波，否则SHE会出现丢波现象
	  if(abs(theta_syn_pre-theta_syn)>PI)
	  {
	  theta_syn_filter_old=theta_syn_filter_old-2*PI;	  
	  }
	   temp_theta=-1*2*PI*600.0*0.00013888;
	   temp_theta=1+temp_theta+temp_theta*temp_theta;
	   theta_syn_filter=(1.0-temp_theta)*theta_syn+temp_theta*theta_syn_filter_old;
	   theta_syn_filter_old=theta_syn_filter;
	   while(theta_syn_filter<0)      theta_syn_filter=theta_syn_filter+2*PI;
	   while(theta_syn_filter>2*PI)    theta_syn_filter=theta_syn_filter-2*PI;
	   theta_syn_pre=theta_syn;

	   //只在SHE时对角度滤波，非必需
	 // if((invfreq>=95)&&(theta_syn_SHE_flag==1))//theta_syn_filter自保持
	 // {
		//  theta_syn_SHE_flag=1;
		//  theta_syn_SHE=theta_syn_filter;
	 // }
	 // else  if((invfreq>=95)&&(theta_syn_SHE_flag==0))//theta_syn_filter切换
	 // {
		//  theta_syn_SHE_flag=1;
		//theta_syn_SHE=theta_syn_filter;
	 // }
	 // else  if((invfreq<95)&&(theta_syn_SHE_flag==0))//theta_syn自保持
	 // {
		//  theta_syn_SHE_flag=0;
		//theta_syn_SHE=theta_syn;
	 // }
	 // else  if((invfreq<95)&&(invfreq>=91)&&(theta_syn_SHE_flag==1))//theta_syn切到theta_syn_filter频率又下降
	 // {
		//  theta_syn_SHE_flag=1;
		//theta_syn_SHE=theta_syn_filter;
	 // }
	 //  else  if((invfreq<91)&&(theta_syn_SHE_flag==1))
	 // {
		//  theta_syn_SHE_flag=0;
		//theta_syn_SHE=theta_syn;
	 // }
}
/*****************************手动或自动（按时间/频率）切换串并联开关设置*************************/
/* name:AUTO_model
 * descript:手动或自动（按时间/频率）切换串并联开关设置
 * OUTPUT: Parallel_AUTO,Serial_Model
 */
void AUTO_model()
{
if (AUTO_model_LK==0)  //手动模式
{
	AUTO_Model=0;
	if(Parallel_LK==1)
	{
		IO_out_ANPC.bit.parallel=1;
		Parallel_Model=1;
		if (IO_out_ANPC.bit.start_inv==1)
		{
		Parallel_time=Parallel_time+0.0001388;
		if (Parallel_time>=(uptime+staytime))
			Parallel_time=(uptime+staytime);
		}
     } 
	if(Parallel_LK==0)
	{
		IO_out_ANPC.bit.parallel=0;
		Parallel_Model=0;
	}
	if(Serial_LK==1)
	{
		IO_out_ANPC.bit.serial=1;
		Serial_Model=1;
		if (IO_out_ANPC.bit.start_inv==1)
		{
		Parallel_time=Parallel_time-0.0001388;
		if (Parallel_time<=0)
			Parallel_time=0;
		}
	} 
	if(Serial_LK==0)
	{
		IO_out_ANPC.bit.serial=0;
		Serial_Model=0;
	}
}
else if (AUTO_model_LK==1)  //自动模式
{
	AUTO_Model=1;
	if(Parallel_AUTO==1)
	{
		IO_out_ANPC.bit.parallel=1;
		Parallel_Model=1;
		if (IO_out_ANPC.bit.start_inv==1)
		{
		Parallel_time=Parallel_time+0.0001388;
		if (Parallel_time>=(uptime+staytime))
			Parallel_time=(uptime+staytime);
		}
	} 
	if(Parallel_AUTO==0)
	{
		IO_out_ANPC.bit.parallel=0;
		Parallel_Model=0;
	}
	if(Serial_AUTO==1)
	{
		IO_out_ANPC.bit.serial=1;
		Serial_Model=1;
		if (IO_out_ANPC.bit.start_inv==1)
		{
		Parallel_time=Parallel_time-0.0001388;
		if (Parallel_time<=0)
			Parallel_time=0;
		}
	} 
	if(Serial_AUTO==0)
	{
		IO_out_ANPC.bit.serial=0;
		Serial_Model=0;
	}
}
time1freq0=0;//选择时间或者频率来切换串联模式和并联模式
if (time1freq0==1)  //按时间自动切串并联
{
 if ((AUTO_Model==1)&&(IO_out_ANPC.bit.start_inv==1))   
 {
	if (Parallel_AUTO==1)  
	   { testgz=1;
		  moderoute=1;
		  // Serial_AUTO_time=0;
		   Parallel_AUTO_time=Parallel_AUTO_time+0.000138888;
		   auto_time=auto_time+0.000138888;
		   if (Parallel_AUTO_time>=(uptime+staytime+downtime+transienttime))
		 {Parallel_AUTO=0;Parallel_AUTO_time=0;}
	   }
	   else if (Serial_AUTO==1)
	   {testgz=2; 
		   moderoute=2;
		   //Parallel_AUTO_time=0;
		   Serial_AUTO_time=Serial_AUTO_time+0.000138888;
		    auto_time=auto_time+0.000138888;
		   if (Serial_AUTO_time>=(uptime+staytime+downtime+transienttime))
		 {Serial_AUTO=0;Serial_AUTO_time=0;}
	   }
	 if ((auto_time>=(uptime+staytime+downtime+transienttime))&&(Serial_AUTO==0)&&(Parallel_AUTO==0))
	 {testgz=3;
		 auto_time=auto_time+0.000138888;
		 if ((auto_time>=(uptime+staytime+downtime+transienttime+closetime))&&(moderoute==1))
			{Serial_AUTO=1;auto_time=0;}
		 else if ((auto_time>=(uptime+staytime+downtime+transienttime+closetime))&&(moderoute==2))
			{Parallel_AUTO=1;auto_time=0;}
	 }
 }
}
else if (time1freq0==0) //按频率自动切串并联   
{
 if ((((invfreq<=30)&&(PWMdata_INV1.vvvf1IncreFlag==1)&&(PWMdata_INV2.vvvf1IncreFlag==1))&&(IO_out_ANPC.bit.start_inv==1))&&(switchflag==0))
 {
	Parallel_AUTO=1;
 }
 else if ((((invfreq>70)&&(invfreq<=300)&&(PWMdata_INV1.vvvf1IncreFlag==0)&&(PWMdata_INV2.vvvf1IncreFlag==0))&&(IO_out_ANPC.bit.start_inv==1))&&(switchflag==1)) 
 {
	 Serial_AUTO=1;
 }	
if ((AUTO_Model==1)&&(IO_out_ANPC.bit.start_inv==1))   
 {
	 if (Parallel_AUTO==1)  
	   { 
		   if ((invfreqpre<=30)&&(invfreq>30)&&(PWMdata_INV1.vvvf1IncreFlag==1)&&(PWMdata_INV2.vvvf1IncreFlag==1))//并联到串联切
			   {closeflag=1;moderoute=3;switchflag=1;}
	   }
	 else if (Serial_AUTO==1)
	   {
		 if ((invfreqpre>70)&&(invfreq<=70)&&(PWMdata_INV1.vvvf1IncreFlag==0)&&(PWMdata_INV2.vvvf1IncreFlag==0))//串联到并联切
			   {closeflag=1;moderoute=4;switchflag=0;}
	   }
	 if (closeflag==1)
	  {
	 close_time=close_time+0.000138888; 
	 if (close_time>=downtime+transienttime)
	 {
		 if (moderoute==3)
			 Parallel_AUTO=0;
		 else if (moderoute==4)
			 Serial_AUTO=0;
	 }
     if ((close_time>=freqclosetime)&&(moderoute==3)&&(Serial_AUTO==0)&&(Parallel_AUTO==0))
		 {Serial_AUTO=1;closeflag=0;close_time=0;}
	 else if ((close_time>=freqclosetime)&&(moderoute==4)&&(Serial_AUTO==0)&&(Parallel_AUTO==0))
		 {Parallel_AUTO=1;closeflag=0;close_time=0;}
	  }	 
 }
}
}


/********************************************开环测试发波，固定频率和调制比*****************************/
/* name:VF_test
 * descript:开环测试发波，固定频率和调制比
 * OUTPUT: Kv_Inv,invfreq
 */
void VF_test()
{
//syn_control_flag=1，标志开环控制  runcmd=3对应启动逆变
	syn_control_flag=1;
	runcmd=3;

	if (Kv_cnt<1)
	{
		Kv_cnt=Kv_cnt+0.0001388*0.2;
	}
	else
		Kv_cnt=1;	 
	Kv_Inv=Kv_Inv_from_LK*Kv_cnt;
	invfreq=invfreq_from_LK;

	//Kv_Inv=0.5;
	//invfreq=50;

	PWMdata_INV1.vvvf1IncreFlag = 1;
	PWMdata_INV2.vvvf1IncreFlag = 1;
}
/********************************************单台变流器电机闭环控制——电流环+不滤波的SHE角度*****************************/
/* name:currentloop_control()
 * descript:单台变流器电机闭环控制——电流环+不滤波的SHE角度
 * OUTPUT: uref_delta1,uref_delta2,inv_Kv1,inv_Kv2
 */
void currentloop_control()
{
//		syn_control_flag=2;

}


/********************************************ANPC模式切换*****************************/
/* ANPC_Inv()  ANPC_Rec()
 * descript:
 * ANPC模式切换
 * Inv_type_a=1；外管开关损耗大，
 * Inv_type_a=2；内管开关损耗大，
 * Inv_type_a=0；NPC模式，
 * INPUT: Inv_type_a，Inv_type_b，Inv_type_c
 */
void ANPC_Inv()
{
	Inv_type_cnt=Inv_type_cnt+0.0001388;
	if(Inv_type_cnt>=3)
		Inv_type_cnt=0;
	
	if(Inv_type_cnt<1)
		Inv_type_a=1;
	else if(Inv_type_cnt<2)
		Inv_type_a=2;
	else
		Inv_type_a=2;
	
	//Inv_type_a=0;
		
	txInvSrioData1.ANPC_type_a=Inv_type_a;
	txInvSrioData1.ANPC_type_b=Inv_type_a;
	txInvSrioData1.ANPC_type_c=Inv_type_a;
}

void ANPC_Rec()
{
	Rec_type_cnt=Rec_type_cnt+0.0001388;
	if(Rec_type_cnt>=3)
		Rec_type_cnt=0;
	
	if(Rec_type_cnt<1)
		Rec_type_a=1;
	else if(Rec_type_cnt<2)
		Rec_type_a=2;
	else
		Rec_type_a=0;
	
	Rec_type_a=0;
	txRecSrioData1.ANPC_type_a=Rec_type_a;
	txRecSrioData1.ANPC_type_b=Rec_type_a;
	txRecSrioData1.ANPC_type_c=Rec_type_a;
}

void Calculate_RMS()
{
	/*CurrentLoop_*/
		ABC_Fb1.A = invIu1;
		ABC_Fb1.B = invIv1;
		ABC_Fb1.C = invIw1;

		ABC_Fb1.Theta = PWMdata_INV1.VFGama;
		Cal_ABC_DQ(&ABC_Fb1);
		Id1=ABC_Fb1.D;
		Iq1=ABC_Fb1.Q;

		Id1_filter=2*PI*5*(Id1-Id1_filter)*0.000138+Id1_filter;
		Iq1_filter=2*PI*5*(Iq1-Iq1_filter)*0.000138+Iq1_filter;

		ABC_Fb2.A = invIu2;
		ABC_Fb2.B = invIv2;
		ABC_Fb2.C = invIw2;

		ABC_Fb2.Theta = PWMdata_INV2.VFGama;
		Cal_ABC_DQ(&ABC_Fb2);
		Id2=ABC_Fb2.D;
		Iq2=ABC_Fb2.Q;

		Id2_filter=2*PI*5*(Id2-Id2_filter)*0.000138+Id2_filter;
		Iq2_filter=2*PI*5*(Iq2-Iq2_filter)*0.000138+Iq2_filter;
		Iinv_rms=sqrt(Id1*Id1+Iq1*Iq1)/sqrt(2);
		Iinv_rms2=sqrt(Id2*Id2+Iq2*Iq2)/sqrt(2);
		
		Uinv_rms=(Udc1+Udc2)*PWMdata_INV1.Kv/sqrt(3);
		Uinv_rms2=(Udc1+Udc2)*PWMdata_INV2.Kv/sqrt(3);
		
		
		ABC_Fb3.A = GridIa;
		ABC_Fb3.B = GridIb;
		ABC_Fb3.C = GridIc;

		ABC_Fb3.Theta = GridFOC_GamaTransf;
		Cal_ABC_DQ(&ABC_Fb3);
		
		Grid_Irms=sqrt(ABC_Fb3.D*ABC_Fb3.D+ABC_Fb3.Q*ABC_Fb3.Q)/sqrt(2);
		
		
}
