/*  
* target:ast7600  	 
* filename: main.c 
* Copyright 2018,8,27 IEE, Inc.
* description: ������,Interrupt occurs every 7200Hz, SVM applied 
* function: 
* debug:
* modification history��
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
	
	/*************step1:������ʼ��*************/	
	Varible_Init_first();//��������read/write���忨�ṹ�������ʼ��
	Control_init();//PR���໷��ʼ��
	initPWMdata(&PWMdata_INV1);//PWM�ṹ�������ʼ��
	initPWMdata(&PWMdata_INV2);
	initPWMdata(&PWMdata_REC1);
	initPWMdata(&PWMdata_REC2);   	
	
	/*****************step2:FPGA�忨��ʼ��*****************/
   	checkAst7700_VME();                     /*���ȼ�⽻����ast7700������selfCheck.bit.ast7700SrioInitOK*/
	//if(selfCheck.bit.ast7700SrioInitOK==1)  /*��������SRIO ·�ɹ��ܳ�ʼ���ɹ�������Զ�ʹ��SRIO���ߵĸ��Ӱ���г�ʼ��*/
	{			
		ast7600PpcSrioInit();				/*CPU ast7600�ϵ�Powerpc��ʼSRIO���ܳ�ʼ��*/
		RecPwm1Init_withSrio();				/*������1��ʼ��*/
		RecPwm2Init_withSrio();				/*������2��ʼ��*/
		InvPwm1Init_withSrio();				/*����1��ʼ��*/
		InvPwm2Init_withSrio();				/*����2��ʼ��*/
		AD1Init_withSrio();					/*AD��1��ʼ��*/
		AD2Init_withSrio();					/*AD2��ʼ��*/
		ClkInit_withSrio();					/*ʱ�Ӱ��ʼ��*/
		ChopInit_withSrio();                /*ն�����ʼ��*/
	}	 	
	rfmInit(); 								/*�����ڴ�PMC5565��ʼ��*/
	IoBoardInit_VME(); 						/*IO���ʼ��*/
	DA4140Init_withVME();					/*DA���ʼ��*/	
	pfb_AB_init();							/*anybus_pfb���� ��ʼ��*/	 
	 	 
	
	#ifdef UDP_DISPLAY
	status = SubMod_Data_DisplayUDP_Init(); //��ʾ������ʾ
	#endif  
	status = subModeModbusTcpInit(); 
	
	clkIntSem= semBCreate(SEM_Q_PRIORITY,SEM_EMPTY);/*�����ж��ź���*/
	
	/*���������ݵ��������ȼ�130*/

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
 	
	/*�ر��ж�IRQ 1 */
	status = intDisable( 1 );
 			
 	/*�����жϷ����� */
	status = intConnect( (VOIDFUNCPTR *)INUM_TO_IVEC(1), (VOIDFUNCPTR)ast7600LocalbusFpgaIsr1, 0 );
	if( ERROR == status )
 	{
 		printf("Connect a C routine to a hardware interrupt fail.\n");
 		return ERROR;
 	}
 	
 	/*ʹ���ж�IRQ 1*/
	status = intEnable( 1 );
 	if( ERROR == status )
 	{
 		printf("Enable the interrupt 1 fail.\n");
 		return ERROR;
 	}
 	
 	/*����Ӱ��ʱ�ӼĴ������ȽϼĴ�����ʹ��irq*/
 	astLocalbusFpgaSynShadowSet( 1,(UINT32)( TimerPeriod/600.0 ));
 	astLocalbusFpgaSynComparerSet( 1,100 );
 	astLocalbusFpgaSynIrqEnable( 1 ); 
 	
 	fpgaIoSem= semBCreate(SEM_Q_PRIORITY,SEM_FULL);/*����FPGA Ioʹ�û����ź���*/
 	cpufreq = sysTimestampFreq();
 	
 	return OK;
    
}
/*********************7600�ϱ���FPGA��ʱ���жϵ��ú���*******************************************************************/
/*Name		:ast7600LocalbusFpgaIsr1
*Description:7600�ϱ���FPGA��ʱ���жϵ��ú���,�ͷ��ź���������һֱ�ȴ���clkinttask����
*Input		:��
*Output		:��
*Return		:OK or ERROR
*History	:2018-04-24,modified by wxx
*/
INT32 ast7600LocalbusFpgaIsr1( void )
{
	int status=0;
	/*���irq�ж�*/
	astLocalbusFpgaSynIrqClear( 1 );
	 
	intCounter1++;
	/*logMsg("We have %d interruputs irq1.\n",astTestIsrTime1,1,2,3,4,5);*/
	status= semGive(clkIntSem);/*�ж�ʱ���ͷ��ж��ź���*/

	/*��תP2A1(GPIO0)�����ƽ,�����ж����*/
	astLocalbusFpgaIODirSet( 0xa2, 1 );//����P2A2����Ϊ��� 
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
   
/********************��ʱ���жϴ�������********************************************************************/
/*Name		:clkIntTask
*Description:7600�ϱ���FPGA��ʱ���жϴ�������,�ȴ��ź���
*Input		:��
*Output		:��
*Return		:OK or ERROR
*History	:2018-04-24,modified by wxx
*/
void clkIntTask(void)/*�����ݳ���*/
{   // ���������ɽǶȲ���SHE�Ƕȣ�����"����VF"����˵��ע��/ȥ��ע��
  int i,j;
  unsigned int  rxcounter=0;	/*���ڼ�����*/
  int status;
 
  FOREVER		/*ѭ��ִ��*/
  {
    semTake(clkIntSem,WAIT_FOREVER);/*�ȴ��ж��ź���*/
    	taskCpuAffinityGet (clkIntTaskId, &pAffinity) ; 
    intCounter2++;
    sysTimestampDisable();		     
    sysTimestampEnable();	
    time1 = sysTimestamp(); 
    /*��תP2C2(GPIO1)�����ƽ,�����жϳ�����ʱ���*/
    
//    semTake(fpgaIoSem,WAIT_FOREVER);/*�ȴ�FPGA IOʹ���ź���*/
//    astLocalbusFpgaIODirSet( 0xc2, 1 );	//����P2C2����Ϊ��� 
//    astLocalbusFpgaGpioDOSet( 0xc2, 1 );/*�����жϴ������ʱ���1������ʱ���0��������ƽ��ȣ��ɵó�����ʱ*/
////  astLocalbusFpgaIODirSet( 0xa2, 1 );	//����P2C2����Ϊ��� 
//	astLocalbusFpgaGpioDOSet( 0xa2, 1 );/*�����жϴ������ʱ���1������ʱ���0��������ƽ��ȣ��ɵó�����ʱ*/
	astLocalbusFpgaIODirSet( 0xa3, 1 );	//����P2C2����Ϊ��� 
	astLocalbusFpgaGpioDOSet( 0xa3, 1 );/*�����жϴ������ʱ���1������ʱ���0��������ƽ��ȣ��ɵó�����ʱ*/
//	semGive(fpgaIoSem);
	
	/************************DA output test begin��������**********************************/	 
	if(selfCheck.bit.DABoardOK==1)
	{	
		datemp=-datemp;
		*(short *)(DALocalBaseAdrs+0x40)=1000;		
//		*(short *)(DALocalBaseAdrs+0x42)=1000;		
//		*(short *)(DALocalBaseAdrs+0x44)=(interrupt_index*100);	
	}
	
	/******************step1:�޸ı���FPGA��ʱ����һ�αȽ��ж�ʱ��********************/
	/*��ʱ���ж�����0~11����ʱ������������ݲ���ͨ���Ƚ�ֵ�����жϣ�ÿ����ݲ����ڲ���12�αȽ��жϣ�
	 * ÿ���޸ıȽ�ֵ�趨��һ���ж�ʱ��*/
	interrupt_index=interrupt_index_next;			/*�����ڵĶ�ʱ���жϼ�������=��һ���ڼ���ֵ*/
	interrupt_index_next++;							/* ������һ�ζ�ʱ���ڵĶ�ʱ���жϼ�������*/
	if(interrupt_index_next>=12)
		interrupt_index_next=0;
	/*7600 local FPGA timer compare register set, clock is 75Mhz,set timer frequency =7200hz, 
	 * preriod=12500,dived by 12,eveay part is 10146.66667, interrupt frequency=600hz,*/
	astLocalbusFpgaSynComparerSet( 1,(UINT32)(10416.66667*interrupt_index_next)+100 );   
	
	/******************step2:��ȡ�ͽ��ո�����������********************/
		 /*�����ж���������ڷ��������DCC����ȡIO��DINָ�
		  * ����ʱ��ʱ����*/
	/*
	if( selfCheck.bit.IOBoardOK==1)
		shutOffCmd=*(short *)(IOLocalBaseAdrs+0xc0);
	if(shutOffCmd !=3)
		runCmd.word=0;  
	*/	
	 	
 	HMI_rxData_modbusTcp();			/*step2.1ͨ��modobus TCP�����˻���������*/
	

	status=rfmReceiveCCU();		/*step2.2�ڶ�Ӧ��ʱ����ݲ���������ʼʱ���е�ʱ�̽���MCUͨ�������ڴ淢�͵�����*/
		
	checkSlaveSrioConnectStatus();	/*step2.3�����Ӱ�SRIO����״̬*/
	
	AD_Sample_SRIO();   			/*step2.4ģ��������*/
	
	if(FaultFlagHMI == 2)
	{
	   fault_protect_SRIO();			/*step2.5���ϱ���*/	
	}
	
	readGpsData_SRIO();				/*step2.6��ȡʱ�Ӱ��GPS��Ϣ*/
	
	read_7700_SrioData();			/*step2.7��ȡ��������7700ͨ��SRIO������������*/
	

	
	//��ѹ�����ߵ�ѹ
	PWMdata_REC1.Ua=GridUa;
	PWMdata_REC1.Ub=GridUb;
	PWMdata_REC1.Uc=GridUc;
	
	PWMdata_REC2.Ua=GridUa;
	PWMdata_REC2.Ub=GridUb;
	PWMdata_REC2.Uc=GridUc;
	//��ѹ��ԭ�ߵ�ѹ	

	Grid_PLL(&PWMdata_REC1); //���໷��ֻ�Ա�����1���࣬������2�Ƕ�Ϊ������1����Ǽ�30��
	PWMdata_REC2.GridFOC_Gama=PWMdata_REC1.GridFOC_Gama;
	Calculate_RMS();
	Udc=Udc1_filter+Udc2_filter;

/*****************************step3:����״̬���任���̣���ִ��ָ�ֻ����״̬�������任״̬��*****************/

	#ifdef NORMAL_RUN 
	
	switch(runState) 
		{
			case WAIT:/* bit0:all_normal,�ɽ������״̬���Լ�״̬*/
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
			  
		   case SELFCHECK_OK:/* bit1:boardCheckOk �ɽ�����ϡ�Ԥ����պ�����״̬*/
			  if(runCmd.word==4)
			  {
				  runState= REC_RUN;
				  logMsg("%d\tNext State: REC_RUN\n\n",statecnt,0,0,0,0,0);
				  statecnt++;
			  }
			  else if((runCmd.word==8 ))//����ŵ�״̬---ͨ��ն���ŵ���뱣֤���ؼ�Ԥ���Ͽ�
				{
					runState= DISCHARGE;
					runCmd.word=0;
				}
			  else if(runCmd.word==6)  //���븴λ״̬
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
				  
		   case REC_RUN:/*bit4:��������   �ɽ�����ϡ�����������ֹͣ������ŵ�״̬*/
			  if(runCmd.word==5)  //�����������״̬
			  {
				  runState= INV_RUN;
				  logMsg("%d\tNext State: INV_RUN\n\n",statecnt,0,0,0,0,0);
				  statecnt++;
			  }
			  else if((runCmd.word==8 ))//����ŵ�״̬---ͨ��ն���ŵ���뱣֤���ؼ�Ԥ���Ͽ�
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
			  	  	  	    
	       case INV_RUN:/*bit5:�������   �ɽ�����ϡ�ֹͣ����ŵ�״̬*/
	    	  if(runCmd.word==6)  //���븴λ״̬
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
			  else if((runCmd.word==8 ))//����ŵ�״̬---ͨ��ն���ŵ���뱣֤���ؼ�Ԥ���Ͽ�
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
			case DISCHARGE://bit9:�ŵ�   �ɽ�����ϡ���λ״̬
				if(((Udc)<10)&& (faultFlag==1))
				{
					runState=FAULT;//����״̬
				} 
				else if((Udc<100)) //���븴λ״̬
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
	
	/*****************����ģʽ�µ�״̬��ת��*****************/
	/*ֻ�����ֶ������runcmd��������������С�������еȼ�������״̬��*/
	#ifdef DEBUG_RUN 
	runcmd=1;
	runCmd.word=runcmd;
		  if(runCmd.word==1 )//=1��������
			{
//			  if(interrupt_index==5 || interrupt_index==11)//�������������½����������һ�ļ�������PWM
			  {	  
			  runState= REC_RUN;
			  VFcontrol_Rec(); 
			  }
			  statecnt++;
			}	  
		  else if(runCmd.word==3  ) //=3,�������һ����
		  {
			  if(interrupt_index==0 )
				  runState= INV_RUN;	
			  VFcontrol_Rec();
			  VFcontrol_Inv(); 
			  
			  if( runcmd_old&2==0  )/*��һ�ο�ʼ������䣬��Ҫ��ʼ��PWMģʽ*/
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
/****************************step4:����״̬����״ִ̬�п���ָ�� ******************************************/
#ifdef NORMAL_RUN 
switch(runState) 
{
case WAIT:/* bit0:�ȴ�״̬�������������������*/
		  disable_AllPWMOut(); //CLK��PWMʹ���ź�Ϊ0
		  disable_rec1PWM(); //�����塢����PWMʹ���ź�Ϊ0
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM();
		  reset_clr_fault(); //���ϱ�־λ���㣬�����������
break;
		  
case SELFCHECK_OK:/* bit1:�忨�Լ�ɹ������������Ը��忨��ʼ��*/
		  disable_AllPWMOut(); 
		  disable_rec1PWM();
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM();
		  reset_clr_fault();
		  runcmd=0;
		  //�������塢�����ʼ��
		  resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_REC1);		  
		  resetPwmBoardReg_SRIO(&txRecSrioData2,&PWMdata_REC2);  
		  resetPWMdata(&PWMdata_INV1);
	      resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV1); 
		  resetPWMdata(&PWMdata_INV2);
		  resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV2);
break;
		
case Precharge:/* bit2:Ԥ���  */  
	      disable_AllPWMOut(); 
		  disable_rec1PWM();
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM();
		  reset_clr_fault();
		  IO_out_ANPC.bit.precharge=1;
 break;
		 
case CloseMainSwitch:/* bit3:�պ�����  */
	      disable_AllPWMOut(); 
		  disable_rec1PWM();
		  disable_rec2PWM();
		  disable_inv1PWM();
		  disable_inv2PWM();
		  reset_clr_fault();
		  IO_out_ANPC.bit.main_switch_led=1;//main_switch_led=1,��һ��ָʾ��������Ӵ������رպϣ������������������
break;
		  
case REC_RUN:/*bit4:��������*/
	      enable_AllPWMOut();	//����������PWM�����
		  enable_rec1PWM();
		  enable_rec2PWM(); 
		  disable_inv1PWM();	  
		  disable_inv2PWM();
		  reset_clr_fault();  //������ϱ�־λ��������������
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
		  /*֮����������״ִ̬�����PWM�����ݺͼĴ�����λ������Ϊ��������Ϊ���Ӷ��л������������ͣ������������״̬*/
		  resetPWMdata(&PWMdata_INV1); 
		  resetPwmBoardReg_SRIO(&PWMdata_INV1,&txInvSrioData1); 
		  resetPWMdata(&PWMdata_INV2);
		  resetPwmBoardReg_SRIO(&PWMdata_INV2,&txInvSrioData2);        	 
break;
		  
case FindPosition:/*bit5:��λ   */
	      enable_AllPWMOut();
		  enable_rec1PWM();
		  enable_rec2PWM(); 
		  reset_clr_fault();
	      define_position();//��λ����
break;

case INV_RUN:/*bit6:�������*/
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
		  
case STOP_INV:/*bit7:ֹͣ���   �ɽ���ֹͣ������ŵ�״̬*/
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

case STOP_REC:/*bit8:ֹͣ����   �ɽ���ŵ�״̬*/
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

case DISCHARGE:/*bit9:�ŵ�   �ɽ��븴λ״̬*/
		  enable_AllPWMOut();
		  disable_rec1PWM();
		  disable_rec2PWM(); 
		  disable_inv1PWM();	  
		  disable_inv2PWM();
		  reset_clr_fault();
		  disCharge(Udc);  //���÷ŵ纯��	

break;

case RESET:/*bit10:��λ״̬  ��λ��ع�ȴ�״̬*/
	     clr_fault();//���������λ
	     Varible_Init();//��������ֵ
	     disable_AllPWMOut(); 
	     disable_rec1PWM();
	     disable_rec2PWM();
	     disable_inv1PWM();
	     disable_inv2PWM();  
	     faultFlag=0;
		//���Ƹ��ṹ������ָ���ֵ
		resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_REC1);		  
		resetPwmBoardReg_SRIO(&txRecSrioData2,&PWMdata_REC2);
		resetPWMdata(&PWMdata_INV1);
		resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV1);
		resetPWMdata(&PWMdata_INV2);
		resetPwmBoardReg_SRIO(&txRecSrioData1,&PWMdata_INV2);
break;

case FAULT:/*bit11:��ͣ*/		
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
//��������ʱruncmd=1���������ʱruncmd=2��3  ����ʱ���������ջ����ƺ��������Ƴ���  ���ʱ���������Ƴ���
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

		 recPWM_EntranceFunc(interrupt_index,&PWMdata_REC1,&txRecSrioData1 );/*����PWM�ӿں���*/
		 recPWM_EntranceFunc(interrupt_index,&PWMdata_REC2,&txRecSrioData2 );/*����PWM�ӿں���*/
 
   if( (runcmd & 2) ==2) //���������ʱ
    {    
		invPWM_EntranceFunc(interrupt_index,&PWMdata_INV1,&txInvSrioData1 );/*���PWM�ӿں���*/
		invPWM_EntranceFunc(interrupt_index,&PWMdata_INV2,&txInvSrioData2 );/*���PWM�ӿں���*/
	}
   //chop_control(UDC_TMEP);
   chop_control(Udc);
} 
#endif


	runcmd_old=runCmd.word;//������������ֵ
	
 /************************step5:����忨�����˿������������********************************/ 
	status=rfmSendCCU(); 	     /*step5.1: ���ͷ����ڴ�����*/
	 
	/*step5.2�ڶ�ʱ���жϺ�����β���Ը��Ӱ�ͨ��SRIOд�Ĵ���*/
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
	HMI_txData_modbusTcp();    			/*step5.3 ͨ��modbus TCP���˻����淢������*/
	#ifdef UDP_DISPLAY
		taskSendHighRateData_24_4K();	 /*step5.4:ͨ��UDP����ʾ�������Ͳ������� */
	#endif
		
	write_7700_SrioData();
	astLocalbusFpgaGpioDOSet( 0xa3, 0 );	/*�����жϴ������ʱ���1������ʱ���0��������ƽ��ȣ��ɵó�����ʱ*/
 
	
	if(selfCheck.bit.DABoardOK==1)
	{	
		datemp=-datemp;
		*(short *)(DALocalBaseAdrs+0x40)=0;		
	}	
	time3 = sysTimestamp();
	work_time = (time3 - time1) / (cpufreq * 1.0 / 1000000);
   	
  }/*end forever*/
}
/*��ʱ���жϴ����������*/
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
	/*��ֹ����ʱ�Ӽ������ж�*/
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
/****************������Ӱ����****************************************************************************/

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
/****************��λ���Ӱ������������****************************************************************************/
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
	vrefDirFlag = 0;//=0,Ƶ���������ӣ�=1��Ƶ�ʼ���
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

