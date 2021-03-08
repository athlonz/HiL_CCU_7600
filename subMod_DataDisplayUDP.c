/*  
* target: 软示波器UDP上传数据显示程序   
* Copyright 2018,4,16 IEE, Inc. 
* description: 使用UDP协议，用网线上传数据给电脑  
* function: 
* debug: 
* modification history:

*/
/********************************************************************************/
#include "subMod_DataDisplayUDP.h"
#include "subMod_ControlFunction.h"
#include "variable_main.h"
#include "GlobalVar_Inv.h" 
#include "math.h"
#include "closeControl.h"
#include "GlobalVar_Rec.h"
 /********************************************************************************/
/*通讯数据定义*/
#define LENGTH_SEND_BUF 480

struct  HIGH_RATE_DISPLAY_SAVE HighRateDisplaySaveBuf;     /*data struct high rate display*/
INT16   SendHighRateDisplaySaveBufUDP[LENGTH_SEND_BUF];   /*send buffer*/

struct  MODU_VOLTAGE_DISPLAY ModuVoltageDisplayBuf;        /*voltage display*/
struct  TEMPR_FAULT_DISPLAY TemprFaultDisplay;

struct  HIGH_RATE_DISPLAY_SAVE HighRateDisplaySaveBufMsg; 
struct  MODU_VOLTAGE_DISPLAY ModuVoltageDisplayBufMsg;        /*voltage display*/
struct  TEMPR_FAULT_DISPLAY TemprFaultDisplayMsg;

/*网络通讯socket定义*/
int sFd;				              /* Socket file descriptor */
struct sockaddr_in serverSockAddr;    /* Address of server */
int sockAddrSize = sizeof (struct sockaddr_in);

int sFd1;
struct sockaddr_in serverSockAddr1;

int sFd2;
struct sockaddr_in serverSockAddr2;

/*系统消息机制定义*/
MSG_Q_ID msgHighRate, msgVoltage, msgFault;
int rxTaskID_HighRate, rxTaskID_Voltage, rxTaskID_Fault;

#define MAX_MESSAGES 100

int NetWorkInit(void);
void task_Msg_init(void);

void taskHighRateSend(void);
void taskVoltageSend(void);
void taskFaultSend(void);
extern short adData1[16];
extern short datemp;
/********************************************************************************/
/*Name		:SubMod_Data_DisplayUDP_Init
*Description:UDP数据显示函数初始化
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
int SubMod_Data_DisplayUDP_Init(void)
{
	task_Msg_init();

	return NetWorkInit();
}
/********************************************************************************/
/*Name		:NetWorkInit
*Description:UDP 协议初始化
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by xufei
*/
int NetWorkInit(void)
{
	/* Create socket */
	if ((sFd = socket (AF_INET, SOCK_DGRAM, 0)) == ERROR)
	{
		return (ERROR);
	}

	/* Bind not required - port number will be dynamic */
	/* Build server socket address */
	bzero ((char *)&serverSockAddr, sockAddrSize);

	serverSockAddr.sin_family      = AF_INET;
	serverSockAddr.sin_port        = htons (RECEIVE_PC_PORT);
	serverSockAddr.sin_addr.s_addr = inet_addr (RECEIVE_PC_IP);
	
	return (OK);
}
/********************************************************************************/
/*Name		:task_Msg_init
*Description:消息初始化
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by xufei
*/
void task_Msg_init(void)
{
	cpuset_t affinity;
	rxTaskID_HighRate = taskSpawn("taskHighRateSend",160,VX_SPE_TASK|VX_FP_TASK,20000,(FUNCPTR)taskHighRateSend,0,0,0,0,0,0,0,0,0,0);
	CPUSET_ZERO(affinity);
 	CPUSET_SET(affinity,3);/*3>=n>=0*/
 	if(taskCpuAffinitySet(rxTaskID_HighRate,affinity)==ERROR)
 	{
 		logMsg("taskHighRateSend task clkIntTask set affinity 3 faild\n",0,0,0,0,0,0); 		
 	}	
 	else
 	{
 		logMsg("taskHighRateSend task clkIntTask set affinity 3 succeeded\n",0,0,0,0,0,0);
 	}/*	rxTaskID_Voltage  = taskSpawn("taskVoltageSend", 150,0,20000,(FUNCPTR)taskVoltageSend ,0,0,0,0,0,0,0,0,0,0);
	rxTaskID_Fault    = taskSpawn("taskHighRateSend",150,0,20000,(FUNCPTR)taskFaultSend   ,0,0,0,0,0,0,0,0,0,0);*/

	msgHighRate = msgQCreate(MAX_MESSAGES ,sizeof(struct HIGH_RATE_DISPLAY_SAVE) ,MSG_Q_FIFO );
/*	msgVoltage  = msgQCreate(MAX_MESSAGES ,sizeof(struct MODU_VOLTAGE_DISPLAY)   ,MSG_Q_FIFO );
	msgFault    = msgQCreate(MAX_MESSAGES ,sizeof(struct TEMPR_FAULT_DISPLAY)    ,MSG_Q_FIFO );*/
}
/********************************************************************************/
/*Name		:SubMod_HightRate_Data_DisplayUDP_Send
*Description:高速数据发送执行函数
*Input		:无		
*Output		:无
*Return		:无
*History	:2016,by xufei
*/
int SubMod_HightRate_Data_DisplayUDP_Send(void)
{
	static int BufCountA = 0, BufCountB = 0;

	/* Send data to HighRate Buffer */
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen1[0]);
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen1[1]);
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen1[2]);
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen1[3]);
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen2[0]); 
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen2[1]);
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen2[2]);
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen2[3]); 
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen3[0]);
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen3[1]);
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen3[2]);
	SendHighRateDisplaySaveBufUDP[BufCountA++] = (HighRateDisplaySaveBuf.Screen3[3]);

	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen4[0]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen4[1]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen4[2]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen4[3]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen5[0]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen5[1]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen5[2]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen5[3]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen6[0]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen6[1]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen6[2]);
	SendHighRateDisplaySaveBufUDP[240+BufCountB++] = (HighRateDisplaySaveBuf.Screen6[3]);


	if (BufCountA == 240)
	{	
		BufCountA = 0;
		BufCountB = 0;
		if (sendto (sFd, (caddr_t)SendHighRateDisplaySaveBufUDP, 960, 0, (struct sockaddr  *) &serverSockAddr,sockAddrSize) == ERROR)
		{
			close (sFd);
			return (ERROR);
		}
	}
	return (OK);
}
/********************************************************************************/
/*Name		:taskHighRateSend
*Description:高速数据发送任务
*Input		:msgHighRate，&HighRateDisplaySaveBuf，sizeof(struct HIGH_RATE_DISPLAY_SAVE，WAIT_FOREVER
*Output		:无
*Return		:无
*History	:2016,by xufei
*/

