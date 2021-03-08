/*  
* target:  invPWM1 invPWM2 board  
* Copyright 2018,4,16 IEE, Inc. 
* description:  整流PWM SVPWM算法核心程序 * 
* modification history: yzg 2006
* 						cdd 2015
*                       wxx 20180606
*/
 
#include "pwmControl.h"
#include "GlobalVar_Rec.h"
#include "variable_main.h"
#include <math.h>
/**************************************************************************************************************************************/
/*Name		:SVPWM_VSR
*Description:整流SVPWM算法
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void SVPWM_VSR(struct PWMdataStruct *PWMdata)
{

    Gama_SVMRec = PWMdata->VFGama;
    //Gama_SVMRec =  Gama_SVMRec+2*PI*PWMdata->vvvf1/1200./2+2*PI*PWMdata->vvvf1/1200.;
	while( Gama_SVMRec<0)       Gama_SVMRec= Gama_SVMRec+2*PI;
	while( Gama_SVMRec>2*PI)     Gama_SVMRec= Gama_SVMRec-2*PI;
	KvRec = PWMdata->Kv;
    /* ------------   SVM Core Caculation Start ------------------------ */
	Sector_NumRec = (int)(Gama_SVMRec/DEG60) ;
	if(Sector_NumRec == 6)
	{Sector_NumRec = 0;}
	Gama_SVMRec -=Sector_NumRec*DEG60; /*  All to Sector 1 */

	/* *************** Calc ta0Rec tb0Rec tc0Rec  ********************** */
	ta0Rec = T1PRHalfSVM_Rec*( 1 - 2*KvRec*sin(Gama_SVMRec) );		
	tb0Rec = T1PRHalfSVM_Rec*( 2*KvRec*sin(DEG60 + Gama_SVMRec) - 1 );		
	tc0Rec = T1PRHalfSVM_Rec*( 1 - 2*KvRec*sin(DEG60 - Gama_SVMRec) ); 	/* tc0Rec = T1PR - ta0Rec - tb0Rec; */

	/* ************ Calc taRec tbRec tcRec and get the Region_NumRec*************** */
	if ((ta0Rec>=0)&& (tb0Rec>=0)&& (tc0Rec>=0))
	{
		taRec=ta0Rec;
		tbRec=tb0Rec;
		tcRec=tc0Rec;
		if (Gama_SVMRec<DEG30)
		   {Region_NumRec=3;}
		else
		   {Region_NumRec=2;}
	}
	if (ta0Rec<0)
	{
		taRec = - ta0Rec;
		tcRec = T1PRHalfSVM_Rec - tb0Rec;
		tbRec = T1PRHalfSVM_Rec - tc0Rec;
		Region_NumRec=4;
	} 
	if (tb0Rec<0)
	{   
		tbRec =  - tb0Rec;
		taRec = T1PRHalfSVM_Rec - tc0Rec;
		tcRec = T1PRHalfSVM_Rec - ta0Rec;
		
		if (Gama_SVMRec<DEG30)
		   {Region_NumRec=1;}
		else
		   {Region_NumRec=0;}
	}
	if (tc0Rec<0)
	{   
		tcRec  = - tc0Rec ;
		taRec  = T1PRHalfSVM_Rec - tb0Rec;
		tbRec  = T1PRHalfSVM_Rec - ta0Rec;
		Region_NumRec = 5;
	}

    /* **************** Calc t1Rec t2Rec t3Rec for DSP CMPR****************** */
    /*  direct NP Change 20100429 */
	if ((Sector_NumRec==1)||(Sector_NumRec==3)||(Sector_NumRec==5))/*  Sector=1 3 5 all n type */
	{    
	    NtypeRec=1;PtypeRec=0;
		if ((Region_NumRec==1 )|| (Region_NumRec==3 )||(Region_NumRec==5)) /* angle less than 30deg          */
		{ 
		    t1Rec = taRec * 0.5 * (1 - PWMdata->NeuVoltCtl_deltTRec);/* N-type time decrease - */
		    t2Rec = t1Rec +  tbRec;
		    t3Rec = t2Rec +  tcRec;
		} 
		else    /* angle bigger than 30deg */
		{
		   t1Rec = tcRec * 0.5 * (1 - PWMdata->NeuVoltCtl_deltTRec);/* P-type time increase + */
		   t2Rec = t1Rec +  taRec;
		   t3Rec = t2Rec +  tbRec;
		}
	}
	else    /*  Sector_NumRec=0 2 4 all P type                                                  */
	{    
	    NtypeRec=0;PtypeRec=1; 
		if ((Region_NumRec==1 )|| (Region_NumRec==3 )||(Region_NumRec==5)) /* angle less than 30deg          */
		{ 
		   t1Rec = taRec * 0.5 * (1 + PWMdata->NeuVoltCtl_deltTRec);/* P-type time increase + */
		   t2Rec = t1Rec +  tbRec;
		   t3Rec = t2Rec +  tcRec;
		} 
		else  /* angle bigger than 30deg */
		{                               
		   t1Rec = tcRec * 0.5 * (1 + PWMdata->NeuVoltCtl_deltTRec);/* N-type time decrease - */
		   t2Rec = t1Rec +  taRec;
		   t3Rec = t2Rec +  tbRec;
		}
	} 
    if ((PWMdata->Reg_UpDown)==1)/* //Down set Up */  /* 20160706 0->1 */
    {
	    NPC_SVM_SetAseRec();
	}
    else if ((PWMdata->Reg_UpDown)==0)/* //Up set Down Up-->1 */  /* 20160706 1->0 */
    {
	    NPC_SVM_SetADesRec();
	}
	MP_testRec=1;/* 0 for without MP_Rec test */

   Minimum_Pulse_TLCRec(PWMdata);  
	
    epwm1cmpaRec = *(NPtimeTableRec[Sector_NumRec][Region_NumRec][0]);
    epwm2cmpaRec = *(NPtimeTableRec[Sector_NumRec][Region_NumRec][0]);
    epwm3cmpaRec = *(NPtimeTableRec[Sector_NumRec][Region_NumRec][1]);
    epwm4cmpaRec = *(NPtimeTableRec[Sector_NumRec][Region_NumRec][1]);
    epwm5cmpaRec = *(NPtimeTableRec[Sector_NumRec][Region_NumRec][2]);
    epwm6cmpaRec = *(NPtimeTableRec[Sector_NumRec][Region_NumRec][2]);

    PWMdata->CMPA[0] = epwm1cmpaRec;
    PWMdata->CMPA[1] = epwm2cmpaRec;
    PWMdata->CMPA[2] = epwm3cmpaRec;
    PWMdata->CMPA[3] = epwm4cmpaRec;
    PWMdata->CMPA[4] = epwm5cmpaRec;
    PWMdata->CMPA[5] = epwm6cmpaRec;
	
	PWMdata->ACQ_ZRO=ZRORec[0]|(ZRORec[1]<<2)|(ZRORec[2]<<4)|(ZRORec[3]<<6)|(ZRORec[4]<<8)|(ZRORec[5]<<10)&0xfff;
	PWMdata->ACQ_CAU=CAURec[0]|(CAURec[1]<<2)|(CAURec[2]<<4)|(CAURec[3]<<6)|(CAURec[4]<<8)|(CAURec[5]<<10)&0xfff;
	PWMdata->ACQ_PRD=PRDRec[0]|(PRDRec[1]<<2)|(PRDRec[2]<<4)|(PRDRec[3]<<6)|(PRDRec[4]<<8)|(PRDRec[5]<<10)&0xfff;
	PWMdata->ACQ_CAD=CADRec[0]|(CADRec[1]<<2)|(CADRec[2]<<4)|(CADRec[3]<<6)|(CADRec[4]<<8)|(CADRec[5]<<10)&0xfff;
}
/**************************************************************************************************************************************/
/*Name		:NPC_SVM_SetAseRec
*Description:定时器增计数时比较方式寄存器和比较寄存器设置
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void NPC_SVM_SetAseRec(void)   
{
  /* ======================== */

   ZRORec[0] = (ActTb_EPwm1Rec[Sector_NumRec][Region_NumRec][0]); 				
   CAURec[0] = (ActTb_EPwm1Rec[Sector_NumRec][Region_NumRec][1]); 

   ZRORec[1] = (ActTb_EPwm2Rec[Sector_NumRec][Region_NumRec][0]);   				
   CAURec[1] = (ActTb_EPwm2Rec[Sector_NumRec][Region_NumRec][1]); 

   ZRORec[2] = (ActTb_EPwm3Rec[Sector_NumRec][Region_NumRec][0]);   				
   CAURec[2] = (ActTb_EPwm3Rec[Sector_NumRec][Region_NumRec][1]); 

   ZRORec[3] = (ActTb_EPwm4Rec[Sector_NumRec][Region_NumRec][0]);   				
   CAURec[3] = (ActTb_EPwm4Rec[Sector_NumRec][Region_NumRec][1]);

   ZRORec[4] = (ActTb_EPwm5Rec[Sector_NumRec][Region_NumRec][0]);   				
   CAURec[4] = (ActTb_EPwm5Rec[Sector_NumRec][Region_NumRec][1]); 

   ZRORec[5] = (ActTb_EPwm6Rec[Sector_NumRec][Region_NumRec][0]);   				
   CAURec[5] = (ActTb_EPwm6Rec[Sector_NumRec][Region_NumRec][1]); 	
}/* NPC_SVM_SetAseRec */
/**************************************************************************************************************************************/
/*Name		:NPC_SVM_SetADesRec
*Description:定时器减计数时比较方式寄存器和比较寄存器设置
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void NPC_SVM_SetADesRec(void)
{                                         
   
/* ======================== */
   PRDRec[0] = (ActTb_EPwm1Rec[Sector_NumRec][Region_NumRec][2]); 				
   CADRec[0] = (ActTb_EPwm1Rec[Sector_NumRec][Region_NumRec][3]); 

   PRDRec[1] = (ActTb_EPwm2Rec[Sector_NumRec][Region_NumRec][2]);   				
   CADRec[1] = (ActTb_EPwm2Rec[Sector_NumRec][Region_NumRec][3]); 

   PRDRec[2] = (ActTb_EPwm3Rec[Sector_NumRec][Region_NumRec][2]);   				
   CADRec[2] = (ActTb_EPwm3Rec[Sector_NumRec][Region_NumRec][3]); 

   PRDRec[3] = (ActTb_EPwm4Rec[Sector_NumRec][Region_NumRec][2]);   				
   CADRec[3] = (ActTb_EPwm4Rec[Sector_NumRec][Region_NumRec][3]); 

   PRDRec[4] = (ActTb_EPwm5Rec[Sector_NumRec][Region_NumRec][2]);   				
   CADRec[4] = (ActTb_EPwm5Rec[Sector_NumRec][Region_NumRec][3]); 

   PRDRec[5] = (ActTb_EPwm6Rec[Sector_NumRec][Region_NumRec][2]);   				
   CADRec[5] = (ActTb_EPwm6Rec[Sector_NumRec][Region_NumRec][3]); 

}/* NPC_SVM_SetADesRec */

