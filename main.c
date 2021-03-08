/*  
* target:ast7600  	 
* filename: main.c 
* Copyright 2018,8,27 IEE, Inc.
* description: 主程序,Interrupt occurs every 7200Hz, SVM applied 
* function: 
* debug:
* modification history：
* 
*/
/*******************************************/
#include "main.h"
#include "chopper.h"
//#define ANPC_MODE 1
UINT32 time1=0,time2=0,time3=0,cpufreq=0;
float timer_realPeriod=0,work_time=0;
float UDC_TMEP=5500;
unsigned int ANPC_MODE;
cpuset_t affinity ;
cpuset_t pAffinity;
int main(void)		
{	
	int 	status;
	sysClkRateSet(5000);/*1ms/tick*/
	
	/*************step1:变量初始化*************/	
	Varible_Init_first();//各变量及read/write各板卡结构体变量初始化
	Control_init();//PR锁相环初始化
	initPWMdata(&PWMdata_INV1);//PWM结构体变量初始化
	initPWMdata(&PWMdata_INV2);
	initPWMdata(&PWMdata_REC1);
	initPWMdata(&PWMdata_REC2);   	
	
	/*****************step2:FPGA板卡初始化*****************/
   	checkAst7700_VME();                     /*首先检测交换卡ast7700，返回selfCheck.bit.ast7700SrioInitOK*/
	//if(selfCheck.bit.ast7700SrioInitOK==1)  /*若交换卡SRIO 路由功能初始化成功，则可以对使用SRIO总线的各从板进行初始化*/
	{			
		ast7600PpcSrioInit();				/*CPU ast7600上的Powerpc开始SRIO功能初始化*/
		RecPwm1Init_withSrio();				/*整流板1初始化*/
		RecPwm2Init_withSrio();				/*整流板2初始化*/
		InvPwm1Init_withSrio();				/*逆变板1初始化*/
		InvPwm2Init_withSrio();				/*逆变板2初始化*/
		AD1Init_withSrio();					/*AD板1初始化*/
		AD2Init_withSrio();					/*AD2初始化*/
		ClkInit_withSrio();					/*时钟板初始化*/
		ChopInit_withSrio();                /*斩波板初始化*/
	}	 	
	rfmInit(); 								/*反射内存PMC5565初始化*/
	IoBoardInit_VME(); 						/*IO板初始化*/
	DA4140Init_withVME();					/*DA板初始化*/	
	pfb_AB_init();							/*anybus_pfb板子 初始化*/	 
	 	 
	
	#ifdef UDP_DISPLAY
	status = SubMod_Data_DisplayUDP_Init(); //软示波器显示
	#endif  
	status = subModeModbusTcpInit(); 
	
	clkIntSem= semBCreate(SEM_Q_PRIORITY,SEM_EMPTY);/*创建中断信号量*/
	
	/*发起收数据的任务，优先级130*/

	clkIntTaskId=taskSpawn("clkIntTask",130,VX_SPE_TASK|VX_FP_TASK,20000,(FUNCPTR)clkIntTask,0,0,0,0,0,0,0,0,0,0);	
	CPUSET_ZERO(affinity);
 	CPUSET_SET(affinity,1);/*3>=n>=0,but core 0 is occupied by system task,so n must larger than 1*/
 	if(taskCpuAffinitySet(clkIntTaskId,affinity)==ERROR)
 	{
 		logMsg("main FPGA timer hard interrupt task clkIntTask set affinity 1 faild\n",0,0,0,0,0,0);
 		return(ERROR);
 	}	
 	else
 	{
 		logMsg("main FPGA timer hard interrupt task clkIntTask set affinity 1 succeeded\n",0,0,0,0,0,0);
 	}
 	
	/*关闭中断IRQ 1 */
	status = intDisable( 1 );
 			
 	/*挂载中断服务函数 */
	status = intConnect( (VOIDFUNCPTR *)INUM_TO_IVEC(1), (VOIDFUNCPTR)ast7600LocalbusFpgaIsr1, 0 );
	if( ERROR == status )
 	{
 		printf("Connect a C routine to a hardware interrupt fail.\n");
 		return ERROR;
 	}
 	
 	/*使能中断IRQ 1*/
	status = intEnable( 1 );
 	if( ERROR == status )
 	{
 		printf("Enable the interrupt 1 fail.\n");
 		return ERROR;
 	}
 	
 	/*设置影子时钟寄存器、比较寄存器、使能irq*/
 	astLocalbusFpgaSynShadowSet( 1,(UINT32)( TimerPeriod/600.0 ));
 	astLocalbusFpgaSynComparerSet( 1,100 );
 	astLocalbusFpgaSynIrqEnable( 1 ); 
 	
 	fpgaIoSem= semBCreate(SEM_Q_PRIORITY,SEM_FULL);/*创建FPGA Io使用互斥信号量*/
 	cpufreq = sysTimestampFreq();
 	
 	return OK;
    
}
/*********************7600上本地FPGA定时器中断调用函数*******************************************************************/
/*Name		:ast7600LocalbusFpgaIsr1
*Description:7600上本地FPGA定时器中断调用函数,释放信号量，触发一直等待的clkinttask任务
*Input		:无
*Output		:无
*Return		:OK or ERROR
*History	:2018-04-24,modified by wxx
*/
INT32 ast7600LocalbusFpgaIsr1( void )
{
	int status=0;
	/*清除irq中断*/
	astLocalbusFpgaSynIrqClear( 1 );
	 
	intCounter1++;
	/*logMsg("We have %d interruputs irq1.\n",astTestIsrTime1,1,2,3,4,5);*/
	status= semGive(clkIntSem);/*中断时，释放中断信号量*/

	/*翻转P2A1(GPIO0)输出电平,测试中断情况*/
	astLocalbusFpgaIODirSet( 0xa2, 1 );//设置P2A2方向为输出 
 	if(astTestLocalbusFpgaIrqIsr1Do)	
 		astTestLocalbusFpgaIrqIsr1Do = 0; 
	else								
		astTestLocalbusFpgaIrqIsr1Do = 1;
	astLocalbusFpgaGpioDOSet( 0xa2, astTestLocalbusFpgaIrqIsr1Do );
	if(selfCheck.bit.DABoardOK==1)
	{	
		datemp=-datemp;
		*(short *)(DALocalBaseAdrs+0x42)=datemp;		
 
	}
	if( ERROR == status )
	 	{
	 		logMsg("release the clkIntSem 0x%x fail.\n", clkIntSem,0,0,0,0,0);
	 		return ERROR;
	 	}
	return OK;
}
   