void taskHighRateSend(void)
{
	FOREVER
	{
		if(msgQReceive(msgHighRate , &HighRateDisplaySaveBuf.Screen1[0], sizeof(struct HIGH_RATE_DISPLAY_SAVE) , WAIT_FOREVER) == ERROR)
		{
			printf("msgQReceive in task taskHighRateSend failed. \n");
		}
		else
		{
			SubMod_HightRate_Data_DisplayUDP_Send();
		}
	}
}
/********************************************************************************/
/*Name		:sendMsgHighRate
*Description:为高速数据发送用消息传递数据
*Input		:msgHighRate，&HighRateDisplaySaveBuf，sizeof(struct HIGH_RATE_DISPLAY_SAVE，WAIT_FOREVER
*Output		:无
*Return		:无
*History	:2016,by xufei
*/
void sendMsgHighRate(void)
{
	if(msgQSend(msgHighRate , &HighRateDisplaySaveBufMsg.Screen1[0] , sizeof(struct HIGH_RATE_DISPLAY_SAVE), WAIT_FOREVER , MSG_PRI_NORMAL) == ERROR)
		printf("msgQ send in task 2 failed \n") ;
}
/********************************************************************************/
/*Name		:SubMod_Data_DisplayUDP_Close
*Description:关闭UDP协议
*Input		:msgHighRate，&HighRateDisplaySaveBuf，sizeof(struct HIGH_RATE_DISPLAY_SAVE，WAIT_FOREVER
*Output		:无
*Return		:无
*History	:2016,by xufei
*/

void SubMod_Data_DisplayUDP_Close(void)
{
	int status; 
	status = taskDelete(rxTaskID_HighRate);
	msgQDelete(msgHighRate);
	close(sFd); 
}
/********************************************************************************/
/*Name		:SwapWord
*Description：16位数据内高低字节垫高
*Input		:Data
*Output		:无
*Return		:无
*History	:2016,by xufei
*/

UINT16 SwapWord(UINT16 Data)
{
	return((Data>>8)|(Data<<8));
}

/********************************************************************************/
/*Name		:taskSendHighRateData_24_4K
*Description：高速数据发送的数据采集函数
*Input		:6个屏幕，每个屏幕4个通道，共24个通道
*Output		:无
*Return		:无
*History	:2016,by xufei
*/

