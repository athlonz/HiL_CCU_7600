///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Author: xufei; Date: 2013-08-27, function: Modbus Tcp Transmission*/
/*  使用说明：
  1. 使用时候需要将     #include subMod_ModbusTcp.h 在相应的.c文件中申明
  2. 在初始化时候调用   subModeModbusTcpInit()函数
  3. 在主进程里面对变量 RTDataUnion 填数
  4. 在结束时调用       subModeModbusTcpClose() 函数
  5. 如果定义 #define MODBUS_TCP_TEST_SEND_DATA，则发送的是测试数据看看通讯是否正常，正常运行时候不定义宏
  6. 如果发送数据里面高8位和低8位相反了，可将#define HIGH_LOW_ERROR宏定义放开
  7. 如果定义 #define KANKAN_DATA，则是用于监测底层数据报文，正常时候不定义该宏*/

/* #define MODBUS_TCP_TEST_SEND_DATA */
/*#define HIGH_LOW_ERROR*/
/*#define KANKAN_DATA*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "vxWorks.h"
#include "math.h"   
#include "subMod_ModbusTcp.h"
#include "variable_main.h"
#include "GlobalVar_Rec.h"
#include "GlobalVar_Inv.h"
#include "slaveBoardRW.h"
#include "main.h"
#ifdef KANKAN_DATA
#include "subMod_IEC104_Look.h"
#endif

/************************局部变量定义********************************************/
TCP_SEND_RECV_DATA TcpSendReceData;
RT_DATA_UNION RTDataUnion;

int rxTaskID_ModbusTcp;

int sockModbusTcp;                   /* socket fd */
int newConnectionModbusTcp;          /* socket fd */
int modbusTcpConnectFlag = 0;

