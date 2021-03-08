/*  
* target:  invertor PWM core funtion  
* Copyright 2018,4,16 IEE, Inc. 
* description:   逆变PWM SVPWM算法核心程序
* author:yzg
* modification history: cdd 2015
*                       wxx 20180606
*/


#include "GlobalVar_Inv.h" 
#include "pwmControl.h" 
#include <math.h>
#include "variable_main.h"

/* INT frequency 1200Hz, counter frequency 25MHz, 
 * INPUT: VFGama, Kv, VFGama, Reg_UpDown
 * OUTPUT: CMPA[0-5], ZRO[0-5], PRD[0-5], CAU[0-5], CAD[0-5]
*/
/**************************************************************************************************************************************/
/*Name		:SVPWMH
*Description:高调制比SVPWM算法
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void SVPWMH(struct PWMdataStruct *PWMdata)
{        
	PWMdata->SVPWMType = 1; 
	if((PWMdata->ChangeSHENPJumpTest) == 1){
	PWMdata->ModeFlag = 0;}
	VVVF_Gama = PWMdata->VFGama;
	//if(syn_control_flag!=0) 
	VVVF_Gama = VVVF_Gama+2*pi*rxFre/1200./2+2*pi*rxFre/1200.;
	while(VVVF_Gama<0)      VVVF_Gama=VVVF_Gama+2*PI;
	while(VVVF_Gama>2*PI)    VVVF_Gama=VVVF_Gama-2*PI;
	Gama_SVM=VVVF_Gama;
	Gama_SVM_DeltT=VVVF_Gama; /* FOR VBC */

	Sector_Num=(int)(Gama_SVM/DEG60);
	if(Sector_Num==6)
	  {Sector_Num=0;}
	Gama_SVM -=Sector_Num*DEG60; 
        if(PWMdata->Kv>=1)         //geng gai cheng xu
        PWMdata->Kv=1;             //geng gai cheng xu
	ta0 = T1PRHalfSVM*( 1 - 2*PWMdata->Kv*sin(Gama_SVM) );		
	tb0 = T1PRHalfSVM*( 2*PWMdata->Kv*sin(DEG60 + Gama_SVM) - 1 );		
	tc0 = T1PRHalfSVM*( 1 - 2*PWMdata->Kv*sin(DEG60 - Gama_SVM) ); 	/* tc0 = T1PR - ta0 - tb0; */

	if ((ta0>=0)&& (tb0>=0)&& (tc0>=0))
	{
		ta=ta0;
		tb=tb0;
		tc=tc0;
        if (Gama_SVM<DEG30)
		   {Region_Num=3;}
		else
		   {Region_Num=2;}
	}
	if (ta0<0)
	{
		ta = - ta0;
		tc =T1PRHalfSVM - tb0;
		tb =T1PRHalfSVM - tc0;
		Region_Num=4;
	} 
	if (tb0<0)
	{   
		tb =  - tb0;
		ta = T1PRHalfSVM - tc0;
		tc = T1PRHalfSVM - ta0;
		
		if (Gama_SVM<DEG30)
		   {Region_Num=1;}
		else
		   {Region_Num=0;}
	}
	if (tc0<0)
	{   
		tc  = - tc0 ;
		ta  = T1PRHalfSVM - tb0;
		tb  = T1PRHalfSVM - ta0;
		Region_Num = 5;
	}

	VBC_test=0;/* for VBC_test  20100326 */

	NeuVoltCtl_deltT=0;
    /*Calc t1 t2 t3 for DSP CMPR*/
    /*  All N-type */
	if ((Sector_Num==1)||(Sector_Num==3)||(Sector_Num==5))/*  Sector=1,3,5 */
	{
		if ((Region_Num==1 )|| (Region_Num==3 )||(Region_Num==5)) /* angle less than 30deg          */
		{ 
		   t1 = ta * 0.5 * (1 - NeuVoltCtl_deltT);/* N-,time+ */
		   t2 = t1 + tb;
		   t3 = t2 + tc; 
		} 
		else    /* angle bigger than 30deg */
		{
		   t1 = tc * 0.5 * (1 - NeuVoltCtl_deltT);/* N-,time+ */
		   t2 = t1 + ta;
		   t3 = t2 + tb;
		}
	}
	else    /*  Sector_Num=0,2,4                                             */
	{    
		if ((Region_Num==1 )|| (Region_Num==3 )||(Region_Num==5)) /* angle less than 30deg          */
		{ 
			t1 = ta * 0.5 * (1 - NeuVoltCtl_deltT);/* N-,time+ */
			t2 = t1 + tc;
			t3 = t2 + tb; 
		} 
		else  /* angle bigger than 30deg */
		{                               
			t1 = tc * 0.5 * (1 - NeuVoltCtl_deltT);/* N-,time+ */
			t2 = t1 + tb;
			t3 = t2 + ta;
		}
	}
    /*Down of Nostep, Single Sample */
	if ((PWMdata->Reg_UpDown)==1)  /* 20160706 0->1 */
	{
		NPC_SVM_SetAse();
	}
	else
	{
		NPC_SVM_SetDes();
	}  
    /*Mininum Pulse for Device*/

	MP_test=1;/* 0 for without MP test */