void taskSendHighRateData_24_4K(void)
{


	static float theta = 0;
	int dataSend;
	float ISR_Timer = 1.0/7200.0;

	/*theta += 10.0*6.28 * ISR_Timer;*/
	theta += 50.0*6.28 * ISR_Timer;
        while(theta>6.283185) theta=theta-6.283185;
	dataSend = 100 * sin(theta);
	

	
    		HighRateDisplaySaveBufMsg.Screen1[0] =  0.1*GridUa;//usdFw;//10*PWMdata_REC1.Kv;  //GridUa;   //1000*Kvfilter.filter_out;//PWMdata_INV1.PWM4Cnt;//GridUa;   //网侧电压A
    		HighRateDisplaySaveBufMsg.Screen1[1] =  0.1*GridUb;//usqFw;//100*PWMdata_REC1.VFGama;//GridUb;   //10*PWMdata_INV1.KvIncreFlag;//GridUb;   //网侧电压B
    		HighRateDisplaySaveBufMsg.Screen1[2] =  0.1*GridUc;//10*chopStart_Flag;//0.01*Ud_Fwd;   //10*svmtest;//GridUc;   //网侧电压C
    		HighRateDisplaySaveBufMsg.Screen1[3] = 	recIa1;	 //10*PWMdata_INV1.InvModulationFlag;//Udc;	 //Udc
    		
    		HighRateDisplaySaveBufMsg.Screen2[0] =  recIb1;   //10*PWMdata_INV1.vvvf1IncreFlag;//Udc1;   //REC1_Ib
    		HighRateDisplaySaveBufMsg.Screen2[1] =  recIc1;   //10*PWMdata_INV1.HalfGama_TotalNum;//Udc2;   //REC1_Ic
    		HighRateDisplaySaveBufMsg.Screen2[2] =  Udc1;//10*PWMdata_REC2.GridFOC_Gama;//10*PWMdata_INV1.vvvf1;//PWMdata_REC1.Kv;
    		HighRateDisplaySaveBufMsg.Screen2[3] =  Udc2;//Udc1_filter;//10*PWMdata_REC2.VFGama;//1000*PWMdata_INV1.Ts;//PWMdata_REC2.Kv;		 //Udc2

    		HighRateDisplaySaveBufMsg.Screen3[0] =  recIa2;//Idref_udc;//10*PWMdata_INV1.Change_SL;//PWMdata_REC1.GridFOC_Gama;   //Udc
    		HighRateDisplaySaveBufMsg.Screen3[1] =  recIb2;//10*PWMdata_REC2.GridFOC_Gama;//10*PWMdata_INV1.ChangeSHENPJumpFlag;//   //REC2_Ia
    		HighRateDisplaySaveBufMsg.Screen3[2] =  recIc2;//10*PWMdata_REC2.VFGama;////PWMdata_REC2.VFGama;  //REC2_Ib
    		HighRateDisplaySaveBufMsg.Screen3[3] =  recUa1;//PWMdata_REC1.vref;//10*adData1[11]*10000.0/8192.0;  //REC2_Ic
    		

    		HighRateDisplaySaveBufMsg.Screen4[0] =  recUa2;
    		HighRateDisplaySaveBufMsg.Screen4[1] =  invUa1;
    		HighRateDisplaySaveBufMsg.Screen4[2] =  invUb1;
    		HighRateDisplaySaveBufMsg.Screen4[3] =  invUc1; 	

    		HighRateDisplaySaveBufMsg.Screen5[0] =  invUa2;//invIu1;//GridIa;//10*Isd_filter;//10*PWMdata_INV1.vvvf1;//adData2[0]*2000.0/8192.0;   //INV2_Ib
    		HighRateDisplaySaveBufMsg.Screen5[1] =  invUb2;//invIu2;//GridIb;//10*Isq_filter;//10*PWMdata_INV2.vvvf1;//adData2[1]*2000.0/8192.0;   //INV2_Ic
    		HighRateDisplaySaveBufMsg.Screen5[2] =  invUc2;//IaMotor;//GridIc;//10*Isd_filter2;//10*PWMdata_INV2.InvModulationFlag;//adData2[2]*2000.0/8192.0;   //INV1_Ua
    		HighRateDisplaySaveBufMsg.Screen5[3] =  invIu1;//10*changestep;//10*Isq_filter2;//10*PWMdata_INV1.InvModulationFlag;//adData2[3]*2000.0/8192.0;   //INV2_Ua

    	   	HighRateDisplaySaveBufMsg.Screen6[0] =  invIv1;//invUa1;//10*recIa1;//10*runcmd;
    		HighRateDisplaySaveBufMsg.Screen6[1] =  invIw1;//invUa2;//10*recIa2;//GridUa;//10*PWMdata_INV1.HalfGama_TotalNum;//PWMdata_INV2.VFGama;
    		HighRateDisplaySaveBufMsg.Screen6[2] =  invIu2;//10*PWMdata_REC1.VFGama;
    		HighRateDisplaySaveBufMsg.Screen6[3] =  invIv2;//PWMdata_REC1.vref;
    		     

//	HighRateDisplaySaveBufMsg.Screen1[0] =  10*recIa1;//usdFw;//10*PWMdata_REC1.Kv;  //GridUa;   //1000*Kvfilter.filter_out;//PWMdata_INV1.PWM4Cnt;//GridUa;   //网侧电压A
//	HighRateDisplaySaveBufMsg.Screen1[1] =  10*recIb1;//usqFw;//100*PWMdata_REC1.VFGama;//GridUb;   //10*PWMdata_INV1.KvIncreFlag;//GridUb;   //网侧电压B
//	HighRateDisplaySaveBufMsg.Screen1[2] =  10*recIc1;//10*chopStart_Flag;//0.01*Ud_Fwd;   //10*svmtest;//GridUc;   //网侧电压C
//	HighRateDisplaySaveBufMsg.Screen1[3] = 	Udc;	 //10*PWMdata_INV1.InvModulationFlag;//Udc;	 //Udc
//	
//	HighRateDisplaySaveBufMsg.Screen2[0] =  Udc1;   //10*PWMdata_INV1.vvvf1IncreFlag;//Udc1;   //REC1_Ib
//	HighRateDisplaySaveBufMsg.Screen2[1] =  Udc2;   //10*PWMdata_INV1.HalfGama_TotalNum;//Udc2;   //REC1_Ic
//	HighRateDisplaySaveBufMsg.Screen2[2] =  VdcRec;//10*PWMdata_REC2.GridFOC_Gama;//10*PWMdata_INV1.vvvf1;//PWMdata_REC1.Kv;
//	HighRateDisplaySaveBufMsg.Screen2[3] =  Isd2;//Udc1_filter;//10*PWMdata_REC2.VFGama;//1000*PWMdata_INV1.Ts;//PWMdata_REC2.Kv;		 //Udc2
//
//	HighRateDisplaySaveBufMsg.Screen3[0] =  10*recIa2;//Idref_udc;//10*PWMdata_INV1.Change_SL;//PWMdata_REC1.GridFOC_Gama;   //Udc
//	HighRateDisplaySaveBufMsg.Screen3[1] =  10*recIb2;//10*PWMdata_REC2.GridFOC_Gama;//10*PWMdata_INV1.ChangeSHENPJumpFlag;//   //REC2_Ia
//	HighRateDisplaySaveBufMsg.Screen3[2] =  10*recIc2;//10*PWMdata_REC2.VFGama;////PWMdata_REC2.VFGama;  //REC2_Ib
//	HighRateDisplaySaveBufMsg.Screen3[3] =  invIu1;//PWMdata_REC1.vref;//10*adData1[11]*10000.0/8192.0;  //REC2_Ic
//	
//
//	HighRateDisplaySaveBufMsg.Screen4[0] =  100*PWMdata_INV1.VFGama;
//	HighRateDisplaySaveBufMsg.Screen4[1] =  100*PWMdata_INV2.VFGama;
//	HighRateDisplaySaveBufMsg.Screen4[2] =  1000*PWMdata_INV1.Kv;
//	HighRateDisplaySaveBufMsg.Screen4[3] =  1000*PWMdata_INV2.Kv; 	
//
//	HighRateDisplaySaveBufMsg.Screen5[0] =  PWMdata_REC1.Idref;//invIu1;//GridIa;//10*Isd_filter;//10*PWMdata_INV1.vvvf1;//adData2[0]*2000.0/8192.0;   //INV2_Ib
//	HighRateDisplaySaveBufMsg.Screen5[1] =  PWMdata_REC2.Idref;//invIu2;//GridIb;//10*Isq_filter;//10*PWMdata_INV2.vvvf1;//adData2[1]*2000.0/8192.0;   //INV2_Ic
//	HighRateDisplaySaveBufMsg.Screen5[2] =  Isd;//IaMotor;//GridIc;//10*Isd_filter2;//10*PWMdata_INV2.InvModulationFlag;//adData2[2]*2000.0/8192.0;   //INV1_Ua
//	HighRateDisplaySaveBufMsg.Screen5[3] =  Isq;//10*changestep;//10*Isq_filter2;//10*PWMdata_INV1.InvModulationFlag;//adData2[3]*2000.0/8192.0;   //INV2_Ua
//
//	HighRateDisplaySaveBufMsg.Screen6[0] =  Isd2;//invUa1;//10*recIa1;//10*runcmd;
//	HighRateDisplaySaveBufMsg.Screen6[1] =  Isq2;//invUa2;//10*recIa2;//GridUa;//10*PWMdata_INV1.HalfGama_TotalNum;//PWMdata_INV2.VFGama;
//	HighRateDisplaySaveBufMsg.Screen6[2] =  10*PWMdata_INV2.Change_SL;//10*PWMdata_REC1.VFGama;
//	HighRateDisplaySaveBufMsg.Screen6[3] =  10*PWMdata_INV2.HalfGama_TotalNum;//PWMdata_REC1.vref;	

	sendMsgHighRate();
}



