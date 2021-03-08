/*  
* target: chooper board initition PWM control   
* Copyright 2019,7,3 IEE, Inc. 
* description:   
* function: 该文件的功能是斩波板的读写，VME的读写和SRIO的读写以及斩波控制
* debug: 
* modification history:20190703 by wxx
*/

#include "vxWorks.h"
#include "vme.h"
#include "math.h" 
#include "h/astVME.h"
#include "h/drv/vme/altioctl.h"

#include "variable_main.h"
#include "GlobalVar_Rec.h"
#include "GlobalVar_Inv.h"
#include "chopper.h"
#include "ast7600Srio.h"
/***************************************************************************************/
unsigned short chopSrioID;
unsigned short chopSrio_status	;//
unsigned short chopSrio_error;//
unsigned short chopSrio_igen_state;
unsigned short rxChopData_srio_rx_live_cnt_old=0;
struct TX_CHOP_SRIO_Struct  txChopSrioData;
struct RX_CHOP_SRIO_Struct  *rxChopSrioData;
int txChopSrioLen= 0;
int rxChopSrioLen= 0;
char*  	ChopLocalBaseAdrs; 
/*******************************************************************************/
/*Name		:ChopInit_withSrio
*Description:斩波PWM卡初始化SRIO设置
*Input		:无		
*Output		:无
*Return		:无
*History	:2019,by wxx
*/
void ChopInit_withSrio(void)
{
	int vme_status=0;
	int status=0;
	unsigned short shortTemp0=0,shortTemp1=0,shortTemp2=0;
	unsigned int j=0;
	
	txChopSrioLen= sizeof(struct TX_CHOP_SRIO_Struct);
	rxChopSrioLen= sizeof(struct RX_CHOP_SRIO_Struct);	
	
	memset( &txChopSrioData, 0, sizeof(struct TX_CHOP_SRIO_Struct));
	memset( &txChopSrioData, 0, sizeof(struct RX_CHOP_SRIO_Struct));
	
	/*check rectifier1 board*/
	vme_status=astVmeMapAlloc(&ChopLocalBaseAdrs,ChopVmeBaseAdrs,0x200000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_SHORT,RDWR_SWAP_NO);
	logMsg("Chopper PWM  board  VME local memory start address is %x \n",ChopLocalBaseAdrs,0,0,0,0,0);
	if(vme_status==OK)
	{
		shortTemp0= *(short *)(ChopLocalBaseAdrs+0x0);
		*(short *)(ChopLocalBaseAdrs+0x2)=0xaaaa;
		shortTemp1=*(short *)(ChopLocalBaseAdrs+0x2);
		*(short *)(ChopLocalBaseAdrs+0x2)=0x5555;
		shortTemp2=*(short *)(ChopLocalBaseAdrs+0x2);
		if(shortTemp0==0x3000 && shortTemp1==0xaaaa && shortTemp2==0x5555)
		{			 
			do 
			{	
				j++;
				*(short *)(ChopLocalBaseAdrs+0x2)=j;
				shortTemp1=*(short *)(ChopLocalBaseAdrs+0x2);			
			}while(j<65535 && shortTemp1==j);				
		}				
	}
	if(j==65535)
	{
		selfCheck.bit.chopBoardOK=1;
		logMsg("chopper PWM board  VME check succeeded\n",0,0,0,0,0,0);
	}
	else
	{
		selfCheck.bit.recBoard1OK=0;
		logMsg("chopper PWM board  VME check failed\n",0,0,0,0,0,0);
	}
	    
	if(selfCheck.bit.chopBoardOK==1 && selfCheck.bit.ast7700SrioInitOK==1) 
	{
		chopSrioID	= *(short *)(ChopLocalBaseAdrs+0X30);//srio_deviceid;
		chopSrio_status	= *(short *)(ChopLocalBaseAdrs+0X3a);//
		chopSrio_error	= *(short *)(ChopLocalBaseAdrs+0X44);//
		chopSrio_igen_state=*(short *)(ChopLocalBaseAdrs+0X48);//
		
		*(short *)(ChopLocalBaseAdrs+0x1e)=1; 			//  registers control right set as SRIO
		*(short *)(ChopLocalBaseAdrs+0x32)=AST_PPC_SRIO_DMA_OUTBOUND_TAR>>16;//0xf000;		
		//	srio_local_target_addr_reg[31:16]<=vme_data;//local srio receive ram start address			
		*(short *)(ChopLocalBaseAdrs+0x34)=AST_PPC_SRIO_DMA_OUTBOUND_TAR&0xffff;//0;			
		//	srio_local_target_addr_reg[15:0]<=vme_data;//local srio receive ram start address			
		*(short *)(ChopLocalBaseAdrs+0x36)=(AST_PPC_SRIO_DMA_INBOUND_BASE+chopSrioID*0x00100000)>>16;
		//0xf000+rec1SrioID*0x0010;		//	srio_dest_start_addr_reg[31:16]<=vme_data;//srio send data detination start address
		*(short *)(ChopLocalBaseAdrs+0x38)=(AST_PPC_SRIO_DMA_INBOUND_BASE+chopSrioID*0x00100000)&0xffff; //0  
		// srio_dest_start_addr_reg[15:0]<=vme_data;////srio send data detination start address											
		*(short *)(ChopLocalBaseAdrs+0x3c)=ast7600SrioID;   //   srio_dest_id[7:0],slave borad upload data to this SRIO ID
		*(short *)(ChopLocalBaseAdrs+0x3e)=srioMutiSendID;	//	组播ID   multicast_deviceid[7:0]		
		*(short *)(ChopLocalBaseAdrs+0x40)= 0x80;			//srio_tx_size[8:0],slave board is to upload bytes number;	
		*(short *)(ChopLocalBaseAdrs+0x42)= 0x80;			//rx_size[8:0],slave board is to receirve bytes number;	
		*(short *)(ChopLocalBaseAdrs+0x46)= 1;				//SRIO_SendEnable
		  
		
		/*general registers setting*/
	  	txChopSrioData.boardID = 0x7600 ;
		txChopSrioData.srio_deviceID = ast7600SrioID;
		txChopSrioData.tx_liveCnt = txChopSrioData.tx_liveCnt++;
		txChopSrioData.tx_frameLen = 0X80;
		txChopSrioData.tx_frameType = 0;//unsopported here
		txChopSrioData.clkDivide = 1;        //50M/2=25M
		txChopSrioData.timerPeriodShadow = T1PRSVM;		
		txChopSrioData.TimerEnable =1;		
		txChopSrioData.clrFaultCmd =1;	
		txChopSrioData.reservered0=0;//unsurpported
 		/*PWM special registers setting*/		
		
		txChopSrioData.CMPR1=0;/*CMPAU_shadow[1] */
		txChopSrioData.CMPR2=0;/*CMPAU_shadow[2] */
		txChopSrioData.CMPR3=0;/*CMPAD_shadow[2] */
		txChopSrioData.CMPR4=0;/*CMPAU_shadow[3] */
	 
		txChopSrioData.minPulseTime=MinPulseCheck;/*min pulse time 10us,50M clock */
		txChopSrioData.PwmEnable=0;/*pwm disable */	
		txChopSrioData.protectEn=0;/*small pulse and deadzone and feedback protect enable*/
		
		status=ast7600PpcSrioSingleSendData(chopSrioID,txChopSrioLen,(UINT8 *)&txChopSrioData);		
		rxChopSrioData =  AST_PPC_SRIO_DMA_INBOUND_TAR + chopSrioID*1*1024*1024;		
 
		logMsg("Chopper PWM board  SRIO initiation succeeded\n",0,0,0,0,0,0);
		logMsg("Chopper PWM board  srio local memory start address is %x \n",rxChopSrioData,0,0,0,0,0);
 
	}
	else
		logMsg("Chopper PWM board  SRIO initiation failed\n",0,0,0,0,0,0);
}
/**********************************************************/
/*Name		:checkChopSrioConnectStatus
*Description:检测chopper板卡的SRIO连接状态，每周期都要检测
*Input		:rxChopSrioData
*Output		:chopSrioConnectOk
*Return		:无
*History	:2016,by wxx
*/
void checkChopSrioConnectStatus(void)
{
	if(selfCheck.bit.chopBoardOK==1) 
	{
		if(	rxChopSrioData->BoardID==0x3000
			&& rxChopSrioData->srio_deviceid==chopSrioID
			&& rxChopSrioData->srio_status & 0x7F == 0X7F
			/*&& rxClkSrioData->h_tc_error==0*/
			&& rxChopSrioData->rx_liveCnt!=rxChopData_srio_rx_live_cnt_old
			&& rxChopSrioData->tx_liveCnt_fb==txChopSrioData.tx_liveCnt)
		{
			selfCheck.bit.chopSrioConnectOk=1;
			 
		}
		else
		{
			selfCheck.bit.chopSrioConnectOk=0;	
			logMsg("chopSrioConnectOk=0\n",0,0,0,0,0,0,0);
		}	
		rxChopData_srio_rx_live_cnt_old=rxChopSrioData->rx_liveCnt;
	}
	
}
/****************************************************************************************/
/*********************************************************************/
/*Name		:enable_chopPWM
*Description:使能chopper PWM卡的PWM输出
*Input		:无
*Output		:无
*Return		:无
*History	:2019,by wxx
*/
void enable_chopPWM(void)
{	txChopSrioData.PwmEnable=1;/*pwm enable */	   
}
/*********************************************************************/
/*Name		:disable_chopPWM
*Description:禁止chopper PWM卡的PWM输出
*Input		:无
*Output		:无
*Return		:无
*History	:2019,by wxx
*/
void disable_chopPWM(void)
{	txChopSrioData.PwmEnable=0;/*pwm disable */	   
}
/*********************************************************************/
/*Name		:chop_control
*Description:斩波控制
*Input		:Udc_input
*Output		:无
*Return		:无
*History	:2019,by wxx
*/
void chop_control(float Udc_input)
{
	static short chopStart=0;
	float chopResult=0;
	if(Udc_input>5400)
		chopStart=1;
	else if(Udc_input<5200)
		chopStart=0;
	if(chopStart==1)
	{	
		chopResult=(Udc_input-5150)*0.001;
		if(chopResult>=0.95)
			chopResult=0.95;
		if(chopResult<=0.05)
			chopResult=0.05;		
		txChopSrioData.CMPR1=chopResult*txChopSrioData.timerPeriodShadow*0.5;
		txChopSrioData.CMPR2=chopResult*txChopSrioData.timerPeriodShadow*0.5;
		txChopSrioData.CMPR3=chopResult*txChopSrioData.timerPeriodShadow*0.5;
		txChopSrioData.CMPR4=chopResult*txChopSrioData.timerPeriodShadow*0.5;
		enable_chopPWM();
		chopStart_Flag=1;
	}
	else
	{
		disable_chopPWM();
		chopStart_Flag=0;
	}
	
}

/*********************************************************************/
/*Name		:disCharge
*Description:放电
*Input		:Udc_input
*Output		:无
*Return		:无
*History	:2019,by wxx
*/
void disCharge(float Udc_input)
{
	if(Udc_input>50)
	{
		txChopSrioData.CMPR1=0.5*txChopSrioData.timerPeriodShadow*0.5;
		txChopSrioData.CMPR2=0.5*txChopSrioData.timerPeriodShadow*0.5;
		txChopSrioData.CMPR3=0.5*txChopSrioData.timerPeriodShadow*0.5;
		txChopSrioData.CMPR4=0.5*txChopSrioData.timerPeriodShadow*0.5;
		enable_chopPWM();
	}
	else
		disable_chopPWM();	
	
}
