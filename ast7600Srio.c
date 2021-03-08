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
*Description:��ʼ��7600-2040DMA
*Input		:��
*Output		:��
*Return		:OK or ERROR
*History	:2018-04-24,modified by wxx
* ����VME��ʼ�����忨��srio����
* �õ�ID
* ���÷��͵�ַ
* ���ý��յ�ַ
*���÷��ʹ�С���ܴ�С 
* ���ù㲥ID
*��γ�ʼ��7700 SRIO�������鲥
*Ȼ���������Ӱ巢��srio
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
		*(UINT32 *)(ast7700LocalBaseAdrs+0x2000)=1;/*VMEд��7700��FPGA��֪ͨ7700:��CPU�Ѿ�SRIO��ʼ�������*/
		*(UINT32 *)(ast7700LocalBaseAdrs+0x2004)=ast7600SrioID;/*VMEд��7700��FPGA��֪ͨ7700:��CPU��SRIO ID��*/
	
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
*Description:��ʼ��DMA�շ�����
*Input		:��
*Output		:��
*Return		:OK or ERROR
*History	:2018-01-10,modified by syjiang
*/
INT32 ast7600PpcSrioDmaInit( void )
{
	INT32 istate = 0;
	INT32 taskId = 0;
	AST_SRIO_INBOUND srioInbound;
	AST_SRIO_OUTBOUND srioOutbound,srioOutbound2;	
	
	/* SRIO��ʼ�� */
	istate=astSrioSetup( );
	if(ERROR == istate)
	{
		printf("Srio setup initialize fail!\n");
		return ERROR;
	}
	else
		printf("Srio setup initialize succeed!\n");
	/* doorbell��ʼ����doorbell����֪ͨ�Է��ѷ������� */
	istate = astSrioDoorbellInit();
	if(ERROR == istate)
	{
		printf("Srio doorbell initialize fail!\n");
		return ERROR;
	}
	else
		printf("Srio doorbell initialize succeed!\n");
	/* message��ʼ�� */
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

	/* outboundӳ�䣬���Ŀ��id����ַ�̶��������ϵͳ��ʼ��ʱ����һ�Σ���ʡ����ʱ��
		 * ��������Ϊoutbound��Ϊ16��subsegment����outbound�ռ�ȷ�Ϊ16��
		 * �粻��Ҫ�ֶΣ���segment��subsegment������Ϊ0����
	 */
	memset(&srioOutbound, 0, sizeof(AST_SRIO_OUTBOUND));
	srioOutbound.outboundNo 	= 2;				/* 1 ~ 8 */
	srioOutbound.targetID 		= 0;/*destId;*/			/* ��ʼĿ��rapidio id */
	srioOutbound.port 			= 0;					/*new CPU is 0,old CPU is 1*/
	srioOutbound.outboundBase 	= AST_PPC_SRIO_DMA_OUTBOUND_BASE;		/* ���ص�ַ */
	srioOutbound.outboundTar 	= AST_PPC_SRIO_DMA_OUTBOUND_TAR;		/* Ŀ��rapidio��ַ */
	srioOutbound.segment		= 1;	/*ÿ��outbound��Ϊ2��segment*/
	srioOutbound.subsegment		= 3;	/*ÿ��segment��Ϊ8��subsegment*/
	srioOutbound.wrtyp			= 4;
	srioOutbound.size 			= AST_PPC_SRIO_DMA_OUTBOUND_SIZE;
	astSrioDmaOutboundMap( srioOutbound );	
/*	*(UINT32*)(0xfe000000 + 0xd0e54) = 0x00440008;*//*��ʱ�ֶ�����SEGMENT ��ʼID����bsp����Ҫ*/
	
	memset(&srioOutbound2, 0, sizeof(AST_SRIO_OUTBOUND));
	srioOutbound2.outboundNo 	= 3;				/* 1 ~ 8 */
	srioOutbound2.targetID 		= srioMutiSendID;/*destId;*/			/* ��ʼĿ��rapidio id */
	srioOutbound2.port 			= 1;
	srioOutbound2.outboundBase 	= AST_PPC_SRIO_DMA_OUTBOUND_BASE+0x01000000;		/* ���ص�ַ */
	srioOutbound2.outboundTar 	= AST_PPC_SRIO_DMA_OUTBOUND_TAR;		/* Ŀ��rapidio��ַ */
	/*srioOutbound2.segment		= 0;	ÿ��outbound��Ϊ4��segment
	srioOutbound2.subsegment	= 0;	ÿ��segment��Ϊ4��subsegment*/
	srioOutbound2.size 			= AST_PPC_SRIO_DMA_OUTBOUND_SIZE;
	srioOutbound2.wrtyp			= 4;
	astSrioDmaOutboundMap( srioOutbound2);			
	
	return OK;
}
/*******************************************************************************************/
/*
* Name		:srioSignleSendData
*Description:��DMA����IO��ʽ��������srio����,�����ݴ�С����IO����DMA
*Input		:destId	Ŀ���豸rapidio id
*			:nbytes	�������ݳ���,������4�ı���
*			��pbuffer�����ͻ���
*Output		:��
*Return		:OK or ERROR
*History	:2018-01-10,modified by syjiang
*/
INT32 ast7600PpcSrioSingleSendData( UINT16 destId, INT32 nbytes, UINT8 * pbuffer )/*�������͸��忨*/
{

	INT32 istate = 0;
	INT32 i = 0;
	if(destId>srioMutiSendID) 
		return ERROR;
	/* �жϳ��� */
	if( nbytes >= AST_PPC_SRIO_DMA_OUTBOUND_SIZE )
	{
	//	printf("Error, the data length [%d] is too big.\n", nbytes);
		return ERROR;
	}	
	if(nbytes<512)
	{	
		/* ͨ��IO�������� */
		for( i = 0; i < ( nbytes / 4 ); i++ )
		{
			*(volatile UINT32*)(AST_PPC_SRIO_DMA_OUTBOUND_BASE + destId * ( AST_PPC_SRIO_DMA_OUTBOUND_SIZE / 16) + i*4) = *(UINT32*)(pbuffer + i*4);
		}
	}
	else
	{	
		/* ͨ��DMA�������� */
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
*Description:���鲥��ʽ����srio����
*Input		:�鲥ID�̶�
*			:nbytes	�������ݳ��ȣ�������4�ı���
*			��pbuffer�����ͻ���
*Output		:��
*Return		:OK or ERROR
*History	:2018-04-18,modified by wxx
*/

INT32 ast7600PpcSrioMutilSendData( INT32 nbytes, UINT8 * pbuffer )/*�鲥���͸��忨����Ҫ��4��PWM��*/
{
	INT32 istate = 0;
	INT32 i = 0;
	 
	/* �жϳ��� */
	if( nbytes >= AST_PPC_SRIO_DMA_OUTBOUND_SIZE )
	{
	//	printf("Error, the data length [%d] is too big.\n", nbytes);
		return ERROR;
	}	
	if(nbytes<512)
	{	
		/* ͨ��IO�������� */
		for( i = 0; i < ( nbytes / 4 ); i++ )
		{
			*(volatile UINT32*)(AST_PPC_SRIO_DMA_OUTBOUND_BASE+0x01000000 + i*4) = *(UINT32*)(pbuffer + i*4);
		}
	}
	else
	{	
		/* ͨ��DMA�������� */
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
*Description:outbound���ڷֶΣ���dma��ʽ����srio����
*Input		:destId	Ŀ���豸rapidio id��0~15
*			:nbytes	�������ݳ���
*Output		:��
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
	
	/* �жϳ��� */
	if( nbytes >= AST_PPC_SRIO_DMA_OUTBOUND_SIZE )
	{
		printf("Error, the data length [%d] is too big.\n", nbytes);
		return ERROR;
	}
	
	/* ��֯���� */
	prand = rand();
	for(i = 0; i< nbytes; i++)
	{
		*(UINT8*)(pbuffer + i) = prand + i;
	}
	
	/* �������� */
	istate = astSrioDmaSend(0, \
					(INT32)pbuffer, \
					AST_PPC_SRIO_DMA_OUTBOUND_BASE+destId*1024*1024, \
					nbytes);
	if(ERROR == istate)
	{
		printf("Srio data send fail.\n");
		return ERROR;
	}
	/* ����doorbell֪ͨ */
		istate = astSrioDbSend(destId, 0x1122);
		if(ERROR == istate)
		{
			printf("Srio doorbell send fail.\n");
			return ERROR;
		}
	/* ��ӡ���͵�ǰ�������� */
	printf("We send data[%08x][%08x][%08x][%08x][%08x][%08x].\n", 
				*(UINT32*)(pbuffer+0), *(UINT32*)(pbuffer+4),*(UINT32*)(pbuffer+8),\
				*(UINT32*)(pbuffer+12),*(UINT32*)(pbuffer+16),*(UINT32*)(pbuffer+20));
	
	return OK;
}