/**********************************************************/
/*Name		:Minimum_Pulse_TLCRec
*Description:整流PWM最小脉宽处理
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by TLC
*/
void Minimum_Pulse_TLCRec(struct PWMdataStruct *PWMdata)/* svmtlc   in1--MP_testRec  in2--VBC_test  in3--NPChangeRec  in4--MP这个数值是必改的。in5--T1PRHalfSVM_Rec  in6--MPhalf必改！ */
/* svmtlc 理论上Minimum_Pulse_TLC应该放置在VBC之后！ */
/* in1--MP_test该值必须为1，否则不执行最小脉宽处理 */
/* svmtlc VBC_test==1表示进行中点平衡处理，VBC_test==0表示不进行处理 */
/* MP_Rec, T1PRHalfSVM_Rec, MPhalfRec,  */
{
/* ////////////////  最小脉宽处理 Start ///////////////// */
	pt1Rec=t1Rec;pt2Rec=t2Rec;pt3Rec=t3Rec;
	pt2t1Rec=pt2Rec-pt1Rec;
	pt3t2Rec=pt3Rec-pt2Rec;
	pt3t1Rec=pt3Rec-pt1Rec;

	 if (PWMdata->NeuVoltCtl_deltTRec<=0)
	    {
	     if (NtypeRec==1)
	       {NPChangeRec=1;}
	     else if (PtypeRec==1)
	       {NPChangeRec=0;}
	    }   
	  else if (PWMdata->NeuVoltCtl_deltTRec>0)
	    {
	     if (NtypeRec==1)
	        {NPChangeRec=0;}
	     else if (PtypeRec==1)
	        {NPChangeRec=1;}
	    }	 
	MP_Debug=0;
	MP_testRec = 1;
		
	if(MP_testRec==1)
	{
		if (NPChangeRec==1)   /* 拆中间补两边 *//* N-type time increase */
		/* if (NPChangeRec==0) */  /*20150820 cdd*/
		{
		if ((pt1Rec<MP_Rec)||((T1PRHalfSVM_Rec-pt3Rec)<MP_Rec))
		{   /* MP_Debug=-1; first */
		    if ((pt1Rec+T1PRHalfSVM_Rec-pt3Rec)>=MP_Rec)/* pt1Rec+delt_t3>MP_Rec    */
			{
				MP_Debug=-1;
				if(pt3t2Rec>=MP_Rec)
				{t1Rec=(pt1Rec+T1PRHalfSVM_Rec-pt3Rec); t2Rec=T1PRHalfSVM_Rec-pt3t2Rec; t3Rec=T1PRHalfSVM_Rec;}/*  right */
				else if ((pt3t2Rec>=MPhalfRec)&&(pt3t2Rec<MP_Rec))
				{
				    if (pt3t1Rec>=MP_Rec)
					{t1Rec=(pt1Rec+T1PRHalfSVM_Rec-pt3Rec); t2Rec=T1PRHalfSVM_Rec-MP_Rec; t3Rec=T1PRHalfSVM_Rec;}
					else if ((pt3t1Rec>=MPhalfRec)&&(pt3t1Rec<MP_Rec))/* pt3t1Rec>=pt3t2Rec */
					{t1Rec=T1PRHalfSVM_Rec-MP_Rec; t2Rec=T1PRHalfSVM_Rec-MP_Rec; t3Rec=T1PRHalfSVM_Rec;}
				}    
				else if (pt3t2Rec<MPhalfRec)
				{
					if (pt3t1Rec>=MP_Rec)/* pt3t1Rec>=pt3t2Rec */
					{t1Rec=(pt1Rec+T1PRHalfSVM_Rec-pt3Rec); t2Rec=T1PRHalfSVM_Rec; t3Rec=T1PRHalfSVM_Rec;}
					else if ((pt3t1Rec>=MPhalfRec)&&(pt3t1Rec<MP_Rec))
					{t1Rec=T1PRHalfSVM_Rec-MP_Rec; t2Rec=T1PRHalfSVM_Rec; t3Rec=T1PRHalfSVM_Rec;}
					else if (pt3t1Rec<MPhalfRec)
					{t1Rec=T1PRHalfSVM_Rec; t2Rec=T1PRHalfSVM_Rec; t3Rec=T1PRHalfSVM_Rec;}
				}        
			} 
		    else if (((pt1Rec+T1PRHalfSVM_Rec-pt3Rec)>=MPhalfRec)&&((pt1Rec+T1PRHalfSVM_Rec-pt3Rec)<MP_Rec))
			{   /* MP_Debug=-2;//20091127 */
			    if(pt3t2Rec>=MP_Rec)/* pt3t1Rec>=pt3t2Rec */
			    {/* MP_Debug=-2;//20091127 */
			       if ((T1PRHalfSVM_Rec-pt3t2Rec)<MP_Rec)
				   {t1Rec=MP_Rec; t2Rec=t1Rec+pt2t1Rec; t3Rec=T1PRHalfSVM_Rec;MP_Debug=-2;}/* Problems20091127 MP_Debug=-2; */
			   else
				  {t1Rec=MP_Rec; t2Rec=T1PRHalfSVM_Rec-pt3t2Rec; t3Rec=T1PRHalfSVM_Rec;}/*  right */
			    }   
			    else if ((pt3t2Rec>=MPhalfRec)&&(pt3t2Rec<MP_Rec))
			    {t1Rec=MP_Rec; t2Rec=T1PRHalfSVM_Rec-MP_Rec; t3Rec=T1PRHalfSVM_Rec;}
			    else if (pt3t2Rec<MPhalfRec)
			    {t1Rec=MP_Rec; t2Rec=T1PRHalfSVM_Rec; t3Rec=T1PRHalfSVM_Rec;}
			}  
		    else if ((pt1Rec+T1PRHalfSVM_Rec-pt3Rec)<MPhalfRec)
			{
			    MP_Debug=-3;
			    if(pt3t2Rec>=MP_Rec)/* pt3t1Rec>=pt3t2Rec */
			    {
			       if ((T1PRHalfSVM_Rec-pt3t2Rec)>=MP_Rec)
				   {t1Rec=0; t2Rec=T1PRHalfSVM_Rec-pt3t2Rec; t3Rec=T1PRHalfSVM_Rec;}/*  right */
			       else if (((T1PRHalfSVM_Rec-pt3t2Rec)>=MPhalfRec)&&((T1PRHalfSVM_Rec-pt3t2Rec)<MP_Rec))
				   {t1Rec=0; t2Rec=MP_Rec; t3Rec=T1PRHalfSVM_Rec;}/* t2Rec=T1PRHalfSVM_Rec-MP_Rec; */
			       else if ((T1PRHalfSVM_Rec-pt3t2Rec)<MPhalfRec)
				  {t1Rec=0; t2Rec=0;t3Rec=T1PRHalfSVM_Rec;}
			    }      
			    else if ((pt3t2Rec>=MPhalfRec)&&(pt3t2Rec<MP_Rec))
			    {t1Rec=0; t2Rec=T1PRHalfSVM_Rec-MP_Rec; t3Rec=T1PRHalfSVM_Rec;}/*  */
			    else if (pt3t2Rec<MPhalfRec)
			    {t1Rec=0; t2Rec=T1PRHalfSVM_Rec; t3Rec=T1PRHalfSVM_Rec;}/*  */
			}		 
		}/* if ((pt1Rec<MP_Rec)||((T1PRHalfSVM_Rec-pt3Rec)<MP_Rec))       */
	    }/* (NPChangeRec==1) NeuVoltCtl_deltTRec<0 */
		else   /* 拆两边补中间 */ /*  N-type time decrease, P-type increase */
		{  
		    if ((pt1Rec<MP_Rec)||((T1PRHalfSVM_Rec-pt3Rec)<MP_Rec))
		    {MP_Debug=1;
		    if ((pt1Rec+T1PRHalfSVM_Rec-pt3Rec)>=MP_Rec)/* delt_t3>MP_Rec */
			{
			    if(pt2t1Rec>=MP_Rec)
			    {t1Rec=0; t2Rec=pt2t1Rec; t3Rec=pt3t1Rec;}/* =(T1PRHalfSVM_Rec-(pt1Rec+T1PRHalfSVM_Rec-pt3Rec)) right */
			    else if ((pt2t1Rec>=MPhalfRec)&&(pt2t1Rec<MP_Rec))
			    {
				    if (pt3t1Rec>=MP_Rec)
					{t1Rec=0; t2Rec=MP_Rec; t3Rec=pt3t1Rec;}
				    else if ((pt3t1Rec>=MPhalfRec)&&(pt3t1Rec<MP_Rec))/* pt3t1Rec>=pt2t1Rec */
					{t1Rec=0; t2Rec=MP_Rec; t3Rec=MP_Rec;}/* or MP_Rec+pt3t2Rec */
			    }    
			    else if (pt2t1Rec<MPhalfRec)
			    {
				    if (pt3t1Rec>=MP_Rec)
					{t1Rec=0; t2Rec=0; t3Rec=pt3t1Rec;}
				    else if ((pt3t1Rec>=MPhalfRec)&&(pt3t1Rec<MP_Rec))
					{t1Rec=0; t2Rec=0; t3Rec=MP_Rec;}
				    else if (pt3t1Rec<MPhalfRec)
					{t1Rec=0; t2Rec=0; t3Rec=0;}
			    }        
			} 
		    else if (((pt1Rec+T1PRHalfSVM_Rec-pt3Rec)>=MPhalfRec)&&((pt1Rec+T1PRHalfSVM_Rec-pt3Rec)<MP_Rec))
			{
			    MP_Debug=2;
			    if(pt2t1Rec>=MP_Rec)
			    {
				    if ((T1PRHalfSVM_Rec-pt2t1Rec)<MP_Rec)
				    {t1Rec=0; t2Rec=T1PRHalfSVM_Rec-MP_Rec; t3Rec=T1PRHalfSVM_Rec-MP_Rec;}
				    else
				    {t1Rec=0; t2Rec=pt2t1Rec; t3Rec=T1PRHalfSVM_Rec-MP_Rec;}
			    }    
			    else if ((pt2t1Rec>=MPhalfRec)&&(pt2t1Rec<MP_Rec))
			    {t1Rec=0; t2Rec=MP_Rec; t3Rec=T1PRHalfSVM_Rec-MP_Rec;}
			    else if (pt2t1Rec<MPhalfRec)
			    {t1Rec=0; t2Rec=0; t3Rec=T1PRHalfSVM_Rec-MP_Rec;}
			}  
		    else if ((pt1Rec+T1PRHalfSVM_Rec-pt3Rec)<MPhalfRec)
			{
			    MP_Debug=3;
			    if(pt2t1Rec>=MP_Rec)
			    {
				    if ((T1PRHalfSVM_Rec-pt2t1Rec)<MP_Rec)
				    {t1Rec=0; t2Rec=T1PRHalfSVM_Rec-MP_Rec; t3Rec=T1PRHalfSVM_Rec;}
				    else
				    {t1Rec=0; t2Rec=pt2t1Rec; t3Rec=T1PRHalfSVM_Rec;}
			    }       
			   else if ((pt2t1Rec>=MPhalfRec)&&(pt2t1Rec<MP_Rec))
			   {t1Rec=0; t2Rec=MP_Rec; t3Rec=T1PRHalfSVM_Rec;}
			   else if (pt2t1Rec<MPhalfRec)
			   {t1Rec=0; t2Rec=0; t3Rec=T1PRHalfSVM_Rec;}
			}        
		    }/* if ((pt1Rec<MP_Rec)||((T1PRHalfSVM_Rec-pt3Rec)<MP_Rec)) */
		} /* else    */
		/* ////////////////  最小脉宽处理 End ///////////////// */
		}/* MP_testRec==1;  */

		if (NtypeRec==1)
		{
		 Ct1t2t3Rec=(Tbt1t2t3Rec[Sector_NumRec][Region_NumRec]);
		 BbitRec=floor(Ct1t2t3Rec*0.01);SbitRec=floor((Ct1t2t3Rec-BbitRec*100)*0.1);GbitRec=floor(Ct1t2t3Rec-BbitRec*100-SbitRec*10);
		}
		else if (PtypeRec==1)
		{
		 Ct1t2t3Rec=(TbPt1t2t3Rec[Sector_NumRec][Region_NumRec]);
		 BbitRec=floor(Ct1t2t3Rec*0.01);SbitRec=floor((Ct1t2t3Rec-BbitRec*100)*0.1);GbitRec=floor(Ct1t2t3Rec-BbitRec*100-SbitRec*10);

		 if((t1Rec==0)||(t1Rec==T1PRHalfSVM_Rec))
		   {t1Rec=T1PRHalfSVM_Rec-t1Rec;}

		 if((t2Rec==0)||(t2Rec==T1PRHalfSVM_Rec))
		   {t2Rec=T1PRHalfSVM_Rec-t2Rec;}

		 if((t3Rec==0)||(t3Rec==T1PRHalfSVM_Rec))
		   {t3Rec=T1PRHalfSVM_Rec-t3Rec;}
		}		
		
		/* NPCAUT */
		if ((PWMdata->Reg_UpDown)==0)  /* 20160706 0->1 */
		{
			if (((BbitRec==1)&&(t1Rec==0))||((SbitRec==1)&&(t2Rec==0))||((GbitRec==1)&&(t3Rec==0)))
			{PRDRec[0]=AQ_SET;CADRec[0]=AQ_SET;}
			else if (((BbitRec==1)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==1)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==1)&&(t3Rec==T1PRHalfSVM_Rec)))
			{PRDRec[0]=AQ_CLEAR;CADRec[0]=AQ_CLEAR;}
		
			if (((BbitRec==2)&&(t1Rec==0))||((SbitRec==2)&&(t2Rec==0))||((GbitRec==2)&&(t3Rec==0)))
			{PRDRec[1]=AQ_SET;CADRec[1]=AQ_SET;}
			else if (((BbitRec==2)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==2)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==2)&&(t3Rec==T1PRHalfSVM_Rec)))
			{PRDRec[1]=AQ_CLEAR;CADRec[1]=AQ_CLEAR;}
		
			if (((BbitRec==3)&&(t1Rec==0))||((SbitRec==3)&&(t2Rec==0))||((GbitRec==3)&&(t3Rec==0)))
			{PRDRec[2]=AQ_SET;CADRec[2]=AQ_SET;}
			else if (((BbitRec==3)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==3)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==3)&&(t3Rec==T1PRHalfSVM_Rec)))
			{PRDRec[2]=AQ_CLEAR;CADRec[2]=AQ_CLEAR;}

			if (((BbitRec==4)&&(t1Rec==0))||((SbitRec==4)&&(t2Rec==0))||((GbitRec==4)&&(t3Rec==0)))
			{PRDRec[3]=AQ_SET;CADRec[3]=AQ_SET;}
			else if (((BbitRec==4)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==4)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==4)&&(t3Rec==T1PRHalfSVM_Rec)))
			{PRDRec[3]=AQ_CLEAR;CADRec[3]=AQ_CLEAR;}

			if (((BbitRec==5)&&(t1Rec==0))||((SbitRec==5)&&(t2Rec==0))||((GbitRec==5)&&(t3Rec==0)))
			{PRDRec[4]=AQ_SET;CADRec[4]=AQ_SET;}
			else if (((BbitRec==5)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==5)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==5)&&(t3Rec==T1PRHalfSVM_Rec)))
			{PRDRec[4]=AQ_CLEAR;CADRec[4]=AQ_CLEAR;}

			if (((BbitRec==6)&&(t1Rec==0))||((SbitRec==6)&&(t2Rec==0))||((GbitRec==6)&&(t3Rec==0)))
			{PRDRec[5]=AQ_SET;CADRec[5]=AQ_SET;}
			else if (((BbitRec==6)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==6)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==6)&&(t3Rec==T1PRHalfSVM_Rec)))
			{PRDRec[5]=AQ_CLEAR;CADRec[5]=AQ_CLEAR;}
		}   
		else    
		{

			if (((BbitRec==1)&&(t1Rec==0))||((SbitRec==1)&&(t2Rec==0))||((GbitRec==1)&&(t3Rec==0)))
		    {ZRORec[0]=AQ_SET;CAURec[0]=AQ_SET;}
			else if (((BbitRec==1)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==1)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==1)&&(t3Rec==T1PRHalfSVM_Rec)))
			{ZRORec[0]=AQ_CLEAR;CAURec[0]=AQ_CLEAR;}

			if (((BbitRec==2)&&(t1Rec==0))||((SbitRec==2)&&(t2Rec==0))||((GbitRec==2)&&(t3Rec==0)))
			{ZRORec[1]=AQ_SET;CAURec[1]=AQ_SET;}
			else if (((BbitRec==2)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==2)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==2)&&(t3Rec==T1PRHalfSVM_Rec)))
			{ZRORec[1]=AQ_CLEAR;CAURec[1]=AQ_CLEAR;}

			if (((BbitRec==3)&&(t1Rec==0))||((SbitRec==3)&&(t2Rec==0))||((GbitRec==3)&&(t3Rec==0)))
			{ZRORec[2]=AQ_SET;CAURec[2]=AQ_SET;}
			else if (((BbitRec==3)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==3)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==3)&&(t3Rec==T1PRHalfSVM_Rec)))
			{ZRORec[2]=AQ_CLEAR;CAURec[2]=AQ_CLEAR;}

			if (((BbitRec==4)&&(t1Rec==0))||((SbitRec==4)&&(t2Rec==0))||((GbitRec==4)&&(t3Rec==0)))
			{ZRORec[3]=AQ_SET;CAURec[3]=AQ_SET;}
			else if (((BbitRec==4)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==4)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==4)&&(t3Rec==T1PRHalfSVM_Rec)))
			{ZRORec[3]=AQ_CLEAR;CAURec[3]=AQ_CLEAR;}

			if (((BbitRec==5)&&(t1Rec==0))||((SbitRec==5)&&(t2Rec==0))||((GbitRec==5)&&(t3Rec==0)))
			{ZRORec[4]=AQ_SET;CAURec[4]=AQ_SET;}
			else if (((BbitRec==5)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==5)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==5)&&(t3Rec==T1PRHalfSVM_Rec)))
			{ZRORec[4]=AQ_CLEAR;CAURec[4]=AQ_CLEAR;}

			if (((BbitRec==6)&&(t1Rec==0))||((SbitRec==6)&&(t2Rec==0))||((GbitRec==6)&&(t3Rec==0)))
			{ZRORec[5]=AQ_SET;CAURec[5]=AQ_SET;}
			else if (((BbitRec==6)&&(t1Rec==T1PRHalfSVM_Rec))||((SbitRec==6)&&(t2Rec==T1PRHalfSVM_Rec))||((GbitRec==6)&&(t3Rec==T1PRHalfSVM_Rec)))
			{ZRORec[5]=AQ_CLEAR;CAURec[5]=AQ_CLEAR;}
		} 
	}  /*  end of Minimum_Pulse_TLC() */