/*     Minimum_Pulse_TLC();   // 注释20150113 */
        Minimum_Pulse_TLC(PWMdata);  /* 20150602 */ 
   /* MinPulseOperation(); */

    PWMdata->CMPA[0] = *(NPtimeTable[Sector_Num][Region_Num][0]);/*PWMA1*/
	PWMdata->CMPA[1] = *(NPtimeTable[Sector_Num][Region_Num][0]);/*PWMA2*/
	PWMdata->CMPA[2] = *(NPtimeTable[Sector_Num][Region_Num][1]);/*PWMB1*/
	PWMdata->CMPA[3] = *(NPtimeTable[Sector_Num][Region_Num][1]);/*PWMB2*/
	PWMdata->CMPA[4] = *(NPtimeTable[Sector_Num][Region_Num][2]);/*PWMC1*/
	PWMdata->CMPA[5] = *(NPtimeTable[Sector_Num][Region_Num][2]);/*PWMC2*/
	PWMdata->ACQ_ZRO=ZRO[0]|(ZRO[1]<<2)|(ZRO[2]<<4)|(ZRO[3]<<6)|(ZRO[4]<<8)|(ZRO[5]<<10)&0xfff;
	PWMdata->ACQ_CAU=CAU[0]|(CAU[1]<<2)|(CAU[2]<<4)|(CAU[3]<<6)|(CAU[4]<<8)|(CAU[5]<<10)&0xfff;
	PWMdata->ACQ_PRD=PRD[0]|(PRD[1]<<2)|(PRD[2]<<4)|(PRD[3]<<6)|(PRD[4]<<8)|(PRD[5]<<10)&0xfff;
	PWMdata->ACQ_CAD=CAD[0]|(CAD[1]<<2)|(CAD[2]<<4)|(CAD[3]<<6)|(CAD[4]<<8)|(CAD[5]<<10)&0xfff;
} /*  end of SVPWMH */

 
/**************************************************************************************************************************************/
/*Name		:SVPWML
*Description:低调制比SVPWM算法
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void SVPWML(struct PWMdataStruct *PWMdata) /* SVMINI  in1 VFGama，in2 Gama_SVM, in3 Kv */
{
	PWMdata->SVPWMType = 0;
	if((PWMdata->ChangeSHENPJumpTest) == 1){
	PWMdata->ModeFlag = 0;}

	if( ((PWMdata->Reg_UpDown) == ET_CTR_ZRO)&&((PWMdata->FirstEnPWMFlag)==1) )   /* IntFlag -> Reg_UpDown, 20160706 */
	{PWMdata->PWM4Cnt = 1;PWMdata->FirstEnPWMFlag = 0;}
	if((PWMdata->PWM4Cnt)>3)
		PWMdata->PWM4Cnt = 3;
	if((PWMdata->PWM4Cnt)<0)
		PWMdata->PWM4Cnt = 0;

	Gama_SVM = PWMdata->VFGama;  /* limitor   0-2*pi */
	//if(syn_control_flag!=0) 
	Gama_SVM = Gama_SVM+2*pi*rxFre/1200./2+2*pi*rxFre/1200.;
	while(Gama_SVM<0)      Gama_SVM=Gama_SVM+2*PI;
	while(Gama_SVM>2*PI)    Gama_SVM=Gama_SVM-2*PI;
	Gama_SVM_DeltT = Gama_SVM;  /* Gama_SVM_DeltT is used for calc NeuVolt */

	/* -------------------Give Kv(ma) Start---------------------- */
	/* if(Kv>0.3) Kv = 0.3;  */ 

	/* ----------------Give  Sector_Num 0:5-------------------- */
	Gama_SVM_Used = Gama_SVM;
	Sector_Num_Pre = Sector_Num;/* TB */
	Sector_Num = (int)(Gama_SVM_Used/DEG60) ;
	if(Sector_Num == 6)
	Sector_Num = 0;
	Gama_SVM_Used -=Sector_Num*DEG60; 

	/* ----------------------Calc ta0 tb0 tc0---------------------- */
	jp_ta = (PWMdata->T1PR)*2*(PWMdata->Kv)*sin(DEG60 - Gama_SVM_Used);
	jp_tc = PWMdata->T1PR*2*PWMdata->Kv*sin(Gama_SVM_Used);
	jp_tbtemp = PWMdata->T1PR*(1 - 2*PWMdata->Kv*sin(DEG60 + Gama_SVM_Used));     
	jp_tb = PWMdata->T1PR - jp_ta - jp_tc;

	if(Sector_Num%2==0)
	{
		t1 = 0.25*jp_tb;
		t2 = 0.25*jp_tb + 0.5*jp_ta;
		t3 = 0.25*jp_tb + 0.5*jp_ta + 0.5*jp_tc;

		t1C = 0.5*PWMdata->T1PR - t1;
		t2C = 0.5*PWMdata->T1PR - t2;
		t3C = 0.5*PWMdata->T1PR - t3;
	}		
	else
	{
		t1 = 0.25*jp_tb;
		t2 = 0.25*jp_tb + 0.5*jp_tc;
		t3 = 0.25*jp_tb + 0.5*jp_tc + 0.5*jp_ta;
		
		t1C = 0.5*PWMdata->T1PR - t1;
		t2C = 0.5*PWMdata->T1PR - t2;
		t3C = 0.5*PWMdata->T1PR - t3;			
	}					
    
	/*  Set PRD,ZRO,CAD,CAU */
	if(PWMdata->Reg_UpDown == ET_CTR_PRD )  /* IntFlag -> Reg_UpDown, 20160706 */
	{
		JpStart_NPC_SVM_SetAse(PWMdata);	/* 设置上升动作 */
		PWMdata->ACQ_ZRO=ZRO[0]|(ZRO[1]<<2)|(ZRO[2]<<4)|(ZRO[3]<<6)|(ZRO[4]<<8)|(ZRO[5]<<10)&0xfff;
		PWMdata->ACQ_CAU=CAU[0]|(CAU[1]<<2)|(CAU[2]<<4)|(CAU[3]<<6)|(CAU[4]<<8)|(CAU[5]<<10)&0xfff;
	}
	else
	{
		JpStart_NPC_SVM_SetDes(PWMdata);  /* 设置下降动作 */
		PWMdata->ACQ_PRD=PRD[0]|(PRD[1]<<2)|(PRD[2]<<4)|(PRD[3]<<6)|(PRD[4]<<8)|(PRD[5]<<10)&0xfff;
		PWMdata->ACQ_CAD=CAD[0]|(CAD[1]<<2)|(CAD[2]<<4)|(CAD[3]<<6)|(CAD[4]<<8)|(CAD[5]<<10)&0xfff;		
	}

   /* SET CMPR */
	if( 	(((PWMdata->Reg_UpDown) == ET_CTR_PRD)&&((PWMdata->PWM4Cnt)<2))||(((PWMdata->Reg_UpDown) == ET_CTR_ZRO)&&(PWMdata->PWM4Cnt)>1)) 
	{  /* IntFlag -> Reg_UpDown, 20160706 */
		PWMdata->CMPA[0] =  *(JpStart_NPtimeTable[Sector_Num][0]);
		PWMdata->CMPA[1] =  *(JpStart_NPtimeTable[Sector_Num][0]);
		PWMdata->CMPA[2] =  *(JpStart_NPtimeTable[Sector_Num][1]);
		PWMdata->CMPA[3] =  *(JpStart_NPtimeTable[Sector_Num][1]);
		PWMdata->CMPA[4] =  *(JpStart_NPtimeTable[Sector_Num][2]);
		PWMdata->CMPA[5] =  *(JpStart_NPtimeTable[Sector_Num][2]);
	}
	else
	{
		PWMdata->CMPA[0] = *(JpStart_NPtimeTableC[Sector_Num][0]);
		PWMdata->CMPA[1] = *(JpStart_NPtimeTableC[Sector_Num][0]);
		PWMdata->CMPA[2] = *(JpStart_NPtimeTableC[Sector_Num][1]);
		PWMdata->CMPA[3] = *(JpStart_NPtimeTableC[Sector_Num][1]);
		PWMdata->CMPA[4] = *(JpStart_NPtimeTableC[Sector_Num][2]);
		PWMdata->CMPA[5] = *(JpStart_NPtimeTableC[Sector_Num][2]);		
	}
	
	(PWMdata->PWM4Cnt)++;/* att3 */
	if((PWMdata->PWM4Cnt)>3)
	PWMdata->PWM4Cnt = 0;/* 0 1 2 3	//att3  */
} /*  end of SVPWML */


 
/**************************************************************************************************************************************/
/*Name		:Minimum_Pulse_TLC
*Description:窄脉冲检测算法
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by TLC
*/
void Minimum_Pulse_TLC(struct PWMdataStruct *PWMdata)/* svmtlc   in1--MP_test  in2--VBC_test  in3--NPChange  in4--MP这个数值是必改的。in5--T1PRHalfSVM  in6--MPhalf必改！ */
{
	double temp = 0;
/* ////////////////  最小脉宽处理 Start ///////////////// */
	pt1=t1;pt2=t2;pt3=t3;
	pt2t1=pt2-pt1;
	pt3t2=pt3-pt2;
	pt3t1=pt3-pt1;

 

	/*NPChange由VBC修改，且电压不失去平衡时为NPChange = 1*/
	NPChange = 1;
	
	if (NPChange==0)
	   {NPChange=1;}
	else
	   {NPChange=0;}

	MP_Debug=0;
	MP_test = 1;
		
	if(MP_test==1)
	{
		if (NPChange==1) /* N-type time increase */
		{
		if ((pt1<MP)||((T1PRHalfSVM-pt3)<MP))
		{   /* MP_Debug=-1; first */
		    if ((pt1+T1PRHalfSVM-pt3)>=MP)/* pt1+delt_t3>MP    */
			{
				MP_Debug=-1;
				if(pt3t2>=MP)
				{t1=(pt1+T1PRHalfSVM-pt3); t2=T1PRHalfSVM-pt3t2; t3=T1PRHalfSVM;}/*  right */
				else if ((pt3t2>=MPhalf)&&(pt3t2<MP))
				{
				    if (pt3t1>=MP)
					{t1=(pt1+T1PRHalfSVM-pt3); t2=T1PRHalfSVM-MP; t3=T1PRHalfSVM;}
					else if ((pt3t1>=MPhalf)&&(pt3t1<MP))/* pt3t1>=pt3t2 */
					{t1=T1PRHalfSVM-MP; t2=T1PRHalfSVM-MP; t3=T1PRHalfSVM;}
				}    
				else if (pt3t2<MPhalf)
				{
					if (pt3t1>=MP)/* pt3t1>=pt3t2 */
					{t1=(pt1+T1PRHalfSVM-pt3); t2=T1PRHalfSVM; t3=T1PRHalfSVM;}
					else if ((pt3t1>=MPhalf)&&(pt3t1<MP))
					{t1=T1PRHalfSVM-MP; t2=T1PRHalfSVM; t3=T1PRHalfSVM;}
					else if (pt3t1<MPhalf)
					{t1=T1PRHalfSVM; t2=T1PRHalfSVM; t3=T1PRHalfSVM;}
				}        
			} 
		    else if (((pt1+T1PRHalfSVM-pt3)>=MPhalf)&&((pt1+T1PRHalfSVM-pt3)<MP))
			{   /* MP_Debug=-2;//20091127 */
			    if(pt3t2>=MP)/* pt3t1>=pt3t2 */
			    {/* MP_Debug=-2;//20091127 */
			       if ((T1PRHalfSVM-pt3t2)<MP)
				   {t1=MP; t2=t1+pt2t1; t3=T1PRHalfSVM;MP_Debug=-2;}/* Problems20091127 MP_Debug=-2; */
			   else
				  {t1=MP; t2=T1PRHalfSVM-pt3t2; t3=T1PRHalfSVM;}/*  right */
			    }   
			    else if ((pt3t2>=MPhalf)&&(pt3t2<MP))
			    {t1=MP; t2=T1PRHalfSVM-MP; t3=T1PRHalfSVM;}
			    else if (pt3t2<MPhalf)
			    {t1=MP; t2=T1PRHalfSVM; t3=T1PRHalfSVM;}
			}  
		    else if ((pt1+T1PRHalfSVM-pt3)<MPhalf)
			{
			    MP_Debug=-3;
			    if(pt3t2>=MP)/* pt3t1>=pt3t2 */
			    {
			       if ((T1PRHalfSVM-pt3t2)>=MP)
				   {t1=0; t2=T1PRHalfSVM-pt3t2; t3=T1PRHalfSVM;}/*  right */
			       else if (((T1PRHalfSVM-pt3t2)>=MPhalf)&&((T1PRHalfSVM-pt3t2)<MP))
				   {t1=0; t2=MP; t3=T1PRHalfSVM;}/* t2=T1PRHalfSVM-MP; */
			       else if ((T1PRHalfSVM-pt3t2)<MPhalf)
				  {t1=0; t2=0;t3=T1PRHalfSVM;}
			    }      
			    else if ((pt3t2>=MPhalf)&&(pt3t2<MP))
			    {t1=0; t2=T1PRHalfSVM-MP; t3=T1PRHalfSVM;}/*  */
			    else if (pt3t2<MPhalf)
			    {t1=0; t2=T1PRHalfSVM; t3=T1PRHalfSVM;}/*  */
			}		 
		}/* if ((pt1<MP)||((T1PRHalfSVM-pt3)<MP))       */
	    }/* (NPChange==1) NeuVoltCtl_deltT<0 */
		else /*  N-type time decrease, P-type increase */
		{  
		    if ((pt1<MP)||((T1PRHalfSVM-pt3)<MP))
		    {MP_Debug=1;
		    if ((pt1+T1PRHalfSVM-pt3)>=MP)/* delt_t3>MP */
			{
			    if(pt2t1>=MP)
			    {t1=0; t2=pt2t1; t3=pt3t1;}/* =(T1PRHalfSVM-(pt1+T1PRHalfSVM-pt3)) right */
			    else if ((pt2t1>=MPhalf)&&(pt2t1<MP))
			    {
				    if (pt3t1>=MP)
					{t1=0; t2=MP; t3=pt3t1;}
				    else if ((pt3t1>=MPhalf)&&(pt3t1<MP))/* pt3t1>=pt2t1 */
					{t1=0; t2=MP; t3=MP;}/* or MP+pt3t2 */
			    }    
			    else if (pt2t1<MPhalf)
			    {
				    if (pt3t1>=MP)
					{t1=0; t2=0; t3=pt3t1;}
				    else if ((pt3t1>=MPhalf)&&(pt3t1<MP))
					{t1=0; t2=0; t3=MP;}
				    else if (pt3t1<MPhalf)
					{t1=0; t2=0; t3=0;}
			    }        
			} 
		    else if (((pt1+T1PRHalfSVM-pt3)>=MPhalf)&&((pt1+T1PRHalfSVM-pt3)<MP))
			{
			    MP_Debug=2;
			    if(pt2t1>=MP)
			    {
				    if ((T1PRHalfSVM-pt2t1)<MP)
				    {t1=0; t2=T1PRHalfSVM-MP; t3=T1PRHalfSVM-MP;}
				    else
				    {t1=0; t2=pt2t1; t3=T1PRHalfSVM-MP;}
			    }    
			    else if ((pt2t1>=MPhalf)&&(pt2t1<MP))
			    {t1=0; t2=MP; t3=T1PRHalfSVM-MP;}
			    else if (pt2t1<MPhalf)
			    {t1=0; t2=0; t3=T1PRHalfSVM-MP;}
			}  
		    else if ((pt1+T1PRHalfSVM-pt3)<MPhalf)
			{
			    MP_Debug=3;
			    if(pt2t1>=MP)
			    {
				    if ((T1PRHalfSVM-pt2t1)<MP)
				    {t1=0; t2=T1PRHalfSVM-MP; t3=T1PRHalfSVM;}
				    else
				    {t1=0; t2=pt2t1; t3=T1PRHalfSVM;}
			    }       
			   else if ((pt2t1>=MPhalf)&&(pt2t1<MP))
			   {t1=0; t2=MP; t3=T1PRHalfSVM;}
			   else if (pt2t1<MPhalf)
			   {t1=0; t2=0; t3=T1PRHalfSVM;}
			}        
		    }/* if ((pt1<MP)||((T1PRHalfSVM-pt3)<MP)) */
		} /* else    */
		/* ////////////////  最小脉宽处理 End ///////////////// */
		}/* MP_test==1;  */

		/* ////////////////  相关寄存器配置 Start ///////////////// */
		Ct1t2t3=(Tbt1t2t3[Sector_Num][Region_Num]);
		Bbit=(int)(floor(Ct1t2t3*0.01));
        Sbit=(int)(floor((Ct1t2t3-Bbit*100)*0.1));
		temp = Ct1t2t3-Bbit*100-Sbit*10;
        Gbit=(int)(floor(temp));
		/* NPCAUT */
		if ((PWMdata->Reg_UpDown)==0)  /* 20160706 1->0 */
		{

			if (((Bbit==1)&&(t1==0))||((Sbit==1)&&(t2==0))||((Gbit==1)&&(t3==0)))
			{PRD[0]=AQ_SET;CAD[0]=AQ_SET;}
			else if (((Bbit==1)&&(t1==T1PRHalfSVM))||((Sbit==1)&&(t2==T1PRHalfSVM))||((Gbit==1)&&(t3==T1PRHalfSVM)))
			{PRD[0]=AQ_CLEAR;CAD[0]=AQ_CLEAR;}

			if (((Bbit==2)&&(t1==0))||((Sbit==2)&&(t2==0))||((Gbit==2)&&(t3==0)))
			{PRD[1]=AQ_SET;CAD[1]=AQ_SET;}
			else if (((Bbit==2)&&(t1==T1PRHalfSVM))||((Sbit==2)&&(t2==T1PRHalfSVM))||((Gbit==2)&&(t3==T1PRHalfSVM)))
			{PRD[1]=AQ_CLEAR;CAD[1]=AQ_CLEAR;}

			if (((Bbit==3)&&(t1==0))||((Sbit==3)&&(t2==0))||((Gbit==3)&&(t3==0)))
			{PRD[2]=AQ_SET;CAD[2]=AQ_SET;}
			else if (((Bbit==3)&&(t1==T1PRHalfSVM))||((Sbit==3)&&(t2==T1PRHalfSVM))||((Gbit==3)&&(t3==T1PRHalfSVM)))
			{PRD[2]=AQ_CLEAR;CAD[2]=AQ_CLEAR;}

			if (((Bbit==4)&&(t1==0))||((Sbit==4)&&(t2==0))||((Gbit==4)&&(t3==0)))
			{PRD[3]=AQ_SET;CAD[3]=AQ_SET;}
			else if (((Bbit==4)&&(t1==T1PRHalfSVM))||((Sbit==4)&&(t2==T1PRHalfSVM))||((Gbit==4)&&(t3==T1PRHalfSVM)))
			{PRD[3]=AQ_CLEAR;CAD[3]=AQ_CLEAR;}

			if (((Bbit==5)&&(t1==0))||((Sbit==5)&&(t2==0))||((Gbit==5)&&(t3==0)))
			{PRD[4]=AQ_SET;CAD[4]=AQ_SET;}
			else if (((Bbit==5)&&(t1==T1PRHalfSVM))||((Sbit==5)&&(t2==T1PRHalfSVM))||((Gbit==5)&&(t3==T1PRHalfSVM)))
			{PRD[4]=AQ_CLEAR;CAD[4]=AQ_CLEAR;}

			if (((Bbit==6)&&(t1==0))||((Sbit==6)&&(t2==0))||((Gbit==6)&&(t3==0)))
			{PRD[5]=AQ_SET;CAD[5]=AQ_SET;}
			else if (((Bbit==6)&&(t1==T1PRHalfSVM))||((Sbit==6)&&(t2==T1PRHalfSVM))||((Gbit==6)&&(t3==T1PRHalfSVM)))
			{PRD[5]=AQ_CLEAR;CAD[5]=AQ_CLEAR;}
		}   
		else    
		{

			if (((Bbit==1)&&(t1==0))||((Sbit==1)&&(t2==0))||((Gbit==1)&&(t3==0)))
		    {ZRO[0]=AQ_SET;CAU[0]=AQ_SET;}
			else if (((Bbit==1)&&(t1==T1PRHalfSVM))||((Sbit==1)&&(t2==T1PRHalfSVM))||((Gbit==1)&&(t3==T1PRHalfSVM)))
			{ZRO[0]=AQ_CLEAR;CAU[0]=AQ_CLEAR;}

			if (((Bbit==2)&&(t1==0))||((Sbit==2)&&(t2==0))||((Gbit==2)&&(t3==0)))
			{ZRO[1]=AQ_SET;CAU[1]=AQ_SET;}
			else if (((Bbit==2)&&(t1==T1PRHalfSVM))||((Sbit==2)&&(t2==T1PRHalfSVM))||((Gbit==2)&&(t3==T1PRHalfSVM)))
			{ZRO[1]=AQ_CLEAR;CAU[1]=AQ_CLEAR;}

			if (((Bbit==3)&&(t1==0))||((Sbit==3)&&(t2==0))||((Gbit==3)&&(t3==0)))
			{ZRO[2]=AQ_SET;CAU[2]=AQ_SET;}
			else if (((Bbit==3)&&(t1==T1PRHalfSVM))||((Sbit==3)&&(t2==T1PRHalfSVM))||((Gbit==3)&&(t3==T1PRHalfSVM)))
			{ZRO[2]=AQ_CLEAR;CAU[2]=AQ_CLEAR;}

			if (((Bbit==4)&&(t1==0))||((Sbit==4)&&(t2==0))||((Gbit==4)&&(t3==0)))
			{ZRO[3]=AQ_SET;CAU[3]=AQ_SET;}
			else if (((Bbit==4)&&(t1==T1PRHalfSVM))||((Sbit==4)&&(t2==T1PRHalfSVM))||((Gbit==4)&&(t3==T1PRHalfSVM)))
			{ZRO[3]=AQ_CLEAR;CAU[3]=AQ_CLEAR;}

			if (((Bbit==5)&&(t1==0))||((Sbit==5)&&(t2==0))||((Gbit==5)&&(t3==0)))
			{ZRO[4]=AQ_SET;CAU[4]=AQ_SET;}
			else if (((Bbit==5)&&(t1==T1PRHalfSVM))||((Sbit==5)&&(t2==T1PRHalfSVM))||((Gbit==5)&&(t3==T1PRHalfSVM)))
			{ZRO[4]=AQ_CLEAR;CAU[4]=AQ_CLEAR;}

			if (((Bbit==6)&&(t1==0))||((Sbit==6)&&(t2==0))||((Gbit==6)&&(t3==0)))
			{ZRO[5]=AQ_SET;CAU[5]=AQ_SET;}
			else if (((Bbit==6)&&(t1==T1PRHalfSVM))||((Sbit==6)&&(t2==T1PRHalfSVM))||((Gbit==6)&&(t3==T1PRHalfSVM)))
			{ZRO[5]=AQ_CLEAR;CAU[5]=AQ_CLEAR;}
		} 
		/* ////////////////  相关寄存器配置 End ///////////////// */
	}  /*  end of Minimum_Pulse_TLC() */