int  cnt=0;
double theta=0;
short txHMI_livecnt=0;
short rxHMI_livecnt=0;
unsigned int FaultFlagHMI;
/****************************函数声明****************************************/
void taskModbusTcp(void);
STATUS ModbusTCPServer(void);
/********************************************************************************/
/*Name		:subModeModbusTcpInit
*Description：modbus TCP功能初始化
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by wxx
*/
STATUS subModeModbusTcpInit(void)
{
	cpuset_t affinity;
//	rxTaskID_ModbusTcp = taskSpawn("MODBUS_TCP",200,2,20000,(FUNCPTR)taskModbusTcp,0,0,0,0,0,0,0,0,0,0);
	rxTaskID_ModbusTcp = taskSpawn("MODBUS_TCP",200,VX_SPE_TASK|VX_FP_TASK,20000,(FUNCPTR)taskModbusTcp,0,0,0,0,0,0,0,0,0,0);
	CPUSET_ZERO(affinity);
	CPUSET_SET(affinity,3);/*3>=n>=0*/
	if(taskCpuAffinitySet(rxTaskID_ModbusTcp,affinity)==ERROR)
	{
		logMsg("MODBUS_TCP task rxTaskID_ModbusTcp set affinity 3 faild\n",0,0,0,0,0,0);		
	}	
	else
	{
		logMsg("MODBUS_TCP task rxTaskID_ModbusTcp set affinity 3 succeeded\n",0,0,0,0,0,0);
	}
	bzero((char *)&TcpSendReceData, sizeof(TCP_SEND_RECV_DATA));

	bzero((char *)&RTDataUnion, sizeof(RT_DATA_UNION));

	return (OK);
}
/********************************************************************************/
/*Name		:subModeModbusTcpClose
*Description：modbus TCP功能关闭
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by wxx
*/
STATUS subModeModbusTcpClose(void)
{
	int status;

	status = taskDelete(rxTaskID_ModbusTcp);

	/* close the sockets */
	if(modbusTcpConnectFlag == 1)
	{
		close (newConnectionModbusTcp);
		close (sockModbusTcp);
	}
	else
	{
		close(sockModbusTcp);
	}

	return (OK);
}
/********************************************************************************/
/*Name		:TcpModbusTranmissionTest
*Description：modbus TCP发送测试
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by xufei
*/
void TcpModbusTranmissionTest(void)
{
	INT16 countFor = 0;
         cnt++;
         if(cnt>2048) cnt=0;
         theta=theta+0.001; 
         if(theta>6.28) theta=theta-6.28; 
	for (countFor = 0; countFor< 50; countFor++)
	{
	
		RTDataUnion.RTData.INT16[countFor] = 0 + countFor+cnt;
		RTDataUnion.RTData.INT32[countFor] =  100+ countFor+cnt;
		RTDataUnion.RTData.FLOAT[countFor] =200+countFor+0.1234+cnt+sin(theta);
        }
}       
/********************************************************************************/
/*Name		:TcpModbusTransmission
*Description：modbus TCP发送函数
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by xufei
*/
void TcpModbusTransmission(void)
{
	INT16 sendNum = 0;
	INT16 startAddr = 0;
	INT16 countFor = 0;
        INT16 recRegNum=0;
        INT16 recByteNum=0;
         if(TcpSendReceData.receBuff[7]==3)/*continus read multi registers*/
        {

	sendNum = WORD_NORMAL(TcpSendReceData.receBuff[10], TcpSendReceData.receBuff[11]);
	startAddr = WORD_NORMAL(TcpSendReceData.receBuff[8], TcpSendReceData.receBuff[9]);

	TcpSendReceData.sendBuff[0] = TcpSendReceData.receBuff[0];  /*事务处理标识符*/
	TcpSendReceData.sendBuff[1] = TcpSendReceData.receBuff[1];

	TcpSendReceData.sendBuff[2] = TcpSendReceData.receBuff[2];  /*协议标识符*/
	TcpSendReceData.sendBuff[3] = TcpSendReceData.receBuff[3];

 

	TcpSendReceData.sendBuff[4] = HIBYTE_NORMAL(sendNum*2 + 3);
	TcpSendReceData.sendBuff[5] = LOBYTE_NORMAL(sendNum*2 + 3);

	TcpSendReceData.sendBuff[6] = TcpSendReceData.receBuff[6]; /*unit flag sign*/
	TcpSendReceData.sendBuff[7] = TcpSendReceData.receBuff[7]; /*function code*/

	TcpSendReceData.sendBuff[8] = sendNum * 2;/*byte number*/

 

	for (countFor = 0; countFor < sendNum; countFor++)
	{
#ifdef	HIGH_LOW_ERROR
		TcpSendReceData.sendBuff[9 + countFor*2] = RTDataUnion.all[startAddr*2 + countFor * 2 + 1];
		TcpSendReceData.sendBuff[9 + countFor*2 + 1] = RTDataUnion.all[startAddr*2 +  countFor * 2];
#else
		TcpSendReceData.sendBuff[9 + countFor*2] = RTDataUnion.all[startAddr*2 + countFor * 2];
		TcpSendReceData.sendBuff[9 + countFor*2 + 1] = RTDataUnion.all[startAddr*2 +  countFor * 2 + 1];
#endif
	}

	TcpSendReceData.sendNum = sendNum*2 + 9;
        }

        if(TcpSendReceData.receBuff[7]==16)/*continus write multi register */
        {

        startAddr = WORD_NORMAL(TcpSendReceData.receBuff[8], TcpSendReceData.receBuff[9]);
        recRegNum = WORD_NORMAL(TcpSendReceData.receBuff[10], TcpSendReceData.receBuff[11]);
        recByteNum=TcpSendReceData.receBuff[12];

        for (countFor = 0; countFor < recRegNum; countFor++)
	{
#ifdef	HIGH_LOW_ERROR
		RTDataUnion.all[startAddr*2 + countFor * 2]      = TcpSendReceData.receBuff[13 + countFor*2+1];
		RTDataUnion.all[startAddr*2 +  countFor * 2 + 1] = TcpSendReceData.receBuff[13+ countFor*2] ;
#else 
		RTDataUnion.all[startAddr*2 + countFor * 2]      = TcpSendReceData.receBuff[13 + countFor*2];
		RTDataUnion.all[startAddr*2 +  countFor * 2 + 1] = TcpSendReceData.receBuff[13+ countFor*2 + 1] ;
#endif
	}        

        TcpSendReceData.sendBuff[0] = TcpSendReceData.receBuff[0];  /*事务处理标识符0*/
	TcpSendReceData.sendBuff[1] = TcpSendReceData.receBuff[1];

	TcpSendReceData.sendBuff[2] = TcpSendReceData.receBuff[2];  /*协议标识符0*/
	TcpSendReceData.sendBuff[3] = TcpSendReceData.receBuff[3];
        
        TcpSendReceData.sendBuff[4] = HIBYTE_NORMAL(6);
	TcpSendReceData.sendBuff[5] = LOBYTE_NORMAL(6);

	TcpSendReceData.sendBuff[6] = TcpSendReceData.receBuff[6]; /*unit flag sign*/
	TcpSendReceData.sendBuff[7] = TcpSendReceData.receBuff[7]; /*function code*/

	TcpSendReceData.sendBuff[8] = TcpSendReceData.receBuff[8];
	TcpSendReceData.sendBuff[9] = TcpSendReceData.receBuff[9];
	TcpSendReceData.sendBuff[10] = TcpSendReceData.receBuff[10];
	TcpSendReceData.sendBuff[11] = TcpSendReceData.receBuff[11];
        TcpSendReceData.sendNum = 12;
        } 
}
/********************************************************************************/
/*Name		:taskModbusTcp
*Description：modbus TCP发送任务
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by xufei
*/
 