/**************************************************************************************************************************************/
/*Name		:VdcDevi2deltT
*Description:整流中点电容电压平衡算法
*Input		:PWMdata->Udc_up_filter,PWMdata->Udc_down_filter/Ia,Ib,Ic/VFGama/Ki_uzRec,Kp_uzRec/NeuVoltCtl_sum	
*Output		:无
*Return		:无
*History	:2016,by TLC
*/
/* Rectifier Neutral Point Voltage balancing
 * INPUT: PWMdata->Udc_up_filter,PWMdata->Udc_down_filter/Ia,Ib,Ic/VFGama/Ki_uzRec,Kp_uzRec/NeuVoltCtl_sum
 * OUTPUT: NeuVoltCtl_deltTRec
 * PARAMETERS: MaxVdcDeviRec,lim_uz,
 */
//void VdcDevi2deltT(struct PWMdataStruct *PWMdata)
//{
//    Vdc1MinuVdc2Rec = fabs(PWMdata->Udc_up_filter) - fabs(PWMdata->Udc_down_filter); /* 20100702 */
//	if (PWMdata->Ia>=0)
//	   sign_iaRec=1;
//	else
//	   sign_iaRec=-1;
//
//	if (PWMdata->Ib>=0)
//	   sign_ibRec=1;
//	else
//	   sign_ibRec=-1;    
//
//	if (PWMdata->Ic>=0)
//	   sign_icRec=1;
//	else
//	   sign_icRec=-1;
//     /**********   the regenerative voltage balance algorithms   **********/
//    Gama_SVM_DeltTRec = PWMdata->VFGama;  /* Gama_SVM_DeltTRec is used for calc NeuVolt */
//	
//	    if (((Gama_SVM_DeltTRec>=11*PIVSR/6)&&(Gama_SVM_DeltTRec<=2*PIVSR))||((Gama_SVM_DeltTRec>=0)&&(Gama_SVM_DeltTRec<PIVSR/6)))
//      { Vdc1MinuVdc2Rec=-Vdc1MinuVdc2Rec*sign_iaRec;
//	  }   
//    else if ((Gama_SVM_DeltTRec>=PIVSR/6)&&(Gama_SVM_DeltTRec<PIVSR/2))
//      {Vdc1MinuVdc2Rec=Vdc1MinuVdc2Rec*sign_icRec;
//	   }
//    else if ((Gama_SVM_DeltTRec>=PIVSR/2)&&(Gama_SVM_DeltTRec<5*PIVSR/6))
//      {Vdc1MinuVdc2Rec=-Vdc1MinuVdc2Rec*sign_ibRec;
//	   }
//    else if ((Gama_SVM_DeltTRec>=5*PIVSR/6)&&(Gama_SVM_DeltTRec<7*PIVSR/6))
//      {Vdc1MinuVdc2Rec=Vdc1MinuVdc2Rec*sign_iaRec;
//       }       
//    else if ((Gama_SVM_DeltTRec>=7*PIVSR/6)&&(Gama_SVM_DeltTRec<9*PIVSR/6))
//      {Vdc1MinuVdc2Rec=-Vdc1MinuVdc2Rec*sign_icRec;
//	   Nostep=1;}
//    else if ((Gama_SVM_DeltTRec>=9*PIVSR/6)&&(Gama_SVM_DeltTRec<11*PIVSR/6))
//      {Vdc1MinuVdc2Rec=Vdc1MinuVdc2Rec*sign_ibRec;  
//       }
//	
// /* 20100702 */
//    if( fabs(Vdc1MinuVdc2Rec) > MaxVdcDeviRec)
//      {		
//		PWMdata->NeuVoltCtl_sum = PWMdata->NeuVoltCtl_sum + PWMdata->Ki_uzRec*Vdc1MinuVdc2Rec*TsSVM/2;
//  	  	if (PWMdata->NeuVoltCtl_sum>lim_uz)
//    	   PWMdata->NeuVoltCtl_sum=lim_uz;  	  
//    	if (PWMdata->NeuVoltCtl_sum<-lim_uz)
//      	   PWMdata->NeuVoltCtl_sum=-lim_uz;
//
//        NeuVoltCtl_deltTRec = PWMdata->NeuVoltCtl_sum + PWMdata->Kp_uzRec*Vdc1MinuVdc2Rec;
//  	  	if (NeuVoltCtl_deltTRec>lim_uz)
//    	   NeuVoltCtl_deltTRec=lim_uz;  	  
//    	if (NeuVoltCtl_deltTRec<-lim_uz)
//      	   NeuVoltCtl_deltTRec=-lim_uz; 
//     }
//	else
//		{
//		NeuVoltCtl_deltTRec = 0;
//		} 
//}  
