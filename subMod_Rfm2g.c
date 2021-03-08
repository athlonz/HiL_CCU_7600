/*  
* target: PMC5565   
* Copyright 2018,4,16 IEE, Inc. 
* description: pmc5565初始化和收发程序，用于MCU和CCU的通讯  
* function: 用于闭环控制中各种基本函数的初始化、计算*
* debug: 
* modification history:	 
*/

/* includes */ 
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
 
#include <stdlib.h>
#include <string.h>

#include "rfm2g_api.h"  
#include <stdlib.h>
#include <string.h>
#include "timers.h"
#include "variable_main.h"
#include "GlobalVar_Inv.h" 
/****************************************/
/* globals */
UINT16 rfmSumIndex = 0;
short rxrfmdata[32];
short txrfmdata[32];
short rxrfmdata1[32];
short rxrfmdata2[32];
/****************************************/ 
/* locals */ 
UINT16 rfmCCUInSize = 0;													/* SIZE of CCU Input struct*/
UINT16 rfmCCUOutSize = 0;													/* SIZE of CCU Output struct*/
UINT32 ccuLivecnt_old=0 ;
short connectOkCnt=0;
RFM2GHANDLE rfmHandle = 0;										            /* Rfm2g Handle*/
float rxFre,rxFre_pre;
unsigned int runcmdOld;
int ResetCnt;


void swapPmcRfmByte(unsigned short * data1,unsigned short * data2,unsigned short size);
/******************函数定义*********************************************************************/
/*Name		:rfmInit
*Description：反射内存初始化，根据当前机箱的ID分配反射内存首地址偏移量，并初始化反射内存并对内存清零
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by wxx
*/
void rfmInit(void)
{
	char * RFM2G_0;
	short i;
	 
	   RFM2G_STATUS rfmStatus;
	   ccuLivecnt_old=0;
	   connectOkCnt=0;
//	   rfmStatus = RFM2gOpen("/PCIRFM2G_0", &rfmHandle );
	   switch(CPS_ID)
	  {
	   case  0X11: 
	  
	  
	    TxCcuRfmBaseAdrs = 0X400000;	 
	    RxCcuRfmBaseAdrs = 0X500000; 
	  
	    break;
	 
	  case   0X12:  
	  
	    
	    TxCcuRfmBaseAdrs = 0X400100;	 
	    RxCcuRfmBaseAdrs = 0X500100;	 
	  
	    break;
	    
	  case  0X21: 
	  	  
	  	  
		TxCcuRfmBaseAdrs = 0X400200;	 
		RxCcuRfmBaseAdrs = 0X500200; 
	  
		break;
	  	    
	  case  0X22: 
	  	  
	  	  
		TxCcuRfmBaseAdrs = 0X400300;	 
		RxCcuRfmBaseAdrs = 0X500300; 
	  
		break;
	    	    
	  };
		 	rfmStatus = RFM2gOpen("/PCIRFM2G_0", &rfmHandle );	//RFM2G_0
		 		if(rfmStatus == RFM2G_SUCCESS)
		 	         selfCheck.bit.pmc5565OK=1;
		 		if( rfmStatus != RFM2G_SUCCESS )
		 		{
		 		  selfCheck.bit.pmc5565OK=0;
		 		  faultCode.convfaultcode2.bit.pmc5565Err=1; 
		 	//	  return ERROR;
		 		 
		 		}		  
	  
		rfmCCUInSize = sizeof(struct rxCCUStruct );
		rfmCCUOutSize = sizeof(struct txCCUStruct );
		 
		bzero((char *)&rxccudata, rfmCCUInSize );/* Clear Struct DATAOUT_CCU*/
		bzero((char *)&txccudata, rfmCCUOutSize );/* Clear Struct DATAIN_CCU*/

		//if( selfCheck.bit.pmc5565OK==1)
		 	rfmStatus = RFM2gWrite( rfmHandle, RxCcuRfmBaseAdrs, (void *)(& rxccudata.livecnt), rfmCCUInSize);	
		       
		//if( selfCheck.bit.pmc5565OK==1)
		 	rfmStatus = RFM2gWrite( rfmHandle, TxCcuRfmBaseAdrs, (void *)(&txccudata.begin), rfmCCUOutSize);
		 	
	 	


	 
}
/*********************************************************************************************************/
/*Name		:rfmClose
*Description：反射内存关闭
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by wxx
*/
STATUS rfmClose(void)
{
	RFM2G_STATUS rfmStatus;
	rfmStatus = RFM2gClose( &rfmHandle );
	if( rfmStatus != RFM2G_SUCCESS )
	{		
		logMsg( "ERROR: RFM2gClose() failed.\n",0,0,0,0,0,0 );
		return(ERROR);
	}
	return (OK);
}
 