/**************************************************************************************************************************************/
/*Name		:NPC_SVM_SetAse
*Description:PWM定时器增计数时比较式寄存器和增计数比较寄存器设置
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by TLC
*/
void NPC_SVM_SetAse(void)   
{
   ZRO[0] = (ActTb_EPwm1[Sector_Num][Region_Num][0]); /*PWMA1*/ 				
   CAU[0] = (ActTb_EPwm1[Sector_Num][Region_Num][1]);  
   ZRO[1] = (ActTb_EPwm2[Sector_Num][Region_Num][0]); /*PWMA2*/   				
   CAU[1] = (ActTb_EPwm2[Sector_Num][Region_Num][1]);                             
   ZRO[2] = (ActTb_EPwm3[Sector_Num][Region_Num][0]); /*PWMB1*/   				
   CAU[2] = (ActTb_EPwm3[Sector_Num][Region_Num][1]);                             
   ZRO[3] = (ActTb_EPwm4[Sector_Num][Region_Num][0]); /*PWMB2*/   				
   CAU[3] = (ActTb_EPwm4[Sector_Num][Region_Num][1]);                            
   ZRO[4] = (ActTb_EPwm5[Sector_Num][Region_Num][0]); /*PWMC1*/  				
   CAU[4] = (ActTb_EPwm5[Sector_Num][Region_Num][1]);                             
   ZRO[5] = (ActTb_EPwm6[Sector_Num][Region_Num][0]); /*PWMC2*/ 				
   CAU[5] = (ActTb_EPwm6[Sector_Num][Region_Num][1]); 	
}/* NPC_SVM_SetAse */
/**************************************************************************************************************************************/
/*Name		:NPC_SVM_SetDes
*Description:PWM定时器减计数时比较式寄存器和减计数比较寄存器设置
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by TLC
*/
void NPC_SVM_SetDes(void)
{                                         
   PRD[0]=(ActTb_EPwm1[Sector_Num][Region_Num][2]); /*PWMA1*/				
   CAD[0]=(ActTb_EPwm1[Sector_Num][Region_Num][3]); 
   PRD[1]=(ActTb_EPwm2[Sector_Num][Region_Num][2]);  /*PWMA2*/ 				
   CAD[1]=(ActTb_EPwm2[Sector_Num][Region_Num][3]); 
   PRD[2]=(ActTb_EPwm3[Sector_Num][Region_Num][2]);   /*PWMB1*/				
   CAD[2]=(ActTb_EPwm3[Sector_Num][Region_Num][3]);  
   PRD[3]=(ActTb_EPwm4[Sector_Num][Region_Num][2]);   /*PWMB2*/				
   CAD[3]=(ActTb_EPwm4[Sector_Num][Region_Num][3]);  
   PRD[4]=(ActTb_EPwm5[Sector_Num][Region_Num][2]);   /*PWMC1*/				
   CAD[4]=(ActTb_EPwm5[Sector_Num][Region_Num][3]); 
   PRD[5]=(ActTb_EPwm6[Sector_Num][Region_Num][2]);  /*PWMC2*/ 				
   CAD[5]=(ActTb_EPwm6[Sector_Num][Region_Num][3]); 
}/* NPC_SVM_SetDes */

