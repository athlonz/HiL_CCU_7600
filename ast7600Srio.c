/*  
* target:ast7600    
* Copyright 2018,4,16 IEE, Inc. 
* description:  the goal is to use SRIO channel to communicate with other slave board 
* function: 
* debug: 
* modification history
*/
/**************************************************************************************/
 

#include "vxWorks.h"
//#include "astSrio.h"
#include "ast7600Srio.h"
//#include "p2041rdb.h"
#include "h/drv/srio/rio_setup.h"
#include "h/drv/srio/rio_msg.h"
#include "h/drv/srio/rio_dbell.h"
#include "h/drv/srio/rio_dma.h"
#include "h/astSrio.h"

#include "variable_main.h"
/*************************************************************************************/


int ast7600SrioID;
INT32 ast7600PpcSrioDmaInit( void );

/*************************************************************************************/
/*Name		:ast7600SRIO_Init
*Description:初始化7600-2040DMA
*Input		:无
*Output		:无
*Return		:OK or ERROR
*History	:2018-04-24,modified by wxx
* 首先VME初始化各板卡的srio配置
* 得到ID
* 设置发送地址
* 设置接收地址
*设置发送大小接受大小 
* 设置广播ID
*其次初始化7700 SRIO，配置组播
*然后启动个从板发送srio
*/
INT32 ast7600PpcSrioInit(void)
{
	INT32 istate = 0;
	if(selfCheck.bit.ast7700SrioInitOK==1)
	{	
		istate=ast7600PpcSrioDmaInit();
		if(istate==ERROR)
		{
			return ERROR ;
			logMsg("AVME7600 PowerPC2040 SRIO function DMA initiation failed\n",0,0,0,0,0,0);
		}
		else
			logMsg("AVME7600 PowerPC2040 SRIO function DMA initiation succeeded\n",0,0,0,0,0,0);
		ast7600SrioID=astSrioDeviceIDGet();
		*(UINT32 *)(ast7700LocalBaseAdrs+0x2000)=1;/*VME写入7700的FPGA，通知7700:本CPU已经SRIO初始化完毕了*/
		*(UINT32 *)(ast7700LocalBaseAdrs+0x2004)=ast7600SrioID;/*VME写入7700的FPGA，通知7700:本CPU的SRIO ID号*/
	
		if(ast7600SrioID==0x00fe || ast7600SrioID==0x0)
		{		
			return ERROR ;
			logMsg("AVME7600 PowerPC2040 SRIO get ID  failed\n",0,0,0,0,0,0);
		}
		
		else
		{		
			logMsg("AVME7600 PowerPC2040 SRIO get ID  succeed\n",0,0,0,0,0,0);
			return OK ;
			 
		}
	}
	else
	{
		logMsg("AVME7600  SRIO function initiation failed\n",0,0,0,0,0,0);
	}	
	
}
/*******************************************************************************************/
/*Name		:astTestSrioDmaInit
*Description:初始化DMA收发测试
*Input		:无
*Output		:无
*Return		:OK or ERROR
*History	:2018-01-10,modified by syjiang
*/
INT32 ast7600PpcSrioDmaInit( void )
{
	INT32 istate = 0;
	INT32 taskId = 0;
	AST_SRIO_INBOUND srioInbound;
	AST_SRIO_OUTBOUND srioOutbound,srioOutbound2;	
	
	/* SRIO初始化 */
	istate=astSrioSetup( );
	if(ERROR == istate)
	{
		printf("Srio setup initialize fail!\n");
		return ERROR;
	}
	else
		printf("Srio setup initialize succeed!\n");
	/* doorbell初始化，doorbell用于通知对方已发送数据 */
	istate = astSrioDoorbellInit();
	if(ERROR == istate)
	{
		printf("Srio doorbell initialize fail!\n");
		return ERROR;
	}
	else
		printf("Srio doorbell initialize succeed!\n");
	/* message初始化 */
	istate = astSrioDmaInit();
	if(ERROR == istate)
	{
		printf("Srio dma initialize fail!\n");
		return ERROR;
	}
	else
		printf("Srio dma initialize succeed!\n");
	
	/* inbound */
	memset(&srioInbound, 0, sizeof(AST_SRIO_INBOUND));
	srioInbound.inboundNo 	= 4;	/* 1~ 4 */
	srioInbound.port 		= 0;
	srioInbound.inboundBase = AST_PPC_SRIO_DMA_INBOUND_BASE;
	srioInbound.inboundTar 	= AST_PPC_SRIO_DMA_INBOUND_TAR;
	srioInbound.size 		= AST_PPC_SRIO_DMA_INBOUND_SIZE;
	astSrioDmaInboundMap(srioInbound);

	/* outbound映射，如果目的id及地址固定，建议可系统初始化时配置一次，节省操作时间
		 * 以下例子为outbound分为16个subsegment，将outbound空间等分为16份
		 * 如不需要分段，则segment及subsegment参数置为0即可
	 */
	memset(&srioOutbound, 0, sizeof(AST_SRIO_OUTBOUND));
	srioOutbound.outboundNo 	= 2;				/* 1 ~ 8 */
	srioOutbound.targetID 		= 0;/*destId;*/			/* 起始目的rapidio id */
	srioOutbound.port 			= 0;					/*new CPU is 0,old CPU is 1*/
	srioOutbound.outboundBase 	= AST_PPC_SRIO_DMA_OUTBOUND_BASE;		/* 本地地址 */
	srioOutbound.outboundTar 	= AST_PPC_SRIO_DMA_OUTBOUND_TAR;		/* 目的rapidio地址 */
	srioOutbound.segment		= 1;	/*每个outbound分为2个segment*/
	srioOutbound.subsegment		= 3;	/*每个segment分为8个subsegment*/
	srioOutbound.wrtyp			= 4;
	srioOutbound.size 			= AST_PPC_SRIO_DMA_OUTBOUND_SIZE;
	astSrioDmaOutboundMap( srioOutbound );	
/*	*(UINT32*)(0xfe000000 + 0xd0e54) = 0x00440008;*//*临时手动设置SEGMENT 起始ID，新bsp不需要*/
	
	memset(&srioOutbound2, 0, sizeof(AST_SRIO_OUTBOUND));
	srioOutbound2.outboundNo 	= 3;				/* 1 ~ 8 */
	srioOutbound2.targetID 		= srioMutiSendID;/*destId;*/			/* 起始目的rapidio id */
	srioOutbound2.port 			= 1;
	srioOutbound2.outboundBase 	= AST_PPC_SRIO_DMA_OUTBOUND_BASE+0x01000000;		/* 本地地址 */
	srioOutbound2.outboundTar 	= AST_PPC_SRIO_DMA_OUTBOUND_TAR;		/* 目的rapidio地址 */
	/*srioOutbound2.segment		= 0;	每个outbound分为4个segment
	srioOutbound2.subsegment	= 0;	每个segment分为4个subsegment*/
	srioOutbound2.size 			= AST_PPC_SRIO_DMA_OUTBOUND_SIZE;
	srioOutbound2.wrtyp			= 4;
	astSrioDmaOutboundMap( srioOutbound2);			
	
	return OK;
}
/*******************************************************************************************/
/*
* Name		:srioSignleSendData
*Description:以DMA或者IO方式单独发送srio数据,视数据大小采用IO或者DMA
*Input		:destId	目的设备rapidio id
*			:nbytes	发送数据长度,必须是4的倍数
*			：pbuffer，发送缓存
*Output		:无
*Return		:OK or ERROR
*History	:2018-01-10,modified by syjiang
*/
INT32 ast7600PpcSrioSingleSendData( UINT16 destId, INT32 nbytes, UINT8 * pbuffer )/*单独发送各板卡*/
{

	INT32 istate = 0;
	INT32 i = 0;
	if(destId>srioMutiSendID) 
		return ERROR;
	/* 判断长度 */
	if( nbytes >= AST_PPC_SRIO_DMA_OUTBOUND_SIZE )
	{
	//	printf("Error, the data length [%d] is too big.\n", nbytes);
		return ERROR;
	}	
	if(nbytes<512)
	{	
		/* 通过IO发送数据 */
		for( i = 0; i < ( nbytes / 4 ); i++ )
		{
			*(volatile UINT32*)(AST_PPC_SRIO_DMA_OUTBOUND_BASE + destId * ( AST_PPC_SRIO_DMA_OUTBOUND_SIZE / 16) + i*4) = *(UINT32*)(pbuffer + i*4);
		}
	}
	else
	{	
		/* 通过DMA发送数据 */
		istate = astSrioDmaSend(0, (INT32)pbuffer, \
				AST_PPC_SRIO_DMA_OUTBOUND_BASE + destId * ( AST_PPC_SRIO_DMA_OUTBOUND_SIZE / 16 ), \
				nbytes);
		 
		if(ERROR == istate)
		{
		//	printf("Srio data send fail.\n");
			return ERROR;
		}	
	}	
	return OK;
}
/*******************************************************************************************/
/*
* Name		:SRO_mutilSendData
*Description:以组播方式发送srio数据
*Input		:组播ID固定
*			:nbytes	发送数据长度，必须是4的倍数
*			：pbuffer，发送缓存
*Output		:无
*Return		:OK or ERROR
*History	:2018-04-18,modified by wxx
*/