/********************定时器中断处理任务********************************************************************/
/*Name		:clkIntTask
*Description:7600上本地FPGA定时器中断处理任务,等待信号量
*Input		:无
*Output		:无
*Return		:OK or ERROR
*History	:2018-04-24,modified by wxx
*/
void clkIntTask(void)/*收数据程序*/
{   // 若本地生成角度测试SHE角度，搜索"本地VF"，按说明注释/去掉注释
  int i,j;
  unsigned int  rxcounter=0;	/*周期计数器*/
  int status;
 
  FOREVER		/*循环执行*/
  {
    semTake(clkIntSem,WAIT_FOREVER);/*等待中断信号量*/
    	taskCpuAffinityGet (clkIntTaskId, &pAffinity) ; 
    intCounter2++;
    sysTimestampDisable();		     
    sysTimestampEnable();	
    time1 = sysTimestamp(); 
    /*翻转P2C2(GPIO1)输出电平,测试中断程序用时情况*/
    
//    semTake(fpgaIoSem,WAIT_FOREVER);/*等待FPGA IO使用信号量*/
//    astLocalbusFpgaIODirSet( 0xc2, 1 );	//设置P2C2方向为输出 
//    astLocalbusFpgaGpioDOSet( 0xc2, 1 );/*进入中断处理程序时输出1，结束时输出0，测量电平宽度，可得程序用时*/
////  astLocalbusFpgaIODirSet( 0xa2, 1 );	//设置P2C2方向为输出 
//	astLocalbusFpgaGpioDOSet( 0xa2, 1 );/*进入中断处理程序时输出1，结束时输出0，测量电平宽度，可得程序用时*/
	astLocalbusFpgaIODirSet( 0xa3, 1 );	//设置P2C2方向为输出 
	astLocalbusFpgaGpioDOSet( 0xa3, 1 );/*进入中断处理程序时输出1，结束时输出0，测量电平宽度，可得程序用时*/
//	semGive(fpgaIoSem);
	
	/************************DA output test begin，测试用**********************************/	 
	if(selfCheck.bit.DABoardOK==1)
	{	
		datemp=-datemp;
		*(short *)(DALocalBaseAdrs+0x40)=1000;		
//		*(short *)(DALocalBaseAdrs+0x42)=1000;		
//		*(short *)(DALocalBaseAdrs+0x44)=(interrupt_index*100);	
	}
	
	/******************step1:修改本板FPGA定时器下一次比较中断时刻********************/
	/*定时器中断索引0~11，定时器是增计数锯齿波，通过比较值产生中断，每个锯齿波周期产生12次比较中断，
	 * 每次修改比较值设定下一次中断时刻*/
	interrupt_index=interrupt_index_next;			/*本周期的定时器中断计数索引=上一周期计算值*/
	interrupt_index_next++;							/* 计算下一次定时周期的定时器中断计数索引*/
	if(interrupt_index_next>=12)
		interrupt_index_next=0;
	/*7600 local FPGA timer compare register set, clock is 75Mhz,set timer frequency =7200hz, 
	 * preriod=12500,dived by 12,eveay part is 10146.66667, interrupt frequency=600hz,*/
	astLocalbusFpgaSynComparerSet( 1,(UINT32)(10416.66667*interrupt_index_next)+100 );   
	
	/******************step2:读取和接收各种输入数据********************/
		 /*电子切断命令，来自于分区计算机DCC，读取IO板DIN指令，
		  * 调试时暂时不用*/
	/*
	if( selfCheck.bit.IOBoardOK==1)
		shutOffCmd=*(short *)(IOLocalBaseAdrs+0xc0);
	if(shutOffCmd !=3)
		runCmd.word=0;  
	*/	
	 	
 	HMI_rxData_modbusTcp();			/*step2.1通过modobus TCP接收人机界面数据*/
	

	status=rfmReceiveCCU();		/*step2.2在对应定时器锯齿波增计数开始时和中点时刻接收MCU通过反射内存发送的数据*/
		
	checkSlaveSrioConnectStatus();	/*step2.3检测各从板SRIO连接状态*/
	
	AD_Sample_SRIO();   			/*step2.4模拟量采样*/
	
	if(FaultFlagHMI == 2)
	{
	   fault_protect_SRIO();			/*step2.5故障保护*/	
	}
	
	readGpsData_SRIO();				/*step2.6读取时钟板的GPS信息*/
	
	read_7700_SrioData();			/*step2.7读取交换机卡7700通过SRIO发过来的数据*/
	

	
	//调压器副边电压
	PWMdata_REC1.Ua=GridUa;
	PWMdata_REC1.Ub=GridUb;
	PWMdata_REC1.Uc=GridUc;
	
	PWMdata_REC2.Ua=GridUa;
	PWMdata_REC2.Ub=GridUb;
	PWMdata_REC2.Uc=GridUc;
	//调压器原边电压	

	Grid_PLL(&PWMdata_REC1); //锁相环，只对变流器1锁相，变流器2角度为变流器1锁相角加30度
	PWMdata_REC2.GridFOC_Gama=PWMdata_REC1.GridFOC_Gama;
	Calculate_RMS();
	Udc=Udc1_filter+Udc2_filter;

/*****************************step3:控制状态机变换流程，不执行指令，只根据状态和条件变换状态机*****************/

	#ifdef NORMAL_RUN 
	
	switch(runState) 
		{
			case WAIT:/* bit0:all_normal,可进入故障状态或自检状态*/
			  if(( selfCheck.bit.ast7700VMEOK==1 )& ( selfCheck.bit.clkBoardOK==1 ))
			  {
				  runState=SELFCHECK_OK;
				  logMsg("%d\tNext State: SELFCHECK_OK\n\n",statecnt,0,0,0,0,0);
				  statecnt++;
			  }	
			  else
			  {
				  runState=WAIT;
				  statenum1++;
				  if (statenum1>=1000)
					  statenum1=1000;
				  if (statenum1==1)
				  logMsg("%d\tCurrent State: Wait\n\n",statecnt,0,0,0,0,0);
			  }
			  if(faultFlag==1)
			  { 
				  runState=FAULT;
			  } 
			  break;  
			  
		   case SELFCHECK_OK:/* bit1:boardCheckOk 可进入故障、预充电或闭合主控状态*/
			  if(runCmd.word==4)
			  {
				  runState= REC_RUN;
				  logMsg("%d\tNext State: REC_RUN\n\n",statecnt,0,0,0,0,0);
				  statecnt++;
			  }
			  else if((runCmd.word==8 ))//进入放电状态---通过斩波放电必须保证主控及预充电断开
				{
					runState= DISCHARGE;
					runCmd.word=0;
				}
			  else if(runCmd.word==6)  //进入复位状态
			  {
				  runState= RESET;
				  logMsg("%d\tNext State: RESET_RUN\n\n",statecnt,0,0,0,0,0);
				  statecnt++;
			  }
			  else
			  {
				  runState=SELFCHECK_OK;
				  statenum2++;
				  if (statenum2>=1000)
					  statenum2=1000;
				  if (statenum2==1)
				  logMsg("%d\tCurrent State: SELFCHECK_OK\n\n",statecnt,0,0,0,0,0);
			  }
			  if(faultFlag==1)
			  { 
				  runState=FAULT;
			  } 
			  break;
				  
		   case REC_RUN:/*bit4:启动整流   可进入故障、或启动逆变或停止整流或放电状态*/
			  if(runCmd.word==5)  //进入启动逆变状态
			  {
				  runState= INV_RUN;
				  logMsg("%d\tNext State: INV_RUN\n\n",statecnt,0,0,0,0,0);
				  statecnt++;
			  }
			  else if((runCmd.word==8 ))//进入放电状态---通过斩波放电必须保证主控及预充电断开
				{
					runState= DISCHARGE;
					runCmd.word=0;
				}
			  else
			  {
				  runState=REC_RUN;
				  statenum5++;
			      if (statenum5>=1000)
				      statenum5=1000;
			      if (statenum5==1)
				  logMsg("%d\tCurrent State: REC_RUN\n\n",statecnt,0,0,0,0,0);
			  }
			  if(faultFlag==1)
			  { 
				  runState=FAULT;
			  } 
			  break;
			  	  	  	    
	       case INV_RUN:/*bit5:启动逆变   可进入故障、停止逆变或放电状态*/
	    	  if(runCmd.word==6)  //进入复位状态
			  {
				  runState= RESET;
				  logMsg("%d\tNext State: RESET_RUN\n\n",statecnt,0,0,0,0,0);
				  statecnt++;
			  }
	    	  if(runCmd.word==4)
	    	  {
	    		  runState= REC_RUN;
	    		  statenum5=0;
	    		  logMsg("%d\tNext State: REC_RUN\n\n",statecnt,0,0,0,0,0);
	    		  statecnt++;
	    	  }
			  else if((runCmd.word==8 ))//进入放电状态---通过斩波放电必须保证主控及预充电断开
				{
					runState= DISCHARGE;
					runCmd.word=0;
				}
	    	  else
	    	  {
	    		  runState= INV_RUN; 
				  statenum6++;
			      if (statenum6>=1000)
				      statenum6=1000;
			      if (statenum6==1)
				  logMsg("%d\tCurrent State: INV_RUN\n\n",statecnt,0,0,0,0,0);
	    	  }
			  if(faultFlag==1)
			  { 
				  runState=FAULT;
			  } 
			  break; 
			case DISCHARGE://bit9:放电   可进入故障、复位状态
				if(((Udc)<10)&& (faultFlag==1))
				{
					runState=FAULT;//故障状态
				} 
				else if((Udc<100)) //进入复位状态
				{
					runState= SELFCHECK_OK;
				}
				else
				{
					runState= DISCHARGE; 
				}
				break; 
	       case RESET:

				  runState= RESET; 
				  statenum11++;
				  if (statenum11>=200)
				  {
					statenum11=0;
					runState= WAIT;
					logMsg("%d\tNext State: WAIT\n\n",statecnt,0,0,0,0,0);
					statecnt++;
				  }
				  if (statenum11>=1000)
					  statenum11=1000;
				  if (statenum11==1)
				  logMsg("%d\tCurrent State: RESET\n\n",statecnt,0,0,0,0,0);

			  break; 
	       case FAULT:
			  runState= FAULT; 
			  
			  if(runCmd.word==6)
			  {
	    		  runState= RESET;
	    		  logMsg("%d\tNext State: RESET\n\n",statecnt,0,0,0,0,0);
	    		  statecnt++;
	    		  runCmd.word=0;
			  }
			  else
			  {
				  runState= FAULT; 
				  statenum10++;
				  if (statenum10>=1000)
					  statenum10=1000;
				  if (statenum10==1)
				  logMsg("%d\tCurrent State: FAULT\n\n",statecnt,0,0,0,0,0);
			  }			  
		  break; 
			  
	   default:
	   		  if(faultFlag==1)
	   		  { 
	   			runState=FAULT;
	   			logMsg("%d\tNext State: FAULT\n\n",statecnt,0,0,0,0,0);
	   			statecnt++;
	   		  }
	   		  else
	   		  {
	   			runState= WAIT;
	   			logMsg("%d\tNext State: WAIT\n\n",statecnt,0,0,0,0,0);
	   			statecnt++;
	   		  }
	   		  break;   
	   	}    
		  
   	#endif  		  
	
	/*****************调试模式下的状态机转换*****************/
	/*只根据手动输入的runcmd命令进行整流运行、逆变运行等几个少数状态机*/
	#ifdef DEBUG_RUN 
	runcmd=1;
	runCmd.word=runcmd;
		  if(runCmd.word==1 )//=1，起整流
			{
//			  if(interrupt_index==5 || interrupt_index==11)//在上升计数和下降计数的最后一拍计算整流PWM
			  {	  
			  runState= REC_RUN;
			  VFcontrol_Rec(); 
			  }
			  statecnt++;
			}	  
		  else if(runCmd.word==3  ) //=3,整流逆变一起起动
		  {
			  if(interrupt_index==0 )
				  runState= INV_RUN;	
			  VFcontrol_Rec();
			  VFcontrol_Inv(); 
			  
			  if( runcmd_old&2==0  )/*第一次开始运行逆变，需要初始化PWM模式*/
			  {	  
				  if(rxMcuData.invfreq<50)
				   { /*initist PWM Ts as SVM Mode*/
					 InitINVPWM_Mode(&PWMdata_INV1,0);//20160606
					 InitINVPWM_Mode(&PWMdata_INV2,0);//20160606
				   }
				   else /*initist PWM Ts as SHE Mode*/
				   {  
					 InitINVPWM_Mode(&PWMdata_INV1,1);//20160606
					 InitINVPWM_Mode(&PWMdata_INV2,1);//20160606
				   }				
					statecnt++;					 
			  }			  
			}
		  else if(runCmd.word==4  ) 
		  {
			  runState= RESET;
		  }
		  else if(runCmd.word==0) 
			{
			  runState= DISCHARGE;			 
			  statecnt++;
			}	
	#endif 
/****************************step4:根据状态机的状态执行控制指令 ******************************************/
#ifdef NORMAL_RUN 
switch(runState) 
{
case WAIT:/* bit0:等待状态，不发波，可输出故障*/
		  disable_AllPWMOut(); //CLK板PWM使能信号为0
		  disable_rec1PWM(); //整流板、逆变板PWM使能信号为0
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM();
		  reset_clr_fault(); //故障标志位清零，允许输出故障
break;
		  
case SELFCHECK_OK:/* bit1:板卡自检成功，不发波，对各板卡初始化*/
		  disable_AllPWMOut(); 
		  disable_rec1PWM();
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM();
		  reset_clr_fault();
		  runcmd=0;
		  //对整流板、逆变板初始化
		  resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_REC1);		  
		  resetPwmBoardReg_SRIO(&txRecSrioData2,&PWMdata_REC2);  
		  resetPWMdata(&PWMdata_INV1);
	      resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV1); 
		  resetPWMdata(&PWMdata_INV2);
		  resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV2);
