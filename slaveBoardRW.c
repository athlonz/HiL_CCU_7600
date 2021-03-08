/*  
* target: rectifier and invertor PWM call enterance   
* Copyright 2018,4,16 IEE, Inc. 
* description:   
* function: 该文件的功能是从板的读写，VME的读写和SRIO的读写
* debug: 
* modification history:20180426 by wxx
*/

#include "vxWorks.h"
#include "vme.h"
#include "math.h" 
#include "h/astVME.h"
#include "h/drv/vme/altioctl.h"
#include "subMod_ControlFunction.h"
#include "variable_main.h"
#include "GlobalVar_Rec.h"
#include "GlobalVar_Inv.h"
#include "pwmControl.h"
#include "ast7600Srio.h"
#include "closeControl.h"
/******************************************************************************/

unsigned short status=0;
unsigned short rec1SrioID;
unsigned short rec1Srio_status	;//
unsigned short rec1Srio_error;//
unsigned short rec1Srio_igen_state;

unsigned short rec2SrioID;
unsigned short rec2Srio_status	;//
unsigned short rec2Srio_error;//
unsigned short rec2Srio_igen_state;

unsigned short inv1SrioID;
unsigned short inv1Srio_status	;//
unsigned short inv1Srio_error;//
unsigned short inv1Srio_igen_state;

unsigned short inv2SrioID;
unsigned short inv2Srio_status	;//
unsigned short inv2Srio_error;//
unsigned short inv2Srio_igen_state;

unsigned short ad1SrioID;
unsigned short ad1Srio_status	;//
unsigned short ad1Srio_error;//
unsigned short ad1Srio_igen_state;

unsigned short ad2SrioID;
unsigned short ad2Srio_status	;//
unsigned short ad2Srio_error;//
unsigned short ad2Srio_igen_state;

unsigned short clkSrioID;
unsigned short clkSrio_status	;//
unsigned short clkSrio_error;//
unsigned short clkSrio_igen_state;

unsigned short rxClkData_srio_rx_live_cnt_old=0;
unsigned short rxRecPWMData1_srio_rx_live_cnt_old=0;
unsigned short rxRecPWMData2_srio_rx_live_cnt_old=0;
unsigned short rxInvPWMData1_srio_rx_live_cnt_old=0;
unsigned short rxInvPWMData2_srio_rx_live_cnt_old=0;
unsigned short rxAdData1_srio_rx_live_cnt_old=0;
unsigned short rxAdData2_srio_rx_live_cnt_old=0;



unsigned short  GPS_OK=0;
unsigned short  YrHDayBuf=0;
unsigned short  DayHrBuf=0;
unsigned short  MinSecBuf=0;


