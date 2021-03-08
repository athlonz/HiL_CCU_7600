/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** ANYBUS_M_POLL.C
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** A simple sample code that does not use interrupt for the AnyBus Evaluation
** board based on the Infineon 80C537. Written for the Keil C compiler, V6.14.
**
** References within square brackets, e.g. (ABS-DGP 1.2.3), refer to the
** corresponding chapter(s) in the AnyBus Slave Design Guide - AnyBus?S
** Parallel Interface, Rev.1.20.
**
** The 
**
**
** Please note:
**
** In order to make this sample code easy to grasp, the entire code is written
** in one single file, this. Otherwise useful programming techniques, such as
** header files, type definitions, structures, macros, enumerations etc, are
** deliberately avoided as not to unnecessarily hide any functionality.
**
********************************************************************************
********************************************************************************
**                                                                            **
** COPYRIGHT NOTIFICATION (c)2001 HMS Industrial Networks AB.                 **
**                                                                            **
** This program is the property of HMS Industrial Networks AB.                **
** It may not be reproduced, distributed, or used without permission          **
** of an authorised company official.                                         **
**                                                                            **
** The following work constitutes example program code and is intended        **
** merely to illustrate a basic application for an AnyBus module.             **
** The user is responsible for applying the code correctly. The code is       **
** provided AS IS without warranty and is in no way guaranteed to be          **
** error-free.                                                                **
**                                                                            **
********************************************************************************
********************************************************************************
**
** Company: HMS Industrial Networks AB
**          Pilefeltsgatan 93-95
**          SE-302 50  Halmstad
**          SWEDEN
**
**          Tel:     +46 (0)35 - 172 900
**          Fax:     +46 (0)35 - 172 909
**          e-mail:  info@hms-networks.com
**
********************************************************************************
********************************************************************************
*/

/*******************************************************************************
********************************************************************************
**
** Change Log
** ----------
**
** Latest modification:
** --------------------
** $Modtime: 01-11-23 9:47 $
** $Author: Jao $
**
** Latest Revision:
**    
**    Rev 2.00e   2004-05-03  MAT   Rebuild for master support
**    Rev 1.03e   2001-11-23  JaO   Mailboxes are now ACK:ed before evaluation.
**    Rev 1.02e   2001-11-12  JaO   References to ABS-DGP added
**                                  Two versions now available; X.XXe (English)
**                                  and X.XXd (German)
**    Rev 1.01    2001-11-07  JaO   Area request/release procedure corrected
**    Rev 1.00    2001-10-30  JaO   First release
**    Rev 0.10    2001-10-25  JaO   Created
**
********************************************************************************
********************************************************************************
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
#include "ioLib.h"
#include "wdLib.h"
#include "h/astVME.h"
#include "h/drv/vme/altioctl.h"
#include "variable_main.h"
#include "anybusPfb.h"
/*******************************************************************************
**
** Environment dependent configuration
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** MONITOR_INTERRUPT_SIGNAL
**
** Use this option if your hardware can monitor the AnyBus interrupt signal
** (ABS-DGP 3.2.1 and 5.1).
**
**------------------------------------------------------------------------------
*/

/*#define MONITOR_INTERRUPT_SIGNAL*/
#undef MONITOR_INTERRUPT_SIGNAL


/*******************************************************************************
**
** Constants
**
********************************************************************************
*/
#define ANBUS_SUBMODULE_1
#define ANBUS_SUBMODULE_2
/*------------------------------------------------------------------------------
**
** The base address of the AnyBus module.
**
**------------------------------------------------------------------------------
*/
/*	Extended Mode (4kbyte DPRAM) memory address start from 0x000
 * 	Standard Mode (2kbyte DPRAM) memory address start from 0x800
 * 	anybus_M address offset is	0x800
 * 	anybus_S address offset is	0x000 or 0x800,because A11 is not used by it.
 * 	*/
#define ABS_DPRAM_BASE_OFFSET1               0x01800/*sub module 1 DRAM address offset*/
#define ABS_DPRAM_BASE_OFFSET2               0x02800/*sub module 2 DRAM address offset*/

/*------------------------------------------------------------------------------
**
** Application and AnyBus Indication register bitmasks (ABS-DGP 4.7).
**
**------------------------------------------------------------------------------
*/

/*
** Common Mailbox bits.
*/

#define INDREG_MIN               0x80
#define INDREG_MOUT              0x40

/*
** Common Area control bits.
*/

#define INDREG_FB_CTRL           0x01
#define INDREG_OUT               0x02
#define INDREG_IN                0x04

/*
** Application Indication register:
** Request/release and Locked bits.
*/

#define APPINDREG_R_R            0x10
#define APPINDREG_L              0x08

/*
** Area request/release control bit mask (bits R/R, L, IN, OUT and FB/CTRL).
*/

#define APPIND_REQ_BITS          0x1F

/*
** Anybus Indication Register:
** Initialisation indication bit.
*/

#define ABINDREG_INIT            0x10


/*------------------------------------------------------------------------------
**
** Mailbox Header Message Information bitmasks (ABS-DGP 6.2.2).
**
**------------------------------------------------------------------------------
*/

#define MBH_INFO_ERR_MASK        0x80
/*****************************************/
#define XBYTE                    *(char *)
/*******************************************************************************
**
** Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** AbIndCopy
**
**------------------------------------------------------------------------------
**
** Holds the latest AnyBus Indication register value (ABS-DGP 4.7).
**
**------------------------------------------------------------------------------
*/

unsigned char AbIndCopy;

unsigned char ABINT  = 0xB3; //AnyBus Int signal
/*------------------------------------------------------------------------------
**
** ProcessData
**
**------------------------------------------------------------------------------
**
** Holds the current process data. In this example only 10 Bytes are used.
** Please note that the used configuration must fit to the hardware module system
** ABM-DEV = max. 512 Byte I/O
** ABM-PDP = max. 512 Byte I/O at 2K DPRAM and max. 1536 Byte I/O at 4K DPRAM
** ABM-ASI = max. 64 Byte I/O
**------------------------------------------------------------------------------
*/

unsigned char ProcessData[10];