break;
		
case Precharge:/* bit2:预充电  */  
	      disable_AllPWMOut(); 
		  disable_rec1PWM();
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM();
		  reset_clr_fault();
		  IO_out_ANPC.bit.precharge=1;
 break;
		 
case CloseMainSwitch:/* bit3:闭合主控  */
	      disable_AllPWMOut(); 
		  disable_rec1PWM();
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM();
		  reset_clr_fault();
		  IO_out_ANPC.bit.main_switch_led=1;//main_switch_led=1,第一个指示灯亮，则接触器开关闭合，网侧电流接入整流侧
break;
		  
case REC_RUN:/*bit4:启动整流*/
	      enable_AllPWMOut();	//允许整流板PWM波输出
		  enable_rec1PWM();
		  enable_rec2PWM(); 
		  disable_inv1PWM();	  
		  disable_inv2PWM();
		  reset_clr_fault();  //允许故障标志位输出，不清除故障
		  //ANPC_Rec();
		  if(ANPC_MODE == 1)
		  { 
			  if(intCounter2<360)
			  {
				  Rec1_anpc1();
				  Rec2_anpc1();
			  }
			  else
			  {
				  Rec1_anpc2();
				  Rec2_anpc2();
			  }
		  }
		  IO_out_ANPC.bit.start_rec=1;
		  runcmd=1;
		  /*之所以在整流状态执行逆变PWM的数据和寄存器复位，是因为变流器因为定子段切换，经常从逆变停机到整流待机状态*/
		  resetPWMdata(&PWMdata_INV1); 
		  resetPwmBoardReg_SRIO(&PWMdata_INV1,&txInvSrioData1); 
		  resetPWMdata(&PWMdata_INV2);
		  resetPwmBoardReg_SRIO(&PWMdata_INV2,&txInvSrioData2);        	 