/**************************************************************************************************************************************/
/*Name		:JpStart_NPC_SVM_SetAse
*Description:SVPWML算法时定时器增计数时比较式寄存器和增计数比较寄存器设置
*Input		:*PWMdata PWM数据结构体指针			
*Output		:无
*Return		:无
*History	:2016,by TLC
*/
void JpStart_NPC_SVM_SetAse(struct PWMdataStruct *PWMdata)   
{
   ZRO[0] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][0]); 				
   CAU[0] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][1]); 
   ZRO[1] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][0]);   				
   CAU[1] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][1]); 
   ZRO[2] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][0]);   				
   CAU[2] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][1]); 
   ZRO[3] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][0]);   				
   CAU[3] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][1]);
   ZRO[4] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][0]);   				
   CAU[4] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][1]); 
   ZRO[5] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][0]);   				
   CAU[5] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][1]); 	
}

/************************************************************/
/*Name		:JpStart_NPC_SVM_SetDes
*Description:SVPWML算法时定时器减计数时比较式寄存器和减计数比较寄存器设置
*Input		:*PWMdata PWM数据结构体指针			
*Output		:无
*Return		:无
*History	:2016,by TLC
*/
void JpStart_NPC_SVM_SetDes(struct PWMdataStruct *PWMdata)
{ 
   PRD[0] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][0]);  				
   CAD[0] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][1]);  
   PRD[1] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][0]);    				
   CAD[1] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][1]);  
   PRD[2] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][0]);    				
   CAD[2] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][1]);  
   PRD[3] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][0]);    				
   CAD[3] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][1]);  
   PRD[4] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][0]);    				
   CAD[4] = (JapStart_ActTb_EPwm1[PWMdata->PWM4Cnt][1]);  
   PRD[5] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][0]);    				
   CAD[5] = (JapStart_ActTb_EPwm2[PWMdata->PWM4Cnt][1]);  
}
/**************************************************************************************************************************************/
/*Name		:ChangeTsSVM
*Description:切换到SVPWM算法时改变定时器周期值到SVM
*Input		:*PWMdata PWM数据结构体指针			
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void ChangeTsSVM(struct PWMdataStruct *PWMdata)
{
	testflag1 |=8;  /* 进入changeTsSVM--第四位置1	 */
	PWMdata->Ts = TsSVM;	
	PWMdata->T1PR = T1PRSVM;  /*  (1/600) * 50M */
	PWMdata->T1PRHalf = T1PRHalfSVM;  /* 600Hz 1.2k */
	PWMdata->InvModulationFlag = 0;