/*------------------------------------------------------------------------------
**
** StartInit()
** AnybusInit()
** AnybusInitData()
** EndInit()
**
**------------------------------------------------------------------------------
**
** Arrays holding the START_INIT, ANYBUS_INIT and END_INIT mailbox commands
** (ABS-DGP 6.3.1.1 - 6.3.1.3).
** Mailboxcommands FBInit(), FBInitData() and RunningIdle(), RunningIdleData()
** are optional and necessary for initialisations phase of ABM-ASI and ABM-DEV.
**
**------------------------------------------------------------------------------
*/
const unsigned char  StartInit[]      = { 0x00, 0x01, 0x40, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
const unsigned char  AnybusInit[]     = { 0x00, 0x0A, 0x40, 0x01, 0x00, 0x02, 0x00, 0x12, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
/*in4bytes,out4bytes*/
//const unsigned char  AnybusInitData[] = { 0x00, 0x4, 0x00, 0x4, 0x00, 0x4, 0x00, 0x4, 0x00, 0x4, 0x00, 0x4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
 
//const unsigned char  AnybusInitData[] ={Input I/O Length,Input DPRAM Length,Input Total Length,Output I/O Length,Output DPRAM Length,Output Total Length,..,,,,}
//Message data word 1 and 3 must be set to the same value, since the
//AnyBus-S for Profibus-DP does not support acyclic data. Message
//data word 4 and 6 are set in the same way.
/*in100bytes,out100bytes*/
const unsigned char  AnybusInitData1[] = { 0x00,10, 0x00,10, 0x00,10, 0x00,10, 0x00,10, 0x00,10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char  AnybusInitData2[] = { 0x00,0x80, 0x00,0x80, 0x00,0x80, 0x00,0x80, 0x00,0x80, 0x00,0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char  EndInit[]        = { 0x00, 0x01, 0x40, 0x01, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};

 
/*------------------------------------------------------------------------------
**
** StartInitResponse()
** AnybusInitResponse()
** AnybusInitDataResponse()
** EndInitResponse()
** FBInitResponse()  			(optional)
** FBInitDataResponse()  		(optional)
** RunningIdleResponse()  		(optional)
** RunningIdleDataResponse()  	(optional)
**
**------------------------------------------------------------------------------
**
** Arrays holding the START_INIT, ANYBUS_INIT and END_INIT mailbox responses
** (ABS-DGP 6.3.1.1 - 6.3.1.3).
**
**------------------------------------------------------------------------------
*/

unsigned char  StartInitResponse[ 16 ];
unsigned char  AnybusInitResponse[ 16 ];
unsigned char  AnybusInitDataResponse[ 18 ];
//unsigned char  FBInitResponse( 16 );
//unsigned char  FBInitDataResponse( 6 );
unsigned char  EndInitResponse[ 16 ];
//unsigned char  RunningIdleResponse(16);
//unsigned char  RunningIdleDataResponse(4);
 
int   ANYBUSVMEAdrs=0xe00000;/*base address of ANYBUS_FPGA*/
unsigned	char* ANYBUSlocalBaseAddr;/*ANYBUS_FPGA基地址对应的vme本地地址*/
unsigned	char *ABS_DPRAM_BASE;
   
int   anybusTimerTaskId1;	/*收数据任务Id*/
int   anybusTimerTaskId2;	/*收数据任务Id*/ 
SEM_ID rxIntSem;	/*中断信号量*/  
SEM_ID exDataTaskSem;	/*中断信号量*/  
char data[16];
char pfb_rxdata1[256];
char pfb_txdata1[256];
char pfb_rxdata2[256];
char pfb_txdata2[256];



short temp=0;
short run=1;
short AB1_INIT_OK=0;
short AB1_RUN=0;
short AB1_ONLINE=0;

short AB2_INIT_OK=0;
short AB2_RUN=0;
short AB2_ONLINE=0;

char  n=0;
unsigned	short verdorID=0;
unsigned	short fieldbusType=0 ; 
unsigned	short moduleType=0;
unsigned	short slaveConfigList1=0;
unsigned	short slaveTranferList1=0;
unsigned	short slaveDiagnosticList1=0;
unsigned	short moduleStatusRegister1=0;
unsigned	short masterState1=0;
unsigned	short slaveConfigList2=0;
unsigned	short slaveTranferList2=0;
unsigned	short slaveDiagnosticList2=0;
unsigned	short moduleStatusRegister2=0;
unsigned	short masterState2=0;


cpuset_t ab_affinity ;
cpuset_t ab_pAffinity;

struct PFBrxStruct CCU11_PFB_RX;
struct PFBtxStruct CCU11_PFB_TX;
/*******************************************************************************/

void anybusExDataTask1(void);		/*收数据任务1*/
void anybusExDataTask2(void);		/*收数据任务2*/
void WaitAnybusStart( void);
void WaitAnybusResponse(void);
void WriteAppIndBits(unsigned char CBits, unsigned char SBits, unsigned char Tbits );
void RequestReleaseArea(unsigned char Request, unsigned char Locked, unsigned char Areas );
unsigned char InitialiseAnyBus( unsigned int boardNum );
void ExchangeData1(void);
void ExchangeData2(void);
void RxData1(void);
void TxData1(void);
void RxData2(void);
void TxData2(void);
/*******************************************************************************
**
** Main routine
**
********************************************************************************/
/*------------------------------------------------------------------------------
**
** anybus_pfb1_init()
**
** anybus_pfb board  sub module initiation .
**
** Two application hardware dependent alternatives are presented:
**
**    1. The AnyBus interrupt signal is polled by an I/O port.
**    2. The AnyBus Watchdog counter register is polled.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**    anybus_pfb1_init();
**
**------------------------------------------------------------------------------
*/

int pfb_AB_init( void )
{
	int x;	
	int status;	
	unsigned char charTemp0,charTemp1,charTemp2,charTemp3,charTemp4,charTemp5;
	unsigned int j=0;
	int vme_status=0;
	memset( &pfb_txdata1, 0, 10);
	memset( &pfb_rxdata1, 0, 10);
	memset( &pfb_txdata2, 0, 10);
	memset( &pfb_rxdata2, 0, 10);
	
	 
	rxIntSem= semBCreate(SEM_Q_PRIORITY,SEM_EMPTY);/*创建中断信号量*/
	exDataTaskSem=semBCreate(SEM_Q_PRIORITY,SEM_FULL);/*创建任务轮转信号量*/
	
	/*如果没有自检过该板，那么check anybis_pfb board,*/
	 
	vme_status=astVmeMapAlloc(&PfbLocalBaseAdrs,PfbVmeBaseAdrs,0x100000,VME_AM_STD_USR_DATA,RDWR_MODE_A24,RDWR_SIZE_BYTE,RDWR_SWAP_NO);
	if(vme_status==OK)
	{
		/*初步检测板卡存在、型号、VME读写正确性*/
		charTemp0= *(char *)(PfbLocalBaseAdrs+0x0); /*boardID high 8 bits*/
		charTemp1= *(char *)(PfbLocalBaseAdrs+0x1); /*boardID low 8 bits*/		
		*(char *)(PfbLocalBaseAdrs+0x2)=0xaa;
		charTemp2=*(char *)(PfbLocalBaseAdrs+0x2);
		*(char *)(PfbLocalBaseAdrs+0x2)=0x55;
		charTemp3=*(char *)(PfbLocalBaseAdrs+0x2);
		*(char *)(PfbLocalBaseAdrs+0x3)=0xaa;
		charTemp4=*(char *)(PfbLocalBaseAdrs+0x3);
		*(char *)(PfbLocalBaseAdrs+0x3)=0x55;
		charTemp5=*(char *)(PfbLocalBaseAdrs+0x3);
		
		/*如果初步检测板卡存在，进一步检测VME读写可靠性，255次写、读验证*/
		if(charTemp0==0x34 && charTemp1==0x12 && charTemp2==0xaa && charTemp3==0x55 && charTemp4==0xaa && charTemp5==0x55)
		{			 
			j=0;
			do 
			{	
				j++;
				*(char *)(PfbLocalBaseAdrs+0x2)=j;
				charTemp1=*(char *)(PfbLocalBaseAdrs+0x2);			
			}while(j<255 && charTemp1==j);	
			if(j==255)
				selfCheck.bit.pfbBoardOK=1;	
			else
				selfCheck.bit.pfbBoardOK=0;
		}
		else
			selfCheck.bit.pfbBoardOK=0;
	}
	else
		selfCheck.bit.pfbBoardOK=0;	
	 
	 

   /*
   ** The AnyBus interrupt line is managed entirely by the DPRAM's internal
   ** logic. This logic is affected by a power-on reset but NOT by a hardware
   ** reset. If your application utilises hardware reset, for example via a
   ** manual reset button, there is a risk of the interrupt line being held at
   ** a low level from the start and that the application starts the
   ** initialisation procedures before the AnyBus-S is ready.
   ** The recommended solution to this is to perform an extra "dummy" read of
   ** the AnyBus Indication register, 7FFh, prior to or at least within t < 200
   ** ms after the reset line is released:
   **
   ** AbIndCopy = XBYTE( ABS_DPRAM_BASE + 0x07FF );
   **
   ** This is not done here (with the AnyBus Evaluation board), since we cannot
   ** start the program immediately after power-on or hardware reset during
   ** debug.
   */

   /*
   ** Wait for the AnyBus to be ready after power-up.
   ** The application is not allowed to write any data in the DPRAM until the
   ** AnyBus has initialised it (ABS-DGP 7.2.1.1).
   */
	#ifdef  ANBUS_SUBMODULE_1
	if(selfCheck.bit.pfbBoardOK==1)
	{	   
	   ABS_DPRAM_BASE=PfbLocalBaseAdrs+ABS_DPRAM_BASE_OFFSET1;	 
	   /*检测anybus模块1是否存在*/
	   charTemp0=  XBYTE( ABS_DPRAM_BASE +0x7CA); /*Vendor ID high 8 bits*/
	   charTemp1=  XBYTE( ABS_DPRAM_BASE +0x7CB); /*Vendor ID low 8 bits*/	
	   verdorID=charTemp0<<8|charTemp1;
	   charTemp0=  XBYTE( ABS_DPRAM_BASE +0x7CC); /*Vendor ID high 8 bits*/
	   charTemp1=  XBYTE( ABS_DPRAM_BASE +0x7CD); /*Vendor ID low 8 bits*/	
	   fieldbusType=charTemp0<<8|charTemp1;
	   charTemp0=  XBYTE( ABS_DPRAM_BASE +0x7E0); /*Vendor ID high 8 bits*/
	   charTemp1=  XBYTE( ABS_DPRAM_BASE +0x7E1); /*Vendor ID low 8 bits*/	
	   moduleType=charTemp0<<8|charTemp1;
	   
	   if(verdorID==0x0001  && (fieldbusType==0x0005 ||fieldbusType==0x0001)&& (moduleType==0x0101 || moduleType==0x0201) )	
		   /*verdorID==1:HMS;
		    * fieldbusType==0x0005:PROFIBUS-DPV1,
		    * fieldbusType==0x0001:PROFIBUS-DP;
		    * moduleType==0x0201:anybus_M,
		    * moduleType==0x0101:anybus_S*/   
			selfCheck.bit.pfbAb1OK=1;
	   else
			selfCheck.bit.pfbAb1OK=0;
		/*如果anbus模块1检测存在，就对它初始化*/	
	   if(selfCheck.bit.pfbAb1OK==1)
	   { 
		   *(char *)(PfbLocalBaseAdrs+0x4)=0X0; /*anybus reset */ 
		   vxbMsDelay(1000);/*delay 100ms*/					/*wait 100ms*/
		   *(char *)(PfbLocalBaseAdrs+0x4)=0X1;
		 		 
		   WaitAnybusStart();

		   /*
		   ** Read the AnyBus Indication register.
		   ** The interrupt signal is thereby restored (ABS-DGP 5.1).
		   */

		   AbIndCopy = XBYTE( ABS_DPRAM_BASE + 0x07FF );

		   /*
		   ** Initialise the AnyBus module.
		   */
		   AB1_INIT_OK=InitialiseAnyBus(1);
		   if( AB1_INIT_OK==1 )
			{
			  logMsg(" ANYBUS module 1 initialisation ok\n",0,0,0,0,0,0);
		   /*
		   ** Wait for the AnyBus initialisation indication (INIT bit) (ABS-DGP 4.7).
		   */
			  while( !( AbIndCopy & ABINDREG_INIT ) )
			  {
				WaitAnybusResponse();
			  }
			} 
		   else
			  /*
			  ** The initialisation failed.
			  ** Execute some error handling (in this sample we just hang).
			  */
			{
			  logMsg(" ANYBUS initialisation failed\n",0,0,0,0,0,0);
			}  

		   /*
		   ** Now that the AnyBus is initialised, start the data exchange.
		   ** If the ABM-ASI module is used the mailbox message:
		   ** MB_SET_RUNNING_IDLE must be sent before.
		   */

		// MB_SET_RUNNUNG_IDLE(); 

			AB1_RUN = (XBYTE( ABS_DPRAM_BASE + 0x07e2 )) & 0X04 ;/*bit2:APRS1,(Application Run/Stop)Application is running*/ 		
			AB1_ONLINE = (XBYTE( ABS_DPRAM_BASE + 0x07e3 )) & 0X1;/*bit0:FBRS (Fieldbus On / Off Line)*/
		
		}	//if selfCheck.bit.pfbAb1OK==1
		
	}
	
	#endif

	#ifdef  ANBUS_SUBMODULE_2
	if(selfCheck.bit.pfbBoardOK==1)
	{	   
	   ABS_DPRAM_BASE=PfbLocalBaseAdrs+ABS_DPRAM_BASE_OFFSET2;
	   charTemp0=  XBYTE( ABS_DPRAM_BASE +0x7CA); /*Vendor ID high 8 bits*/
	   charTemp1=  XBYTE( ABS_DPRAM_BASE +0x7CB); /*Vendor ID low 8 bits*/	
	   verdorID=charTemp0<<8|charTemp1;
	   charTemp0=  XBYTE( ABS_DPRAM_BASE +0x7CC); /*Vendor ID high 8 bits*/
	   charTemp1=  XBYTE( ABS_DPRAM_BASE +0x7CD); /*Vendor ID low 8 bits*/	
	   fieldbusType=charTemp0<<8|charTemp1;
	   charTemp0=  XBYTE( ABS_DPRAM_BASE +0x7E0); /*Vendor ID high 8 bits*/
	   charTemp1=  XBYTE( ABS_DPRAM_BASE +0x7E1); /*Vendor ID low 8 bits*/	
	   moduleType=charTemp0<<8|charTemp1;
	   
	   if(verdorID==0x0001  && (fieldbusType==0x0005 || fieldbusType==0x0001) && (moduleType==0x0101 || moduleType==0x0201))	  
		   /*verdorID==1:HMS;
			* fieldbusType==0x0005:PROFIBUS-DPV1,
			* fieldbusType==0x0001:PROFIBUS-DP;
			* moduleType==0x0201:anybus_M,
			* moduleType==0x0101:anybus_S*/
			selfCheck.bit.pfbAb2OK=1;
	   else
			selfCheck.bit.pfbAb2OK=0;
		/*如果anbus模块2检测存在，就对它初始化*/	
	   if(selfCheck.bit.pfbAb2OK==1)
	   { 				   
		   *(char *)(PfbLocalBaseAdrs+0x5)=0X0; /*anybus reset */ 
		   vxbMsDelay(1000);/*delay 500ms*/						/*delay 100ms*/
		   *(char *)(PfbLocalBaseAdrs+0x5)=0X1;
		 		 
		   WaitAnybusStart();

		   /*
		   ** Read the AnyBus Indication register.
		   ** The interrupt signal is thereby restored (ABS-DGP 5.1).
		   */

		   AbIndCopy = XBYTE( ABS_DPRAM_BASE + 0x07FF );

		   /*
		   ** Initialise the AnyBus module.
		   */
		   AB2_INIT_OK=InitialiseAnyBus(2);
		   if( AB2_INIT_OK==1 )
			{
			  logMsg(" ANYBUS module 2 initialisation ok\n",0,0,0,0,0,0);
		   /*
		   ** Wait for the AnyBus initialisation indication (INIT bit) (ABS-DGP 4.7).
		   */
			  while( !( AbIndCopy & ABINDREG_INIT ) )
			  {
				WaitAnybusResponse();
			  }
			} 
		   else
			  /*
			  ** The initialisation failed.
			  ** Execute some error handling (in this sample we just hang).
			  */
			{
			  logMsg(" ANYBUS initialisation failed\n",0,0,0,0,0,0);
			}  

		   /*
		   ** Now that the AnyBus is initialised, start the data exchange.
		   ** If the ABM-ASI module is used the mailbox message:
		   ** MB_SET_RUNNING_IDLE must be sent before.
		   */

		// MB_SET_RUNNUNG_IDLE(); 

			AB2_RUN = (XBYTE( ABS_DPRAM_BASE + 0x07e2 )) & 0X04 ;/*bit2:APRS1,(Application Run/Stop)Application is running*/ 
			
			AB2_ONLINE = (XBYTE( ABS_DPRAM_BASE + 0x07e3 )) & 0X1;/*bit0:FBRS (Fieldbus On / Off Line)*/		 
		} 
	}
	#endif
	
	if(AB1_INIT_OK==1 | AB2_INIT_OK==1)  
	{
		/*发起收数据的任务，优先级130*/
		anybusTimerTaskId1=taskSpawn("anybusExDataTask1",210,VX_SPE_TASK|VX_FP_TASK,200000,(FUNCPTR)anybusExDataTask1,0,0,0,0,0,0,0,0,0,0);
		if(anybusTimerTaskId1==ERROR)
		  logMsg("taskSpawn exDataTask1 failed\n",0,0,0,0,0,0);
		else
		  logMsg("taskSpawn exDataTask1 succeed\n",0,0,0,0,0,0); 	
		CPUSET_ZERO(ab_affinity);
		CPUSET_SET(ab_affinity,2);/*3>=n>=0*/
		if(taskCpuAffinitySet(anybusTimerTaskId1,ab_affinity)==ERROR)
		{
			logMsg("anybus exchange data task anybusExDataTask1 set affinity 2 faild\n",0,0,0,0,0,0);			
		}	
		else
		{
			logMsg("anybus exchange data task anybusExDataTask1 set affinity 2 succeeded\n",0,0,0,0,0,0);
		}
	}	
	else	
		return(ERROR); 
	/*if( AB2_INIT_OK==1)
	{	
		anybusTimerTaskId2=taskSpawn("anybusExDataTask2",220,VX_SPE_TASK|VX_FP_TASK,20000,(FUNCPTR)anybusExDataTask2,0,0,0,0,0,0,0,0,0,0);
		if(anybusTimerTaskId2==ERROR)
		  logMsg("taskSpawn exDataTask2 failed\n",0,0,0,0,0,0);
		else
		  logMsg("taskSpawn exDataTask2 succeed\n",0,0,0,0,0,0); 	 
	}	
	else	
		return(ERROR); 	*/
	return(OK); 
//	astLocalbusFpgaIODirSet( 0xa3, 1 );//设置P2A2方向为输出 
	
}/*end main function*/
/*********************************************************************/
/*------------------------------------------------------------------------------
**
** anybusExDataTask1()
**
** exchange data with anybus in timer interrupt operation task.
**
** Two application hardware dependent alternatives are presented:
**
**    1. The AnyBus interrupt signal is polled by an I/O port.
**    2. The AnyBus Watchdog counter register is polled.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**    anybusExDataTask1();
**
**------------------------------------------------------------------------------
*/
void anybusExDataTask1(void)/*子模块1收发数据任务*/
{
  int i,j;
  
  unsigned int  rxcounter=0;	/*周期计数器*/
  int status; 
 
  FOREVER		/*循环执行*/
  {
	 vxbUsDelay(5000);//delay 5ms
	 taskCpuAffinityGet (anybusTimerTaskId1, &ab_pAffinity) ;
// 	semTake(exDataTaskSem,WAIT_FOREVER);
	if(selfCheck.bit.DABoardOK==1)
	{			
		*(short *)(DALocalBaseAdrs+0x42)=1000;//datemp;		
	}	
	#ifdef  ANBUS_SUBMODULE_1
	if  ( AB1_INIT_OK==1)
	{ 
		
		ABS_DPRAM_BASE=PfbLocalBaseAdrs+ABS_DPRAM_BASE_OFFSET1;
        AB1_RUN = (XBYTE( ABS_DPRAM_BASE + 0x07e2 )) & 0X4 ;/*bit2:APRS1,(Application Run/Stop)Application is running*/ 
        AB1_ONLINE = (XBYTE( ABS_DPRAM_BASE + 0x07e3 )) & 0X1;/*bit0:FBRS (Fieldbus On / Off Line),even if only a slave is connected,the bit is 1*/
        if( AB1_RUN==0x04)
        	selfCheck.bit.pfbAb1Run=1;
        else
        	selfCheck.bit.pfbAb1Run=0;
        if(  AB1_ONLINE==0x1)
        	selfCheck.bit.pfbAb1OnLine=1;
		else
			selfCheck.bit.pfbAb1OnLine=0;
       /* anybusM fieldbus status*/
        slaveConfigList1= XBYTE( ABS_DPRAM_BASE + 0x0640 );/*if slave n is configued,the bit will be set*/
        slaveTranferList1= XBYTE( ABS_DPRAM_BASE + 0x0650 );/*if slave n is exchange data with master,the bit will be set*/
        slaveDiagnosticList1= XBYTE( ABS_DPRAM_BASE + 0x0660 ); /*if slave n is Diagnosticed,the bit will be set*/
        masterState1= XBYTE( ABS_DPRAM_BASE + 0x0670 ); /*if slave n is Diagnosticed,the bit will be set*/
        /*Master Status
			00h - Offline
			40h - Stop
			80h - Clear
			C0h - Operate*/
        moduleStatusRegister1=((XBYTE( ABS_DPRAM_BASE + 0x07e2 ))<<8) | (XBYTE( ABS_DPRAM_BASE + 0x07e3 ));
		/*用户需要通过第一块anybs模块的发送数据在这里赋值*/
//        for(i=0;i<120;i++)
//        	pfb_txdata1[i]=i+n;
        //ISG
		if(preChargecmd==1)
			CCU11_PFB_TX.ISG.ctrlWord.bit.PreChargeSwitchOn=1;	
		else
			CCU11_PFB_TX.ISG.ctrlWord.bit.PreChargeSwitchOn=0;	
		
		if(switchINcmd==1)
			CCU11_PFB_TX.ISG.ctrlWord.bit.SwitchOn=1;	
		else
			CCU11_PFB_TX.ISG.ctrlWord.bit.SwitchOn=0;
		//OSG
		if(switch_DT_BTcmd==1)
			CCU11_PFB_TX.OSG.ctrlWord.bit.DTTBSwitchOn=1;
		else
			CCU11_PFB_TX.OSG.ctrlWord.bit.DTTBSwitchOn=0;
		
		if(switchOUTcmd==1)
			CCU11_PFB_TX.OSG.ctrlWord.bit.SwitchOn=1;
		else
			CCU11_PFB_TX.OSG.ctrlWord.bit.SwitchOn=0;	
		for(i=0;i<2;i++)
		{
			*((short *)(&pfb_txdata1)+i)=SWAP( *(  (short *)(&CCU11_PFB_TX)+i)   );//高低字节序颠倒
		}
		if( (AB1_RUN==0X04) && (AB1_ONLINE==0X1))/*if module is running and online,then exchange data with other node*/
		{
			n++;
//			ExchangeData1();
			RxData1();
			vxbUsDelay(1000);/*delay 1ms*/
			TxData1();
		}	
		for(i=0;i<2;i++)
		{
			*((short *)(&CCU11_PFB_RX)+i)=SWAP( *(  (short *)(&pfb_rxdata1)+i)   );//高低字节序颠倒
		}		
		/*从第一块anybus模块接收的数据在这里付给目标变量*/
	/*	a=pfb_rxdata1[0];
		b=pfb_rxdata1[0];*/
		
   } /* end of while( 1 ) */
   #endif   
	vxbUsDelay(1000);/*delay 1ms*/
	#ifdef  ANBUS_SUBMODULE_2
	if  ( AB2_INIT_OK==1)
	{ 
	  ABS_DPRAM_BASE=PfbLocalBaseAdrs+ABS_DPRAM_BASE_OFFSET2;
      AB2_RUN = (XBYTE( ABS_DPRAM_BASE + 0x07e2 )) & 0X4 ;/*bit2:APRS1,(Application Run/Stop)Application is running*/ 
      AB2_ONLINE = (XBYTE( ABS_DPRAM_BASE + 0x07e3 )) & 0X1;/*bit0:FBRS (Fieldbus On / Off Line)*/
      slaveConfigList2= XBYTE( ABS_DPRAM_BASE + 0x0640 );/*if slave n is configued,the bit will be set*/
	  slaveTranferList2= XBYTE( ABS_DPRAM_BASE + 0x0650 );/*if slave n is exchange data with master,the bit will be set*/
	  slaveDiagnosticList2= XBYTE( ABS_DPRAM_BASE + 0x0660 ); /*if slave n is Diagnosticed,the bit will be set*/
	  masterState2= XBYTE( ABS_DPRAM_BASE + 0x0670 ); /*if slave n is Diagnosticed,the bit will be set*/
	  moduleStatusRegister2=((XBYTE( ABS_DPRAM_BASE + 0x07e2 ))<<8) | (XBYTE( ABS_DPRAM_BASE + 0x07e3 ));

      if( AB2_RUN==0X04)
      	selfCheck.bit.pfbAb2Run=1;
      else
      	selfCheck.bit.pfbAb2Run=0;
      if(  AB2_ONLINE==0X1)
      	selfCheck.bit.pfbAb2OnLine=1;
		else
			selfCheck.bit.pfbAb2OnLine=0;
      
		/*用户需要通过第2块anybs模块的发送数据在这里赋值*/
      for(i=0;i<120;i++)
      	pfb_txdata2[i]=i+n;	
		 
		if( AB2_RUN==0X04 && AB2_ONLINE==0X1)/*if module is running and online,then exchange data with other node*/
		{
			n++;
//			ExchangeData2();
			RxData2();
			vxbUsDelay(1000);/*delay 1ms*/
			TxData2();
		}		
		/*从第2块anybus模块接收的数据在这里付给目标变量*/
		/*	a=pfb_rxdata2[0];
			b=pfb_rxdata2[0];*/
		
	} /* end of while( 1 ) */
 	 #endif
	if(selfCheck.bit.DABoardOK==1)
	{	
	
		*(short *)(DALocalBaseAdrs+0x42)=0;//datemp;		
	}	
	status= semGive(exDataTaskSem);
  }/*end FOREVER*/
}
/****************************************************************************/
/*------------------------------------------------------------------------------
**
** anybusExDataTask2()
**
** exchange data with anybus in timer interrupt operation task.
**
** Two application hardware dependent alternatives are presented:
**
**    1. The AnyBus interrupt signal is polled by an I/O port.
**    2. The AnyBus Watchdog counter register is polled.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**    anybusExDataTask2();
**
**------------------------------------------------------------------------------
*/
void anybusExDataTask2(void)/*子模块1收发数据任务*/
{
  int i,j;
  
  unsigned int  rxcounter=0;	/*周期计数器*/
  int status; 
 
  FOREVER		/*循环执行*/
  {
	vxbUsDelay(5000);/*delay 1ms*/
//	semTake(exDataTaskSem,WAIT_FOREVER);
	if(selfCheck.bit.DABoardOK==1)
	{			
		*(short *)(DALocalBaseAdrs+0x46)=1000;//datemp;		
	}		
   
   #ifdef  ANBUS_SUBMODULE_2
   if  ( AB2_INIT_OK==1)
	{ 
		ABS_DPRAM_BASE=PfbLocalBaseAdrs+ABS_DPRAM_BASE_OFFSET2;
        AB2_RUN = (XBYTE( ABS_DPRAM_BASE + 0x07e2 )) & 0X4 ;/*bit2:APRS1,(Application Run/Stop)Application is running*/ 
        AB2_ONLINE = (XBYTE( ABS_DPRAM_BASE + 0x07e3 )) & 0X1;/*bit0:FBRS (Fieldbus On / Off Line)*/
        if( AB2_RUN==0X04)
        	selfCheck.bit.pfbAb2Run=1;
        else
        	selfCheck.bit.pfbAb2Run=0;
        if(  AB2_ONLINE==0X1)
        	selfCheck.bit.pfbAb2OnLine=1;
		else
			selfCheck.bit.pfbAb2OnLine=0;
        
		/*用户需要通过第2块anybs模块的发送数据在这里赋值*/
        for(i=0;i<100;i++)
        	pfb_txdata2[i]=i;	
		 
		if( AB2_RUN==0X04 && AB2_ONLINE==0X1)/*if module is running and online,then exchange data with other node*/
		{
			n++;
			ExchangeData2();
		}		
		/*从第2块anybus模块接收的数据在这里付给目标变量*/
		/*	a=pfb_rxdata2[0];
			b=pfb_rxdata2[0];*/
		
   } /* end of while( 1 ) */
   #endif

	
	if(selfCheck.bit.DABoardOK==1)
	{	
	
		*(short *)(DALocalBaseAdrs+0x46)=0;//datemp;		
	}	
	status= semGive(exDataTaskSem);
  }/*end FOREVER*/
}
/*******************************************************************************
**
** Functions
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** WaitAnybusStart()
**
** Waits for the AnyBus to be ready after power-up.
**
** Two application hardware dependent alternatives are presented:
**
**    1. The AnyBus interrupt signal is polled by an I/O port.
**    2. The AnyBus Watchdog counter register is polled.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**    WaitAnybusStart();
**
**------------------------------------------------------------------------------
*/

void WaitAnybusStart( void )
{
#ifdef MONITOR_INTERRUPT_SIGNAL

   /*
   ** Alternative 1:
   **
   ** Wait for the AnyBus interrupt signal to indicate that the AnyBus has
   ** written to the AnyBus indication register (ABS-DGP 3.2.1 and 5.1).
   */

   while( ABINT )
   {
   }

#else

   unsigned char CheckNr;
   unsigned char WatchDog;
   unsigned char WatchDogTemp;

   /*
   ** Alternative 2:
   **
   ** If your hardware cannot monitor the interrupt signal, then wait until the
   ** Watchdog counter out register (ABS-DGP 4.6.1) has changed at least 5
   ** times.
   **
   ** Note: The Watchdog counter out register will be read without access to the
   **       control registers. This is an exception from what's stipulated in
   **       the AnyBus-S Design Guide. It will not have any side effects at this
   **       point except for the fact that a simultaneous access to the same
   **       DPARM location can give random values - that's why we do 5 checks.
   */

   WatchDog     = 0;
   WatchDogTemp = 0;

   for( CheckNr = 0; CheckNr < 5; CheckNr++ )
   {
      while( WatchDog == WatchDogTemp )
      {
         WatchDogTemp = XBYTE( ABS_DPRAM_BASE + 0x07D5 );
        /* vxbUsDelay(1);*//*the interval time between write and read is not too fast*/  
      }

      WatchDog = WatchDogTemp;
   }

#endif /* MONITOR_INTERRUPT_SIGNAL */

} /* end of WaitAnybusStart() */


/*------------------------------------------------------------------------------
**
** WaitAnybusResponse()
**
** Awaits an AnyBus response to any previously posted request and updates the
** AnyBus Indication register copy.
**
** Two application hardware dependent alternatives are presented:
**
**    1. The AnyBus interrupt signal is polled by an I/O port.
**    2. The AnyBus Indication register is polled.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**    WaitAnybusResponse();
**
**------------------------------------------------------------------------------
*/

void WaitAnybusResponse( void )
{
   unsigned char DoubleReadTemp;

#ifdef MONITOR_INTERRUPT_SIGNAL

   /*
   ** Alternative 1:
   **
   ** Wait for the AnyBus interrupt signal to indicate that something has
   ** been written to the AnyBus indication register (ABS-DGP 3.2.1 and 5.1).
   */

   while( ABINT )
   {
   }

#else

   /*
   ** Alternative 2:
   **
   ** If your hardware cannot monitor the interrupt signal, then a change in the
   ** register itself must be awaited.
   **
   ** Note: Even though the Toggle bit is added to make it possible for a polled
   **       application to detect a NAK to an area request (without the Toggle
   **       bit the register wouldn't change at all on a NAK), you should always
   **       detect AnyBus Indication register updates by comparing the whole
   **       register rather than only observing the Toggle bit. Otherwise there
   **       is a risk of missing an update if the register is updated multiple
   **       times in succession.
   */
/*
   while( AbIndCopy == XBYTE( ABS_DPRAM_BASE + 0x07FF ) )
   {
     //vxbUsDelay(1);
   }*/

#endif /* MONITOR_INTERRUPT_SIGNAL */

   /*
   ** We now know that something has been written to the register.
   ** In case of DPRAM collision: repeat until two consecutive readings agree.
   ** For further information regarding the Cypress DPRAM CY7C136, please see
   ** the Cypress webpage www.cypress.com.
   */

   DoubleReadTemp = XBYTE( ABS_DPRAM_BASE + 0x07FF );
   AbIndCopy      = XBYTE( ABS_DPRAM_BASE + 0x07FF );

   while( AbIndCopy != DoubleReadTemp )
   {
      DoubleReadTemp = AbIndCopy;
      AbIndCopy      = XBYTE( ABS_DPRAM_BASE + 0x07FF );
      vxbUsDelay(1);/*the interval time between write and read is not too fast*/
   }

} /* end of WaitAnybusResponse() */


/*------------------------------------------------------------------------------
**
** WriteAppIndBits()
**
** Updates the Application Indication register according to the values given.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    CBits                   - The bits that are to be cleared.
**    SBits                   - The bits that are to be set.
**    Tbits                   - The bits that are to be toggled.
**
** Outputs:
**    None
**
** Usage:
**     WriteAppIndBits( CBits, SBits, Tbits );
**
**------------------------------------------------------------------------------
*/

void WriteAppIndBits( unsigned char CBits, unsigned char SBits, unsigned char Tbits )
{
   unsigned char AppIndCopy;

   /*
   ** Get a copy of the Application Indication register.
   ** Update the copy according to the values given.
   **
   ** Note: The bits are manipulated in a copy of the register to avoid
   **       multiple writing to the Application Indication register as we only
   **       want to write ONE value to the Application Indication register
   **       (ABS-DGP 5.1).
   */

   AppIndCopy  = XBYTE( ABS_DPRAM_BASE + 0x07FE );
   AppIndCopy &= ~CBits;
   AppIndCopy |=  SBits;
   AppIndCopy ^=  Tbits;

   /*
   ** Write the contents of the copy to the Application Indication register.
   ** Repeat until success in case the write fails due to a DPRAM collision.
   ** For further information regarding the Cypress DPRAM CY7C136, please see
   ** the Cypress webpage www.cypress.com.
   */

   do
   {
      XBYTE( ABS_DPRAM_BASE + 0x07FE ) = AppIndCopy;
      vxbUsDelay(1);/*the interval time between write and read is not too fast*/
   }
   while( XBYTE( ABS_DPRAM_BASE + 0x07FE ) != AppIndCopy );

} /* end of WriteAppIndBits() */


/*------------------------------------------------------------------------------
**
** RequestReleaseArea()
**
** Posts area request or release to selected areas of the AnyBus-S module
** (ABS-DGP 4.7, 5.2 and 5.3).
**
**------------------------------------------------------------------------------
**
** Inputs:
**    Request                 - 1: Request.
**                            - 0: Release.
**
**    Locked                  - 1: Locked.
**                            - 0: Unlocked.
**
**    Areas                   - The area bits for the areas to request/release.
**
** Outputs:
**    None
**
** Usage:
**    RequestReleaseArea( 1, 0, INDREG_OUT );
**
**------------------------------------------------------------------------------
*/

void RequestReleaseArea( unsigned char Request, unsigned char Locked, unsigned char Areas )
{
   unsigned char AppIndCopy;

   /*
   ** Get a copy of the Application Indication register.
   ** Update copy according to the values given.
   **
   ** Note: The bits are manipulated in a copy of the register to avoid
   **       multiple writing to the Application Indication register as we only
   **       want to write ONE value to the Application Indication register.
   */

   AppIndCopy  = XBYTE( ABS_DPRAM_BASE + 0x07FE );

   /*
   ** Clear all area request bits (R/R, L, IN, OUT and FB/CTRL).
   */

   AppIndCopy &= ~APPIND_REQ_BITS;

   if( Request )
   {
      /*
      ** Set the request (R/R) bit.
      */

      AppIndCopy |= APPINDREG_R_R;
   }

   if( Locked )
   {
      /*
      ** Set the locked (L) bit.
      */

      AppIndCopy |= APPINDREG_L;
   }

   /*
   ** Set the area bits for the areas to request or release.
   */

   AppIndCopy |= Areas;

   /*
   ** Write the contents of the copy to the Application Indication register.
   ** Repeat until success in case the write fails due to a DPRAM collision.
   ** For further information regarding the Cypress DPRAM CY7C136, please see
   ** the Cypress webpage www.cypress.com.
   */

   do
   {
      XBYTE( ABS_DPRAM_BASE + 0x07FE ) = AppIndCopy;
      vxbUsDelay(1);/*the interval time between write and read is not too fast*/
   }
   while( XBYTE( ABS_DPRAM_BASE + 0x07FE ) != AppIndCopy );

} /* end of RequestReleaseArea() */


/*------------------------------------------------------------------------------
**
** InitialiseAnyBus()
**
** Sends the initialisation mailboxes needed to initialise the AnyBus module
** (ABS-DGP 6.3.1.1 - 6.3.1.3).
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    InitSuccess             - 1: The initialisation is completed.
**                            - 0: The initialisation failed.
**
** Usage:
**     InitialiseAnyBus();
**
**------------------------------------------------------------------------------
*/

unsigned char InitialiseAnyBus( unsigned int boardNum )
{
   unsigned char Byte;

   /*
   ** Enter the START_INIT mailbox command in the mailbox IN area, but first
   ** check that the mailbox IN area is free (the MIN bits are equal)
   ** (ABS-DGP 5.3.5).
   **
   ** Naturally, it should be free at this point, but you should always do a
   ** routine check before entering data.
   */

   if( ( XBYTE( ABS_DPRAM_BASE + 0x07FF ) ^ XBYTE( ABS_DPRAM_BASE + 0x07FE ) ) & INDREG_MIN )
   {
      return( 0 );
   }

   for( Byte = 0; Byte < 16; Byte++ )
   {
   	XBYTE( ABS_DPRAM_BASE + 0x0400 + Byte ) = StartInit[ Byte ];
   }

   /*
   ** Post the mailbox. Toggle the MIN bit (ABS-DGP 4.7 and 5.3.5).
   */

   WriteAppIndBits( 0, 0, INDREG_MIN );

   /*
   ** Wait for and read the mailbox response (ABS-DGP 5.3.5).
   */

   do
   {
      WaitAnybusResponse();
     /* vxbUsDelay(1);*//*the interval time between write and read is not too fast*/
   }
   while( !( ( AbIndCopy ^ XBYTE( ABS_DPRAM_BASE + 0x07FE ) ) & INDREG_MOUT ) );

   for( Byte = 0; Byte < 16; Byte++ )
   {
   	StartInitResponse[ Byte] = XBYTE( ABS_DPRAM_BASE + 0x0520 + Byte );
   }

   /*
   ** Acknowledge the mailbox. Toggle the MOUT bit.
   */

   WriteAppIndBits( 0, 0, INDREG_MOUT );

   /*
   ** Check the response (at least check the error bit).
   */

   if( StartInitResponse[ 2] & MBH_INFO_ERR_MASK )
   {
      /*
      ** We've received an error response (ABS-DGP 6.2.2).
      ** Report failure.
      */

      return( 0 );
   }




   /*
   ** Enter the ANYBUS_INIT mailbox command in the mailbox IN area,
   ** but first wait if the mailbox IN area is not yet free.
   */

   while( ( ( AbIndCopy ^ XBYTE( ABS_DPRAM_BASE + 0x07FE ) ) & INDREG_MIN ) )
   {
      WaitAnybusResponse();
     /* vxbUsDelay(1);*//*the interval time between write and read is not too fast*/
   }

   for( Byte = 0; Byte < 16; Byte++ )
   {
   	XBYTE( ABS_DPRAM_BASE + 0x0400 + Byte ) = AnybusInit[Byte ];
   }

   /*
   ** Enter the ANYBUS_INIT message data.
   */

   for( Byte = 0; Byte < 18; Byte++ )
   {
		if(boardNum==1)
		   XBYTE( ABS_DPRAM_BASE + 0x0420 + Byte ) = AnybusInitData1[Byte ];
		if(boardNum==2)
			XBYTE( ABS_DPRAM_BASE + 0x0420 + Byte ) = AnybusInitData2[Byte ];
   }

   /*
   ** Post the mailbox. Toggle the MIN bit.
   */

   WriteAppIndBits( 0, 0, INDREG_MIN );

   /*
   ** Wait for and read the mailbox response.
   */

   do
   {
      WaitAnybusResponse();
     /* vxbUsDelay(1);*//*the interval time between write and read is not too fast*/
   }
   while( !( ( AbIndCopy ^ XBYTE( ABS_DPRAM_BASE + 0x07FE ) ) & INDREG_MOUT ) );

   for( Byte = 0; Byte < 16; Byte++ )
   {
   	AnybusInitResponse[Byte ] = XBYTE( ABS_DPRAM_BASE + 0x0520 + Byte );
   }

   for( Byte = 0; Byte < 18; Byte++ )
   {
   	AnybusInitDataResponse[Byte ] = XBYTE( ABS_DPRAM_BASE + 0x0540 + Byte );
   }

   /*
   ** Acknowledge the mailbox. Toggle the MOUT bit.
   */

   WriteAppIndBits( 0, 0, INDREG_MOUT );

   /*
   ** Check the response (at least check the error bit).
   */

   if( AnybusInitResponse[ 2 ] & MBH_INFO_ERR_MASK )
   {
      /*
      ** We've received an error response.
      ** Report failure.
      */

      return( 0 );
   }

   /*
   ** Enter the END_INIT mailbox command in the mailbox IN area,
   ** but first wait if the mailbox IN area is not yet free.
   */

   while( ( ( AbIndCopy ^ XBYTE( ABS_DPRAM_BASE + 0x07FE ) ) & INDREG_MIN ) )
   {
      WaitAnybusResponse();
     /* vxbUsDelay(1);*//*the interval time between write and read is not too fast*/
   }

   for( Byte = 0; Byte < 16; Byte++ )
   {
   	XBYTE( ABS_DPRAM_BASE + 0x0400 + Byte ) = EndInit[Byte ];
   }

   /*
   ** Post the mailbox. Toggle the MIN bit.
   */

   WriteAppIndBits( 0, 0, INDREG_MIN );

   /*
   ** Wait for and read the mailbox response.
   */

   do
   {
      WaitAnybusResponse();
     /* vxbUsDelay(1);*//*the interval time between write and read is not too fast*/
   }
   while( !( ( AbIndCopy ^ XBYTE( ABS_DPRAM_BASE + 0x07FE ) ) & INDREG_MOUT ) );

   for( Byte = 0; Byte < 16; Byte++ )
   {
   	EndInitResponse[Byte ] = XBYTE( ABS_DPRAM_BASE + 0x0520 + Byte );
   }

   /*
   ** Acknowledge the mailbox. Toggle the MOUT bit.
   */

   WriteAppIndBits( 0, 0, INDREG_MOUT );

   /*
   ** Check the response (at least check the error bit).
   */

   if( EndInitResponse[ 2 ] & MBH_INFO_ERR_MASK )
   {
      /*
      ** We've received an error response.
      ** Report failure.
      */

      return( 0 );
   }

   /*
   ** The initialisation mailboxes have all been processed successfully.
   */

   return( 1 );

} /* end of InitialiseAnyBus() */




/*------------------------------------------------------------------------------
**
** ExchangeData()
**
** Data exchange routine. Reads the first byte in the OUT area and loops the
** data (bit-wise inverted) to the IN area (ABS-DGP 5.2.1).
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**     ExchangeData();
**
**------------------------------------------------------------------------------
*/

void ExchangeData1( void )
{
   int x=0;
   
   /*
   ** Do a locked request to the OUT area and wait for access.
   ** The result of a locked request is, that in case we first get a NAK, access
   ** to the area will be granted automatically when the AnyBus releases it
   ** (ABS-DGP 5.3.2.1).
   */
   x++;
   RequestReleaseArea( 1, 1, INDREG_OUT );

   do
   {
      WaitAnybusResponse();
   }
   while( !( AbIndCopy & INDREG_OUT ) );

   /*
   ** The OUT area is now accessed.
   ** Read the first 10 bytes (ABS-DGP 5.2.1).
   */
	for(x=0;x<100;x++)
	{	
		pfb_rxdata1[x] = XBYTE( ABS_DPRAM_BASE + 0x0200 + x);
		vxbUsDelay(1);
	}	
   /*
   ** Release the OUT area and wait for the response (OUT area released).
   */

   RequestReleaseArea( 0, 0, INDREG_OUT );

   do
   {
      WaitAnybusResponse();
   }
   while( AbIndCopy & INDREG_OUT );

   /*
   ** Request the IN area (locked) and wait for access.
   */

   RequestReleaseArea( 1, 1, INDREG_IN );

   do
   {
      WaitAnybusResponse();
   }
   while( !( AbIndCopy & INDREG_IN ) );

   /*
   ** The IN area is now accessed.
   ** Return the first 10 bytes.
   */

	for(x=0;x<100;x++)
	{	
	   XBYTE( ABS_DPRAM_BASE + x ) = pfb_txdata1[x];//+n;
	   vxbUsDelay(1);
	}   

   /*
   ** Release the IN area and wait for the response (IN area released).
   */

   RequestReleaseArea( 0, 0, INDREG_IN );

   do
   {
      WaitAnybusResponse();
   }
   while( AbIndCopy & INDREG_IN );

} /* end of ExchangeData() */


void RxData1( void )
{
   int x=0;
   
   /*
   ** Do a locked request to the OUT area and wait for access.
   ** The result of a locked request is, that in case we first get a NAK, access
   ** to the area will be granted automatically when the AnyBus releases it
   ** (ABS-DGP 5.3.2.1).
   */
   x++;    
   				
    	
   RequestReleaseArea( 1, 1, INDREG_OUT );/*about 6us*/
   	   

   do
   {
      WaitAnybusResponse();
   }
   while( !( AbIndCopy & INDREG_OUT ) );/*about 125us*/
   	   	
   /*
   ** The OUT area is now accessed.
   ** Read the first 10 bytes (ABS-DGP 5.2.1).
   */
	for(x=0;x<20;x++)					/*about 200us*/
	{	
//		 *((char *)(&pfb_rxdata_1)+x) = XBYTE( ABS_DPRAM_BASE + 0x0200 + x);
		pfb_rxdata1[x] = XBYTE( ABS_DPRAM_BASE + 0x0200 + x);
//		*((char *)(&CCU11_PFB_RX)+x) = XBYTE( ABS_DPRAM_BASE + 0x0200 + x);
		  vxbUsDelay(1);
	}	
   /*
   ** Release the OUT area and wait for the response (OUT area released).
   */
		
   RequestReleaseArea( 0, 0, INDREG_OUT );/*about 6us*/
  
   do
   {
      WaitAnybusResponse();
   }
   while( AbIndCopy & INDREG_OUT );/*about 200us*/
   	
}

void TxData1( void )
{
	int x=0;
   /*
   ** Request the IN area (locked) and wait for access.
   */
	
   RequestReleaseArea( 1, 1, INDREG_IN );
  
   do
   {
      WaitAnybusResponse();
   }
   while( !( AbIndCopy & INDREG_IN ) );
   
   /*
   ** The IN area is now accessed.
   ** Return the first 10 bytes.
   */
//   pfb_txdata1[0]=0x11;
//   pfb_txdata1[1]=0x22;
//   pfb_txdata1[2]=0x33;
//   pfb_txdata1[4]=0x44;
//   pfb_txdata1[5]=0x55;
//   pfb_txdata1[6]=0x66;
//   pfb_txdata1[7]=0x77;
//   pfb_txdata1[8]=0x88;
//   pfb_txdata1[9]=0x99;
//   pfb_txdata1[10]=0xaa;
//   pfb_txdata1[11]=0xbb;
//   pfb_txdata1[12]=0xcc;
//   pfb_txdata1[13]=0xdd;
//   pfb_txdata1[14]=0xee;
//   pfb_txdata1[15]=0xff;
//   pfb_txdata1[16]=0x11;
//   pfb_txdata1[17]=0x22;
//   pfb_txdata1[18]=0x33;
//   pfb_txdata1[19]=0x44;
//   pfb_txdata1[20]=0x55;
   
	for(x=0;x<20;x++)
	{
//	   XBYTE( ABS_DPRAM_BASE + x ) = *((char *)(&pfb_txdata_1)+x);//+n;
	   XBYTE( ABS_DPRAM_BASE + x ) = pfb_txdata1[x];//+n;
 
//		 XBYTE( ABS_DPRAM_BASE + x ) = *( (char *)(&CCU11_PFB_TX)+x);//+n;
	   vxbUsDelay(1);
	}   
		
   /*
   ** Release the IN area and wait for the response (IN area released).
   */
		
   RequestReleaseArea( 0, 0, INDREG_IN );
  
   do
   {
      WaitAnybusResponse();
   }
   while( AbIndCopy & INDREG_IN );
  
} /* end of ExchangeData() */
/*------------------------------------------------------------------------------
**
** ExchangeData()
**
** Data exchange routine. Reads the first byte in the OUT area and loops the
** data (bit-wise inverted) to the IN area (ABS-DGP 5.2.1).
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**     ExchangeData();
**
**------------------------------------------------------------------------------
*/

void ExchangeData2( void )
{
   int x=0;
   
   /*
   ** Do a locked request to the OUT area and wait for access.
   ** The result of a locked request is, that in case we first get a NAK, access
   ** to the area will be granted automatically when the AnyBus releases it
   ** (ABS-DGP 5.3.2.1).
   */
   x++;
   RequestReleaseArea( 1, 1, INDREG_OUT );

   do
   {
      WaitAnybusResponse();
   }
   while( !( AbIndCopy & INDREG_OUT ) );

   /*
   ** The OUT area is now accessed.
   ** Read the first 10 bytes (ABS-DGP 5.2.1).
   */
	for(x=0;x<100;x++)
	{	
		pfb_rxdata2[x] = XBYTE( ABS_DPRAM_BASE + 0x0200 + x);
		vxbUsDelay(1);
	}
   /*
   ** Release the OUT area and wait for the response (OUT area released).
   */

   RequestReleaseArea( 0, 0, INDREG_OUT );

   do
   {
      WaitAnybusResponse();
   }
   while( AbIndCopy & INDREG_OUT );

   /*
   ** Request the IN area (locked) and wait for access.
   */

   RequestReleaseArea( 1, 1, INDREG_IN );

   do
   {
      WaitAnybusResponse();
   }
   while( !( AbIndCopy & INDREG_IN ) );

   /*
   ** The IN area is now accessed.
   ** Return the first 10 bytes.
   */

	for(x=0;x<100;x++)
	{
	   XBYTE( ABS_DPRAM_BASE + x ) = pfb_txdata2[x];//+n;
	   vxbUsDelay(1);
	}
   /*
   ** Release the IN area and wait for the response (IN area released).
   */

   RequestReleaseArea( 0, 0, INDREG_IN );

   do
   {
      WaitAnybusResponse();
   }
   while( AbIndCopy & INDREG_IN );

} /* end of ExchangeData() */

void RxData2( void )
{
   int x=0;
   
   /*
   ** Do a locked request to the OUT area and wait for access.
   ** The result of a locked request is, that in case we first get a NAK, access
   ** to the area will be granted automatically when the AnyBus releases it
   ** (ABS-DGP 5.3.2.1).
   */
   x++;
   	
   RequestReleaseArea( 1, 1, INDREG_OUT );
  
   do
   {
      WaitAnybusResponse();
   }
   while( !( AbIndCopy & INDREG_OUT ) );
   	  
   /*
   ** The OUT area is now accessed.
   ** Read the first 10 bytes (ABS-DGP 5.2.1).
   */
	for(x=0;x<120;x++)
	{	
		pfb_rxdata2[x] = XBYTE( ABS_DPRAM_BASE + 0x0200 + x);
		vxbUsDelay(1);
	}	
	
   /*
   ** Release the OUT area and wait for the response (OUT area released).
   */

   RequestReleaseArea( 0, 0, INDREG_OUT );
   
   do
   {
      WaitAnybusResponse();
   }
   while( AbIndCopy & INDREG_OUT );
   
   /*
   ** Request the IN area (locked) and wait for access.
   */

}

void TxData2( void )
{
	int x=0;
	
   RequestReleaseArea( 1, 1, INDREG_IN );
   
   do
   {
	  WaitAnybusResponse();
   }
   while( !( AbIndCopy & INDREG_IN ) );
   
   /*
   ** The IN area is now accessed.
   ** Return the first 10 bytes.
   */
   	   
	for(x=0;x<120;x++)
	{	
	   XBYTE( ABS_DPRAM_BASE + x ) = pfb_txdata2[x];//+n;
	   vxbUsDelay(1);
	}   
		
   /*
   ** Release the IN area and wait for the response (IN area released).
   */

   RequestReleaseArea( 0, 0, INDREG_IN );
  
   do
   {
      WaitAnybusResponse();
   }
   while( AbIndCopy & INDREG_IN );
  
} /* end of ExchangeData() */
/*------------------------------------------------------------------------------
**
** stopAnybusTask()
**
** stop anybus timer interrupt task
** 
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**     stopAnybusTask();
**
**------------------------------------------------------------------------------
*/
void stopAnybusTask(void)
{

	int status;
	if(anybusTimerTaskId1!=0 && anybusTimerTaskId1!=ERROR )
		status=taskDelete(anybusTimerTaskId1);	
	if(status==OK)
		logMsg("\n delete anybusExDataTask1 succeed\n",0,0,0,0,0,0);
	else
		logMsg("\n delete anybusExDataTask1 failed\n",0,0,0,0,0,0);	
/*	
 	status=taskDelete(anybusTimerTaskId2);	
	if(status==OK)
		logMsg("deleta anybusExDataTask2 succeed\n",0,0,0,0,0,0);
	else
		logMsg("deleta anybusExDataTask2 failed\n",0,0,0,0,0,0);	
*/	 
 
}
/*******************************************************************************
**
** End of ANYBUS.C
**
********************************************************************************
*/