break;
		  
case FindPosition:/*bit5:定位   */
	      enable_AllPWMOut();
		  enable_rec1PWM();
		  enable_rec2PWM(); 
		  reset_clr_fault();
	      define_position();//定位函数
break;

case INV_RUN:/*bit6:启动逆变*/
		  enable_AllPWMOut();
		  enable_rec1PWM();
		  enable_rec2PWM(); 
		  if(changestep == 1)
		  {
			  disable_inv1PWM();	  
			  disable_inv2PWM();
		  }
		  else
		  {
		     enable_inv1PWM();	  
		     enable_inv2PWM();
		  }
		  reset_clr_fault();
		  if(ANPC_MODE == 1)
		  {
			  if(intCounter2<360)
			  {
				  Rec1_anpc1();
				  Rec2_anpc1();
				  Inv1_anpc1();
				  Inv2_anpc1();
			  }
			  else
			  {
				  Rec1_anpc2();
				  Rec2_anpc2();
				  Inv1_anpc2();
				  Inv2_anpc2();			  
			  }
		  }
		  IO_out_ANPC.bit.start_inv=1;
break; 
		  
case STOP_INV:/*bit7:停止逆变   可进入停止整流或放电状态*/
		  enable_AllPWMOut();
		  enable_rec1PWM();
		  enable_rec2PWM(); 
		  disable_inv1PWM();	  
	  	  disable_inv2PWM();
	  	  reset_clr_fault();
	  	  IO_out_ANPC.bit.start_inv=0; 
	  	  resetPWMdata(&PWMdata_INV1);
		  resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV1);
		  resetPWMdata(&PWMdata_INV2);
		  resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV2);
	  	  runcmd=1;