INT32 ast7600PpcSrioMutilSendData( INT32 nbytes, UINT8 * pbuffer )/*组播发送各板卡，主要是4个PWM板*/
{
	INT32 istate = 0;
	INT32 i = 0;
	 
	/* 判断长度 */
	if( nbytes >= AST_PPC_SRIO_DMA_OUTBOUND_SIZE )
	{
	//	printf("Error, the data length [%d] is too big.\n", nbytes);
		return ERROR;
	}	
	if(nbytes<512)
	{	
		/* 通过IO发送数据 */
		for( i = 0; i < ( nbytes / 4 ); i++ )
		{
			*(volatile UINT32*)(AST_PPC_SRIO_DMA_OUTBOUND_BASE+0x01000000 + i*4) = *(UINT32*)(pbuffer + i*4);
		}
	}
	else
	{	
		/* 通过DMA发送数据 */
		istate = astSrioDmaSend(0, (INT32)pbuffer, 	AST_PPC_SRIO_DMA_OUTBOUND_BASE+0x01000000 , nbytes);
		 
		if(ERROR == istate)
		{
		//	printf("Srio data send fail.\n");
			return ERROR;
		}	
	}	
	return OK;
}
/*****************************************************************************
*Name		:astTestSrioDmaSegmentSend
*Description:outbound窗口分段，以dma方式发送srio数据
*Input		:destId	目的设备rapidio id，0~15
*			:nbytes	发送数据长度
*Output		:无
*Return		:OK or ERROR
*History	:2018-01-10,modified by syjiang
*/
INT32 astTestSrioDmaSegmentSend( UINT16 destId, INT32 nbytes )
{
	INT32 istate = 0;
	INT32 i = 0;
	UINT8 pbuffer[4096 + 1] = {0};
	UINT32 prand = 0;
	AST_SRIO_OUTBOUND srioOutbound;
	
	/* 判断长度 */
	if( nbytes >= AST_PPC_SRIO_DMA_OUTBOUND_SIZE )
	{
		printf("Error, the data length [%d] is too big.\n", nbytes);
		return ERROR;
	}
	
	/* 组织数据 */
	prand = rand();
	for(i = 0; i< nbytes; i++)
	{
		*(UINT8*)(pbuffer + i) = prand + i;
	}
	
	/* 发送数据 */
	istate = astSrioDmaSend(0, \
					(INT32)pbuffer, \
					AST_PPC_SRIO_DMA_OUTBOUND_BASE+destId*1024*1024, \
					nbytes);
	if(ERROR == istate)
	{
		printf("Srio data send fail.\n");
		return ERROR;
	}
	/* 发送doorbell通知 */
		istate = astSrioDbSend(destId, 0x1122);
		if(ERROR == istate)
		{
			printf("Srio doorbell send fail.\n");
			return ERROR;
		}
	/* 打印发送的前几个数据 */
	printf("We send data[%08x][%08x][%08x][%08x][%08x][%08x].\n", 
				*(UINT32*)(pbuffer+0), *(UINT32*)(pbuffer+4),*(UINT32*)(pbuffer+8),\
				*(UINT32*)(pbuffer+12),*(UINT32*)(pbuffer+16),*(UINT32*)(pbuffer+20));
	
	return OK;
}