/*******************************************************************************************/
/*Name		:checkAst7700_VME
*Description:SRIO交换卡ast7700板检测，通过VME A32 D32的读写的方式
*Input		:ast7700VmeAdrs		
*Output		:ast7700LocalBaseAdrs
*Return		:OK or ERROR
*History	:2016,by wxx
*/
void checkAst7700_VME(void)
{
	int vme_status=0;	
	unsigned int intTemp0=0,intTemp1=0,intTemp2=0; 	
	unsigned int j=0;
	unsigned int prand=0  ;
	int 		 waitCnt=0;
	/*check ast7700 board*/
	vme_status=astVmeMapAlloc(&ast7700LocalBaseAdrs,ast7700VmeAdrs,0x100000,VME_AM_EXT_USR_DATA,RDWR_MODE_A32,RDWR_SIZE_INT,RDWR_SWAP_NO);
	logMsg("avme7700 VME local memory start address is %x \n",ast7700LocalBaseAdrs,0,0,0,0,0);
	if(vme_status==OK)
    {	
		intTemp0= *(UINT32 *)(ast7700LocalBaseAdrs+0x4);
		*(UINT32 *)(ast7700LocalBaseAdrs+0x8)=0xaaaaaaaa;
		intTemp1=*(UINT32 *)(ast7700LocalBaseAdrs+0x8);
		*(UINT32 *)(ast7700LocalBaseAdrs+0x8)=0x55555555;
		intTemp2=*(UINT32 *)(ast7700LocalBaseAdrs+0x8);		
		if(intTemp0==0x51002140 && intTemp1==0xaaaaaaaa && intTemp2==0x55555555)
		{		 
			do 
			{	
				j++;
				prand = rand();//产生随机数
				*(UINT32 *)(ast7700LocalBaseAdrs+0x8)=prand;
				intTemp1=*(UINT32 *)(ast7700LocalBaseAdrs+0x8);			
			}while(j<100 && intTemp1==prand);	//测试100次，7700VME读写特别慢			
		}			
    }
    if(j==100)
	{
		selfCheck.bit.ast7700VMEOK=1;	
		logMsg("AVME7700 VME is checked succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.ast7700VMEOK=0;
		logMsg("AVME7700 VME is checked failed\n",0,0,0,0,0,0);
	}
   
    if(selfCheck.bit.ast7700VMEOK==1)
    {	
		/*wait for ast7700 finishing SRIO route initiation*/	
		do
		{
			intTemp0=*(UINT32 *)(ast7700LocalBaseAdrs+0x1000);
			taskDelay(sysClkRateGet());/*等待1秒后再独一次*/
			waitCnt++;/*等待10s 直到7700初始化完成SRIO*/
			logMsg("wait for avme7700 to finishing SRIO route initiation\n",0,0,0,0,0,0);
		}
		while(intTemp0!=1 && waitCnt<  60.0 );/*等10秒，不行放弃*/
		if(intTemp0==1)		
		{
			selfCheck.bit.ast7700SrioInitOK=1;	
			logMsg("AVME7700 SRIO initiation succeed\n",0,0,0,0,0,0);
		}
		else
		{
			selfCheck.bit.ast7700SrioInitOK=0;
			logMsg("AVME7700 SRIO initiation failed\n",0,0,0,0,0,0);
		}
		/*指定接收7700数据的内存地址*/
		rx_7700_data_pointer=AST_PPC_SRIO_DMA_INBOUND_TAR+0*1*1024*1024;/*7700交换卡对应的SRIO节点号是0*/
		logMsg("avme7700 srio local memory start address is %x \n",rx_7700_data_pointer,0,0,0,0,0);
    }
    taskDelay(sysClkRateGet());/*wait 1 second to finishing printf message*/
}
/*******************************************************************************/
/*Name		:IoBoardInit_VME
*Description:IO板自检并初始化,使用VME总线
*Input		:IOVmeBaseAdrs		
*Output		:IOLocalBaseAdrs
*Return		:OK or ERROR
*History	:2016,by wxx
*/
void IoBoardInit_VME(void)
{
	int vme_status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
		
	/*check IO board*/
	vme_status=astVmeMapAlloc(&IOLocalBaseAdrs,IOVmeBaseAdrs,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
	logMsg("IO board VME local memory start address is %x \n",IOLocalBaseAdrs,0,0,0,0,0);
	if(vme_status==OK)
	{
		shortTemp0= *(short *)(IOLocalBaseAdrs+0x0);
		*(short *)(IOLocalBaseAdrs+0x2)=0xaaaa;
		shortTemp1=*(short *)(IOLocalBaseAdrs+0x2);
		*(short *)(IOLocalBaseAdrs+0x2)=0x5555;
		shortTemp2=*(short *)(IOLocalBaseAdrs+0x2);
		if(shortTemp0==0xC000 && shortTemp1==0xaaaa && shortTemp2==0x5555)
		{			
			do 
			{	
				j++;
				*(short *)(IOLocalBaseAdrs+0x2)=j;
				shortTemp1=*(short *)(IOLocalBaseAdrs+0x2);			
			}while(j<65535 && shortTemp1==j);	
			
		}		
	}
	if(j==65535)
	{
		selfCheck.bit.IOBoardOK=1;	
		logMsg("IO board VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.IOBoardOK=0;
		logMsg("IO board VME check failed\n",0,0,0,0,0,0);
	}
	if(selfCheck.bit.IOBoardOK==1)
		  {
		      *(short *)(IOLocalBaseAdrs+0x04)=6;/*clkDivide 2 n cifang */
			*(short *)(IOLocalBaseAdrs+0x08)=600;//1024;/*encoderline */
			*(unsigned short *)(IOLocalBaseAdrs + 0xC0)=0;/*IO input */

		  }

}
/*******************************************************************************/
/*Name		:RecPwm1Init_withSrio
*Description:整流PWM卡1初始化SRIO设置
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void RecPwm1Init_withSrio(void)
{
	int vme_status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
	
	/*check rectifier1 board*/
	vme_status=astVmeMapAlloc(&RecLocalBaseAdrs1,RecVmeBaseAdrs1,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
	logMsg("recitifier PWM  board 1 VME local memory start address is %x \n",RecLocalBaseAdrs1,0,0,0,0,0);
	if(vme_status==OK)
	{
		shortTemp0= *(short *)(RecLocalBaseAdrs1+0x0);
		*(short *)(RecLocalBaseAdrs1+0x2)=0xaaaa;
		shortTemp1=*(short *)(RecLocalBaseAdrs1+0x2);
		*(short *)(RecLocalBaseAdrs1+0x2)=0x5555;
		shortTemp2=*(short *)(RecLocalBaseAdrs1+0x2);
		if(shortTemp0==0x3000 && shortTemp1==0xaaaa && shortTemp2==0x5555)
		{			 
			do 
			{	
				j++;
				*(short *)(RecLocalBaseAdrs1+0x2)=j;
				shortTemp1=*(short *)(RecLocalBaseAdrs1+0x2);			
			}while(j<65535 && shortTemp1==j);				
		}				
	}
	if(j==65535)
	{
		selfCheck.bit.recBoard1OK=1;
		logMsg("rectifier PWM board 1 VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.recBoard1OK=0;
		logMsg("rectifier PWM board 1 VME check failed\n",0,0,0,0,0,0);
	}
	    
	if(selfCheck.bit.recBoard1OK==1 && selfCheck.bit.ast7700SrioInitOK==1) 
	{
		rec1SrioID	= *(short *)(RecLocalBaseAdrs1+0X30);//srio_deviceid;
		rec1Srio_status	= *(short *)(RecLocalBaseAdrs1+0X3a);//
		rec1Srio_error	= *(short *)(RecLocalBaseAdrs1+0X44);//
		rec1Srio_igen_state=*(short *)(RecLocalBaseAdrs1+0X48);//
		
		*(short *)(RecLocalBaseAdrs1+0x1e)=1; 			//  registers control right set as SRIO
		*(short *)(RecLocalBaseAdrs1+0x32)=AST_PPC_SRIO_DMA_OUTBOUND_TAR>>16;//0xf000;		
		//	srio_local_target_addr_reg[31:16]<=vme_data;//local srio receive ram start address			
		*(short *)(RecLocalBaseAdrs1+0x34)=AST_PPC_SRIO_DMA_OUTBOUND_TAR&0xffff;//0;			
		//	srio_local_target_addr_reg[15:0]<=vme_data;//local srio receive ram start address			
		*(short *)(RecLocalBaseAdrs1+0x36)=(AST_PPC_SRIO_DMA_INBOUND_BASE+rec1SrioID*0x00100000)>>16;
		//0xf000+rec1SrioID*0x0010;		//	srio_dest_start_addr_reg[31:16]<=vme_data;//srio send data detination start address
		*(short *)(RecLocalBaseAdrs1+0x38)=(AST_PPC_SRIO_DMA_INBOUND_BASE+rec1SrioID*0x00100000)&0xffff; //0  
		// srio_dest_start_addr_reg[15:0]<=vme_data;////srio send data detination start address											
		*(short *)(RecLocalBaseAdrs1+0x3c)=ast7600SrioID;   //   srio_dest_id[7:0],slave borad upload data to this SRIO ID
		*(short *)(RecLocalBaseAdrs1+0x3e)=srioMutiSendID;	//	组播ID   multicast_deviceid[7:0]		
		*(short *)(RecLocalBaseAdrs1+0x40)= 0x80;			//srio_tx_size[8:0],slave board is to upload bytes number;	
		*(short *)(RecLocalBaseAdrs1+0x42)= 0x80;			//rx_size[8:0],slave board is to receirve bytes number;	
		*(short *)(RecLocalBaseAdrs1+0x46)= 1;				//SRIO_SendEnable
		  
		
		/*general registers setting*/
	  	txRecSrioData1.boardID = 0x7600 ;
		txRecSrioData1.srio_deviceID = ast7600SrioID;
		txRecSrioData1.tx_liveCnt = txRecSrioData1.tx_liveCnt++;
		txRecSrioData1.tx_frameLen = 0X80;
		txRecSrioData1.tx_frameType = 0;//unsopported here
		txRecSrioData1.clkDivide = 1;        //50M/2=25M
		txRecSrioData1.timerPeriodShadow = T1PRSVM;		
		txRecSrioData1.TimerEnable =1;		
		txRecSrioData1.clrFaultCmd =1;	
		txRecSrioData1.reservered0=0;//unsurpported
 		/*PWM special registers setting*/		
		txRecSrioData1.ACQ_ZRO=0xaaaa;/*ACQ_ZRO */
		txRecSrioData1.ACQ_CAU=0xaaaa;/*ACQ_CAU */
		txRecSrioData1.ACQ_PRD=0xaaaa;/*ACQ_PRD */
		txRecSrioData1.ACQ_CAD=0xaaaa;/*ACQ_CAD */	
		txRecSrioData1.CMPAU1=0;/*CMPAU_shadow[1] */
		txRecSrioData1.CMPAD1=0;/*CMPAD_shadow[1] */
		txRecSrioData1.CMPAU2=0;/*CMPAU_shadow[2] */
		txRecSrioData1.CMPAD2=0;/*CMPAD_shadow[2] */
		txRecSrioData1.CMPAU3=0;/*CMPAU_shadow[3] */
		txRecSrioData1.CMPAD3=0;/*CMPAD_shadow[3] */
		txRecSrioData1.CMPAU4=0;/*CMPAU_shadow[4] */
		txRecSrioData1.CMPAD4=0;/*CMPAD_shadow[4] */
		txRecSrioData1.CMPAU5=0;/*CMPAU_shadow[5] */
		txRecSrioData1.CMPAD5=0;/*CMPAD_shadow[5] */
		txRecSrioData1.CMPAU6=0;/*CMPAU_shadow[6] */
		txRecSrioData1.CMPAD6=0;/*CMPAD_shadow[6] */
		txRecSrioData1.minPulseTime=MinPulseCheck;/*min pulse time 10us,50M clock */
		txRecSrioData1.deadZone=DeadZone;/*dead zone time 50us,50M clock */
		txRecSrioData1.turnOffTime=250;/*IGCT1&4 turn off time befoe IGCT2&3 */
		txRecSrioData1.PwmEnable=1;/*pwm disable */	
		txRecSrioData1.protectEn=1;//1;/*small pulse and deadzone and feedback protect enable*/
		
//		txRecSrioData1.ANPC_type_a=0;
//		txRecSrioData1.ANPC_type_b=0;
//		txRecSrioData1.ANPC_type_c=0;
		Rec1_npc();
		status=ast7600PpcSrioSingleSendData(rec1SrioID,txPWMSrioLen,(UINT8 *)&txRecSrioData1);
		
		rxRecSrioData1=AST_PPC_SRIO_DMA_INBOUND_TAR+rec1SrioID*1*1024*1024;
		
 
		logMsg("rectifier PWM board 1 SRIO initiation succeeded\n",0,0,0,0,0,0);
		logMsg("rectifier PWM board 1 srio local memory start address is %x \n",rxRecSrioData1,0,0,0,0,0);
 
	}
	else
		logMsg("rectifier PWM board 1 SRIO initiation failed\n",0,0,0,0,0,0);
}
/*******************************************************************************/
/*Name		:RecPwm2Init_withSrio
*Description:整流PWM卡2初始化SRIO设置，并初始化该卡的PWM寄存器
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void RecPwm2Init_withSrio(void)
{
	int vme_status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
	
	/*check rectifier2 board*/
	vme_status=astVmeMapAlloc(&RecLocalBaseAdrs2,RecVmeBaseAdrs2,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
	logMsg("recitifier PWM  board 2 VME local memory start address is %x \n",RecLocalBaseAdrs2,0,0,0,0,0);
	if(vme_status==OK)
	{
		shortTemp0= *(short *)(RecLocalBaseAdrs2+0x0);
		*(short *)(RecLocalBaseAdrs2+0x2)=0xaaaa;
		shortTemp1=*(short *)(RecLocalBaseAdrs2+0x2);
		*(short *)(RecLocalBaseAdrs2+0x2)=0x5555;
		shortTemp2=*(short *)(RecLocalBaseAdrs2+0x2);
		if(shortTemp0==0x3000 && shortTemp1==0xaaaa && shortTemp2==0x5555)
		{			 
			do 
			{	
				j++;
				*(short *)(RecLocalBaseAdrs2+0x2)=j;
				shortTemp1=*(short *)(RecLocalBaseAdrs2+0x2);			
			}while(j<65535 && shortTemp1==j);	
			
		}			
	}
	if(j==65535)
	{
		selfCheck.bit.recBoard2OK=1;
		logMsg("rectifier PWM board 2 VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.recBoard2OK=0;
		logMsg("rectifier PWM board 2 VME check failed\n",0,0,0,0,0,0);
	}  
	
	if(selfCheck.bit.recBoard2OK==1 && selfCheck.bit.ast7700SrioInitOK==1)  
	{
		rec2SrioID	= *(short *)(RecLocalBaseAdrs2+0X30);//srio_deviceid;
		rec2Srio_status	= *(short *)(RecLocalBaseAdrs2+0X3a);//
		rec2Srio_error	= *(short *)(RecLocalBaseAdrs2+0X44);//
		rec2Srio_igen_state=*(short *)(RecLocalBaseAdrs2+0X48);//
		
		*(short *)(RecLocalBaseAdrs2+0x1e)=1; 			
		//  registers control right set as SRIO
		*(short *)(RecLocalBaseAdrs2+0x32)=AST_PPC_SRIO_DMA_OUTBOUND_TAR>>16;		
		//	srio_local_target_addr_reg[31:16]<=vme_data;//local srio receive ram start address			
		*(short *)(RecLocalBaseAdrs2+0x34)=AST_PPC_SRIO_DMA_OUTBOUND_TAR&0xffff;	
		//	srio_local_target_addr_reg[15:0]<=vme_data;//local srio receive ram start address			
		*(short *)(RecLocalBaseAdrs2+0x36)=(AST_PPC_SRIO_DMA_INBOUND_BASE+rec2SrioID*0x00100000)>>16;
		//	srio_dest_start_addr_reg[31:16]<=vme_data;//srio send data detination start address
		*(short *)(RecLocalBaseAdrs2+0x38)=(AST_PPC_SRIO_DMA_INBOUND_BASE+rec2SrioID*0x00100000)&0xffff; 
		// srio_dest_start_addr_reg[15:0]<=vme_data;////srio send data detination start address											
		*(short *)(RecLocalBaseAdrs2+0x3c)=ast7600SrioID;   
		//   srio_dest_id[7:0],slave borad upload data to this SRIO ID
		*(short *)(RecLocalBaseAdrs2+0x3e)=srioMutiSendID;	
		//	组播ID   multicast_deviceid[7:0]		
		*(short *)(RecLocalBaseAdrs2+0x40)= 0x80;			
		//srio_tx_size[8:0],slave board is to upload bytes number;	
		*(short *)(RecLocalBaseAdrs2+0x42)= 0x80;			
		//rx_size[8:0],slave board is to receirve bytes number;	
		*(short *)(RecLocalBaseAdrs2+0x46)= 1;				
		//SRIO_SendEnable

		/*general registers setting*/
	  	txRecSrioData2.boardID = 0x7600 ;
		txRecSrioData2.srio_deviceID = ast7600SrioID;
		txRecSrioData2.tx_liveCnt = txRecSrioData2.tx_liveCnt++;
		txRecSrioData2.tx_frameLen = 0X80;
		txRecSrioData2.tx_frameType = 0;//unsopported here
		txRecSrioData2.clkDivide = 1;         //50M/2=25M
		txRecSrioData2.timerPeriodShadow = T1PRSVM;		
		txRecSrioData2.TimerEnable =1;		
		txRecSrioData2.clrFaultCmd =1;	
		txRecSrioData2.reservered0=0;//unsurpported
 		/*PWM special registers setting*/		
		txRecSrioData2.ACQ_ZRO=0xaaaa;/*ACQ_ZRO */
		txRecSrioData2.ACQ_CAU=0xaaaa;/*ACQ_CAU */
		txRecSrioData2.ACQ_PRD=0xaaaa;/*ACQ_PRD */
		txRecSrioData2.ACQ_CAD=0xaaaa;/*ACQ_CAD */	
		txRecSrioData2.CMPAU1=0;/*CMPAU_shadow[1] */
		txRecSrioData2.CMPAD1=0;/*CMPAD_shadow[1] */
		txRecSrioData2.CMPAU2=0;/*CMPAU_shadow[2] */
		txRecSrioData2.CMPAD2=0;/*CMPAD_shadow[2] */
		txRecSrioData2.CMPAU3=0;/*CMPAU_shadow[3] */
		txRecSrioData2.CMPAD3=0;/*CMPAD_shadow[3] */
		txRecSrioData2.CMPAU4=0;/*CMPAU_shadow[4] */
		txRecSrioData2.CMPAD4=0;/*CMPAD_shadow[4] */
		txRecSrioData2.CMPAU5=0;/*CMPAU_shadow[5] */
		txRecSrioData2.CMPAD5=0;/*CMPAD_shadow[5] */
		txRecSrioData2.CMPAU6=0;/*CMPAU_shadow[6] */
		txRecSrioData2.CMPAD6=0;/*CMPAD_shadow[6] */
		txRecSrioData2.minPulseTime=MinPulseCheck;/*min pulse time 10us,50M clock */
		txRecSrioData2.deadZone=DeadZone;/*dead zone time 50us,50M clock */
		txRecSrioData2.turnOffTime=250;/*IGCT1&4 turn off time befoe IGCT2&3 */
		txRecSrioData2.PwmEnable=1;/*pwm disable */	
		txRecSrioData2.protectEn=1;//1;/*small pulse and deadzone and feedback protect enable*/
		Rec2_npc();
		status=ast7600PpcSrioSingleSendData(rec2SrioID,txPWMSrioLen,(UINT8 *)&txRecSrioData2);		
		
		rxRecSrioData2=AST_PPC_SRIO_DMA_INBOUND_TAR+rec2SrioID*0x00100000;
		logMsg("rectifier PWM board 2 SRIO initiation succeeded\n",0,0,0,0,0,0);
		logMsg("rectifier PWM board 2 srio local memory start address is %x \n",rxRecSrioData2,0,0,0,0,0);
	}
	else
		logMsg("rectifier PWM board 2 SRIO initiation failed\n",0,0,0,0,0,0);
}
/*******************************************************************************/
/*Name		:InvPwm1Init_withSrio
*Description:逆变PWM卡1初始化SRIO设置，并初始化该卡的PWM寄存器
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void InvPwm1Init_withSrio(void)
{
	int vme_status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
	
	/*check invertor1 board*/
    vme_status=astVmeMapAlloc(&InvLocalBaseAdrs1,InvVmeBaseAdrs1,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
    logMsg("Invertor PWM  board 1 VME local memory start address is %x \n",InvLocalBaseAdrs1,0,0,0,0,0);
    if(vme_status==OK)
  	{
		shortTemp0= *(short *)(InvLocalBaseAdrs1+0x0);
		*(short *)(InvLocalBaseAdrs1+0x2)=0xaaaa;
		shortTemp1=*(short *)(InvLocalBaseAdrs1+0x2);
		*(short *)(InvLocalBaseAdrs1+0x2)=0x5555;
		shortTemp2=*(short *)(InvLocalBaseAdrs1+0x2);
		if(shortTemp0==0x3000 && shortTemp1==0xaaaa && shortTemp2==0x5555)
		{
			do 
			{	
				j++;
				*(short *)(InvLocalBaseAdrs1+0x2)=j;
				shortTemp1=*(short *)(InvLocalBaseAdrs1+0x2);			
			}while(j<65535 && shortTemp1==j);			
		}		  
  	}
	if(j==65535)
	{
		selfCheck.bit.invBoard1OK=1;
		logMsg("invertor PWM board 1 VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.invBoard1OK=0;
		logMsg("invertor PWM board 1 VME check failed\n",0,0,0,0,0,0);
	}  
    
	if(selfCheck.bit.invBoard1OK==1&& selfCheck.bit.ast7700SrioInitOK==1)   
	{
		inv1SrioID	= *(short *)(InvLocalBaseAdrs1+0X30);//srio_deviceid;
		inv1Srio_status	= *(short *)(InvLocalBaseAdrs1+0X3a);//
		inv1Srio_error	= *(short *)(InvLocalBaseAdrs1+0X44);//
		inv1Srio_igen_state=*(short *)(InvLocalBaseAdrs1+0X48);//
			
		*(short *)(InvLocalBaseAdrs1+0x1e)=1; 			
		//  registers control right set as SRIO
		*(short *)(InvLocalBaseAdrs1+0x32)=AST_PPC_SRIO_DMA_OUTBOUND_TAR>>16;		
		//	srio_local_target_addr_reg[31:16]<=vme_data;//local srio receive ram start address			
		*(short *)(InvLocalBaseAdrs1+0x34)=AST_PPC_SRIO_DMA_OUTBOUND_TAR&0xffff;			
		//	srio_local_target_addr_reg[15:0]<=vme_data;//local srio receive ram start address			
		*(short *)(InvLocalBaseAdrs1+0x36)=(AST_PPC_SRIO_DMA_INBOUND_BASE+inv1SrioID*0x00100000)>>16;		
		//	srio_dest_start_addr_reg[31:16]<=vme_data;//srio send data detination start address
		*(short *)(InvLocalBaseAdrs1+0x38)=(AST_PPC_SRIO_DMA_INBOUND_BASE+inv1SrioID*0x00100000)&0xffff;   		// srio_dest_start_addr_reg[15:0]<=vme_data;////srio send data detination start address											
		*(short *)(InvLocalBaseAdrs1+0x3c)=ast7600SrioID;   
		//   srio_dest_id[7:0],slave borad upload data to this SRIO ID
		*(short *)(InvLocalBaseAdrs1+0x3e)=srioMutiSendID;	
		//	组播ID   multicast_deviceid[7:0]		
		*(short *)(InvLocalBaseAdrs1+0x40)= 0x80;			
		//srio_tx_size[8:0],slave board is to upload bytes number;	
		*(short *)(InvLocalBaseAdrs1+0x42)= 0x80;			
		//rx_size[8:0],slave board is to receirve bytes number;	
		*(short *)(InvLocalBaseAdrs1+0x46)= 1;				
		//SRIO_SendEnable

		/*general registers setting*/
	  	txInvSrioData1.boardID = 0x7600 ;
		txInvSrioData1.srio_deviceID = ast7600SrioID;
		txInvSrioData1.tx_liveCnt = txInvSrioData1.tx_liveCnt++;
		txInvSrioData1.tx_frameLen = 0X80;
		txInvSrioData1.tx_frameType = 0;//unsopported here
		txInvSrioData1.clkDivide = 1;			//50M/2=25M
		txInvSrioData1.timerPeriodShadow = T1PRSVM;		
		txInvSrioData1.TimerEnable =1;		
		txInvSrioData1.clrFaultCmd =1;	
		txInvSrioData1.reservered0=0;//unsurpported
 		/*PWM special registers setting*/		
		txInvSrioData1.ACQ_ZRO=0xaaaa;/*ACQ_ZRO */
		txInvSrioData1.ACQ_CAU=0xaaaa;/*ACQ_CAU */
		txInvSrioData1.ACQ_PRD=0xaaaa;/*ACQ_PRD */
		txInvSrioData1.ACQ_CAD=0xaaaa;/*ACQ_CAD */	
		txInvSrioData1.CMPAU1=0;/*CMPAU_shadow[1] */
		txInvSrioData1.CMPAD1=0;/*CMPAD_shadow[1] */
		txInvSrioData1.CMPAU2=0;/*CMPAU_shadow[2] */
		txInvSrioData1.CMPAD2=0;/*CMPAD_shadow[2] */
		txInvSrioData1.CMPAU3=0;/*CMPAU_shadow[3] */
		txInvSrioData1.CMPAD3=0;/*CMPAD_shadow[3] */
		txInvSrioData1.CMPAU4=0;/*CMPAU_shadow[4] */
		txInvSrioData1.CMPAD4=0;/*CMPAD_shadow[4] */
		txInvSrioData1.CMPAU5=0;/*CMPAU_shadow[5] */
		txInvSrioData1.CMPAD5=0;/*CMPAD_shadow[5] */
		txInvSrioData1.CMPAU6=0;/*CMPAU_shadow[6] */
		txInvSrioData1.CMPAD6=0;/*CMPAD_shadow[6] */
		txInvSrioData1.minPulseTime=MinPulseCheck;/*min pulse time 10us,50M clock */
		txInvSrioData1.deadZone=DeadZone;/*dead zone time 50us,50M clock */
		txInvSrioData1.turnOffTime=250;/*IGCT1&4 turn off time befoe IGCT2&3 */
		txInvSrioData1.PwmEnable=0;/*pwm disable */	
		txInvSrioData1.protectEn=1;/*small pulse and deadzone and feedback protect enable*/
		
//		txInvSrioData1.ANPC_type_a=0;
//		txInvSrioData1.ANPC_type_b=0;
//		txInvSrioData1.ANPC_type_c=0;
		Inv1_npc();
		status=ast7600PpcSrioSingleSendData(inv1SrioID,txPWMSrioLen,(UINT8 *)&txInvSrioData1); 
		
		rxInvSrioData1=AST_PPC_SRIO_DMA_INBOUND_TAR+inv1SrioID*0x00100000;
		logMsg("invertor PWM board 1 SRIO initiation succeeded\n",0,0,0,0,0,0);
		logMsg("invertor PWM board 1 srio local memory start address is %x \n",rxInvSrioData1,0,0,0,0,0);
	}
	else
		logMsg("invertor PWM board 1 SRIO initiation failed\n",0,0,0,0,0,0);	
}
/*******************************************************************************/
/*Name		:InvPwm2Init_withSrio
*Description:逆变PWM卡2初始化SRIO设置，并初始化该卡的PWM寄存器
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void InvPwm2Init_withSrio(void)
{
	int vme_status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
	
	/*check invertor2 board*/
	vme_status=astVmeMapAlloc(&InvLocalBaseAdrs2,InvVmeBaseAdrs2,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
    logMsg("Invertor PWM  board 2 VME local memory start address is %x \n",InvLocalBaseAdrs2,0,0,0,0,0);
	if(vme_status==OK)
	{
		shortTemp0= *(short *)(InvLocalBaseAdrs2+0x0);
		*(short *)(InvLocalBaseAdrs2+0x2)=0xaaaa;
		shortTemp1=*(short *)(InvLocalBaseAdrs2+0x2);
		*(short *)(InvLocalBaseAdrs2+0x2)=0x5555;
		shortTemp2=*(short *)(InvLocalBaseAdrs2+0x2);
		if(shortTemp0==0x3000 && shortTemp1==0xaaaa && shortTemp2==0x5555)
		{
			do 
			{	
				j++;
				*(short *)(InvLocalBaseAdrs2+0x2)=j;
				shortTemp1=*(short *)(InvLocalBaseAdrs2+0x2);			
			}while(j<65535 && shortTemp1==j);				
		}		
	}
	if(j==65535)
	{
		selfCheck.bit.invBoard2OK=1;
		logMsg("invertor PWM board 2 VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.invBoard2OK=0;
		logMsg("invertor PWM board 2 VME check failed\n",0,0,0,0,0,0);
	}  	
	if(selfCheck.bit.invBoard2OK==1&& selfCheck.bit.ast7700SrioInitOK==1)   
	{
		inv2SrioID	= *(short *)(InvLocalBaseAdrs2+0X30);//srio_deviceid;
		inv2Srio_status	= *(short *)(InvLocalBaseAdrs2+0X3a);//
		inv2Srio_error	= *(short *)(InvLocalBaseAdrs2+0X44);//
		inv2Srio_igen_state=*(short *)(InvLocalBaseAdrs2+0X48);//
			
		*(short *)(InvLocalBaseAdrs2+0x1e)=1; 			
		//  registers control right set as SRIO
		*(short *)(InvLocalBaseAdrs2+0x32)=AST_PPC_SRIO_DMA_OUTBOUND_TAR>>16;		
		//	srio_local_target_addr_reg[31:16]<=vme_data;//local srio receive ram start address			
		*(short *)(InvLocalBaseAdrs2+0x34)=AST_PPC_SRIO_DMA_OUTBOUND_TAR&0xffff;			
		//	srio_local_target_addr_reg[15:0]<=vme_data;//local srio receive ram start address			
		*(short *)(InvLocalBaseAdrs2+0x36)=(AST_PPC_SRIO_DMA_INBOUND_BASE+inv2SrioID*0x00100000)>>16;		
		//	srio_dest_start_addr_reg[31:16]<=vme_data;//srio send data detination start address
		*(short *)(InvLocalBaseAdrs2+0x38)=(AST_PPC_SRIO_DMA_INBOUND_BASE+inv2SrioID*0x00100000)&0xffff;   		// srio_dest_start_addr_reg[15:0]<=vme_data;////srio send data detination start address											
		*(short *)(InvLocalBaseAdrs2+0x3c)=ast7600SrioID;   
		//   srio_dest_id[7:0],slave borad upload data to this SRIO ID
		*(short *)(InvLocalBaseAdrs2+0x3e)=srioMutiSendID;	
		//	组播ID   multicast_deviceid[7:0]		
		*(short *)(InvLocalBaseAdrs2+0x40)= 0x80;			
		//srio_tx_size[8:0],slave board is to upload bytes number;	
		*(short *)(InvLocalBaseAdrs2+0x42)= 0x80;			
		//rx_size[8:0],slave board is to receirve bytes number;	
		*(short *)(InvLocalBaseAdrs2+0x46)= 1;				
		//SRIO_SendEnable
	 
		/*general registers setting*/
	  	txInvSrioData2.boardID = 0x7600 ;
		txInvSrioData2.srio_deviceID = ast7600SrioID;
		txInvSrioData2.tx_liveCnt = txInvSrioData2.tx_liveCnt++;
		txInvSrioData2.tx_frameLen = 0X80;
		txInvSrioData2.tx_frameType = 0;//unsopported here
		txInvSrioData2.clkDivide = 1;			//50M/2=25M
		txInvSrioData2.timerPeriodShadow = T1PRSVM;		
		txInvSrioData2.TimerEnable =1;		
		txInvSrioData2.clrFaultCmd =1;	
		txInvSrioData2.reservered0=0;//unsurpported
 		/*PWM special registers setting*/		
		txInvSrioData2.ACQ_ZRO=0xaaaa;/*ACQ_ZRO */
		txInvSrioData2.ACQ_CAU=0xaaaa;/*ACQ_CAU */
		txInvSrioData2.ACQ_PRD=0xaaaa;/*ACQ_PRD */
		txInvSrioData2.ACQ_CAD=0xaaaa;/*ACQ_CAD */	
		txInvSrioData2.CMPAU1=0;/*CMPAU_shadow[1] */
		txInvSrioData2.CMPAD1=0;/*CMPAD_shadow[1] */
		txInvSrioData2.CMPAU2=0;/*CMPAU_shadow[2] */
		txInvSrioData2.CMPAD2=0;/*CMPAD_shadow[2] */
		txInvSrioData2.CMPAU3=0;/*CMPAU_shadow[3] */
		txInvSrioData2.CMPAD3=0;/*CMPAD_shadow[3] */
		txInvSrioData2.CMPAU4=0;/*CMPAU_shadow[4] */
		txInvSrioData2.CMPAD4=0;/*CMPAD_shadow[4] */
		txInvSrioData2.CMPAU5=0;/*CMPAU_shadow[5] */
		txInvSrioData2.CMPAD5=0;/*CMPAD_shadow[5] */
		txInvSrioData2.CMPAU6=0;/*CMPAU_shadow[6] */
		txInvSrioData2.CMPAD6=0;/*CMPAD_shadow[6] */
		txInvSrioData2.minPulseTime=MinPulseCheck;/*min pulse time 10us,50M clock */
		txInvSrioData2.deadZone=DeadZone;/*dead zone time 50us,50M clock */
		txInvSrioData2.turnOffTime=250;/*IGCT1&4 turn off time befoe IGCT2&3 */
		txInvSrioData2.PwmEnable=0;/*pwm disable */	
		txInvSrioData2.protectEn=1;/*small pulse and deadzone and feedback protect enable*/
		Inv2_npc();
		status=ast7600PpcSrioSingleSendData(inv2SrioID,txPWMSrioLen,(UINT8 *)&txInvSrioData2);
		
		rxInvSrioData2=AST_PPC_SRIO_DMA_INBOUND_TAR+inv2SrioID*0x00100000;
		
		logMsg("invertor PWM board 2 SRIO initiation succeeded\n",0,0,0,0,0,0);
		logMsg("invertor PWM board 2 srio local memory start address is %x \n",rxInvSrioData2,0,0,0,0,0);
	}
	else
		logMsg("invertor PWM board 2 SRIO initiation failed\n",0,0,0,0,0,0);	
}
 
/*******************************************************************************/
/*Name		:AD1Init_withSrio
*Description:AD卡1初始化SRIO设置，主要是初始化该卡的软保护值
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void AD1Init_withSrio(void)
{
	int vme_status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
	
	/*check AD1 board*/
	vme_status=astVmeMapAlloc(&ADLocalBaseAdrs1,ADVmeBaseAdrs1,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
    logMsg("AD  board 1 VME local memory start address is %x \n",ADLocalBaseAdrs1,0,0,0,0,0);
	if(vme_status==OK)
	{
		shortTemp0= *(short *)(ADLocalBaseAdrs1+0x0);
		*(short *)(ADLocalBaseAdrs1+0x2)=0xaaaa;
		shortTemp1=*(short *)(ADLocalBaseAdrs1+0x2);
		*(short *)(ADLocalBaseAdrs1+0x2)=0x5555;
		shortTemp2=*(short *)(ADLocalBaseAdrs1+0x2);
		if(shortTemp0==0x1000 && shortTemp1==0xaaaa && shortTemp2==0x5555)
		{
			do 
			{	
				j++;
				*(short *)(ADLocalBaseAdrs1+0x2)=j;
				shortTemp1=*(short *)(ADLocalBaseAdrs1+0x2);			
			}while(j<65535 && shortTemp1==j);				
		}	 
	}
	if(j==65535)
	{
		selfCheck.bit.adBoard1OK=1;
		logMsg("AD board 1 VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.adBoard1OK=0;
		logMsg("AD board 1 VME check failed\n",0,0,0,0,0,0);
	}  	
		
  if(selfCheck.bit.adBoard1OK==1&& selfCheck.bit.ast7700SrioInitOK==1)   
  {
	ad1SrioID	= *(short *)(ADLocalBaseAdrs1+0X30);//srio_deviceid;
	ad1Srio_status	= *(short *)(ADLocalBaseAdrs1+0X3a);//
	ad1Srio_error	= *(short *)(ADLocalBaseAdrs1+0X44);//
	ad1Srio_igen_state=*(short *)(ADLocalBaseAdrs1+0X48);//
	
	*(short *)(ADLocalBaseAdrs1+0x1e)=1; 			
	//  registers control right set as SRIO
	*(short *)(ADLocalBaseAdrs1+0x32)=AST_PPC_SRIO_DMA_OUTBOUND_TAR>>16;		
	//	srio_local_target_addr_reg[31:16]<=vme_data;//local srio receive ram start address			
	*(short *)(ADLocalBaseAdrs1+0x34)=AST_PPC_SRIO_DMA_OUTBOUND_TAR&0xffff;			
	//	srio_local_target_addr_reg[15:0]<=vme_data;//local srio receive ram start address			
	*(short *)(ADLocalBaseAdrs1+0x36)=(AST_PPC_SRIO_DMA_INBOUND_BASE+ad1SrioID*0x00100000)>>16;		
	//	srio_dest_start_addr_reg[31:16]<=vme_data;//srio send data detination start address
	*(short *)(ADLocalBaseAdrs1+0x38)=(AST_PPC_SRIO_DMA_INBOUND_BASE+ad1SrioID*0x00100000)&0xffff;   		// srio_dest_start_addr_reg[15:0]<=vme_data;////srio send data detination start address											
	*(short *)(ADLocalBaseAdrs1+0x3c)=ast7600SrioID;   
	//   srio_dest_id[7:0],slave borad upload data to this SRIO ID
	*(short *)(ADLocalBaseAdrs1+0x3e)=srioMutiSendID;	
	//	组播ID   multicast_deviceid[7:0]		
	*(short *)(ADLocalBaseAdrs1+0x40)= 0x80;			
	//srio_tx_size[8:0],slave board is to upload bytes number;	
	*(short *)(ADLocalBaseAdrs1+0x42)= 0x80;			
	//rx_size[8:0],slave board is to receirve bytes number;	
	*(short *)(ADLocalBaseAdrs1+0x46)= 1;				
	//SRIO_SendEnable
	
	txAdSrioData1.boardID = 0x7600 ;
	txAdSrioData1.srio_deviceID = ast7600SrioID;
	txAdSrioData1.tx_liveCnt = txAdSrioData1.tx_liveCnt++;
	txAdSrioData1.tx_frameLen = 0X80;
	txAdSrioData1.tx_frameType = 0;//unsopported here
	txAdSrioData1.clkDivide = 0;
	txAdSrioData1.timerPeriodShadow = T1PRSVM;		
	txAdSrioData1.TimerEnable =1;		
	txAdSrioData1.clrFaultCmd =1;	
	txAdSrioData1.reservered0=0;//unsurpported
		/*AD special registers setting*/
	txAdSrioData1.softProtect_value[0]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[1]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[2]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[3]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[4]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[5]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[6]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[7]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[8]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[9]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[10]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[11]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[12]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[13]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[14]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_value[15]=8.0/10.0*8192;//8V
	txAdSrioData1.softProtect_en=0x0000;//every bit 1 channel
	txAdSrioData1.hardProtect_en=0x0000;//every bit 1 channel
	status=ast7600PpcSrioSingleSendData(ad1SrioID,txADSrioLen,(UINT8 *)&txAdSrioData1);
	
	rxAdSrioData1=AST_PPC_SRIO_DMA_INBOUND_TAR+ad1SrioID*0x00100000;
	logMsg("AD board 1 SRIO initiation succeeded\n",0,0,0,0,0,0);
	logMsg("AD board 1 srio local memory start address is %x \n",rxAdSrioData1,0,0,0,0,0);
  }
  else
	logMsg("AD board 1 SRIO initiation failed\n",0,0,0,0,0,0);
}   
/*******************************************************************************/
/*Name		:AD2Init_withSrio
*Description:AD卡2初始化SRIO设置，主要是初始化该卡的软保护值
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void AD2Init_withSrio(void)
{
	int vme_status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
	
	/*check AD2 board*/
	vme_status=astVmeMapAlloc(&ADLocalBaseAdrs2,ADVmeBaseAdrs2,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
    logMsg("AD  board 2 VME local memory start address is %x \n",ADLocalBaseAdrs2,0,0,0,0,0);
	if(vme_status==OK)
	{
		shortTemp0= *(short *)(ADLocalBaseAdrs2+0x0);
		*(short *)(ADLocalBaseAdrs2+0x2)=0xaaaa;
		shortTemp1=*(short *)(ADLocalBaseAdrs2+0x2);
		*(short *)(ADLocalBaseAdrs2+0x2)=0x5555;
		shortTemp2=*(short *)(ADLocalBaseAdrs2+0x2);
		if(shortTemp0==0x1000 && shortTemp1==0xaaaa && shortTemp2==0x5555)
		{
			do 
			{	
				j++;
				*(short *)(ADLocalBaseAdrs2+0x2)=j;
				shortTemp1=*(short *)(ADLocalBaseAdrs2+0x2);			
			}while(j<65535 && shortTemp1==j);				
		}		
	}
	if(j==65535)
	{
		selfCheck.bit.adBoard2OK=1;
		logMsg("AD board 2 VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.adBoard2OK=0;
		logMsg("AD board 2 VME check failed\n",0,0,0,0,0,0);
	}	
	
	if(selfCheck.bit.adBoard2OK==1&& selfCheck.bit.ast7700SrioInitOK==1)  
	{	  
		ad2SrioID	= *(short *)(ADLocalBaseAdrs2+0X30);//srio_deviceid;
		ad2Srio_status	= *(short *)(ADLocalBaseAdrs2+0X3a);//
		ad2Srio_error	= *(short *)(ADLocalBaseAdrs2+0X44);//
		ad2Srio_igen_state=*(short *)(ADLocalBaseAdrs2+0X48);//
			
		*(short *)(ADLocalBaseAdrs2+0x1e)=1; 			
		//  registers control right set as SRIO
		*(short *)(ADLocalBaseAdrs2+0x32)=AST_PPC_SRIO_DMA_OUTBOUND_TAR>>16;		
		//	srio_local_target_addr_reg[31:16]<=vme_data;//local srio receive ram start address			
		*(short *)(ADLocalBaseAdrs2+0x34)=AST_PPC_SRIO_DMA_OUTBOUND_TAR&0xffff;			
		//	srio_local_target_addr_reg[15:0]<=vme_data;//local srio receive ram start address			
		*(short *)(ADLocalBaseAdrs2+0x36)=(AST_PPC_SRIO_DMA_INBOUND_BASE+ad2SrioID*0x00100000)>>16;		
		//	srio_dest_start_addr_reg[31:16]<=vme_data;//srio send data detination start address
		*(short *)(ADLocalBaseAdrs2+0x38)=(AST_PPC_SRIO_DMA_INBOUND_BASE+ad2SrioID*0x00100000)&0xffff;   		
		// srio_dest_start_addr_reg[15:0]<=vme_data;////srio send data detination start address											
		*(short *)(ADLocalBaseAdrs2+0x3c)=ast7600SrioID;   
		//   srio_dest_id[7:0],slave borad upload data to this SRIO ID
		*(short *)(ADLocalBaseAdrs2+0x3e)=srioMutiSendID;	
		//	组播ID   multicast_deviceid[7:0]		
		*(short *)(ADLocalBaseAdrs2+0x40)= 0x80;			
		//srio_tx_size[8:0],slave board is to upload bytes number;	
		*(short *)(ADLocalBaseAdrs2+0x42)= 0x80;			
		//rx_size[8:0],slave board is to receirve bytes number;	
		*(short *)(ADLocalBaseAdrs2+0x46)= 1;				
	//SRIO_SendEnable	  
		
		
		txAdSrioData2.boardID = 0x7600 ;
		txAdSrioData2.srio_deviceID = ast7600SrioID;
		txAdSrioData2.tx_liveCnt = txAdSrioData2.tx_liveCnt++;
		txAdSrioData2.tx_frameLen = 0X80;
		txAdSrioData2.tx_frameType = 0;//unsopported here
		txAdSrioData2.clkDivide = 0;
		txAdSrioData2.timerPeriodShadow = T1PRSVM;		
		txAdSrioData2.TimerEnable =1;		
		txAdSrioData2.clrFaultCmd =1;	
		txAdSrioData2.reservered0=0;//unsurpported
			/*AD special registers setting*/
		txAdSrioData2.softProtect_value[0]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[1]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[2]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[3]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[4]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[5]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[6]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[7]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[8]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[9]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[10]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[11]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[12]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[13]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[14]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_value[15]=8.0/10.0*8192;//8V
		txAdSrioData2.softProtect_en=0x0000;//every bit 1 channel
		txAdSrioData2.hardProtect_en=0x0000;//every bit 1 channel
		status=ast7600PpcSrioSingleSendData(ad2SrioID,txADSrioLen,(UINT8 *)&txAdSrioData2);
		
		rxAdSrioData2=AST_PPC_SRIO_DMA_INBOUND_TAR+ad2SrioID*0x00100000;
		logMsg("AD board 1 SRIO initiation succeeded\n",0,0,0,0,0,0);
		logMsg("AD board 2 srio local memory start address is %x \n",rxAdSrioData2,0,0,0,0,0);
	}
	  else
		logMsg("AD board 2 SRIO initiation failed\n",0,0,0,0,0,0);	
}   
/*******************************************************************************/
/*Name		:ClkInit_withSrio
*Description:CLOCK卡初始化SRIO设置，主要是初始化该卡的定时器和同步信号
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void ClkInit_withSrio(void)/*clock  FPGA board iniitation GPS and PWM_EN setting through SRIO channel*/
{
	int vme_status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
	
	/*check clock board*/
	vme_status=astVmeMapAlloc(&ClkLocalBaseAdrs,ClkVmeBaseAdrs,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
    logMsg("Clock  board VME local memory start address is %x \n",ClkLocalBaseAdrs,0,0,0,0,0);
	if(vme_status==OK)
	{
		shortTemp0= *(short *)(ClkLocalBaseAdrs+0x0);
		*(short *)(ClkLocalBaseAdrs+0x2)=0xaaaa;
		shortTemp1=*(short *)(ClkLocalBaseAdrs+0x2);
		*(short *)(ClkLocalBaseAdrs+0x2)=0x5555;
		shortTemp2=*(short *)(ClkLocalBaseAdrs+0x2);
		if(shortTemp0==0x6000 && shortTemp1==0xaaaa && shortTemp2==0x5555)	 
		{			 
			do 
			{	
				j++;
				*(short *)(ClkLocalBaseAdrs+0x2)=j;
				shortTemp1=*(short *)(ClkLocalBaseAdrs+0x2);			
			}while(j<65535 && shortTemp1==j);			
		}		

	}
	if(j==65535)
	{
		selfCheck.bit.clkBoardOK=1;
		logMsg("CLOCK board  VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.clkBoardOK=0;
		logMsg("CLOCK board  VME check failed\n",0,0,0,0,0,0);
	}
	/*CLOCL BOARD initiation*/    
  if(selfCheck.bit.clkBoardOK==1&& selfCheck.bit.ast7700SrioInitOK==1)   
  {
	  
	  clkSrioID	= *(short *)(ClkLocalBaseAdrs+0X30);//srio_deviceid;
	  clkSrio_status	= *(short *)(ClkLocalBaseAdrs+0X3a);//
	  clkSrio_error	= *(short *)(ClkLocalBaseAdrs+0X44);//
	  clkSrio_igen_state=*(short *)(ClkLocalBaseAdrs+0X48);//
	  *(short *)(ClkLocalBaseAdrs+0x20)=1; /*synchouns out period,every n timer period output a syn_clk*/
	  /*n=1,every 1 period toggle 1 syn_clk, here clk board timer is 600hz,so syn_clk is 300 hz*/
	  
	  *(short *)(ClkLocalBaseAdrs+0x1e)=1; 			
	  //  registers control right set as SRIO
	  *(short *)(ClkLocalBaseAdrs+0x32)=AST_PPC_SRIO_DMA_OUTBOUND_TAR>>16;		
	  //	srio_local_target_addr_reg[31:16]<=vme_data;//local srio receive ram start address			
	  *(short *)(ClkLocalBaseAdrs+0x34)=AST_PPC_SRIO_DMA_OUTBOUND_TAR&0xffff;			
	  //	srio_local_target_addr_reg[15:0]<=vme_data;//local srio receive ram start address			
	  *(short *)(ClkLocalBaseAdrs+0x36)=(AST_PPC_SRIO_DMA_INBOUND_BASE+clkSrioID*0x00100000)>>16;		
	  //	srio_dest_start_addr_reg[31:16]<=vme_data;//srio send data detination start address
	  *(short *)(ClkLocalBaseAdrs+0x38)=(AST_PPC_SRIO_DMA_INBOUND_BASE+clkSrioID*0x00100000)&0xffff;   		// srio_dest_start_addr_reg[15:0]<=vme_data;////srio send data detination start address											
	  *(short *)(ClkLocalBaseAdrs+0x3c)=ast7600SrioID;  
	  //   srio_dest_id[7:0],slave borad upload data to this SRIO ID
	  *(short *)(ClkLocalBaseAdrs+0x3e)=srioMutiSendID;	
	  //	组播ID   multicast_deviceid[7:0]		
	  *(short *)(ClkLocalBaseAdrs+0x40)= 0x80;			
	  //srio_tx_size[8:0],slave board is to upload bytes number;	
	  *(short *)(ClkLocalBaseAdrs+0x42)= 0x80;			
	  //rx_size[8:0],slave board is to receirve bytes number;	
	  *(short *)(ClkLocalBaseAdrs+0x46)= 1;				
	  //SRIO_SendEnable
	 
	  txClkSrioData.boardID = 0x7600 ;
	  txClkSrioData.srio_deviceID = ast7600SrioID;
	  txClkSrioData.tx_liveCnt = txClkSrioData.tx_liveCnt++;
	  txClkSrioData.tx_frameLen = 0X80;
	  txClkSrioData.tx_frameType = 0;//unsopported here
	  txClkSrioData.clkDivide = 1;                   //50M clock frequency is dived by 2,changed to 25M
	  txClkSrioData.timerPeriodShadow = T1PRSVM;	//41666*0.04us=1.6666ms,50M clk is dived by 2	
	  txClkSrioData.TimerEnable =1;		
	  txClkSrioData.clrFaultCmd =1;	
	  txClkSrioData.reservered0=0;//unsurpported 	
	  txClkSrioData.GPS_Channel=3;
	  txClkSrioData.PwmEnable=0;//所有的PWM使能是从时钟板的背板局部总线bus_enable1发出去的
	  status=ast7600PpcSrioSingleSendData(clkSrioID,txClkSrioLen,(UINT8 *)&txClkSrioData);	
	  
	  rxClkSrioData=AST_PPC_SRIO_DMA_INBOUND_TAR+clkSrioID*0x00100000;
	  logMsg("CLOCK board SRIO initiation succeeded\n",0,0,0,0,0,0);
	  logMsg("CLOCK board srio local memory start address is %x \n",rxClkSrioData,0,0,0,0,0);
	}
  else
	  logMsg("CLOCK board SRIO initiation failed\n",0,0,0,0,0,0);
}    
/******************************************************************************/ 
/*Name		:DA4140Init_withVME
*Description:GE VMIC414 DA卡初始化
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void DA4140Init_withVME(void)
{
	int vme_status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
	
	/*check DA VMIC4140 board*/
	vme_status=astVmeMapAlloc(&DALocalBaseAdrs,DAVmeBaseAdrs,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
    logMsg("DA board VME local memory start address is %x \n",DALocalBaseAdrs,0,0,0,0,0);	
	if(vme_status==OK)
	{
		shortTemp0= *(short *)(DALocalBaseAdrs+0x0);		 
	}	
	if(shortTemp0==0x2A00 )
	{
		selfCheck.bit.DABoardOK=1;
		logMsg("DA board 4140 VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.DABoardOK=0; 
		logMsg("DA board 4140 VME check failed\n",0,0,0,0,0,0);
	}
	
	if(selfCheck.bit.DABoardOK==1)
	{	
		*(short *)(DALocalBaseAdrs+0x2)=0Xc000;/*DA4140 init */
		logMsg("DA board 4140 VME initiation succeeded\n",0,0,0,0,0,0);
	}
}

/*************************************************************************/
/*Name		:AD_Sample_SRIO
*Description:直接从7600CPU的对应 AD卡上传 srio数据的内存里读取数据；14bitAD芯片，bit14是符号位，反向补码
*Input		:rxAdSrioData1,rxAdSrioData2		
*Output		:recIa1等
*Return		:无
*History	:2016,by wxx
*/

void AD_Sample_SRIO(void)
{
	/*2 AD boards made by ieecas ,20 channal signal*/

	short adData1[16],adData2[16];/*2 AD boards made by ieecas ,20 channal signal*/
	 float adData1float[16];  
	 short i;
	 float k_ad_I,k_ad_V,k_u_grid;/*ad convert coefficient*/
	 double Coefficient_volt  = 1.12,Coefficient_current = 1.1;
	 
	 k_ad_I=0.048828125;//10.0/8192/15.0/0.4*2000*Coefficient_current;
	 k_ad_V=0.13253348;//10.0/8192/15.0/0.4*5000*Coefficient_volt;
     k_u_grid=0.305175781/1.64;//10*250/8192
	 //将AD板的电气量测量值赋给数组
	 if(selfCheck.bit.adBoard1OK==1)
	 {
		for(i=0;i<16;i++)
		 {
		 // adData1[i]=*(short *)(ADLocalBaseAdrs1+0x80+2*i);
			adData1[i]=rxAdSrioData1->AD_chanel_data[i];
		 }
	 }
	 if(selfCheck.bit.adBoard2OK==1)
	 {
		 for(i=0;i<16;i++)
		 {
		  //adData2[i]=*(short *)(ADLocalBaseAdrs2+0x80+2*i);
			 adData2[i]=rxAdSrioData2->AD_chanel_data[i];
		 }
	 }
	 for(i=0;i<16;i++)
	{
	 adData1[i]=(short)(adData1[i]<<2)>>2;
	 adData2[i]=(short)(adData2[i]<<2)>>2;
	}
	//变压器变比17.5/3
	//GridUa = adData1[0]*50000.0/8192.0*3/35;       //输入连接单个变压器时，除以17.5；输入通过变压器串联时除以35 ,测量得到网测电压，通过计算折算到副边(为了计算前馈)
	//GridUb = adData1[1]*50000.0/8192.0*3/35;
	//GridUc = adData1[2]*50000.0/8192.0*3/35;

	 GridUa = adData1[0]*(50000.0/8192.0*3/17.5);       //输入连接单个变压器时，除以17.5；输入通过变压器并联时除以15 ,测量得到网测电压，通过计算折算到副边(为了计算前馈)
	 GridUb = adData1[1]*(50000.0/8192.0*3/17.5);
	 GridUc = adData1[2]*(50000.0/8192.0*3/17.5);

	 GridIa = adData2[13]*10000.0/8192.0;            //网侧三相电流
	 GridIb = adData2[14]*10000.0/8192.0;
	 GridIc = adData2[15]*10000.0/8192.0;

	 recUa1 = adData1[12]*10000.0/8192.0;            //整流1 Ua

	 recIa1 = adData1[3]*10000.0/8192.0;            //整流1 三相电流
	 recIb1 = adData1[4]*10000.0/8192.0;
	 recIc1 = adData1[5]*10000.0/8192.0;

	 recUa2 = adData1[13]*10000.0/8192.0;            //整流2 Ua

	 recIa2 = adData1[9]*10000.0/8192.0;            //整流2 三相电流
	 recIb2 = adData1[10]*10000.0/8192.0;
	 recIc2 = adData1[11]*10000.0/8192.0;
	 
	 Udc1 = adData1[6]*10000.0/8192.0;              //母线电容电压
	 Udc2 = adData1[7]*10000.0/8192.0;
//	 Udc  = adData1[8]*10000.0/8192.0;
	 
	  
	 invIu1 = adData2[4]*10000.0/8192.0;           //逆变1 三相电流
	 invIv1 = adData2[5]*10000.0/8192.0;
	 invIw1 = adData2[6]*10000.0/8192.0;

	 invUa1 = adData1[14]*10000.0/8192.0;           //逆变1 三相电压
	 invUb1 = adData1[15]*10000.0/8192.0;
	 invUc1 = adData2[0]*10000.0/8192.0;

	 invIu2 = adData2[7]*10000.0/8192.0;           //逆变2 三相电流
	 invIv2 = adData2[8]*10000.0/8192.0;
	 invIw2 = adData2[9]*10000.0/8192.0;
	 
	 invUa2 = adData2[1]*10000.0/8192.0;           //逆变2 三相电压
	 invUb2 = adData2[2]*10000.0/8192.0;
	 invUc2 = adData2[3]*10000.0/8192.0;
	 
	 IaMotor = adData2[10]*10000.0/8192.0;         //负载 三相电流
	 IbMotor = adData2[11]*10000.0/8192.0;
	 IcMotor = adData2[12]*10000.0/8192.0;
		 
	 
	 filter_Udc1.filter_input=Udc1;
	 filter_function(&filter_Udc1);
	 Udc1_filter=filter_Udc1.filter_out;
	 
	 filter_Udc2.filter_input=Udc2;
	 filter_function(&filter_Udc2);
	 Udc2_filter=filter_Udc2.filter_out;

	  
	 
	 
	}
/*********************************************************************************************/
/*Name		:readGpsData_VME
*Description:通过VME读取时钟板的GPS数据,保留不用
*Input		:ClkLocalBaseAdrs		
*Output		:YrHDayBuf，DayHrBuf，MinSecBuf等
*Return		:无
*History	:2016,by wxx
*/
void readGpsData_VME(void)
{
  if( selfCheck.bit.clkBoardOK==1)
 { 
	 GPS_OK=*(short *)(ClkLocalBaseAdrs+0x98);
	 if(GPS_OK!=0 )/*&& rxMcuData.GPS_OK==1)*/
	 {
		selfCheck.bit.GPSOK=1;
		YrHDayBuf=*(short *)(ClkLocalBaseAdrs+0x96);
		DayHrBuf=*(short *)(ClkLocalBaseAdrs+0x94);
		MinSecBuf=*(short *)(ClkLocalBaseAdrs+0x92);
	 }
 }
}
/**********************************************************/
/*Name		:readGpsData_VME
*Description:通过SRIO读取时钟板的GPS数据
*Input		:ClkLocalBaseAdrs		
*Output		:YrHDayBuf，DayHrBuf，MinSecBuf等
*Return		:无
*History	:2016,by wxx
*/
void readGpsData_SRIO(void)
{
  if( selfCheck.bit.clkBoardOK==1)
 { 	 
	 if(rxClkSrioData->GPS_OK!=0)/* && rxMcuData.GPS_OK==1)*/
	 {
		selfCheck.bit.GPSOK=1;
		YrHDayBuf=rxClkSrioData->YrHDayBuf;
		DayHrBuf=rxClkSrioData->DayHrBuf;
		MinSecBuf=rxClkSrioData->MinSecBuf;
	 }
	 else
	 {
		selfCheck.bit.GPSOK=0;			 
	 }	 
 }
}
/**********************************************************/
/*Name		:checkSlaveSrioConnectStatus
*Description:检测各板卡的SRIO连接状态，每周期都要检测
*Input		:rxClkSrioData，rxRecSrioData1，	rxRecSrioData2，	rxInvSrioData1，rxInvSrioData2，rxAdSrioData1，rxAdSrioData2
*Output		:clkSrioConnectOk
*Return		:无
*History	:2016,by wxx
*/
void checkSlaveSrioConnectStatus(void)
{
	if(selfCheck.bit.clkBoardOK==1) 
	{
		if(	rxClkSrioData->BoardID==0x6000
			&& rxClkSrioData->srio_deviceid==clkSrioID
			&& rxClkSrioData->srio_status & 0x7F == 0X7F
			/*&& rxClkSrioData->h_tc_error==0*/
			&& rxClkSrioData->rx_liveCnt!=rxClkData_srio_rx_live_cnt_old
			&& rxClkSrioData->tx_liveCnt_fb==txClkSrioData.tx_liveCnt)
		{
			selfCheck.bit.clkSrioConnectOk=1;
			 
		}
		else
		{
			if(selfCheck.bit.clkSrioConnectOk==1)
			logMsg("clkSrioConnectOk=0\n",0,0,0,0,0,0,0);
			selfCheck.bit.clkSrioConnectOk=0;	
			 
		}	
		rxClkData_srio_rx_live_cnt_old=rxClkSrioData->rx_liveCnt;
	}
	if(selfCheck.bit.recBoard1OK==1) 
	{
		if(	rxRecSrioData1->BoardID==0x3000
			&& rxRecSrioData1->srio_deviceid==rec1SrioID
			&& rxRecSrioData1->srio_status& 0x7F == 0X7F
			/*&& rxRecSrioData1->h_tc_error==0*/
			&& rxRecSrioData1->rx_liveCnt!=rxRecPWMData1_srio_rx_live_cnt_old
			&& rxRecSrioData1->tx_liveCnt_fb==txRecSrioData1.tx_liveCnt	)
		{
			selfCheck.bit.rec1SrioConnectOk=1;
			 
		}
		else
		{
			if(selfCheck.bit.rec1SrioConnectOk==1)
				logMsg("rec1SrioConnectOk=0\n",0,0,0,0,0,0,0);
			selfCheck.bit.rec1SrioConnectOk=0;
			 
		}
		rxRecPWMData1_srio_rx_live_cnt_old=rxRecSrioData1->rx_liveCnt;
	}
	if(selfCheck.bit.recBoard2OK==1) 
	{
		if(	rxRecSrioData2->BoardID==0x3000
			&& rxRecSrioData2->srio_deviceid==rec2SrioID
			&& rxRecSrioData2->srio_status& 0x7F == 0X7F
			/*&& rxRecSrioData2->h_tc_error==0*/
			&& rxRecSrioData2->rx_liveCnt!=rxRecPWMData2_srio_rx_live_cnt_old
			&& rxRecSrioData2->tx_liveCnt_fb==txRecSrioData2.tx_liveCnt	)
		{
			selfCheck.bit.rec2SrioConnectOk=1;
		}
		else
		{
			if(selfCheck.bit.rec2SrioConnectOk==1)
				logMsg("rec2SrioConnectOk=0\n",0,0,0,0,0,0,0);
			selfCheck.bit.rec2SrioConnectOk=0;	
 
		}
		rxRecPWMData2_srio_rx_live_cnt_old=rxRecSrioData2->rx_liveCnt;

	}
	
	if(selfCheck.bit.invBoard1OK==1) 
	{
		if(	rxInvSrioData1->BoardID==0x3000
			&& rxInvSrioData1->srio_deviceid==inv1SrioID
			&& rxInvSrioData1->srio_status& 0x7F == 0X7F
			/*&& rxInvSrioData1->h_tc_error==0*/
			&& rxInvSrioData1->rx_liveCnt!=rxInvPWMData1_srio_rx_live_cnt_old
			&& rxInvSrioData1->tx_liveCnt_fb==txInvSrioData1.tx_liveCnt)
		{
			selfCheck.bit.inv1SrioConnectOk=1;
			rxInvPWMData1_srio_rx_live_cnt_old=rxInvSrioData1->rx_liveCnt;
		}
		else
		{
			if(selfCheck.bit.inv1SrioConnectOk==1)
				logMsg("inv1SrioConnectOk=0\n",0,0,0,0,0,0,0);
			selfCheck.bit.inv1SrioConnectOk=0;
			 
		}	
	}
	if(selfCheck.bit.invBoard2OK==1) 
	{
		if(	rxInvSrioData2->BoardID==0x3000
			&& rxInvSrioData2->srio_deviceid==inv2SrioID
			&& rxInvSrioData2->srio_status& 0x7F == 0X7F
			/*&& rxInvSrioData2->h_tc_error==0*/
			&& rxInvSrioData2->rx_liveCnt!=rxInvPWMData2_srio_rx_live_cnt_old
			&& rxInvSrioData2->tx_liveCnt_fb==txInvSrioData2.tx_liveCnt)
		{
			selfCheck.bit.inv2SrioConnectOk=1;
		}
		else
		{
			if(selfCheck.bit.inv2SrioConnectOk==1)
				logMsg("inv2SrioConnectOk=0\n",0,0,0,0,0,0,0);
			selfCheck.bit.inv2SrioConnectOk=0;	
			 
		}
		rxInvPWMData2_srio_rx_live_cnt_old=rxInvSrioData2->rx_liveCnt;
	}
	if(selfCheck.bit.adBoard1OK==1) 
	{
		if(	rxAdSrioData1->BoardID==0x1000
			&& rxAdSrioData1->srio_deviceid==ad1SrioID
			&& rxAdSrioData1->srio_status& 0x7F == 0X7F
			/*&& rxAdSrioData1->h_tc_error==0*/
			&& rxAdSrioData1->rx_liveCnt!=rxAdData1_srio_rx_live_cnt_old
			&& rxAdSrioData1->tx_liveCnt_fb==txAdSrioData1.tx_liveCnt)
		{
			selfCheck.bit.ad1SrioConnectOk=1;
		}
		else
		{
			if(selfCheck.bit.ad1SrioConnectOk==1)
				logMsg("ad1SrioConnectOk=0\n",0,0,0,0,0,0,0);
			selfCheck.bit.ad1SrioConnectOk=0;	
			 
		}	
		rxAdData1_srio_rx_live_cnt_old=rxAdSrioData1->rx_liveCnt;
	}
	if(selfCheck.bit.adBoard2OK==1) 
	{
		if(	rxAdSrioData2->BoardID==0x1000
			&& rxAdSrioData2->srio_deviceid==ad2SrioID
			&& rxAdSrioData2->srio_status & 0x7F == 0X7F
			/*&& rxAdSrioData2->h_tc_error==0*/
			&& rxAdSrioData2->rx_liveCnt!=rxAdData2_srio_rx_live_cnt_old
			&& rxAdSrioData2->tx_liveCnt_fb==txAdSrioData2.tx_liveCnt)
		{
			selfCheck.bit.ad2SrioConnectOk=1;
		}
		else
		{
			if(selfCheck.bit.ad2SrioConnectOk==1)
				logMsg("ad2SrioConnectOk=0\n",0,0,0,0,0,0,0);
			selfCheck.bit.ad2SrioConnectOk=0;	
			 
		}
		rxAdData2_srio_rx_live_cnt_old=rxAdSrioData2->rx_liveCnt;

	}
}
/*******************************************************************************************/
/*
* Name		:read_7700_SrioData
*Description:从SRIO接收内存对应交换卡7700的地址读出7700发送过来的数据
*Input		:结构体指针*rx_7700_data_pointer
*Output		:结构体rx_7700_data
*Return		:无
*History	:2018-07-18,modified by wxx
*/
void read_7700_SrioData(void)
{
	int i=0;
	int size=0;
	size=sizeof(struct RX_7700_STRUCT);
	if(selfCheck.bit.ast7700SrioInitOK==1)
	{
		memcpy((void *)&rx_7700_data,(const void *)rx_7700_data_pointer,size);
	}
}
/*******************************************************************************************/
/*
* Name		:write_7700_SrioData
*Description:向SRIO发送内存对应交换卡7700的地址写数据
*Input		:结构体指针tx_7700_data
*Output		:无
*Return		:无
*History	:2018-07-18,modified by wxx
*/
void write_7700_SrioData(void)
{
	int i=0;
	int size=0;
	size=sizeof(struct RX_7700_STRUCT);
	if(selfCheck.bit.ast7700SrioInitOK==1)
	{
		tx_7700_data.boardID=0x7600;
		tx_7700_data.liveCnt++;
		tx_7700_data.short_variable[0]=0x1234;
		tx_7700_data.int_variable[0]=0x12345678;
		tx_7700_data.float_variable[0]=1234.5678;
		
		/* add test code here ,switch test */		

				if(switchINcmd==1)
					tx_7700_data.short_variable[1]=0x1122;
				else
					tx_7700_data.short_variable[1]=0x3344;
				
				if(switch_DT_BTcmd==1)
					tx_7700_data.short_variable[2]=0x1122;
				else
					tx_7700_data.short_variable[2]=0x3344;
				
				if(switchOUTcmd==1)
					tx_7700_data.short_variable[3]=0x1122;
				else
					tx_7700_data.short_variable[3]=0x3344;	
				
				if(preChargecmd==1)
					tx_7700_data.short_variable[4]=0x1122;
				else
					tx_7700_data.short_variable[4]=0x3344;	
				
				tx_7700_data.short_variable[5]=Ratiocmd;   //变比


				
		status=ast7600PpcSrioSingleSendData(0,sizeof(struct TX_7700_STRUCT),(UINT8 *)&tx_7700_data);

	 }
}