break;

case STOP_REC:/*bit8:停止整流   可进入放电状态*/
	      disable_AllPWMOut();
		  disable_rec1PWM();
		  disable_rec2PWM(); 
		  disable_inv1PWM();	  
	  	  disable_inv2PWM();
	  	  reset_clr_fault();
	  	  IO_out_ANPC.bit.start_rec=0; 
	  	resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_REC1);		  
	  	resetPwmBoardReg_SRIO(&txRecSrioData2,&PWMdata_REC2);
	  	  runcmd=0;
break;

case DISCHARGE:/*bit9:放电   可进入复位状态*/
		  enable_AllPWMOut();
		  disable_rec1PWM();
		  disable_rec2PWM(); 
		  disable_inv1PWM();	  
		  disable_inv2PWM();
		  reset_clr_fault();
		  disCharge(Udc);  //调用放电函数	

break;

case RESET:/*bit10:复位状态  复位后回归等待状态*/
	     clr_fault();//清除各故障位
	     Varible_Init();//变量赋初值
	     disable_AllPWMOut(); 
	     disable_rec1PWM();
	     disable_rec2PWM();
	     disable_inv1PWM();
	     disable_inv2PWM();  
	     faultFlag=0;
		//调制各结构体变量恢复初值
		resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_REC1);		  
		resetPwmBoardReg_SRIO(&txRecSrioData2,&PWMdata_REC2);
		resetPWMdata(&PWMdata_INV1);
		resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV1);
		resetPWMdata(&PWMdata_INV2);
		resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV2);