void taskModbusTcp(void)
{
	FOREVER
	{
		taskDelay(0.02*sysClkRateGet());/*delay 20ms*/

		ModbusTCPServer();
	}
}
/********************************************************************************/
/*Name		:ModbusTCPServer
*Description：modbus TCP 服务器处理数据
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by xufei
*/
STATUS ModbusTCPServer (void)
{
	int 		len;                    /* length of newConnAddr */ 
	int 		numRead;                /* number of bytes read */
	int 		optionVal;              /* value of setsocket option */
	struct linger 	linger;             /* amount of time to SO_LINGER */
	struct sockaddr_in	serverAddr;     /* server's address */
	struct sockaddr_in	newConnAddr;    /* client's address */
	int status;

	modbusTcpConnectFlag = 0;

	bzero ((char *) &serverAddr, sizeof (serverAddr));

	/* open the TCP socket */
	sockModbusTcp = socket (AF_INET, SOCK_STREAM, 0);

	if (sockModbusTcp < 0)
	{
		perror ("socket");
		return (ERROR);
	}

	optionVal = 1; /* Turn ON the diff. setsockopt options */

	if (setsockopt (sockModbusTcp, SOL_SOCKET, SO_REUSEADDR, (char *) &optionVal,
		sizeof(optionVal)) == ERROR)
	{
		perror ("echoTcpForever: setsockopt SO_REUSEADDR failed");
		close (sockModbusTcp);
		return (ERROR);
	}

	if (setsockopt (sockModbusTcp, SOL_SOCKET, SO_KEEPALIVE, (char *) &optionVal,
		sizeof(optionVal)) == ERROR)
	{
		perror ("echoTcpForever: setsockopt SO_KEEPALIVE failed");
		close (sockModbusTcp);
		return (ERROR);
	}

	if (setsockopt (sockModbusTcp, IPPROTO_TCP, TCP_NODELAY, (char *) &optionVal,
		sizeof(optionVal)) == ERROR)
	{
		perror ("echoTcpForever: setsockopt TCP_NODELAY failed");
		close (sockModbusTcp);
		return (ERROR);
	}

	linger.l_onoff = 1; 
	linger.l_linger = 0;

	if (setsockopt (sockModbusTcp, SOL_SOCKET, SO_LINGER, (char *) &linger, sizeof (linger))
		== ERROR)
	{
		perror ("echoTcpForever: setsockopt SO_LINGER failed");
		close (sockModbusTcp);
		return (ERROR);
	}

	/* Set up our internet address, and bind it so the client can connect. */
	serverAddr.sin_family      = AF_INET;
	serverAddr.sin_port        = htons (echoServerPort);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind (sockModbusTcp, (struct sockaddr *) &serverAddr, sizeof (serverAddr)) < 0)
	{
		perror ("bind");
		close (sockModbusTcp);
		return (ERROR);
	}

	/* Listen for the client to connect to us. */
	if (listen (sockModbusTcp, 5) < 0)
	{
		perror ("listen");
		close (sockModbusTcp);
		return (ERROR);
	}

	/* The client has connected.  Accept, and receive chars */
	len = sizeof (newConnAddr);
	newConnectionModbusTcp = accept (sockModbusTcp, (struct sockaddr  *) &newConnAddr, &len);
	if (newConnectionModbusTcp == ERROR)
	{
		perror ("accept failed");
		close (sockModbusTcp);
		return (ERROR);
	}
	else
	{
		modbusTcpConnectFlag = 1;
	}

	while ((numRead = recv (newConnectionModbusTcp, TcpSendReceData.receBuff, sizeof (TcpSendReceData.receBuff), 0)) > 0)
	{
#ifdef KANKAN_DATA
		if (iec104LookConnectFlag == 1)
		{
			status = send (newConnectionIec104Look, (char *)(TcpSendReceData.receBuff), numRead, 0);
		}
#endif

#ifdef MODBUS_TCP_TEST_SEND_DATA
		TcpModbusTranmissionTest();
#endif
                
			TcpModbusTransmission();
                 
		status =  send(newConnectionModbusTcp, (char *)TcpSendReceData.sendBuff, TcpSendReceData.sendNum, 0);

#ifdef KANKAN_DATA
		if (iec104LookConnectFlag == 1)
		{
			status = send (newConnectionIec104Look, (char *)(TcpSendReceData.sendBuff), TcpSendReceData.sendNum, 0);
		}
#endif	
		
	} 

	/* close the sockets */
	close (newConnectionModbusTcp);
	close (sockModbusTcp);

	return (OK);
}
/********************************************************************************/
/*Name		:HMI_txData_modbusTcp
*Description：为modbus TCP 上传给HMI而采集数据
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by wxx
*/
void HMI_txData_modbusTcp(void)
{
	  RTDataUnion.RTData.INT16[0] = txHMI_livecnt++;
	  RTDataUnion.RTData.INT16[1] = CntKv;//runCmd.word;
	  RTDataUnion.RTData.INT16[2] = runState;
	  
	  RTDataUnion.RTData.INT32[0] = selfCheck.word;
	  RTDataUnion.RTData.INT32[1] =faultCode.convfaultcode1.word;
	  RTDataUnion.RTData.INT32[2] =faultCode.convfaultcode2.word;

	  RTDataUnion.RTData.INT32[3] = faultCode.rec1_igctfault;
	  RTDataUnion.RTData.INT32[4] = faultCode.rec1_DeadZoneErr;
	  RTDataUnion.RTData.INT32[5] = faultCode.rec1_MinPulseErr ;
	  RTDataUnion.RTData.INT32[6] = faultCode.rec1_errstate;

	  RTDataUnion.RTData.INT32[7] = faultCode.rec2_igctfault;
	  RTDataUnion.RTData.INT32[8] = faultCode.rec2_MinPulseErr;
	  RTDataUnion.RTData.INT32[9] = faultCode.rec2_DeadZoneErr;
	  RTDataUnion.RTData.INT32[10] = faultCode.rec2_errstate;

	  RTDataUnion.RTData.INT32[11] = faultCode.inv1_igctfault;
	  RTDataUnion.RTData.INT32[12] = faultCode.inv1_MinPulseErr;
	  RTDataUnion.RTData.INT32[13] = faultCode.inv1_DeadZoneErr;
	  RTDataUnion.RTData.INT32[14] = faultCode.inv1_errstate;

	  RTDataUnion.RTData.INT32[15] = faultCode.inv2_igctfault;
	  RTDataUnion.RTData.INT32[16] = faultCode.inv2_MinPulseErr;
	  RTDataUnion.RTData.INT32[17] = faultCode.inv2_DeadZoneErr;
	  RTDataUnion.RTData.INT32[18] = faultCode.inv2_errstate;

	  RTDataUnion.RTData.INT32[19] = Precharge_OK;
	  RTDataUnion.RTData.INT32[19] = Parallel_Model;
	  RTDataUnion.RTData.INT32[20] = Serial_Model;
	  RTDataUnion.RTData.INT32[21] = AUTO_Model;
	 //  RTDataUnion.RTData.INT16[26] = time1freq0;

	  RTDataUnion.RTData.FLOAT[0] = Udc1;
	  RTDataUnion.RTData.FLOAT[1] = Udc2;
	  RTDataUnion.RTData.FLOAT[2] = (Udc1+Udc2);
	  RTDataUnion.RTData.FLOAT[3] = Urec_rms;
	  RTDataUnion.RTData.FLOAT[4] = Irec_rms;
	  RTDataUnion.RTData.FLOAT[5] = Uinv_rms;	  
	  RTDataUnion.RTData.FLOAT[6] = Iinv_rms;

	  RTDataUnion.RTData.FLOAT[7] = Urec_rms2;
	  RTDataUnion.RTData.FLOAT[8] = Irec_rms2;
	  RTDataUnion.RTData.FLOAT[9] = Uinv_rms2;		  
	  RTDataUnion.RTData.FLOAT[10] = Iinv_rms2;
	  RTDataUnion.RTData.FLOAT[11] = PWMdata_INV1.Kv;
	  RTDataUnion.RTData.FLOAT[12] = PWMdata_INV2.Kv;
	  RTDataUnion.RTData.FLOAT[13] = rxccudata.invfreq;
	  RTDataUnion.RTData.FLOAT[14] = GridUa;
	  RTDataUnion.RTData.FLOAT[15] = GridUb;//GridUb;		  
	  RTDataUnion.RTData.FLOAT[16] = GridUc;//300;//GridUc;
	  RTDataUnion.RTData.FLOAT[17] = GridIa;//400;//GridIa;
	  RTDataUnion.RTData.FLOAT[18] = GridIb;//GridIb;
	  RTDataUnion.RTData.FLOAT[19] = GridIc;//GridIc;
	  RTDataUnion.RTData.FLOAT[20] = GridU/sqrt(2);
	  RTDataUnion.RTData.FLOAT[21] = Idref*2*3/17.5;
	  RTDataUnion.RTData.FLOAT[22] = Idref;
	  RTDataUnion.RTData.FLOAT[23] = Isd_filter;
	  RTDataUnion.RTData.FLOAT[24] = Idref;
	  RTDataUnion.RTData.FLOAT[25] =Isd_filter2;
	  
	  
	  

}
/********************************************************************************/
/*Name		:HMI_txData_modbusTcp
*Description：接收HMI通过modubus TCP下传来的数据
*Input		:无
*Output		:无
*Return		:无
*History	:2016,by wxx
*/
void HMI_rxData_modbusTcp(void)
{
	  FaultFlagHMI = RTDataUnion.RTData.RX_INT16[0];  //301
	  ANPC_MODE    = RTDataUnion.RTData.RX_INT16[1];
	
	
//	  rxHMI_livecnt=RTDataUnion.RTData.INT16[50];
//	  MainSwitch   =RTDataUnion.RTData.INT16[51];
//	  Start_Rec    =RTDataUnion.RTData.INT16[52];
//	  Start_Inv    =RTDataUnion.RTData.INT16[53];
//	  Stop_Inv     =RTDataUnion.RTData.INT16[54];
//	  Stop_Rec     =RTDataUnion.RTData.INT16[55];
//	  Discharge    =RTDataUnion.RTData.INT16[56];
//	  Reset        =RTDataUnion.RTData.INT16[57];
//	  Position     =RTDataUnion.RTData.INT16[58];
//	  UdcSet       =RTDataUnion.RTData.INT16[59];
//	  Udc_protect_LK    =RTDataUnion.RTData.INT16[60];
//	  UdcNobalane_protect_LK =RTDataUnion.RTData.INT16[61];
//	  Iinv_protect_LK  =RTDataUnion.RTData.INT16[62];
//	  Irec_protect_LK  =RTDataUnion.RTData.INT16[63];
//	  Kv_Inv_from_LK  =RTDataUnion.RTData.FLOAT[20];
//	  invfreq_from_LK  =RTDataUnion.RTData.INT16[65];
//	  Precharge_LK =RTDataUnion.RTData.INT16[66];
//	  Parallel_LK =RTDataUnion.RTData.INT16[67];
//	  Serial_LK =RTDataUnion.RTData.INT16[68];
//	  AUTO_model_LK =RTDataUnion.RTData.INT16[69]; //为0，手动  为1，自动
}