/*	*(short *)((PWMdata->PWMTransLocalAddr)+0x08) = 1;     */        /*定时器时钟分频，25MHz*/
/*	*(short *)((PWMdata->PWMTransLocalAddr)+0x06)= T1PRSVM; */ /*20160531 del*/
	SHE_VME_Flag = 0;
}
/**************************************************************************************************************************************/
/*Name		:NP_JUMP_AVOIDy2
*Description:判断NP跳变
*Input		:*PWMdata PWM数据结构体指针			
*Output		:无
*Return		:无
*History	:2010,modified by cdd
*/
void NP_JUMP_AVOIDy2(struct PWMdataStruct *PWMdata)  /* 20150603仔细阅读，功能确实是判断电平两级跳变*/

{
    if((ZRO[0]==AQ_CLEAR)&&((CAU[0]==0)||(CAU[0]==AQ_CLEAR)))
       {NPZROhead_A1=0;NPZROtail_A1=0;}
    else if((ZRO[0]==AQ_CLEAR)&&(CAU[0]==3))
       {NPZROhead_A1=0;NPZROtail_A1=1;}
    else if((ZRO[0]==AQ_SET)&&((CAU[0]==0)||(CAU[0]==AQ_SET)))
       {NPZROhead_A1=1;NPZROtail_A1=1;}
    else if((ZRO[0]==AQ_SET)&&(CAU[0]==3))
       {NPZROhead_A1=1;NPZROtail_A1=0;}

    if((ZRO[1]==2)&&((CAU[1]==0)||(CAU[1]==2)))
       {NPZROhead_A2=0;NPZROtail_A2=0;}
    else if((ZRO[1]==2)&&(CAU[1]==3))
       {NPZROhead_A2=0;NPZROtail_A2=1;}
    else if((ZRO[1]==AQ_SET)&&((CAU[1]==0)||(CAU[1]==1)))
       {NPZROhead_A2=1;NPZROtail_A2=1;}
    else if((ZRO[1]==AQ_SET)&&(CAU[1]==3))
       {NPZROhead_A2=1;NPZROtail_A2=0;}

    if((ZRO[2]==2)&&((CAU[2]==0)||(CAU[2]==2)))
       {NPZROhead_B1=0;NPZROtail_B1=0;}
    else if((ZRO[2]==2)&&(CAU[2]==3))
       {NPZROhead_B1=0;NPZROtail_B1=1;}
    else if((ZRO[2]==AQ_SET)&&((CAU[2]==0)||(CAU[2]==1)))
       {NPZROhead_B1=1;NPZROtail_B1=1;}
    else if((ZRO[2]==AQ_SET)&&(CAU[2]==3))
       {NPZROhead_B1=1;NPZROtail_B1=0;}

    if((ZRO[3]==2)&&((CAU[3]==0)||(CAU[3]==2)))
       {NPZROhead_B2=0;NPZROtail_B2=0;}
    else if((ZRO[3]==2)&&(CAU[3]==3))
       {NPZROhead_B2=0;NPZROtail_B2=1;}
    else if((ZRO[3]==AQ_SET)&&((CAU[3]==0)||(CAU[3]==1)))
       {NPZROhead_B2=1;NPZROtail_B2=1;}
    else if((ZRO[3]==AQ_SET)&&(CAU[3]==3))
       {NPZROhead_B2=1;NPZROtail_B2=0;}

    if((ZRO[4]==2)&&((CAU[4]==0)||(CAU[4]==2)))
       {NPZROhead_C1=0;NPZROtail_C1=0;}
    else if((ZRO[4]==2)&&(CAU[4]==3))
       {NPZROhead_C1=0;NPZROtail_C1=1;}
    else if((ZRO[4]==AQ_SET)&&((CAU[4]==0)||(CAU[4]==1)))
       {NPZROhead_C1=1;NPZROtail_C1=1;}
    else if((ZRO[4]==AQ_SET)&&(CAU[4]==3))
	   {NPZROhead_C1=1;NPZROtail_C1=0;}

    if((ZRO[5]==2)&&((CAU[5]==0)||(CAU[5]==2)))
       {NPZROhead_C2=0;NPZROtail_C2=0;}
    else if((ZRO[5]==2)&&(CAU[5]==3))
       {NPZROhead_C2=0;NPZROtail_C2=1;}
    else if((ZRO[5]==AQ_SET)&&((CAU[5]==0)||(CAU[5]==1)))
       {NPZROhead_C2=1;NPZROtail_C2=1;}
    else if((ZRO[5]==AQ_SET)&&(CAU[5]==3))
	   {NPZROhead_C2=1;NPZROtail_C2=0;}


    if((PRD[0]==2)&&((CAD[0]==0)||(CAD[0]==2)))
       {NPPRDhead_A1=0;NPPRDtail_A1=0;}
    else if((PRD[0]==2)&&(CAD[0]==3))
       {NPPRDhead_A1=0;NPPRDtail_A1=1;}
    else if((PRD[0]==AQ_SET)&&((CAD[0]==0)||(CAD[0]==1)))
       {NPPRDhead_A1=1;NPPRDtail_A1=1;}
    else if((PRD[0]==AQ_SET)&&(CAD[0]==3))
	   {NPPRDhead_A1=1;NPPRDtail_A1=0;}

    if((PRD[1]==2)&&(CAD[1]==0)||(CAD[1]==2))
       {NPPRDhead_A2=0;NPPRDtail_A2=0;}
    else if((PRD[1]==2)&&(CAD[1]==3))
       {NPPRDhead_A2=0;NPPRDtail_A2=1;}
    else if((PRD[1]==AQ_SET)&&(CAD[1]==0)||(CAD[1]==1))
       {NPPRDhead_A2=1;NPPRDtail_A2=1;}
    else if((PRD[1]==AQ_SET)&&(CAD[1]==3))
	   {NPPRDhead_A2=1;NPPRDtail_A2=0;}

    if((PRD[2]==2)&&(CAD[2]==0)||(CAD[2]==2))
       {NPPRDhead_B1=0;NPPRDtail_B1=0;}
    else if((PRD[2]==2)&&(CAD[2]==3))
       {NPPRDhead_B1=0;NPPRDtail_B1=1;}
    else if((PRD[2]==AQ_SET)&&(CAD[2]==0)||(CAD[2]==1))
       {NPPRDhead_B1=1;NPPRDtail_B1=1;}
    else if((PRD[2]==AQ_SET)&&(CAD[2]==3))
	   {NPPRDhead_B1=1;NPPRDtail_B1=0;}

	if((PRD[3]==2)&&(CAD[3]==0)||(CAD[3]==2))
       {NPPRDhead_B2=0;NPPRDtail_B2=0;}
    else if((PRD[3]==2)&&(CAD[3]==3))
       {NPPRDhead_B2=0;NPPRDtail_B2=1;}
    else if((PRD[3]==AQ_SET)&&(CAD[3]==0)||(CAD[3]==1))
       {NPPRDhead_B2=1;NPPRDtail_B2=1;}
    else if((PRD[3]==AQ_SET)&&(CAD[3]==3))
	   {NPPRDhead_B2=1;NPPRDtail_B2=0;}

    if((PRD[4]==2)&&(CAD[4]==0)||(CAD[4]==2))
       {NPPRDhead_C1=0;NPPRDtail_C1=0;}
    else if((PRD[4]==2)&&(CAD[4]==3))
       {NPPRDhead_C1=0;NPPRDtail_C1=1;}
    else if((PRD[4]==1)&&(CAD[4]==0)||(CAD[4]==1))
       {NPPRDhead_C1=1;NPPRDtail_C1=1;}
    else if((PRD[4]==1)&&(CAD[4]==3))
	   {NPPRDhead_C1=1;NPPRDtail_C1=0;}

    if((PRD[5]==2)&&(CAD[5]==0)||(CAD[5]==2))
       {NPPRDhead_C2=0;NPPRDtail_C2=0;}
    else if((PRD[5]==2)&&(CAD[5]==3))
       {NPPRDhead_C2=0;NPPRDtail_C2=1;}
    else if((PRD[5]==1)&&(CAD[5]==0)||(CAD[5]==1))
       {NPPRDhead_C2=1;NPPRDtail_C2=1;}
    else if((PRD[5]==1)&&(CAD[5]==3))
	   {NPPRDhead_C2=1;NPPRDtail_C2=0;}

	if((PWMdata->Reg_UpDown) == ET_CTR_ZRO)/* ET_CTR_ZRO  */  /* IntFlag -> Reg_UpDown, 20160706 */
	{
	   if(((NPZROtail_A1==0)&&(NPZROtail_A2==0))&&((NPPRDhead_A1==1)&&(NPPRDhead_A2==1)))
		 {NPjumpA=1;}
	   else
		 {NPjumpA=0;}
	   if(((NPZROtail_A1==1)&&(NPZROtail_A2==1))&&((NPPRDhead_A1==0)&&(NPPRDhead_A2==0)))
		 {PNjumpA=1;}
	   else
		 {PNjumpA=0;}
	   if(((NPZROtail_B1==0)&&(NPZROtail_B2==0))&&((NPPRDhead_B1==1)&&(NPPRDhead_B2==1)))
		 {NPjumpB=1;}
	   else
		 {NPjumpB=0;}
	   if(((NPZROtail_B1==1)&&(NPZROtail_B2==1))&&((NPPRDhead_B1==0)&&(NPPRDhead_B2==0)))
		 {PNjumpB=1;}
	   else
		 {PNjumpB=0;}
	   if(((NPZROtail_C1==0)&&(NPZROtail_C2==0))&&((NPPRDhead_C1==1)&&(NPPRDhead_C2==1)))
		 {NPjumpC=1;}
	   else
		 {NPjumpC=0;}
	   if(((NPZROtail_C1==1)&&(NPZROtail_C2==1))&&((NPPRDhead_C1==0)&&(NPPRDhead_C2==0)))
		 {PNjumpC=1;}
	   else
		 {PNjumpC=0;}
	}
	else/* PRD  */ 
	 {
	   if(((NPPRDtail_A1==0)&&(NPPRDtail_A2==0))&&((NPZROhead_A1==1)&&(NPZROhead_A2==1)))
		 {NPjumpA=1;}
	   else
		 {NPjumpA=0;}
	   if(((NPPRDtail_A1==1)&&(NPPRDtail_A2==1))&&((NPZROhead_A1==0)&&(NPZROhead_A2==0)))
		 {PNjumpA=1;}
	   else
		 {PNjumpA=0;}

	   if(((NPPRDtail_B1==0)&&(NPPRDtail_B2==0))&&((NPZROhead_B1==1)&&(NPZROhead_B2==1)))
		 {NPjumpB=1;}
	   else
		 {NPjumpB=0;}
	   if(((NPPRDtail_B1==1)&&(NPPRDtail_B2==1))&&((NPZROhead_B1==0)&&(NPZROhead_B2==0)))
		 {PNjumpB=1;}
	   else
		 {PNjumpB=0;}
	   if(((NPPRDtail_C1==0)&&(NPPRDtail_C2==0))&&((NPZROhead_C1==1)&&(NPZROhead_C2==1)))
		 {NPjumpC=1;}
	   else
		 {NPjumpC=0;}
	   if(((NPPRDtail_C1==1)&&(NPPRDtail_C2==1))&&((NPZROhead_C1==0)&&(NPZROhead_C2==0)))
		 {PNjumpC=1;}
	   else
		 {PNjumpC=0;}
	 }

/*	if ((NPjumpA==1)||(PNjumpA==1)||(NPjumpB==1)||(PNjumpB==1)||(NPjumpC==1)||(PNjumpC==1))
	{   
		if(ChangeSHENPJumpTest == 0)
		{ChangeSHENPJumpFlag = 1;}
		else
		{ErrFlag = ErrFlag | 0x0040;}
	} */
	if((PWMdata->ChangeSHENPJumpTest) == 0)
	{
		if ((NPjumpA==1)||(PNjumpA==1)||(NPjumpB==1)||(PNjumpB==1)||(NPjumpC==1)||(PNjumpC==1))
			{PWMdata->ChangeSHENPJumpFlag = 1;}
		else	
			PWMdata->ChangeSHENPJumpFlag = 0;
	}
	else 
	{
		if ((NPjumpA==1)||(PNjumpA==1)||(NPjumpB==1)||(PNjumpB==1)||(NPjumpC==1)||(PNjumpC==1))
			{ErrFlag = ErrFlag | 0x0040;}
	}
}  /* NP_JUMP_AVOIDy2()   */
/**************************************************************************************************************************************/
/*Name		:CheckSwich2SHE
*Description:判断PWM模式是否切换到SHE模式
*Input		:*PWMdata PWM数据结构体指针			
*Output		:无
*Return		:无
*History	:2010,modified by cdd
*/
void CheckSwich2SHE(struct PWMdataStruct *PWMdata)/* in1--Kv(by CalcKvUsRef) in2--vvvf1 in3--Ts in4--VFGama in5--T1PRHalf  */
{
	ma_index = (int)(ceil( (PWMdata->Kv) * 100));
	if(ma_index>100) ma_index = 100;  
	
	VFGamatemp = PWMdata->VFGama;
	VFGamatemp = VFGamatemp+2*pi*PWMdata->vvvf1/7200./2+2*pi*PWMdata->vvvf1/7200.;//+2*pi*PWMdata->vvvf1/7200.;
	while(VFGamatemp<0)      VFGamatemp=VFGamatemp+2*PI;
	while(VFGamatemp>2*PI)    VFGamatemp=VFGamatemp-2*PI;
	
/*	VFGamatemp = VFGamaSHEReady + vvvf1*2*pi*(0.5*TsSVM);  *//* 20150908 */
	while(VFGamatemp>=2*pi) VFGamatemp=VFGamatemp-2*pi; 
	while(VFGamatemp<0) VFGamatemp=VFGamatemp+2*pi; 
	PWMdata->Gama_TotalNum_Pre = 0;	

//频率上升段
	if (PWMdata->vvvf1IncreFlag==1)
{
	//SHE11
	//	if((PWMdata->vvvf1 < f2point) && (PWMdata->HalfGama_TotalNum!=9))
	//	{
	//		PWMdata->Gama_TotalNum = 22;
	//		PWMdata->HalfGama_TotalNum = 11;
	//		TbStartPointer = SHE_11;	
 //        }
 // //SHE_9
 //else if((PWMdata->vvvf1 < f3point) && (PWMdata->HalfGama_TotalNum!=7))
	//	{
	//		PWMdata->Gama_TotalNum = 18;
	//		PWMdata->HalfGama_TotalNum = 9;
	//		TbStartPointer = SHE_9;  	
	//	}
 ////SHE_7
 //else if( (PWMdata->vvvf1 < f4point) && (PWMdata->HalfGama_TotalNum!=5)) 
	//	{
	//		PWMdata->Gama_TotalNum = 14;
	//		PWMdata->HalfGama_TotalNum = 7;
	//		TbStartPointer = SHE_7;		
	//	}
 // //SHE_5
 //else if( (PWMdata->vvvf1 < f5point) && (PWMdata->HalfGama_TotalNum!=3)) 
	//{
	//	PWMdata->Gama_TotalNum = 10;
	//	PWMdata->HalfGama_TotalNum = 5;
	//	TbStartPointer = SHE_5;  
	//}
 // //SHE_3
 //else 
  if((PWMdata->vvvf1 < f6point)&&(PWMdata->HalfGama_TotalNum!=1) )//HalfGama_TotalNum!=1防止切到SHE1频率下降但标志位没有变又切回来SHE3，防止来回切				  	
	{
		PWMdata->Gama_TotalNum = 6;
		PWMdata->HalfGama_TotalNum = 3;
		TbStartPointer = SHE_3;
	}
 else /* SHE_1 */
	{
		PWMdata->Gama_TotalNum = 2;
		PWMdata->HalfGama_TotalNum = 1;
		TbStartPointer = SHE_1;
	}
 }
//频率下降段
else if (PWMdata->vvvf1IncreFlag==0)
{
	//SHE1
		if((PWMdata->vvvf1 >= f6pointB) && (PWMdata->HalfGama_TotalNum!=3))
		{
		PWMdata->Gama_TotalNum = 2;
		PWMdata->HalfGama_TotalNum = 1;
		TbStartPointer = SHE_1;
         }
  //SHE_3
 else if((PWMdata->vvvf1 >= f5pointB) && (PWMdata->HalfGama_TotalNum!=5))
		{
		PWMdata->Gama_TotalNum = 6;
		PWMdata->HalfGama_TotalNum = 3;
		TbStartPointer = SHE_3;
		}
 ////SHE_5
 //else if( (PWMdata->vvvf1 >= f4pointB) && (PWMdata->HalfGama_TotalNum!=7)) 
	//	{
	//	PWMdata->Gama_TotalNum = 10;
	//	PWMdata->HalfGama_TotalNum = 5;
	//	TbStartPointer = SHE_5; 
	//	}
 // //SHE_7
 //else if( (PWMdata->vvvf1 >= f3pointB) && (PWMdata->HalfGama_TotalNum!=9)) 
	//{
	//	PWMdata->Gama_TotalNum = 14;
	//	PWMdata->HalfGama_TotalNum = 7;
	//	TbStartPointer = SHE_7;		
	//}
 // //SHE_9
 //else if((PWMdata->vvvf1 >= f2pointB)&&(PWMdata->HalfGama_TotalNum!=11) )//HalfGama_TotalNum!=11防止切到SHE9频率上升但标志位没有变又切回来SHE11，防止来回切				  	
	//{
	//	PWMdata->Gama_TotalNum = 18;
	//	PWMdata->HalfGama_TotalNum = 9;
	//	TbStartPointer = SHE_9;  	
	//}
 //else /* SHE_11 */
	//{
	//	PWMdata->Gama_TotalNum = 22;
	//	PWMdata->HalfGama_TotalNum = 11;
	//	TbStartPointer = SHE_11;	
	//}
 }


	
	for(g_i = 0; g_i<6; g_i++)
	{
		switch(g_i)
		{
			case 0:
				VFGama6[g_i] = VFGamatemp - 4.712388980385;
				break;	
			case 1:	
				VFGama6[g_i] = VFGamatemp - 1.570796326795;   	
				break;		
			case 2:
			    VFGama6[g_i] = VFGamatemp  - 0.523598775598;							     
				break;	
			case 3:
			    VFGama6[g_i] = VFGamatemp  - 3.665191429188;										     	 	 
				break;	
			case 4:
				VFGama6[g_i] = VFGamatemp  -  2.617993877991;
				break;	
			case 5:
				VFGama6[g_i] = VFGamatemp  -  5.759586531581;
				break;	 	 	 		
		}	
		/* 限幅 */
		if(VFGama6[g_i]<0)	
			VFGama6[g_i] = VFGama6[g_i] + 6.283185307179;
			
		/* gama_Num[g_i] */
		PWMdata->gama_Num[g_i] = 0;		     	    
		if(VFGama6[g_i]>=pi)
			PWMdata->gama_Num[g_i] = PWMdata->Gama_TotalNum;
		else
		{
			if((VFGama6[g_i]<Halfpi))/* 0-1/4 */
			{		
				while(   (VFGama6[g_i] > *(TbStartPointer + ( (ma_index - 1) * (PWMdata->HalfGama_TotalNum) ) + PWMdata->gama_Num[g_i]) ) &&(PWMdata->gama_Num[g_i]<PWMdata->HalfGama_TotalNum))
				{	PWMdata->gama_Num[g_i]++;	}
			}
			else
			{
				VFGama6_g_iC = pi - VFGama6[g_i];

				while(   (VFGama6_g_iC > *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + PWMdata->gama_Num[g_i]) ) &&(PWMdata->gama_Num[g_i]<PWMdata->HalfGama_TotalNum))
				{	PWMdata->gama_Num[g_i]++;	}

				PWMdata->gama_Num[g_i] = PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i]; 	     	      	
			}	
		}
		
		/* gama_Num_pre[g_i] 									     */
		VFGama_Pre6[g_i] =  VFGama6[g_i] - (8.726646e-4)*(PWMdata->vvvf1);
		if(VFGama_Pre6[g_i]<0)	
			VFGama_Pre6[g_i] = VFGama_Pre6[g_i] + 6.283185307179;

		PWMdata->gama_Num_pre[g_i] = 0;
		if(VFGama_Pre6[g_i]>=pi)
			PWMdata->gama_Num_pre[g_i] = PWMdata->Gama_TotalNum;
		else
		{
			if((VFGama_Pre6[g_i]<Halfpi))/* 0-1/4 */
			{		
				while(   (VFGama_Pre6[g_i] > *(TbStartPointer + ( (ma_index - 1) * (PWMdata->HalfGama_TotalNum) ) + PWMdata->gama_Num_pre[g_i]) ) &&(PWMdata->gama_Num_pre[g_i]<PWMdata->HalfGama_TotalNum)     )
				{	PWMdata->gama_Num_pre[g_i]++;	}
			}
			else
			{
				VFGama_Pre6_g_iC = pi - VFGama_Pre6[g_i];

				while(   (VFGama_Pre6_g_iC > *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + PWMdata->gama_Num_pre[g_i]) ) &&(PWMdata->gama_Num_pre[g_i]<PWMdata->HalfGama_TotalNum)     )
				{	PWMdata->gama_Num_pre[g_i]++;	}
				PWMdata->gama_Num_pre[g_i] = PWMdata->Gama_TotalNum - PWMdata->gama_Num_pre[g_i]; 	     	      	

			}	
		}
		if( (g_i%2)==0 )
		{	          					
			if(PWMdata->gama_Num_pre[g_i] == PWMdata->gama_Num[g_i])
			{
				if( ((PWMdata->gama_Num[g_i])%2)==0 )  // {前1}
				{
					ZRO[g_i] = AQ_CLEAR;
					CAU[g_i] = AQ_NO_ACTION;
				}
				else
				{
					ZRO[g_i] = AQ_SET; 			
					CAU[g_i] = AQ_NO_ACTION;				           				    	
				}	
			}
			else
			{
				if(  (fabs(VFGama6[g_i] - VFGama_Pre6[g_i])>pi)&&(VFGamatemp != VFGamaStart)    )         	
				{
					ZRO[g_i] = AQ_CLEAR; 		
					CAU[g_i] = AQ_NO_ACTION;
				}
				else if(fabs(PWMdata->gama_Num[g_i] - PWMdata->gama_Num_pre[g_i]) == 2)
				{
					if( ((PWMdata->gama_Num[g_i])%2)==0)
					{
						ZRO[g_i] = AQ_CLEAR;	  
						CAU[g_i] = AQ_NO_ACTION; 	
					}	
					else
					{
						ZRO[g_i] = AQ_SET; 			
						CAU[g_i] = AQ_NO_ACTION;					              		
					}
				
				}	
				else
				{
					if( ((PWMdata->gama_Num[g_i])%2)==0 )  // {后1}
					{
					ZRO[g_i] = AQ_SET;	 
					CAU[g_i] = AQ_TOGGLE;              
					}	
					else
					{   		
					 ZRO[g_i] = AQ_CLEAR;	
					 CAU[g_i] = AQ_TOGGLE;  		
					}				           				    	  	
				}	
			}	
		}
		else
		{
			if((PWMdata->gama_Num_pre[g_i]) == (PWMdata->gama_Num[g_i]))
			{
			   if( ((PWMdata->gama_Num[g_i])%2)==0 )  // {前2}
			   {
					ZRO[g_i] = AQ_SET;/* AQ_CLEAR; */
					CAU[g_i] = AQ_NO_ACTION;
			   }
			   else
			   {
					ZRO[g_i] = AQ_CLEAR;/* AQ_SET; 			 */
					CAU[g_i] = AQ_NO_ACTION;
			   }	
			}
			else
			{
				if(  (fabs(VFGama6[g_i] - VFGama_Pre6[g_i])>pi)&&(VFGamatemp != VFGamaStart)    )         	
				{
					ZRO[g_i] = AQ_SET;/* AQ_CLEAR; 		 */
					CAU[g_i] = AQ_NO_ACTION;
				}
				else if(fabs((PWMdata->gama_Num[g_i]) - (PWMdata->gama_Num_pre[g_i])) == 2)
				{
					if(  ((PWMdata->gama_Num[g_i])%2) ==0)
					{
						ZRO[g_i] = AQ_SET;	  
						CAU[g_i] = AQ_NO_ACTION; 	
					}
					else
					{
						ZRO[g_i] = AQ_CLEAR; 			
						CAU[g_i] = AQ_NO_ACTION;					              		
					}
				}	
				else
				{
					if( ((PWMdata->gama_Num[g_i])%2)==0 )  // {后2}
					{
						ZRO[g_i] = AQ_CLEAR;	 
						CAU[g_i] = AQ_TOGGLE;              
					}	
					else
					{   		
						 ZRO[g_i] = AQ_SET;	
						 CAU[g_i] = AQ_TOGGLE;  		
					}
				}	
			}	
		}

		if(g_i==0)
		{		
			SVM_PRD[g_i] = PRD[g_i];   //PRD和CAD的动作均为index=0时设置的SVM下降沿动作，ZRO和CAU动作为index=11时设置的SHE上升沿动作，用来判断SVM到SHE的切换是否存在最小脉宽
			SVM_CAD[g_i] = CAD[g_i];
			PWMdata->EPwm1PRDPre =  PRD[g_i];  /* For the use in  MinPulseSetAse */
			PWMdata->EPwm1CADPre =  CAD[g_i]; 
		}
		else if(g_i==1)
		{		
			SVM_PRD[g_i] = PRD[g_i];
			SVM_CAD[g_i] = CAD[g_i];
			PWMdata->EPwm2PRDPre =  PRD[g_i];  /* For the use in  MinPulseSetAse */
			PWMdata->EPwm2CADPre =  CAD[g_i];												
		}	
		else if(g_i==2)
		{		
			SVM_PRD[g_i] = PRD[g_i];
			SVM_CAD[g_i] = CAD[g_i];
			PWMdata->EPwm3PRDPre =  PRD[g_i];  /* For the use in  MinPulseSetAse */
			PWMdata->EPwm3CADPre =  CAD[g_i];															
		}	
		else if(g_i==3)
		{		
			SVM_PRD[g_i] = PRD[g_i];
			SVM_CAD[g_i] = CAD[g_i];
			PWMdata->EPwm4PRDPre =  PRD[g_i];  /* For the use in  MinPulseSetAse */
			PWMdata->EPwm4CADPre =  CAD[g_i];															
		}	
		else if(g_i==4)
		{		
			SVM_PRD[g_i] = PRD[g_i];
			SVM_CAD[g_i] = CAD[g_i];
			PWMdata->EPwm5PRDPre =  PRD[g_i];  /* For the use in  MinPulseSetAse */
			PWMdata->EPwm5CADPre =  CAD[g_i];															
		}	
		else if(g_i==5)
		{		
			SVM_PRD[g_i] = PRD[g_i];
			SVM_CAD[g_i] = CAD[g_i];
			PWMdata->EPwm6PRDPre =  PRD[g_i];  /* For the use in  MinPulseSetAse */
			PWMdata->EPwm6CADPre =  CAD[g_i];															
		}		
	   
        /* SVMH */
		if((PWMdata->SVPWMType)==1)	
		{
			if( (g_i%2)==0 )/* as1 bs1 cs1 */
			{
				if(  (ZRO[g_i]==AQ_SET)&&(CAU[g_i]==AQ_TOGGLE)   )                         					
				{
					SHEReadyFLAG[g_i] = 0;                           						
				}
				else
					SHEReadyFLAG[g_i] = 1; 
							
			}	
			else
			{
				if(  ((ZRO[g_i]==AQ_CLEAR)&&(CAU[g_i]==AQ_NO_ACTION) )||( (ZRO[g_i]==AQ_SET)&&(CAU[g_i]==AQ_NO_ACTION))   )																	    	
				{
					SHEReadyFLAG[g_i] = 1;                        	
				}
				else if(  (ZRO[g_i]==AQ_CLEAR)&&(CAU[g_i]==AQ_TOGGLE)   )  
				{
					if(  (SVM_PRD[g_i]==AQ_SET)&&((SVM_CAD[g_i]==AQ_NO_ACTION)||(SVM_CAD[g_i]==AQ_SET))  )	
					{
						SHEReadyFLAG[g_i] = 0;                          							 	
					}	        													
					else/* SVM_LHL */
						SHEReadyFLAG[g_i] = 1;              													
				}
				else if(  (ZRO[g_i]==AQ_SET)&&(CAU[g_i]==AQ_TOGGLE)   )         
				{
					if(  (SVM_PRD[g_i]==AQ_SET)&&((SVM_CAD[g_i]==AQ_NO_ACTION)||(SVM_CAD[g_i]==AQ_SET)  ))
					{
						SHEReadyFLAG[g_i] = 1;                          							 	
					}	        													
					else/* SVM_LHL */
						SHEReadyFLAG[g_i] = 0;                            						
				}
			}	
		}
	  
	    /* SVML */
		else                     	
		{
			if( (g_i%2)==0 )
			{
				if(  ((ZRO[g_i]==AQ_CLEAR)&&(CAU[g_i]==AQ_NO_ACTION) )||( (ZRO[g_i]==AQ_SET)&&(CAU[g_i]==AQ_NO_ACTION)))																	    	                        	      	
				{
					SHEReadyFLAG[g_i] = 1;                        	
				}
				else if(  (ZRO[g_i]==AQ_CLEAR)&&(CAU[g_i]==AQ_TOGGLE))                          					
				{
					if(  (SVM_PRD[g_i]==AQ_CLEAR)&&((SVM_CAD[g_i]==AQ_NO_ACTION)||(SVM_CAD[g_i]==AQ_CLEAR)))		
						SHEReadyFLAG[g_i] = 1; 
					else if(  (SVM_PRD[g_i]==AQ_CLEAR)&&(SVM_CAD[g_i]==AQ_TOGGLE))	            													
						SHEReadyFLAG[g_i] = 0;             													
					else if(  (SVM_PRD[g_i]==AQ_SET)&&(SVM_CAD[g_i]==AQ_TOGGLE))		
						SHEReadyFLAG[g_i] = 1;             													
				}
				else if(  (ZRO[g_i]==AQ_SET)&&(CAU[g_i]==AQ_TOGGLE))         
				{
					if(  (SVM_PRD[g_i]==AQ_CLEAR)&&((SVM_CAD[g_i]==AQ_NO_ACTION)||(SVM_CAD[g_i]==AQ_CLEAR)))		
						SHEReadyFLAG[g_i] = 0; 
					else if(  (SVM_PRD[g_i]==AQ_CLEAR)&&(SVM_CAD[g_i]==AQ_TOGGLE))	
						SHEReadyFLAG[g_i] = 1;             													
					else if(  (SVM_PRD[g_i]==AQ_SET)&&(SVM_CAD[g_i]==AQ_TOGGLE))	
						SHEReadyFLAG[g_i] = 0;                             						
				}	
			}
			else
			{
				if(  ((ZRO[g_i]==AQ_CLEAR)&&(CAU[g_i]==AQ_NO_ACTION) )||( (ZRO[g_i]==AQ_SET)&&(CAU[g_i]==AQ_NO_ACTION))   )																	    	                        					
				{
					SHEReadyFLAG[g_i] = 1;                        	
				}
				else if(  (ZRO[g_i]==AQ_CLEAR)&&(CAU[g_i]==AQ_TOGGLE)   )  
				{
					if(  (SVM_PRD[g_i]==AQ_SET)&&((SVM_CAD[g_i]==AQ_NO_ACTION)||(SVM_CAD[g_i]==AQ_SET))  )	
						SHEReadyFLAG[g_i] = 0;                       							 	
					else if(  (SVM_PRD[g_i]==AQ_SET)&&(SVM_CAD[g_i]==AQ_TOGGLE))
						SHEReadyFLAG[g_i] = 1;
					else if(  (SVM_PRD[g_i]==AQ_CLEAR)&&(SVM_CAD[g_i]==AQ_TOGGLE)  )	
						SHEReadyFLAG[g_i] = 0;                   						              													
				}
				else if(  (ZRO[g_i]==AQ_SET)&&(CAU[g_i]==AQ_TOGGLE)   )         	
				{
					if(  (SVM_PRD[g_i]==AQ_SET)&&((SVM_CAD[g_i]==AQ_NO_ACTION)||(SVM_CAD[g_i]==AQ_SET)))	
						SHEReadyFLAG[g_i] = 1;                       							 	
					else if(  (SVM_PRD[g_i]==AQ_SET)&&(SVM_CAD[g_i]==AQ_TOGGLE)  )
						SHEReadyFLAG[g_i] = 0;
					else if(  (SVM_PRD[g_i]==AQ_CLEAR)&&(SVM_CAD[g_i]==AQ_TOGGLE)  )	
						SHEReadyFLAG[g_i] = 1;                      						
				}	
			}	                        	
			
		}	
	}
	if(  (SHEReadyFLAG[0]==1)&&(SHEReadyFLAG[1]==1)&&(SHEReadyFLAG[2]==1)&&(SHEReadyFLAG[3]==1)&&(SHEReadyFLAG[4]==1)&&(SHEReadyFLAG[5]==1)  )                /* 1:6OK */
		PWMdata->SHEReady = 1;
	else
		PWMdata->SHEReady = 0;
}
/**************************************************************************************************************************************/


	