break;

case FAULT:/*bit11:急停*/		
		  disable_AllPWMOut(); 
		  disable_rec1PWM();
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM(); 
//		  reset_clr_fault();
		  IO_out_ANPC.bit.precharge=0;
		  IO_out_ANPC.bit.start_rec=0;
		  IO_out_ANPC.bit.start_inv=0;
		  IO_out_ANPC.bit.main_switch_led=0;
		  runcmd=0;
break;
		
default:
		  disable_AllPWMOut(); 
		  disable_rec1PWM();
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM();  
		  reset_clr_fault();
break;   
	}


if( (runcmd & 1) ==1) 
//启动整流时runcmd=1，启动逆变时runcmd=2或3  整流时调用整流闭环控制和整流调制程序  逆变时调用逆变调制程序
 {

		PWMdata_REC1.Ia=recIa1;
		PWMdata_REC1.Ib=recIb1;
		PWMdata_REC1.Ic=recIc1;
		PWMdata_REC1.Udc_up_filter=Udc1_filter;
		PWMdata_REC1.Udc_down_filter=Udc2_filter;
		Grid_FOC_HighPower_REC1(&PWMdata_REC1);

		PWMdata_REC2.Ia=recIa2;
		PWMdata_REC2.Ib=recIb2;
		PWMdata_REC2.Ic=recIc2;
		PWMdata_REC2.Udc_up_filter=Udc1_filter;
		PWMdata_REC2.Udc_down_filter=Udc2_filter;
		Grid_FOC_HighPower_REC2(&PWMdata_REC2);
//		VFcontrol_Rec();  

		 recPWM_EntranceFunc(interrupt_index,&PWMdata_REC1,&txRecSrioData1 );/*整流PWM接口函数*/
		 recPWM_EntranceFunc(interrupt_index,&PWMdata_REC2,&txRecSrioData2 );/*整流PWM接口函数*/
 
   if( (runcmd & 2) ==2) //即启动逆变时
    {    
		invPWM_EntranceFunc(interrupt_index,&PWMdata_INV1,&txInvSrioData1 );/*逆变PWM接口函数*/
		invPWM_EntranceFunc(interrupt_index,&PWMdata_INV2,&txInvSrioData2 );/*逆变PWM接口函数*/
	}
   //chop_control(UDC_TMEP);
   chop_control(Udc);
} 
#endif


	runcmd_old=runCmd.word;//保留本拍命令值
	
 /************************step5:向各板卡、各端口输出各种数据********************************/ 
	status=rfmSendCCU(); 	     /*step5.1: 发送反射内存数据*/
	 
	/*step5.2在定时器中断函数结尾处对各从板通过SRIO写寄存器*/
	{
	txRecSrioData1.tx_liveCnt++;
	txRecSrioData2.tx_liveCnt++;
	txInvSrioData1.tx_liveCnt++;
	txInvSrioData2.tx_liveCnt++;
	txClkSrioData.tx_liveCnt++;
	if(selfCheck.bit.recBoard1OK==1) 
		ast7600PpcSrioSingleSendData(rec1SrioID,txPWMSrioLen,(UINT8 *)&txRecSrioData1);
	if(selfCheck.bit.recBoard2OK==1) 
		ast7600PpcSrioSingleSendData(rec2SrioID,txPWMSrioLen,(UINT8 *)&txRecSrioData2);
	if(selfCheck.bit.invBoard1OK==1) 
		ast7600PpcSrioSingleSendData(inv1SrioID,txPWMSrioLen,(UINT8 *)&txInvSrioData1);
	if(selfCheck.bit.invBoard2OK==1) 
		ast7600PpcSrioSingleSendData(inv2SrioID,txPWMSrioLen,(UINT8 *)&txInvSrioData2);
	if(selfCheck.bit.adBoard1OK==1) 
		ast7600PpcSrioSingleSendData(ad1SrioID,txADSrioLen,(UINT8 *)&txAdSrioData1);
	if(selfCheck.bit.adBoard2OK==1) 
		ast7600PpcSrioSingleSendData(ad2SrioID,txADSrioLen,(UINT8 *)&txAdSrioData2);
	if(selfCheck.bit.clkBoardOK==1) 
		ast7600PpcSrioSingleSendData(clkSrioID,txClkSrioLen,(UINT8 *)&txClkSrioData);	
	if(selfCheck.bit.chopBoardOK==1) 
	    ast7600PpcSrioSingleSendData(chopSrioID,txChopSrioLen,(UINT8 *)&txChopSrioData);
	  
	}
	HMI_txData_modbusTcp();    			/*step5.3 通过modbus TCP向人机界面发送数据*/
	#ifdef UDP_DISPLAY
		taskSendHighRateData_24_4K();	 /*step5.4:通过UDP向软示波器发送波形数据 */
	#endif
		
	write_7700_SrioData();
	astLocalbusFpgaGpioDOSet( 0xa3, 0 );	/*进入中断处理程序时输出1，结束时输出0，测量电平宽度，可得程序用时*/
 
	
	if(selfCheck.bit.DABoardOK==1)
	{	
		datemp=-datemp;
		*(short *)(DALocalBaseAdrs+0x40)=0;		
	}	
	time3 = sysTimestamp();
	work_time = (time3 - time1) / (cpufreq * 1.0 / 1000000);
   	
  }/*end forever*/
}
/*定时器中断处理任务结束*/
/****************************************************************************************/

