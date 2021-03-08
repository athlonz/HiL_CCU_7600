/*  
* target: AD、PWM hard protection and cpu soft protect   
* Copyright 2018,4,16 IEE, Inc. 
* description:  fault protection  
* function: 
* debug: 
* modification history:20180426 by wxx
*/
#include "vxWorks.h"
#include "taskLib.h"
#include "stdio.h"
#include "logLib.h"
#include "intLib.h"
#include "iv.h"  
#include "sysLib.h"
#include "vme.h"
#include "semLib.h"
#include "math.h"  
#include "ioLib.h"
#include "wdLib.h"

#include "variable_main.h"
#include "GlobalVar_Rec.h"
#include "GlobalVar_Inv.h"

#define   Iac_protect 8000//1500
#define   Idc_protect 3000 
#define   Udc_protect 8000
#define   UdcNobalane_protect 600

//double Udc_protect=6000;//530;//350;//250;//300;
//double UdcNobalane_protect=500;//80;//150;
//double Iinv_protect=3000;
//double Irec_protect=1000;

/************************************************************************************************/
/*Name		:clr_fault_VME
*Description:用VME写的方式清除故障状态
*Input		:无
*Output		:无
*Return		:无
*History	:2018,by wxx
*/
void clr_fault_VME(void)
{
  
   
  faultCode.convfaultcode1.word =0 ;
  faultCode.convfaultcode2.word =0 ;
  faultFlag =0;
  if(selfCheck.bit.recBoard1OK==1)
  *(short *)(RecLocalBaseAdrs1+0x36)=1;/*clear fault */
  if(selfCheck.bit.recBoard2OK==1)
  *(short *)(RecLocalBaseAdrs2+0x36)=1;/*clear fault */
  if(selfCheck.bit.invBoard1OK==1) 
  *(short *)(InvLocalBaseAdrs1+0x36)=1;/*clear fault */
  if(selfCheck.bit.invBoard2OK==1) 
  *(short *)(InvLocalBaseAdrs2+0x36)=1;/*clear fault */
  if(selfCheck.bit.clkBoardOK==1) 
  *(short *)(ClkLocalBaseAdrs+0x18)=1;
}
/************************************************************************************************/
/*Name		:fault_protect_VME
*Description:用VME读的方式监控故障状态信息
*Input		:无
*Output		:无
*Return		:无
*History	:2018,by wxx
*/
void fault_protect_VME(void)
	{
	  /*board selfcheck protect*/
	if(selfCheck.bit.clkBoardOK==0 )
	faultCode.convfaultcode2.bit.clkBoardErr=1;

	if(selfCheck.bit.recBoard1OK==0 )
	faultCode.convfaultcode2.bit.recBoard1Err=1;

	if(selfCheck.bit.recBoard2OK==0 && CPS_PowerLevel=="H")
	faultCode.convfaultcode2.bit.recBoard2Err=1;

	if(selfCheck.bit.invBoard1OK==0 )
	faultCode.convfaultcode2.bit.invBoard1Err=1;

	if(selfCheck.bit.invBoard2OK==0 && CPS_PowerLevel=="H")
	faultCode.convfaultcode2.bit.invBoard2Err=1;

	if(selfCheck.bit.adBoard1OK==0 )
	faultCode.convfaultcode2.bit.adBoard1Err=1;

	if(selfCheck.bit.adBoard2OK==0 && CPS_PowerLevel=="H")
	faultCode.convfaultcode2.bit.adBoard2Err=1;

	//if(selfCheck.bit.pmc5565OK==0 )
	//faultCode.convfaultcode2.bit.pmc5565Err=1;

	 /*hardware protect check*/
	if(selfCheck.bit.recBoard1OK==1)
	  {
		faultCode.rec1_igctfault=*(unsigned short *)(RecLocalBaseAdrs1+0x40);
		faultCode.rec1_MinPulseErr=*(unsigned short *)(RecLocalBaseAdrs1+0x3c);
		faultCode.rec1_DeadZoneErr=*(unsigned short *)(RecLocalBaseAdrs1+0x3e);
		faultCode.rec1_errstate=*(unsigned short *)(RecLocalBaseAdrs1+0x42);
	
	  }
	if(selfCheck.bit.recBoard2OK==1)
	  {
		faultCode.rec2_igctfault=*(unsigned short *)(RecLocalBaseAdrs2+0x40);
		faultCode.rec2_MinPulseErr=*(unsigned short *)(RecLocalBaseAdrs2+0x3c);
		faultCode.rec2_DeadZoneErr=*(unsigned short *)(RecLocalBaseAdrs2+0x3e);
		faultCode.rec2_errstate=*(unsigned short *)(RecLocalBaseAdrs2+0x42);
	  }
	if(selfCheck.bit.invBoard1OK==1) 
	  {
		faultCode.inv1_igctfault=*(unsigned short *)(InvLocalBaseAdrs1+0x40);
		faultCode.inv1_MinPulseErr=*(unsigned short *)(InvLocalBaseAdrs1+0x3c);
		faultCode.inv1_DeadZoneErr=*(unsigned short *)(InvLocalBaseAdrs1+0x3e);
		faultCode.inv1_errstate=*(unsigned short *)(InvLocalBaseAdrs1+0x42);
	  }
	if(selfCheck.bit.invBoard2OK==1) 
	  {
		faultCode.inv2_igctfault=*(unsigned short *)(InvLocalBaseAdrs2+0x40);
		faultCode.inv2_MinPulseErr=*(unsigned short *)(InvLocalBaseAdrs2+0x3c);
		faultCode.inv2_DeadZoneErr=*(unsigned short *)(InvLocalBaseAdrs2+0x3e);
		faultCode.inv2_errstate=*(unsigned short *)(InvLocalBaseAdrs2+0x42);
	  }

	 /*software protect*/
//	if(fabs(recIa1)>=Iac_protect)
//	faultCode.convfaultcode1.bit.recia1_overcurrent=1;
//	if(fabs(recIb1)>=Iac_protect)
//	faultCode.convfaultcode1.bit.recib1_overcurrent=1;
//	if(fabs(recIc1)>=Iac_protect)
//	faultCode.convfaultcode1.bit.recic1_overcurrent=1;
/*
	if(fabs(recIa2)>=Iac_protect)
	faultCode.convfaultcode1.bit.recia2_overcurrent=1;
	if(fabs(recIb2)>=Iac_protect)
	faultCode.convfaultcode1.bit.recib2_overcurrent=1;
	if(fabs(recIc2)>=Iac_protect)
	faultCode.convfaultcode1.bit.recic2_overcurrent=1;
*/
	if(fabs(invIu1)>=Iac_protect)
	faultCode.convfaultcode1.bit.invIu1_overcurrent=1;
	if(fabs(invIv1)>=Iac_protect)
	faultCode.convfaultcode1.bit.invIv1_overcurrent=1;
	if(fabs(invIw1)>=Iac_protect)
	faultCode.convfaultcode1.bit.invIw1_overcurrent=1;

	if(fabs(invIu2)>=Iac_protect)
	faultCode.convfaultcode1.bit.invIu2_overcurrent=1;
	if(fabs(invIv2)>=Iac_protect)
	faultCode.convfaultcode1.bit.invIv2_overcurrent=1;
	if(fabs(invIw2)>=Iac_protect)
	faultCode.convfaultcode1.bit.invIw2_overcurrent=1;

//	if(fabs(Idc1)>=Idc_protect)
//	faultCode.convfaultcode1.bit.Idc1_overcurrent=1;
//	if(fabs(Idc2)>=Idc_protect)
//	faultCode.convfaultcode1.bit.Idc2_overcurrent=1;

	if(fabs(Udc1+Udc2)>=Udc_protect)
	faultCode.convfaultcode1.bit.Udc_overVoltage=1;
	if(fabs(Udc1-Udc2)>=UdcNobalane_protect)
	faultCode.convfaultcode1.bit.Udc_Nobalance=1;
                     

	 /**********************sum up***********************************/ 
	if( faultCode.rec1_errstate !=0 || faultCode.rec2_errstate !=0 || faultCode.inv1_errstate !=0 ||
		faultCode.inv2_errstate !=0 || faultCode.convfaultcode1.word !=0 || faultCode.convfaultcode2.word !=0 )
	faultFlag =1;
	else
	faultFlag =0;

}
/************************************************************************************************/
/*Name		:clr_fault_SRIO
*Description:用SRIO写的方式清除故障状态
*Input		:无
*Output		:无
*Return		:无
*History	:2018,by wxx
*/
void clr_fault_SRIO(void)
{
     
  faultCode.convfaultcode1.word =0 ;
  faultCode.convfaultcode2.word =0 ;
  faultFlag =0;
  
  if(selfCheck.bit.recBoard1OK==1)
	  txClkSrioData.clrFaultCmd=1;/*clear fault */
  
  if(selfCheck.bit.recBoard2OK==1)
	  txRecSrioData1.clrFaultCmd=1;/*clear fault */
  
  if(selfCheck.bit.invBoard1OK==1) 
	  txRecSrioData2.clrFaultCmd=1;/*clear fault */
  
  if(selfCheck.bit.invBoard2OK==1) 
	  txInvSrioData1.clrFaultCmd=1;/*clear fault */
  
  if(selfCheck.bit.clkBoardOK==1) 
	  txInvSrioData2.clrFaultCmd=1;
  
  if(selfCheck.bit.adBoard1OK==1) 
	  txAdSrioData1.clrFaultCmd=1;
  
  if(selfCheck.bit.adBoard2OK==1) 
	  txAdSrioData2.clrFaultCmd=1;
  
}