/************************************************************************/
/*Name		:rfmSendCCU
*Description：反射内存发送数据
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by wxx
*/

STATUS rfmSendCCU(void)		
{  
	RFM2G_STATUS rfmStatus;
	int status;
	clockid_t   clock_id;/* clock ID (always CLOCK_REALTIME) */
	struct     timespec  tp;       /* where to store current time */
	status= clock_gettime ( CLOCK_REALTIME,  &tp  );
	rfmStatus=RFM2gSetDMAThreshold(rfmHandle,2048); 
	/************************************************************************/
	/* Write the RFM */                                                                     
	/************************************************************************/	
	txccudata.begin = 11;

	txccudata.livecnt++;
	txccudata.inv1PWMmode=PWMdata_INV1.ModeFlag;
	txccudata.inv2PWMmode=PWMdata_INV2.ModeFlag;	
	txccudata.selfcheck  = selfCheck.word;
	txccudata.runstate   = runState;
	txccudata.faultLevel = PWMdata_INV1.HalfGama_TotalNum;//0;
 

	txccudata.Udc    = Udc;

	txccudata.invIu1 = invIu1;
	txccudata.invIv1 = invIv1;
	txccudata.invIw1 = invIw1;

	txccudata.invUa1 = invUa1;
	txccudata.invUb1 = invUb1;
	txccudata.invUc1 = invUc1;

	txccudata.invIu2 = invIu2;
	txccudata.invIv2 = invIv2;
	txccudata.invIw2 = invIw2;

	txccudata.invUa2 = invUa2;
	txccudata.invUb2 = invUb2;
	txccudata.invUc2 = invUc2;

	txccudata.IaMotor = IaMotor;
	txccudata.IbMotor = IbMotor;
	txccudata.IcMotor = IcMotor;
	
	txccudata.return_switchINcmd=rx_7700_data.short_variable[0];
	txccudata.return_switch_DT_BTcmd=rx_7700_data.short_variable[1];
	txccudata.return_switchOUTcmd=rx_7700_data.short_variable[2];			

	txccudata.end = 1111;
        
        
//        if( selfCheck.bit.pmc5565OK==1)
 	rfmStatus = RFM2gWrite( rfmHandle, TxCcuRfmBaseAdrs, (void *)(& txccudata.begin), rfmCCUOutSize);	

//	if ( rfmStatus != RFM2G_SUCCESS )
//	{
//          selfCheck.bit.pmc5565OK=0;
//	  faultCode.convfaultcode2.bit.pmc5565Err=1;
//	  return(ERROR);
//	}
	return (OK);
}


/************************************************************************/
/*Name		:rfmReceiveCCU
*Description：反射内存接收数据
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by wxx
*/ 