/****************************************************************************************/
void stop()
{
	int status;
  
	status = intDisable( 1 );	
	if(status==OK)
	logMsg("disable 7600 local FPGA interrupt succeed\n",0,0,0,0,0,0);
	/*semFlush(clkIntSem );*/
	
	status = astLocalbusFpgaSynIrqDisable(1);	
	/*禁止本地时钟计数器中断*/
	logMsg("disable 7600 local FPGA interrupt succeed\n",0,0,0,0,0,0);
		/*semFlush(clkIntSem );*/
	
	status=taskDelete(clkIntTaskId);
	if(status==OK)
		logMsg("delete task succeed\n",0,0,0,0,0,0);
	
	status=semDelete(clkIntSem);
	if(status==OK)
		logMsg("delete sem succeed\n",0,0,0,0,0,0);
	

	if( selfCheck.bit.recBoard1OK==1){
		*(short *)(RecLocalBaseAdrs1+0x2a)=0;}/*pwm disable */
	if( selfCheck.bit.recBoard2OK==1){
		*(short *)(RecLocalBaseAdrs2+0x2a)=0;}/*pwm disable */	
	if( selfCheck.bit.invBoard1OK==1){
		*(short *)(InvLocalBaseAdrs1+0x2a)=0;}/*pwm disable */
	if( selfCheck.bit.invBoard2OK==1){
		*(short *)(InvLocalBaseAdrs2+0x2a)=0;}/*pwm disable */
	if( selfCheck.bit.pfbBoardOK==1)
		stopAnybusTask();
	 
	status=rfmClose();
	#ifdef UDP_DISPLAY
	  SubMod_Data_DisplayUDP_Close();
	#endif
	  subModeModbusTcpClose();
	astVmeMapFree();  
//	reboot();
}
/****************清除各从板故障****************************************************************************/