/************************************************************************************************/
/*Name		:fault_protect_SRIO
*Description:用读SRIO的方式监控故障状态信息
*Input		:无
*Output		:无
*Return		:无
*History	:2018,by wxx
*/
void fault_protect_SRIO(void)
	{
	  /*board selfcheck protect*/
	if(selfCheck.bit.ast7700VMEOK==0 )
	{
		   err1++;
		if (err1>=1000)
			err1=1000;
		faultCode.convfaultcode2.bit.ast7700BoardErr=1;
//		if (err1==1)
//		logMsg("%d\tCurrent fault: ast7700VME_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
	}
	
	if(selfCheck.bit.clkBoardOK==0 )
	{
		 err2++;
		if (err2>=1000)
			err2=1000;
		faultCode.convfaultcode2.bit.clkBoardErr=1;
//		if (err2==1)
//	    logMsg("%d\tCurrent fault: clkBoard_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
	}

	if(selfCheck.bit.recBoard1OK==0 )
	{   
		 err3++;
		if (err3>=1000)
			err3=1000;
		faultCode.convfaultcode2.bit.recBoard1Err=1;
//		if (err3==1)
//		logMsg("%d\tCurrent fault: recBoard1_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
	}

	if(selfCheck.bit.recBoard2OK==0 )
	{
		 err4++;
		if (err4>=1000)
			err4=1000;
		faultCode.convfaultcode2.bit.recBoard2Err=1;
//		if (err4==1)
//		logMsg("%d\tCurrent fault: recBoard2_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
	}

	if(selfCheck.bit.invBoard1OK==0 )
	{
		 err5++;
		if (err5>=1000)
			err5=1000;
		faultCode.convfaultcode2.bit.invBoard1Err=1;
//		if (err5==1)
//		logMsg("%d\tCurrent fault: invBoard1_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
	}

	if(selfCheck.bit.invBoard2OK==0 )
	{
		 err6++;
	if (err6>=1000)
		err6=1000;
		faultCode.convfaultcode2.bit.invBoard2Err=1;
//		if (err6==1)
//	   logMsg("%d\tCurrent fault: invBoard2_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
		}

	if(selfCheck.bit.adBoard1OK==0 )
	{
		 err7++;
		if (err7>=1000)
			err7=1000;
		faultCode.convfaultcode2.bit.adBoard1Err=1;
//		if (err7==1)
//		logMsg("%d\tCurrent fault: adBoard1_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
	}

	if(selfCheck.bit.adBoard2OK==0 )
	{
		 err8++;
	if (err8>=1000)
		err8=1000;
		faultCode.convfaultcode2.bit.adBoard2Err=1;
//		if (err8==1)
//		logMsg("%d\tCurrent fault: adBoard2_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
	}
	
//	if(selfCheck.bit.IOBoardOK==0 )
//	{
//		 err9++;
//	if (err9>=1000)
//		err9=1000;
//		faultCode.convfaultcode2.bit.ioBoardErr=1;
//		if (err9==1)
//		logMsg("%d\tCurrent fault: IOBoard_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
//	}	
	
//	if(selfCheck.bit.pfbBoardOK==0 )
//	{
//		 err10++;
//		if (err10>=1000)
//			err10=1000;
//		faultCode.convfaultcode2.bit.pfbBoardErr=1;
//		if (err10==1)
//	   logMsg("%d\tCurrent fault: pfbBoard_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
//	}	
//	
//	if(selfCheck.bit.pmc5565OK==0 )
//	{
//		err11++;
//		if (err11>=1000)
//			err11=1000;
//		faultCode.convfaultcode2.bit.pmc5565Err=1;
//		if (err11==1)
//	logMsg("%d\tCurrent fault: pmc5565_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
//		}	
//	
	
	
	
	if(selfCheck.bit.ast7700SrioInitOK==0 )
	{
		err12++;
	if (err12>=1000)
		err12=1000;
		faultCode.convfaultcode2.bit.ast7700SrioInitErr=1;
//		if (err12==1)
//	logMsg("%d\tCurrent fault: ast7700SrioInit_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
	}
	
//	if(selfCheck.bit.clkSrioConnectOk==0 )
//	{
//		err13++;
//		if (err13>=1000)
//			err13=1000;
//		faultCode.convfaultcode2.bit.clkSrioConnectErr=1;
//		if (err13==1)
//	logMsg("%d\tCurrent fault: clkSrioConnect_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
//		}
	
//	if(selfCheck.bit.rec1SrioConnectOk==0 )
//	{
//		err14++;
//		if (err14>=1000)
//			err14=1000;
//		faultCode.convfaultcode2.bit.rec1SrioConnectErr=1;
////		if (err14==1)
////	logMsg("%d\tCurrent fault: rec1SrioConnect_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
//	}
//	
//	if(selfCheck.bit.rec2SrioConnectOk==0 )
//	{
//		err15++;
//	if (err15>=1000)
//		err15=1000;
//		faultCode.convfaultcode2.bit.rec2SrioConnectErr=1;
////		if (err15==1)
////	logMsg("%d\tCurrent fault: rec2SrioConnect_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
//	}
//	
//	if(selfCheck.bit.inv1SrioConnectOk==0 )
//	{
//		err16++;
//	if (err16>=1000)
//		err16=1000;
//		faultCode.convfaultcode2.bit.inv1SrioConnectErr=1;
////		if (err16==1)
////	logMsg("%d\tCurrent fault: inv1SrioConnect_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
//	}
//	
//	if(selfCheck.bit.inv2SrioConnectOk==0 )
//	{
//		err17++;
//	if (err17>=1000)
//		err17=1000;
//		faultCode.convfaultcode2.bit.inv2SrioConnectErr=1;
////		if (err17==1)
////	logMsg("%d\tCurrent fault: inv2SrioConnect_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
//		}
//	
	if(selfCheck.bit.ad1SrioConnectOk==0 )
	{
		err18++;
	if (err18>=1000)
		err18=1000;
		faultCode.convfaultcode2.bit.ad1SrioConnectErr=1;
//		if (err18==1)
//	logMsg("%d\tCurrent fault: ad1SrioConnect_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
		}
	
	if(selfCheck.bit.ad2SrioConnectOk==0 )
	{
		err19++;
		if (err19>=1000)
			err19=1000;
		faultCode.convfaultcode2.bit.ad2SrioConnectErr=1;	
//		if (err19==1)
//	logMsg("%d\tCurrent fault: ad2SrioConnect_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
	}
	
//	if(selfCheck.bit.GPSOK==0 )
//	{
//		err20++;
//	if (err20>=1000)
//		err20=1000;
//		faultCode.convfaultcode2.bit.GPSErr=1;	
//		if (err20==1)
//	logMsg("%d\tCurrent fault: GPS_SELFCHECK_fault\n\n",statecnt,0,0,0,0,0);
//	}

	 /*hardware protect check*/
	if(selfCheck.bit.recBoard1OK==1)
	{
		faultCode.rec1_igctfault=rxRecSrioData1->pwmfb_err;
		//faultCode.rec1_MinPulseErr=rxRecSrioData1->minPulseErr;
		//faultCode.rec1_DeadZoneErr=rxRecSrioData1->deadZoneErr;
		faultCode.rec1_errstate=rxRecSrioData1->global_err_state;
		
		faultCode.rec1_DeadZoneErr_A=rxRecSrioData1->DeadZoneErr_A;
		faultCode.rec1_DeadZoneErr_B=rxRecSrioData1->DeadZoneErr_B;
		faultCode.rec1_DeadZoneErr_C=rxRecSrioData1->DeadZoneErr_C;
		faultCode.rec1_PWM_shortCuirtErr_ANPC=rxRecSrioData1->PWM_shortCuirtErr_ANPC;
		faultCode.rec1_MinPulseErr_A=rxRecSrioData1->MinPulseErr_A; 
		faultCode.rec1_MinPulseErr_B=rxRecSrioData1->MinPulseErr_B; 
		faultCode.rec1_MinPulseErr_C=rxRecSrioData1->MinPulseErr_C;
		
		
		
		if(faultCode.rec1_igctfault!=0 )
				{	
					err[0]++;
				if (err[0]>=1000)
					err[0]=1000;
//				if (err[0]==1)
//				logMsg("%d\tCurrent fault: rec1_igctfault\n\n",statecnt,0,0,0,0,0);
				}
		
		if(faultCode.rec1_MinPulseErr_A!=0 )
		{	
			err[1]++;
		if (err[1]>=1000)
			err[1]=1000;
//		if (err[1]==1)
//		logMsg("%d\tCurrent fault: rec1_MinPulseErr_A\n\n",statecnt,0,0,0,0,0);
		}
		
		if(faultCode.rec1_MinPulseErr_B!=0 )
				{	
			       err[2]++;
				if ( err[2]>=1000)
					 err[2]=1000;
//				if ( err[2]==1)
//				logMsg("%d\tCurrent fault: rec1_MinPulseErr_B\n\n",statecnt,0,0,0,0,0);
				}
		
		if(faultCode.rec1_MinPulseErr_C!=0 )
				{	
			err[3]++;
				if (err[3]>=1000)
					err[3]=1000;
//				if (err[3]==1)
//				logMsg("%d\tCurrent fault: rec1_MinPulseErr_C\n\n",statecnt,0,0,0,0,0);
				}
		
		if(faultCode.rec1_DeadZoneErr_A!=0 )
				{	
					err[4]++;
				if (err[4]>=1000)
					err[4]=1000;
//				if (err[4]==1)
//				logMsg("%d\tCurrent fault: rec1_DeadZoneErr_A\n\n",statecnt,0,0,0,0,0);
				}
		
		if(faultCode.rec1_DeadZoneErr_B!=0 )
					{	
						err[5]++;
					if (err[5]>=1000)
						err[5]=1000;
//					if (err[5]==1)
//					logMsg("%d\tCurrent fault: rec1_DeadZoneErr_B\n\n",statecnt,0,0,0,0,0);
					}
		if(faultCode.rec1_DeadZoneErr_C!=0 )
					{	
						err[6]++;
					if (err[6]>=1000)
						err[6]=1000;
//					if (err[6]==1)
//					logMsg("%d\tCurrent fault: rec1_DeadZoneErr_C\n\n",statecnt,0,0,0,0,0);
					}
				
//		if(faultCode.rec1_MinPulseErr!=0 )
//		{	err22++;
//		if (err22>=1000)
//			err22=1000;
//		if (err22==1)
//		logMsg("%d\tCurrent fault: rec1_MinPulseErr\n\n",statecnt,0,0,0,0,0);
//		}
		
		if(faultCode.rec1_MinPulseErr!=0 )
				{	err22++;
				if (err22>=1000)
					err22=1000;
//				if (err22==1)
//				logMsg("%d\tCurrent fault: rec1_MinPulseErr\n\n",statecnt,0,0,0,0,0);
				}
		
		
		
		if(faultCode.rec1_DeadZoneErr!=0 )
		{	err23++;
		if (err23>=1000)
			err23=1000;
//		if (err23==1)
//		logMsg("%d\tCurrent fault: rec1_DeadZoneErr\n\n",statecnt,0,0,0,0,0);
		}
		if(faultCode.rec1_errstate!=0 )
		{	err24++;
		if (err24>=1000)
			err24=1000;
//		if (err24==1)
//		logMsg("%d\tCurrent fault: rec1_Err\n\n",statecnt,0,0,0,0,0);
		}
	}
	if(selfCheck.bit.recBoard2OK==1)
	{
		faultCode.rec2_igctfault=rxRecSrioData2->pwmfb_err;
		faultCode.rec2_MinPulseErr=rxRecSrioData2->minPulseErr;
		faultCode.rec2_DeadZoneErr=rxRecSrioData2->deadZoneErr;
		faultCode.rec2_errstate=rxRecSrioData2->global_err_state;
		if(faultCode.rec2_igctfault!=0 )
		{	err25++;
		if (err25>=1000)
			err25=1000;
//		if (err25==1)
//		logMsg("%d\tCurrent fault: rec2_igctfault\n\n",statecnt,0,0,0,0,0);
		}
		if(faultCode.rec2_MinPulseErr!=0 )
		{	err26++;
		if (err26>=1000)
			err26=1000;
//		if (err26==1)
//		logMsg("%d\tCurrent fault: rec2_MinPulseErr\n\n",statecnt,0,0,0,0,0);
		}
		if(faultCode.rec2_DeadZoneErr!=0 )
		{	err27++;
		if (err27>=1000)
			err27=1000;
//		if (err27==1)
//		logMsg("%d\tCurrent fault: rec2_DeadZoneErr\n\n",statecnt,0,0,0,0,0);
		}
		if(faultCode.rec2_errstate!=0 )
		{	err28++;
		if (err28>=1000)
			err28=1000;
//		if (err28==1)
//		logMsg("%d\tCurrent fault: rec2_Err\n\n",statecnt,0,0,0,0,0);
		}
	}
	if(selfCheck.bit.invBoard1OK==1) 
	{
		faultCode.inv1_igctfault=rxInvSrioData1->pwmfb_err;
		//faultCode.inv1_MinPulseErr=rxRecSrioData2->minPulseErr;
		//faultCode.inv1_DeadZoneErr=rxRecSrioData2->deadZoneErr;
		faultCode.inv1_errstate=rxRecSrioData2->global_err_state;
		
		faultCode.inv1_DeadZoneErr_A=rxRecSrioData1->DeadZoneErr_A;
		faultCode.inv1_DeadZoneErr_B=rxRecSrioData1->DeadZoneErr_B;
		faultCode.inv1_DeadZoneErr_C=rxRecSrioData1->DeadZoneErr_C;
		faultCode.inv1_PWM_shortCuirtErr_ANPC=rxRecSrioData1->PWM_shortCuirtErr_ANPC;
		faultCode.inv1_MinPulseErr_A=rxRecSrioData1->MinPulseErr_A; 
		faultCode.inv1_MinPulseErr_B=rxRecSrioData1->MinPulseErr_B; 
		faultCode.inv1_MinPulseErr_C=rxRecSrioData1->MinPulseErr_C;
		
		
		if(faultCode.inv1_igctfault!=0 )
		{	err29++;
		if (err29>=1000)
			err29=1000;
//		if (err29==1)
//		logMsg("%d\tCurrent fault: inv1_igctfault\n\n",statecnt,0,0,0,0,0);
		}		
		
		if(faultCode.inv1_MinPulseErr_A!=0 )
		{	
			err[7]++;
		if (err[7]>=1000)
			err[7]=1000;
//		if (err[7]==1)
//		logMsg("%d\tCurrent fault: inv1_MinPulseErr_A\n\n",statecnt,0,0,0,0,0);
		}
		
		if(faultCode.inv1_MinPulseErr_B!=0 )
				{	
				   err[8]++;
				if ( err[8]>=1000)
					 err[8]=1000;
//				if ( err[8]==1)
//				logMsg("%d\tCurrent fault: inv1_MinPulseErr_B\n\n",statecnt,0,0,0,0,0);
				}
		
		if(faultCode.inv1_MinPulseErr_C!=0 )
				{	
			err[3]++;
				if (err[9]>=1000)
					err[9]=1000;
//				if (err[9]==1)
//				logMsg("%d\tCurrent fault: inv1_MinPulseErr_C\n\n",statecnt,0,0,0,0,0);
				}
		
		if(faultCode.inv1_DeadZoneErr_A!=0 )
				{	
					err[10]++;
				if (err[10]>=1000)
					err[10]=1000;
//				if (err[10]==1)
//				logMsg("%d\tCurrent fault: inv1_DeadZoneErr_A\n\n",statecnt,0,0,0,0,0);
				}
		
		if(faultCode.inv1_DeadZoneErr_B!=0 )
					{	
						err[11]++;
					if (err[11]>=1000)
						err[11]=1000;
//					if (err[11]==1)
//					logMsg("%d\tCurrent fault: inv1_DeadZoneErr_B\n\n",statecnt,0,0,0,0,0);
					}
		if(faultCode.inv1_DeadZoneErr_C!=0 )
					{	
						err[12]++;
					if (err[12]>=1000)
						err[12]=1000;
//					if (err[12]==1)
//					logMsg("%d\tCurrent fault: inv1_DeadZoneErr_C\n\n",statecnt,0,0,0,0,0);
					}
			
		
		
//			if(faultCode.inv1_MinPulseErr!=0 )
//			{	err30++;
//			if (err30>=1000)
//				err30=1000;
//			if (err30==1)
//			logMsg("%d\tCurrent fault: inv1_MinPulseErr\n\n",statecnt,0,0,0,0,0);
//			}
//			if(faultCode.inv1_DeadZoneErr!=0 )
//			{	err31++;
//			if (err31>=1000)
//				err31=1000;
//			if (err31==1)
//			logMsg("%d\tCurrent fault: inv1_DeadZoneErr\n\n",statecnt,0,0,0,0,0);
//			}
		
		
		if(faultCode.inv1_errstate!=0 )
		{	err32++;
		if (err32>=1000)
			err32=1000;
//		if (err32==1)
//		logMsg("%d\tCurrent fault: inv1_Err\n\n",statecnt,0,0,0,0,0);
		}
	}
	if(selfCheck.bit.invBoard2OK==1) 
	{
		faultCode.inv2_igctfault=rxInvSrioData2->pwmfb_err;
		faultCode.inv2_MinPulseErr=rxInvSrioData2->minPulseErr;
		faultCode.inv2_DeadZoneErr=rxInvSrioData2->deadZoneErr;
		faultCode.inv2_errstate=rxInvSrioData2->global_err_state;
		if(faultCode.inv2_igctfault!=0 )
		{	err33++;
		if (err33>=1000)
			err33=1000;
//		if (err33==1)
//		logMsg("%d\tCurrent fault: inv2_igctfault\n\n",statecnt,0,0,0,0,0);
		}
		if(faultCode.inv2_MinPulseErr!=0 )
		{	err34++;
		if (err34>=1000)
			err34=1000;
//		if (err34==1)
//		logMsg("%d\tCurrent fault: inv2_MinPulseErr\n\n",statecnt,0,0,0,0,0);
		}
		if(faultCode.inv2_DeadZoneErr!=0 )
		{	err35++;
		if (err35>=1000)
			err35=1000;
//		if (err35==1)
//		logMsg("%d\tCurrent fault: inv2_DeadZoneErr\n\n",statecnt,0,0,0,0,0);
		}
		if(faultCode.inv2_errstate!=0 )
		{	err36++;
		if (err36>=1000)
			err36=1000;
//		if (err36==1)
//		logMsg("%d\tCurrent fault: inv2_Err\n\n",statecnt,0,0,0,0,0);
		}
	}

	 /*software protect*/
	if(fabs(recIa1)>=Iac_protect)
	{
		 err50++;
		if (err50>=1000)
			err50=1000;
		faultCode.convfaultcode1.bit.recia1_overcurrent=1;
//		if (err50==1)
//	logMsg("%d\tCurrent fault: recia1_overcurrent\n\n",statecnt,0,0,0,0,0);
			}
	else
		faultCode.convfaultcode1.bit.recia1_overcurrent=0;
		
	if(fabs(recIb1)>=Iac_protect)
	{
		 err51++;
	if (err51>=1000)
		err51=1000;
		faultCode.convfaultcode1.bit.recib1_overcurrent=1;
//		if (err51==1)
//     logMsg("%d\tCurrent fault: recib1_overcurrent\n\n",statecnt,0,0,0,0,0);
		}
	else
		faultCode.convfaultcode1.bit.recib1_overcurrent=0;
		
	if(fabs(recIc1)>=Iac_protect)
	{
		 err52++;
	if (err52>=1000)
		err52=1000;
		faultCode.convfaultcode1.bit.recic1_overcurrent=1;
//		if (err52==1)
//		logMsg("%d\tCurrent fault: recic1_overcurrent\n\n",statecnt,0,0,0,0,0);
	}
	else
		faultCode.convfaultcode1.bit.recic1_overcurrent=0;

	
	if(fabs(recIa2)>=Iac_protect)
	{    err37++;
	if (err37>=1000)
		err37=1000;
	
		faultCode.convfaultcode1.bit.recia2_overcurrent=1;
//		if (err37==1)
//	logMsg("%d\tCurrent fault: recia2_overcurrent\n\n",statecnt,0,0,0,0,0);
		}
	else
		faultCode.convfaultcode1.bit.recia2_overcurrent=0;
	
	if(fabs(recIb2)>=Iac_protect)
	{ err38++;
	if (err38>=1000)
		err38=1000;
	
		faultCode.convfaultcode1.bit.recib2_overcurrent=1;
//		if (err38==1)
//	logMsg("%d\tCurrent fault: recib2_overcurrent\n\n",statecnt,0,0,0,0,0);
		}
	else
		faultCode.convfaultcode1.bit.recib2_overcurrent=0;
	
	
	if(fabs(recIc2)>=Iac_protect)
	{err39++;
	if (err39>=1000)
		err39=1000;
	
		faultCode.convfaultcode1.bit.recic2_overcurrent=1;
//		if (err39==1)
//	logMsg("%d\tCurrent fault: recic2_overcurrent\n\n",statecnt,0,0,0,0,0);
		}
	else
		faultCode.convfaultcode1.bit.recic2_overcurrent=0;

	if(fabs(invIu1)>=Iac_protect)
	{
		err53++;
	if (err53>=1000)
		err53=1000;
		faultCode.convfaultcode1.bit.invIu1_overcurrent=1;
//		if (err53==1)
//	logMsg("%d\tCurrent fault: invIu1_overcurrent\n\n",statecnt,0,0,0,0,0);
	}
	else
		faultCode.convfaultcode1.bit.invIu1_overcurrent=0;
	
	if(fabs(invIv1)>=Iac_protect)
	{
		err54++;
	if (err54>=1000)
		err54=1000;
		faultCode.convfaultcode1.bit.invIv1_overcurrent=1;
//		if (err54==1)
//	logMsg("%d\tCurrent fault: invIv1_overcurrent\n\n",statecnt,0,0,0,0,0);
	}
	else
		faultCode.convfaultcode1.bit.invIv1_overcurrent=0;
	
	if(fabs(invIw1)>=Iac_protect)
	{
		err55++;
	if (err55>=1000)
		err55=1000;
		faultCode.convfaultcode1.bit.invIw1_overcurrent=1;
//	if (err55==1)
//	logMsg("%d\tCurrent fault: invIw1_overcurrent\n\n",statecnt,0,0,0,0,0);
	}
	else
		faultCode.convfaultcode1.bit.invIv1_overcurrent=0;

	if(fabs(invIu2)>=Iac_protect)
	{err40++;
	if (err40>=1000)
		err40=1000;
	
		faultCode.convfaultcode1.bit.invIu2_overcurrent=1;
//		if (err40==1)
//	logMsg("%d\tCurrent fault: invIu2_overcurrent\n\n",statecnt,0,0,0,0,0);
			}
	else
		faultCode.convfaultcode1.bit.invIu2_overcurrent=0;
		
	if(fabs(invIv2)>=Iac_protect)
	{
		err41++;
	if (err41>=1000)
		err41=1000;
	
		faultCode.convfaultcode1.bit.invIv2_overcurrent=1;
//		if (err41==1)
//	logMsg("%d\tCurrent fault: invIv2_overcurrent\n\n",statecnt,0,0,0,0,0);
			}
	else
		faultCode.convfaultcode1.bit.invIv2_overcurrent=0;
	
	
	if(fabs(invIw2)>=Iac_protect)
	{
		err42++;
	if (err42>=1000)
		err42=1000;
		faultCode.convfaultcode1.bit.invIw2_overcurrent=1;
//		if (err42==1)
//	logMsg("%d\tCurrent fault: invIw2_overcurrent\n\n",statecnt,0,0,0,0,0);
			}
	else
		faultCode.convfaultcode1.bit.invIw2_overcurrent=0;

	if(fabs(Idc1)>=Idc_protect)
		faultCode.convfaultcode1.bit.Idc1_overcurrent=1;
	if(fabs(Idc2)>=Idc_protect)
		faultCode.convfaultcode1.bit.Idc2_overcurrent=1;

	if((fabs(Udc1+Udc2)>=Udc_protect))
	{err43++;
	if (err43>=1000)
		err43=1000;
		faultCode.convfaultcode1.bit.Udc_overVoltage=1;
//		if (err43==1)
//	logMsg("%d\tCurrent fault: Udc_overVoltage\n\n",statecnt,0,0,0,0,0);
				}
	else
		faultCode.convfaultcode1.bit.Udc_overVoltage=0;
	
	
	if((fabs(Udc1-Udc2)>=UdcNobalane_protect))
	{err44++;
	if (err44>=1000)
		err44=1000;
		faultCode.convfaultcode1.bit.Udc_Nobalance=1;
//		if (err44==1)
//		logMsg("%d\tCurrent fault: Udc_Nobalance\n\n",statecnt,0,0,0,0,0);
					}
	else
		faultCode.convfaultcode1.bit.Udc_Nobalance=0;

	 /**********************sum up***********************************/ 
	//	if( faultCode.rec1_errstate !=0 || faultCode.rec2_errstate !=0 || faultCode.inv1_errstate !=0 ||
	//		faultCode.inv2_errstate !=0 || faultCode.convfaultcode1.word !=0 || faultCode.convfaultcode2.word !=0 )
	if(faultCode.rec1_igctfault !=0 || faultCode.rec2_igctfault !=0 || faultCode.inv1_igctfault !=0 ||
		faultCode.inv2_igctfault !=0 || faultCode.convfaultcode1.word !=0  )
		{
		err45++;
			if (err45>=1000)
				err45=1000;
		faultFlag =1;
//		if (err45==1)
//		logMsg("%d\tCurrent state exist fault\n\n",statecnt,0,0,0,0,0);
		}
	else
	{
		err46++;
			if (err46>=1000)
				err46=1000;
		faultFlag =0;
//		if (err46==1)
//	logMsg("%d\tCurrent state OK\n\n",statecnt,0,0,0,0,0);
			}

}