STATUS rfmReceiveCCU(void)
{
	RFM2G_STATUS rfmStatus;
        short i;
	int loopNum = 0;
	rfmStatus=RFM2gSetDMAThreshold(rfmHandle,32);
 	rfmStatus = RFM2gRead( rfmHandle, RxCcuRfmBaseAdrs, (void *)(& rxccudata.livecnt), rfmCCUInSize);
 
	if( rxccudata.livecnt!=ccuLivecnt_old)
	  {
	   connectOkCnt++;
	   if(connectOkCnt>=100)
		  {
			connectOkCnt=100;
			selfCheck.bit.ccuConnnetOK=1;
		  }
	  }
	else
	  { 
		connectOkCnt--;
		if(connectOkCnt<=0)
		  {
			connectOkCnt=0;
			if(selfCheck.bit.ccuConnnetOK==1)
			   faultCode.convfaultcode2.bit.ccuConErr=1;
			selfCheck.bit.ccuConnnetOK=0;
		  } 
	  }
	ccuLivecnt_old= rxccudata.livecnt; 
	
		runcmd = rxccudata.runCmd; 
		 if( (runcmd & 1) ==1)
		 {
				runCmd.word=4;
				if( (runcmd & 2) ==2)
					runCmd.word=5;			
		 }
		 
		 if(((runcmdOld & 2) ==2) && (runcmd==0))
			 runCmd.word=0;
		 
		 if( runcmd==6)     //复位跳转
		 {	
			 ResetCnt++;
		    if(ResetCnt >= 600)
		   {	
			 runCmd.word=6;
			 ResetCnt=0;
	       }
		 }
		 
		 if( runcmd==8)     //放电
		 {	
			 runCmd.word=8;
	      }
			 
		 

		 /*消除异常点*/
		rxFre = rxccudata.invfreq;
		if (abs(rxFre- rxFre_pre) > 10)
			rxFre = rxFre_pre;
	
		rxFre_pre = rxFre;
       		 
		PWMdata_INV1.vvvf1=fabs(rxFre);
		PWMdata_INV1.vvvf1Interpolation=rxccudata.invfreq;
		PWMdata_INV1.Kv=rxccudata.invKv1;
		//PWMdata_INV1.KvPre=PWMdata_INV1.Kv;
		PWMdata_INV1.VFGama=rxccudata.invGama1; 
		PWMdata_INV1.vvvf1IncreFlag = rxccudata.vvvf1IncreFlag;
	
		FreqWithDire = rxccudata.invfreq;  // 20160713,带方向的频率，用于插值限制角度回撤
	
		PWMdata_INV2.vvvf1=fabs(rxFre); 
		PWMdata_INV2.vvvf1Interpolation=rxccudata.invfreq;
		PWMdata_INV2.Kv=rxccudata.invKv2;
		//PWMdata_INV2.KvPre=PWMdata_INV2.Kv;	
		PWMdata_INV2.VFGama=rxccudata.invGama2;
		PWMdata_INV2.vvvf1IncreFlag = rxccudata.vvvf1IncreFlag;
		while(PWMdata_INV1.VFGama>2*PI) PWMdata_INV1.VFGama=PWMdata_INV1.VFGama-2*PI;
		while(PWMdata_INV1.VFGama<0) PWMdata_INV1.VFGama=PWMdata_INV1.VFGama+2*PI;
		while(PWMdata_INV2.VFGama>2*PI) PWMdata_INV2.VFGama=PWMdata_INV2.VFGama-2*PI; 
		while(PWMdata_INV2.VFGama<0) PWMdata_INV2.VFGama=PWMdata_INV2.VFGama+2*PI;
		
		PWMdata_INV1.VFGamaOrigin = PWMdata_INV1.VFGama; 
		PWMdata_INV2.VFGamaOrigin = PWMdata_INV2.VFGama; 
		
		Ud_Fwd  = rxccudata.INVPower;
		changestep = rxccudata.changestep_flag;
		changestep_SST_Flag=rxccudata.changeStep_SST_Flag;
		
		
		switchINcmd=rxccudata.switchINcmd;
		switch_DT_BTcmd=rxccudata.switch_DT_BTcmd;
		switchOUTcmd=rxccudata.switchOUTcmd;
		preChargecmd=rxccudata.preChargecmd;
		Ratiocmd=rxccudata.Ratio;
   
		runcmdOld = runcmd;
			
        return(OK);
}