void clr_fault(void)
{
	runcmd=0x0;
	faultFlag=0;
	txRecSrioData1.clrFaultCmd =1;	
	txRecSrioData2.clrFaultCmd =1;	
	txInvSrioData1.clrFaultCmd =1;	
	txInvSrioData2.clrFaultCmd =1;	
	txAdSrioData1.clrFaultCmd =1;	
	txAdSrioData2.clrFaultCmd =1;	
	txClkSrioData.clrFaultCmd =1;
	faultCode.convfaultcode1.word=0;
	faultCode.convfaultcode2.word=0;
	faultFlag=0;
}
/****************复位各从板清除故障命令****************************************************************************/
void reset_clr_fault(void)
{
	 
	txRecSrioData1.clrFaultCmd =0;	
	txRecSrioData2.clrFaultCmd =0;	
	txInvSrioData1.clrFaultCmd =0;	
	txInvSrioData2.clrFaultCmd =0;	
	txAdSrioData1.clrFaultCmd =0;	
	txAdSrioData2.clrFaultCmd =0;	
	txClkSrioData.clrFaultCmd =0;
}
/***************************************************************************************/
void begin(void)
{
	runcmd=3;
	invfreq=0.1;
	vrefDirFlag = 0;//=0,频率正向增加，=1，频率减少
	errcnt=0;
	Kv_Inv=0.01;

}

void startREC(void)
{
	runCmd.word=4;
	runcmd = 1;
}

void end(void)
{
	runcmd=0;
}
 
void clr(void)
{
	runcmd=4;
}



void ANPC1()
{
	Inv_type_a=1;
}
void ANPC2()
{
	Inv_type_a=2;
}
void NPC()
{
	Inv_type_a=0;
}
void reset()
{
	runCmd.word=6;
}

