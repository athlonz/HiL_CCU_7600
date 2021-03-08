/*  
* target: rectifier and invertor PWM call enterance   
* Copyright 2018,4,16 IEE, Inc. 
* description:   
* function: 
* debug: 
* modification history:20180426 by wxx
*/
/*************************************************************************************/
#include "vxWorks.h"
#include "ast7600Srio.h"
#include "variable_main.h"
#include "GlobalVar_Rec.h"
#include "GlobalVar_Inv.h"
#include "pwmControl.h"
#include "subMod_ControlFunction.h"
/*************************************************************************************/
struct PWMdataStruct PWMdata_INV1; 
struct PWMdataStruct PWMdata_INV2;
struct PWMdataStruct PWMdata_REC1; 
struct PWMdataStruct PWMdata_REC2;

int 	SHERoute[6] = {0};
/*********************��ʼ��PWM����*********************************************************/
/*Name		:initPWMdata
*Description:��ʼ��PWM���ݣ��ڲ�PWM�㷨����
*Input		:struct PWMdataStruct *PWMdata��Ҫ����ʼ����PWM���ݽṹ��ʵ������Ӧ�Ŀ�PWM��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void initPWMdata(struct PWMdataStruct *PWMdata)
{
	short i;
	
	PWMdata->vvvf1 = 20.0;
	PWMdata->vvvf1Interpolation = 20.0;
	PWMdata->Kv = 0.02;
	PWMdata->KvPre = 0.02;
	PWMdata->KvIncreFlag = 1;
	PWMdata->VFGama = 0;
	PWMdata->VFGama_Pre = 0;
	PWMdata->Ts = TsSVM;
	
	for(i=0;i<6;i++)
	{ 
		PWMdata->CMPA[i]=0;
		PWMdata->VFGama6[i] = 0.01;
		PWMdata->VFGama_Pre6[i]= 0.01 ;
		PWMdata->gama_Num[i]= 0 ;
		PWMdata->gama_Num_pre[i]= 0 ;
	}
	
	PWMdata->Reg_UpDown = 0;
	PWMdata->IntFlag = 1;
	PWMdata->Gama_TotalNum_Pre = 0;
	PWMdata->HalfGama_TotalNum = 0;
	PWMdata->Gama_TotalNum = 0;
	
	PWMdata->Epwm1_timeconti = MinP+1;
	PWMdata->Epwm2_timeconti = MinP+1;
	PWMdata->Epwm3_timeconti = MinP+1;
	PWMdata->Epwm4_timeconti = MinP+1;
	PWMdata->Epwm5_timeconti = MinP+1;
	PWMdata->Epwm6_timeconti = MinP+1;
	PWMdata->Epwm_g_i_timeconti = MinP+1;

	PWMdata->EPwm1PRDPre=AQ_CLEAR;
	PWMdata->EPwm1CADPre=AQ_NO_ACTION;
	PWMdata->EPwm1ZROPre=AQ_CLEAR;
	PWMdata->EPwm1CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm2PRDPre=AQ_CLEAR;
	PWMdata->EPwm2CADPre=AQ_NO_ACTION;
	PWMdata->EPwm2ZROPre=AQ_CLEAR;
	PWMdata->EPwm2CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm3PRDPre=AQ_CLEAR;
	PWMdata->EPwm3CADPre=AQ_NO_ACTION;
	PWMdata->EPwm3ZROPre=AQ_CLEAR;
	PWMdata->EPwm3CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm4PRDPre=AQ_CLEAR;
	PWMdata->EPwm4CADPre=AQ_NO_ACTION;
	PWMdata->EPwm4ZROPre=AQ_CLEAR;
	PWMdata->EPwm4CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm5PRDPre=AQ_CLEAR;
	PWMdata->EPwm5CADPre=AQ_NO_ACTION;
	PWMdata->EPwm5ZROPre=AQ_CLEAR;
	PWMdata->EPwm5CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm6PRDPre=AQ_CLEAR;
	PWMdata->EPwm6CADPre=AQ_NO_ACTION;
	PWMdata->EPwm6ZROPre=AQ_CLEAR;
	PWMdata->EPwm6CAUPre=AQ_NO_ACTION;
	 
	PWMdata->EPwm1EndState=1;
	PWMdata->EPwm2EndState=1;
	PWMdata->EPwm3EndState=1;
	PWMdata->EPwm4EndState=1;
	PWMdata->EPwm5EndState=1;
	PWMdata->EPwm6EndState=1;
	PWMdata->EPwmEndState=0;  

	PWMdata->EPwm1PRDPre=AQ_CLEAR;
	PWMdata->EPwm1CADPre=AQ_NO_ACTION;
	PWMdata->EPwm1ZROPre=AQ_CLEAR;
	PWMdata->EPwm1CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm2PRDPre=AQ_CLEAR;
	PWMdata->EPwm2CADPre=AQ_NO_ACTION;
	PWMdata->EPwm2ZROPre=AQ_CLEAR;
	PWMdata->EPwm2CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm3PRDPre=AQ_CLEAR;
	PWMdata->EPwm3CADPre=AQ_NO_ACTION;
	PWMdata->EPwm3ZROPre=AQ_CLEAR;
	PWMdata->EPwm3CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm4PRDPre=AQ_CLEAR;
	PWMdata->EPwm4CADPre=AQ_NO_ACTION;
	PWMdata->EPwm4ZROPre=AQ_CLEAR;
	PWMdata->EPwm4CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm5PRDPre=AQ_CLEAR;
	PWMdata->EPwm5CADPre=AQ_NO_ACTION;
	PWMdata->EPwm5ZROPre=AQ_CLEAR;
	PWMdata->EPwm5CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm6PRDPre=AQ_CLEAR;
	PWMdata->EPwm6CADPre=AQ_NO_ACTION;
	PWMdata->EPwm6ZROPre=AQ_CLEAR;
	PWMdata->EPwm6CAUPre=AQ_NO_ACTION;
	
	PWMdata->ACQ_ZRO = 0x0999;
	PWMdata->ACQ_CAU = 0;
	PWMdata->ACQ_PRD = 0x0999;
	PWMdata->ACQ_CAD = 0;
	 
	PWMdata->EPwm1EndState=1;
	PWMdata->EPwm2EndState=1;
	PWMdata->EPwm3EndState=1;
	PWMdata->EPwm4EndState=1;
	PWMdata->EPwm5EndState=1;
	PWMdata->EPwm6EndState=1;
	PWMdata->EPwmEndState=0;
	
	PWMdata->FirstEnPWMFlag = 1;
	PWMdata->vvvf1IncreFlag = 1;
	PWMdata->InvModulationFlag = 0;
	PWMdata->PWM4Cnt = 1;
	PWMdata->T1PR = T1PRSVM;
	PWMdata->T1PRHalf = T1PRHalfSVM; 
	PWMdata->Change_SL=0;
	PWMdata->SVMModeSwitchUp = 0;
	PWMdata->SVMModeSwitchDn = 0;
	PWMdata->SwitchSpeedDnOnce = 0;
	PWMdata->SwitchSpeedUpOnce = 0;
	PWMdata->ChangeSHENPJumpTest = 0;
	PWMdata->ModeFlag = 0;
	PWMdata->ChangeSHENPJumpFlag = 1;
	PWMdata->SVPWMType = 0;
	PWMdata->SHEReady = 0;
	PWMdata->MinPulseOK= 1 ;
	PWMdata->SHEShutFlag = 0;
	
	PWMdata->Ua = 0;
	PWMdata->Ub = 0;
	PWMdata->Uc = 0;
	PWMdata->Ia = 0;
	PWMdata->Ib = 0;
	PWMdata->Ic = 0;
	PWMdata->Theta_SecdFwd = 0;
	PWMdata->KiPLL = 0.1;
	PWMdata->KpPLL = 0.05;
	PWMdata->Delta = 0;
	PWMdata->GridFOC_Gama = 0;
	PWMdata->sum_usq = 0;
	PWMdata->Udc_up_filter = 0;
	PWMdata->Udc_down_filter = 0;
	PWMdata->NeuVoltCtl_sum = 0;
	PWMdata->Ki_uzRec = 0;
	PWMdata->Kp_uzRec = 0.03;
	
	//PWMdata->vref = 5000;
	PWMdata->vref = 4536;//3345;
	PWMdata->VKi = 1; //0.5;///ioworks VKi=0.0833
	PWMdata->vsum = 0.0;
	PWMdata->VKp = 1;//1; //2  //1//ioworks VKp=0.5
	PWMdata->vdsum=0;
	PWMdata->Idq_Ki = 1;  //1.0 //ioworks VKi=0.003
	PWMdata->Idq_Kp = 2; //0.7   ioworks VKp=0.05
	PWMdata->Idref = 0.0;
	PWMdata->Iqref = 0.0;
	PWMdata->vqsum = 0;	
}
/***********************��λPWM����*******************************************************/
/*Name		:resetPWMdata
*Description:��λPWM���ݣ���������ʼ��PWM���ݴ�����ͬ�� Ŀ���������������ͣ����λ�������ݣ��Ա����¿�ʼ�󲻻�Ӱ��PWM����������
*Input		:struct PWMdataStruct *PWMdata��Ҫ����λ��PWM���ݽṹ��ʵ������Ӧ�Ŀ�PWM��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
  
void resetPWMdata(struct PWMdataStruct *PWMdata)
{
	short i;
	
/* 	PWMdata->vvvf1 = 20.0;
	PWMdata->Kv = 0.02;
	PWMdata->VFGama = 0;
	PWMdata->VFGama_Pre = 0;
	PWMdata->Ts = TsSVM; */   // 20160601 cdd
	
	for(i=0;i<6;i++)
	{ 
		PWMdata->CMPA[i]=0;
		PWMdata->VFGama6[i] = 0.01;
		PWMdata->VFGama_Pre6[i]= 0.01 ;
		PWMdata->gama_Num[i]= 0 ;
		PWMdata->gama_Num_pre[i]= 0 ;
	}
	
	PWMdata->Reg_UpDown = 0;
	PWMdata->IntFlag = 1;
	PWMdata->Gama_TotalNum_Pre = 0;
	PWMdata->HalfGama_TotalNum = 0;
	PWMdata->Gama_TotalNum = 0;
	
/*	PWMdata->Epwm1_timeconti = MinP+1;
	PWMdata->Epwm2_timeconti = MinP+1;
	PWMdata->Epwm3_timeconti = MinP+1;
	PWMdata->Epwm4_timeconti = MinP+1;
	PWMdata->Epwm5_timeconti = MinP+1;
	PWMdata->Epwm6_timeconti = MinP+1;
*/
	PWMdata->Epwm1_timeconti = 0;
	PWMdata->Epwm2_timeconti = 0;
	PWMdata->Epwm3_timeconti = 0;
	PWMdata->Epwm4_timeconti = 0;
	PWMdata->Epwm5_timeconti = 0;
	PWMdata->Epwm6_timeconti = 0;

	PWMdata->Epwm_g_i_timeconti = MinP+1;

	PWMdata->EPwm1PRDPre=AQ_CLEAR;
	PWMdata->EPwm1CADPre=AQ_NO_ACTION;
	PWMdata->EPwm1ZROPre=AQ_CLEAR;
	PWMdata->EPwm1CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm2PRDPre=AQ_CLEAR;
	PWMdata->EPwm2CADPre=AQ_NO_ACTION;
	PWMdata->EPwm2ZROPre=AQ_CLEAR;
	PWMdata->EPwm2CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm3PRDPre=AQ_CLEAR;
	PWMdata->EPwm3CADPre=AQ_NO_ACTION;
	PWMdata->EPwm3ZROPre=AQ_CLEAR;
	PWMdata->EPwm3CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm4PRDPre=AQ_CLEAR;
	PWMdata->EPwm4CADPre=AQ_NO_ACTION;
	PWMdata->EPwm4ZROPre=AQ_CLEAR;
	PWMdata->EPwm4CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm5PRDPre=AQ_CLEAR;
	PWMdata->EPwm5CADPre=AQ_NO_ACTION;
	PWMdata->EPwm5ZROPre=AQ_CLEAR;
	PWMdata->EPwm5CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm6PRDPre=AQ_CLEAR;
	PWMdata->EPwm6CADPre=AQ_NO_ACTION;
	PWMdata->EPwm6ZROPre=AQ_CLEAR;
	PWMdata->EPwm6CAUPre=AQ_NO_ACTION;
	 
	PWMdata->EPwm1EndState=1;
	PWMdata->EPwm2EndState=1;
	PWMdata->EPwm3EndState=1;
	PWMdata->EPwm4EndState=1;
	PWMdata->EPwm5EndState=1;
	PWMdata->EPwm6EndState=1;
	PWMdata->EPwmEndState=0;  

	PWMdata->EPwm1PRDPre=AQ_CLEAR;
	PWMdata->EPwm1CADPre=AQ_NO_ACTION;
	PWMdata->EPwm1ZROPre=AQ_CLEAR;
	PWMdata->EPwm1CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm2PRDPre=AQ_CLEAR;
	PWMdata->EPwm2CADPre=AQ_NO_ACTION;
	PWMdata->EPwm2ZROPre=AQ_CLEAR;
	PWMdata->EPwm2CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm3PRDPre=AQ_CLEAR;
	PWMdata->EPwm3CADPre=AQ_NO_ACTION;
	PWMdata->EPwm3ZROPre=AQ_CLEAR;
	PWMdata->EPwm3CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm4PRDPre=AQ_CLEAR;
	PWMdata->EPwm4CADPre=AQ_NO_ACTION;
	PWMdata->EPwm4ZROPre=AQ_CLEAR;
	PWMdata->EPwm4CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm5PRDPre=AQ_CLEAR;
	PWMdata->EPwm5CADPre=AQ_NO_ACTION;
	PWMdata->EPwm5ZROPre=AQ_CLEAR;
	PWMdata->EPwm5CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm6PRDPre=AQ_CLEAR;
	PWMdata->EPwm6CADPre=AQ_NO_ACTION;
	PWMdata->EPwm6ZROPre=AQ_CLEAR;
	PWMdata->EPwm6CAUPre=AQ_NO_ACTION;
	
	PWMdata->ACQ_ZRO = 0x0999;
	PWMdata->ACQ_CAU = 0;
	PWMdata->ACQ_PRD = 0x0999;
	PWMdata->ACQ_CAD = 0;
	 
	PWMdata->EPwm1EndState=1;
	PWMdata->EPwm2EndState=1;
	PWMdata->EPwm3EndState=1;
	PWMdata->EPwm4EndState=1;
	PWMdata->EPwm5EndState=1;
	PWMdata->EPwm6EndState=1;
	PWMdata->EPwmEndState=0;
	
	PWMdata->FirstEnPWMFlag = 1;
	PWMdata->vvvf1IncreFlag = 1;
// 	  PWMdata->InvModulationFlag = 0;    // 20160601 cdd
	PWMdata->PWM4Cnt = 1;
 //   	PWMdata->T1PR = T1PRSVM;
//	PWMdata->T1PRHalf = T1PRHalfSVM; 
//	PWMdata->Ts = TsSVM;     // 20160601 cdd */
	PWMdata->Change_SL=0;
	PWMdata->SVMModeSwitchUp = 0;
	PWMdata->SVMModeSwitchDn = 0;
	PWMdata->SwitchSpeedDnOnce = 0;
	PWMdata->SwitchSpeedUpOnce = 0;
	PWMdata->ChangeSHENPJumpTest = 1;
	PWMdata->ModeFlag = 0;
	PWMdata->ChangeSHENPJumpFlag = 0;
	PWMdata->SVPWMType = 0;
	PWMdata->SHEReady = 0;
	PWMdata->MinPulseOK= 1 ;
	
}
/**********************��ʼ��PWMģʽ****************************************************************************************************************/
/*Name		:InitINVPWM_Mode
*Description:Ŀ���ǵ��г��ӻ����������������ʱ�����PWM��ֱ�ӽ�����Ӧ��PWMģʽ 
*Input		:*PWMdata�����PWM���ݽṹ��ָ��
			InvModulationFlag ����ģʽ
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void InitINVPWM_Mode(struct PWMdataStruct *PWMdata,short InvModulationFlag)
{
	if(InvModulationFlag==0)	// SVM
	{
		PWMdata->InvModulationFlag = 0;   
		PWMdata->T1PR = T1PRSVM;
		PWMdata->T1PRHalf = T1PRHalfSVM; 
		PWMdata->Ts = TsSVM;    
		PWMdata->SwitchSpeedDnOnce = 0;
		PWMdata->SwitchSpeedUpOnce = 0;
		if(PWMdata->Kv >= 0.3)  // SVMH
		{
			PWMdata->KvIncreFlag = 1;
			PWMdata->Change_SL = 1;  
			PWMdata->SVMModeSwitchUp = 0;
			PWMdata->SVMModeSwitchDn = 0;
		}
		else  // SVML
		{
			PWMdata->KvIncreFlag = 1;
			PWMdata->PWM4Cnt=1;
			PWMdata->Change_SL = 0;  // SVMH
			PWMdata->SVMModeSwitchUp = 0;
			PWMdata->SVMModeSwitchDn = 0;	
		}
	}
	else  // SHE
	{
		PWMdata->InvModulationFlag = 1;  
		PWMdata->T1PR = T1PRSHE;
		PWMdata->T1PRHalf = T1PRHalfSHE;   
		PWMdata->Ts = TsSHE;    
		PWMdata->SwitchSpeedDnOnce = 0;
		PWMdata->SwitchSpeedUpOnce = 1;
	}	
}

/***********************���ø�PWM��SRIO���ͽṹ���е�PWM������ʽ�Ĵ����ͱȽϼĴ���ֵ*******************************************************************/
/*Name		:setPWMBoardReg_SRIO
*Description:���ý�Ҫ���͸�PWM����SRIO�����ж�Ӧ�ļĴ��� ����Ҫ�Ƕ�����ʽ�Ĵ����ͱȽϼĴ���
*Input		:*PWMdata�����PWM���ݽṹ��ָ��
			 *TxPwmBoardSrioData ���͸�PWM����SRIO���ݽṹ��ָ��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void setPWMBoardReg_SRIO(struct PWMdataStruct *PWMdata,struct TX_PWM_SRIO_Struct *TxPwmBoardSrioData)
{ 
  short i=0; 
 
  if(PWMdata->Reg_UpDown==1)/*PWM board timer counter decrease down */  /* 20160706 */
  {
	PWMdata->TransWriteFlag = 0;
	
    TxPwmBoardSrioData->tx_liveCnt = TxPwmBoardSrioData->tx_liveCnt++;	 
   	TxPwmBoardSrioData->timerPeriodShadow = PWMdata->T1PR;	
   	
   	TxPwmBoardSrioData->ACQ_ZRO=PWMdata->ACQ_ZRO;/*ACQ_ZRO */
   	TxPwmBoardSrioData->ACQ_CAU=PWMdata->ACQ_CAU;/*ACQ_CAU */ 	
	
	TxPwmBoardSrioData->CMPAU1=PWMdata->CMPA[0];/*CMPAU_shadow[1] */	 
	TxPwmBoardSrioData->CMPAU2=PWMdata->CMPA[1];/*CMPAU_shadow[2] */	 
	TxPwmBoardSrioData->CMPAU3=PWMdata->CMPA[2];/*CMPAU_shadow[3] */	 
	TxPwmBoardSrioData->CMPAU4=PWMdata->CMPA[3];/*CMPAU_shadow[4] */	 
	TxPwmBoardSrioData->CMPAU5=PWMdata->CMPA[4];/*CMPAU_shadow[5] */	 
	TxPwmBoardSrioData->CMPAU6=PWMdata->CMPA[5];/*CMPAU_shadow[6] */
	
	TxPwmBoardSrioData->ANPC_type_c=0;//Inv_type_a;
  }
   else/*PWM board timer counter increase up */
  {
    PWMdata->TransWriteFlag = 1;
    TxPwmBoardSrioData->tx_liveCnt = TxPwmBoardSrioData->tx_liveCnt++;	 
   	TxPwmBoardSrioData->timerPeriodShadow = PWMdata->T1PR;		
   	TxPwmBoardSrioData->TimerEnable =1;		
   	TxPwmBoardSrioData->clrFaultCmd =0;		
   	
   	/*PWM special registers setting*/		
 
   	TxPwmBoardSrioData->ACQ_PRD=PWMdata->ACQ_PRD;/*ACQ_PRD */
   	TxPwmBoardSrioData->ACQ_CAD=PWMdata->ACQ_CAD;/*ACQ_CAD */   	
   
   	TxPwmBoardSrioData->CMPAD1=PWMdata->CMPA[0];/*CMPAD_shadow[1] */   	 
   	TxPwmBoardSrioData->CMPAD2=PWMdata->CMPA[1];/*CMPAD_shadow[2] */    
   	TxPwmBoardSrioData->CMPAD3=PWMdata->CMPA[2];/*CMPAD_shadow[3] */   	 
   	TxPwmBoardSrioData->CMPAD4=PWMdata->CMPA[3];/*CMPAD_shadow[4] */   	 
   	TxPwmBoardSrioData->CMPAD5=PWMdata->CMPA[4];/*CMPAD_shadow[5] */   	 
   	TxPwmBoardSrioData->CMPAD6=PWMdata->CMPA[5];/*CMPAD_shadow[6] */

  }
	
}
/***********************��λ��PWM��SRIO���ͽṹ���е�PWM������ʽ�Ĵ����ͱȽϼĴ���ֵ********************/
/*Name		:resetPwmBoardReg_SRIO
*Description:��λPWM���ķ���SRIO�����ж�Ӧ�ļĴ��� ����Ҫ�Ƕ�����ʽ�Ĵ����ͱȽϼĴ���
*Input		:*PWMdata�����PWM���ݽṹ��ָ��
			 *TxPwmBoardSrioData ���͸�PWM����SRIO���ݽṹ��ָ��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void resetPwmBoardReg_SRIO( struct TX_PWM_SRIO_Struct *TxPwmBoardSrioData, struct PWMdataStruct *PWMdata)
{
    
    PWMdata->NeuVoltCtl_sum=0;
	TxPwmBoardSrioData->ACQ_ZRO=0xaaaa;/*ACQ_ZRO */
  	TxPwmBoardSrioData->ACQ_CAU=0xaaaa;/*ACQ_CAU */ 	
	
	TxPwmBoardSrioData->CMPAU1=0;/*CMPAU_shadow[1] */	 
	TxPwmBoardSrioData->CMPAU2=0;/*CMPAU_shadow[2] */	 
	TxPwmBoardSrioData->CMPAU3=0;/*CMPAU_shadow[3] */	 
	TxPwmBoardSrioData->CMPAU4=0;/*CMPAU_shadow[4] */	 
	TxPwmBoardSrioData->CMPAU5=0;/*CMPAU_shadow[5] */	 
	TxPwmBoardSrioData->CMPAU6=0;/*CMPAU_shadow[6] */
	
	TxPwmBoardSrioData->ACQ_PRD=0xaaaa;/*ACQ_PRD */
   	TxPwmBoardSrioData->ACQ_CAD=0xaaaa;/*ACQ_CAD */	
   	
   
   	TxPwmBoardSrioData->CMPAD1=0;/*CMPAD_shadow[1] */   	 
   	TxPwmBoardSrioData->CMPAD2=0;/*CMPAD_shadow[2] */    
   	TxPwmBoardSrioData->CMPAD3=0;/*CMPAD_shadow[3] */   	 
   	TxPwmBoardSrioData->CMPAD4=0;/*CMPAD_shadow[4] */   	 
   	TxPwmBoardSrioData->CMPAD5=0;/*CMPAD_shadow[5] */   	 
   	TxPwmBoardSrioData->CMPAD6=0;/*CMPAD_shadow[6] */
    PWMdata->NeuVoltCtl_sum=0;
}	 
	 
/*********************************************************************/
/*Name		:enable_AllPWMOut
*Description:ʹ������PWM����PWM�����ͨ������ʱ�Ӱ�ķ���SRIO�����е�PWMʹ�ܣ���ʱ�Ӱ���P2 bus�е�PWM_EN1����
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void enable_AllPWMOut(void)
{
	txClkSrioData.PwmEnable=1;/*pwm enable */
}
/*********************************************************************/
/*Name		:disable_AllPWMOut
*Description:��ֹ����PWM����PWM�����ͨ������ʱ�Ӱ�ķ���SRIO�����е�PWMʹ�ܣ���ʱ�Ӱ���P2 bus�е�PWM_EN1����
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void disable_AllPWMOut(void)
{
	txClkSrioData.PwmEnable=0;/*pwm enable */
}
/*********************************************************************/
/*Name		:enable_rec1PWM
*Description:ʹ������1PWM����PWM���
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void enable_rec1PWM(void)
{
	txRecSrioData1.PwmEnable=1;/*pwm enable */	   
}
/*********************************************************************/
/*Name		:disable_rec1PWM
*Description:��ֹ����1PWM����PWM���
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void disable_rec1PWM(void)
{
	txRecSrioData1.PwmEnable=0;/*pwm disable */	   
}
/*********************************************************************/
/*Name		:enable_rec2PWM
*Description:ʹ������2PWM����PWM���
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void enable_rec2PWM(void)
{	txRecSrioData2.PwmEnable=1;/*pwm enable */	   
}
/*********************************************************************/
/*Name		:disable_rec2PWM
*Description:��ֹ����2PWM����PWM���
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void disable_rec2PWM(void)
{	txRecSrioData2.PwmEnable=0;/*pwm disable */	   
}
/*********************************************************************/
/*Name		:enable_inv1PWM
*Description:ʹ�����1PWM����PWM���
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void enable_inv1PWM(void)
{
	txInvSrioData1.PwmEnable=1;/*pwm enable */	   
}
/*********************************************************************/
/*Name		:disable_inv1PWM
*Description:��ֹ���1PWM����PWM���
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void disable_inv1PWM(void)
{
	txInvSrioData1.PwmEnable=0;/*pwm disable */	   
}
/*********************************************************************/
/*Name		:enable_inv2PWM
*Description:ʹ�����2PWM����PWM���
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void enable_inv2PWM(void)
{	txInvSrioData2.PwmEnable=1;/*pwm enable */	   
}
/*********************************************************************/
/*Name		:disable_inv2PWM
*Description:��ֹ���2PWM����PWM���
*Input		:��
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void disable_inv2PWM(void)
{	txInvSrioData2.PwmEnable=0;/*pwm disable */	   
}
/****************************����PWM�㷨���ýӿں���**********************************************************************************************************/
/*Name		:recPWM_EntranceFunc
*Description:��PWM�㷨���ýӿں�����1��������
*Input		:index ��ʱ������������0~11
			*PWMdata PWM���ݽṹ��ָ��		
*Output		:*TxPwmBoardSrioData PWM SRIO���ͽṹ��ָ��
*Return		:��
*History	:2018,by wxx
*/
void recPWM_EntranceFunc(unsigned short index,struct PWMdataStruct *PWMdata,struct TX_PWM_SRIO_Struct *TxPwmBoardSrioData)
{
    
	/*����PWM���ÿ�ʼ*/	 
	
	if(index>= 6)/*6~11,��ʱ�����ǲ��½���*/
	{
		PWMdata->Reg_UpDown = 1;
		PWMdata->IntFlag = 0;
					
	}
	else/*0~5,��ʱ�����ǲ�������*/
	{
		PWMdata->Reg_UpDown = 0;		
		PWMdata->IntFlag = 1;						
	}
	SVPWM_VSR(PWMdata); 		
	setPWMBoardReg_SRIO(PWMdata,TxPwmBoardSrioData); // 20160720
	PWMdata->KvPre =PWMdata->Kv;
	
	/*����PWM���ý���*/	 		
}

/******************************���PWM�㷨���ýӿں���********************************************************************************************************/
/*Name		:invPWM_EntranceFunc
*Description:���PWM�㷨���ýӿں�����1������
*Input		:index ��ʱ������������0~11
			*PWMdata PWM���ݽṹ��ָ��		
*Output		:*TxPwmBoardSrioData PWM SRIO���ͽṹ��ָ��
*Return		:��
*History	:2018,by wxx
*/
void invPWM_EntranceFunc(unsigned short index,struct PWMdataStruct *PWMdata,struct TX_PWM_SRIO_Struct *TxPwmBoardSrioData)
{
    /*����PWM���ÿ�ʼ*/
	/* ���PWM����Ƶ���Զ��л�SVM��SHE */
	/*InvModulationFlag=0: SVPWM,InvModulationFlag=1:SHEPWM*/
	if((index == 6) && (PWMdata->InvModulationFlag == 0)) 
		{PWMdata->IntFlag = 0;PWMdata->Reg_UpDown = 1;InvIntFlag = 1;
		}
	else if((index == 0) && (PWMdata->InvModulationFlag == 0))
		{PWMdata->IntFlag = 1;PWMdata->Reg_UpDown = 0;InvIntFlag = 1;
		 }
	else if((index %2 == 1) && (PWMdata->InvModulationFlag == 1))  
		{PWMdata->IntFlag = 0;PWMdata->Reg_UpDown = 1;InvIntFlag = 1;
		 }
	else if((index %2 == 0) && (PWMdata->InvModulationFlag == 1))
		{PWMdata->IntFlag = 1;PWMdata->Reg_UpDown = 0;InvIntFlag = 1;
		 }	
	invPwmSwitchOncePrinciple(PWMdata/*&PWMdata_INV1*/); 	   /*���PWM�л�ԭ��*/ 
	InvPwmModeModulation(PWMdata);				   /*���PWM���ƺ���	*/
	setPWMBoardReg_SRIO(PWMdata,TxPwmBoardSrioData);   /*���ø�PWM��SRIO���ͽṹ���е�PWM������ʽ�Ĵ����ͱȽϼĴ���ֵ*/
	PWMdata->KvPre =PWMdata->Kv;
	InvIntFlag = 0; 	
	 
	 	 		
}
/********************************���PWM�л�ԭ��*******************************************************************/
/*Name		:invPwmSwitchOncePrinciple
*Description:���PWM�л�ԭ��1������
*Input		:*PWMdata PWM���ݽṹ��ָ��		
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void invPwmSwitchOncePrinciple(struct PWMdataStruct *PWMdata)
{
//	if(PWMdata->vvvf1IncreFlag == 1)
//	{
//		PWMdata->SwitchSpeedDnOnce = 0;
//		if(PWMdata->SwitchSpeedUpOnce == 0)
//		{
//			PWMdata->SwitchSpeedUpOnce = 0;
//		}
//	}
//	else if(PWMdata->vvvf1IncreFlag == 0)
//	{
//		PWMdata->SwitchSpeedUpOnce = 0;
//		if(PWMdata->SwitchSpeedDnOnce == 0)
//		{
//			PWMdata->SwitchSpeedDnOnce = 0;
//		}
//	}
//	if(PWMdata->Kv > PWMdata->KvPre)
//	{
//		if(KvIncre1Decre0 == 0)
//			KvCnt = 0;
//		KvIncre1Decre0 = 1;
//		KvCnt++;
//		if(KvCnt >= 1401)
//			KvCnt = 1401;
//		if(KvCnt>=1400)
//			PWMdata->KvIncreFlag = 1;
//	}
//	else if(PWMdata->Kv < PWMdata->KvPre)
//	{
//		if(KvIncre1Decre0 == 1)
//			KvCnt = 0;
//		KvIncre1Decre0 = 0;
//		KvCnt++;
//		if(KvCnt >= 1401)
//			KvCnt = 1401;
//		if(KvCnt>=1400)
//			PWMdata->KvIncreFlag = 0;
//	}
//	else
//	{
//		KvCnt++;
//		if(KvCnt >= 1401)
//			KvCnt = 1401;
//		if(KvIncre1Decre0 == 1 && KvCnt>=1400)
//			PWMdata->KvIncreFlag = 1;
//		else if(KvIncre1Decre0 == 0 && KvCnt>=1400)
//			PWMdata->KvIncreFlag = 0;	
//	}
//	
//	if(PWMdata->KvIncreFlag == 1){
//		PWMdata->SVMModeSwitchDn = 0;
//	if(PWMdata->SVMModeSwitchUp == 0)
//		PWMdata->SVMModeSwitchUp = 0;
//	}
//	else if(PWMdata->KvIncreFlag == 0){
//		PWMdata->SVMModeSwitchUp = 0;
//	if(PWMdata->SVMModeSwitchDn == 0)
//		PWMdata->SVMModeSwitchDn = 0;		
//	}
	
	
//	kv1_filter_pre=kv1_filter;
//	kv1_filter=2*PI*10*(PWMdata->Kv-kv1_filter)*0.000138+kv1_filter;
	
	Kvfilter.filter_input = PWMdata->Kv;
	filter_function(&Kvfilter);
	
        KvCnt++;
		if(KvCnt >= 14005)
			KvCnt = 0;
	if (KvCnt==5) 
		 {
			 PWMdata->KvPre1=Kvfilter.filter_out;//kv1_filter;//PWMdata->Kv;
	    } 
	if (KvCnt==2800) 
		 {
			 PWMdata->KvPre2=Kvfilter.filter_out;//kv1_filter;//PWMdata->Kv;
	    } 
	if (KvCnt==5600) 
		 {
			 PWMdata->KvPre3=Kvfilter.filter_out;//kv1_filter;//PWMdata->Kv;
	    } 
	if (KvCnt==8400) 
		 {
			 PWMdata->Kv1=Kvfilter.filter_out;//kv1_filter;//PWMdata->Kv;
	    } 
	if (KvCnt==11200) 
		 {
			 PWMdata->Kv2=Kvfilter.filter_out;//kv1_filter;//PWMdata->Kv;
	    } 
	if (KvCnt==14000)
	{
		PWMdata->Kv3=Kvfilter.filter_out;//kv1_filter;//PWMdata->Kv;
		PWMdata->Kvave=PWMdata->Kv3;//(PWMdata->Kv1+PWMdata->Kv2+PWMdata->Kv3)/3.;
		PWMdata->KvPreave=PWMdata->KvPre1;//(PWMdata->KvPre+PWMdata->KvPre2+PWMdata->KvPre3)/3.;
     if(PWMdata->Kvave > PWMdata->KvPreave)
		 PWMdata->KvIncreFlag = 1;
	 else if (PWMdata->Kvave < PWMdata->KvPreave)
		 PWMdata->KvIncreFlag = 0;
	 else if (PWMdata->Kvave == PWMdata->KvPreave)
		 PWMdata->KvIncreFlag=PWMdata->KvIncreFlag;
	}	
	 
}

/************************************���PWM���ú����������������SVPWM����SHEPWM**************************************************************************************************/
/*Name		:InvPwmModeModulation
*Description:���PWM�����㷨�������������SVPWM����SHEPWM
*Input		:*PWMdata PWM���ݽṹ��ָ��		
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void InvPwmModeModulation(struct PWMdataStruct *PWMdata) 
{
	if((PWMdata->SHEShutFlag)!=0)   /*  SHEShutFlag changed when fault evaluated */
		{}
		else	  	
		{
			vvvf1Pre = PWMdata->vvvf1;
		}
		if(EnablePWM==1)
		{
			
			if (PWMdata->vvvf1IncreFlag==1)
			{
				if ((PWMdata->Change_SL !=2)&&(PWMdata->vvvf1 < f5point))
				{
					changestep_gaozhan_up=0;
				}
				else if ((PWMdata->Change_SL !=2)&&(PWMdata->vvvf1 >= f5point))
				{
				if (changestep_SST_Flag==1)	
					changestep_gaozhan_up=1;
				else if (changestep_SST_Flag==0)	
					changestep_gaozhan_up=0;
				}
				else if (PWMdata->Change_SL ==2)
				{
					changestep_gaozhan_up=1;
				}
			}
			else if (PWMdata->vvvf1IncreFlag==0)
			{
				if ((PWMdata->Change_SL ==2)&&(PWMdata->vvvf1 >= f5pointB))
				{
					changestep_gaozhan_dn=0;
				}
				else if ((PWMdata->Change_SL ==2)&&(PWMdata->vvvf1 <f5pointB))
				{
				if (changestep_SST_Flag==1)	
					changestep_gaozhan_dn=1;
				else if (changestep_SST_Flag==0)	
					changestep_gaozhan_dn=0;
				}
				else if (PWMdata->Change_SL !=2)
				{
					changestep_gaozhan_dn=1;
				}
			}
					
			
			// (1)Ƶ��������
			if((((PWMdata->vvvf1) < f5point)  && (changestep_gaozhan_up==1)&& ((PWMdata->vvvf1IncreFlag)==1)&& (PWMdata->InvModulationFlag) == 1) ) //SHE-SHE ��Ӧ���е�SHEƵ���½�����־λû�иı����������������У�Ӧ����SHE
			{	
				PWMRoute=1;
				ChangeTsSHE(PWMdata);
				SHEPWM(PWMdata);
				PWMdata->Change_SL = 2;
				//TransBoardRegSet(PWMdata);  // 20160720
			}			
			else if((((PWMdata->vvvf1) < f5point) && (changestep_gaozhan_up==0) && ((PWMdata->vvvf1IncreFlag)==1)&& (PWMdata->InvModulationFlag) == 0) ) //SVM�Ա���
			{
				PWMRoute=2;
			if(interrupt_index == 0 || interrupt_index == 6)  // SVM only when index == 0 or 6
					{
						ChangeTsSVM(PWMdata);
						SVPWM(PWMdata);		
						//TransBoardRegSet(PWMdata);  // 20160720
					}
			}
			else if((((PWMdata->vvvf1) >= f5point) && (changestep_gaozhan_up==1)&&((PWMdata->vvvf1IncreFlag)==1) && (PWMdata->InvModulationFlag) == 0)) //SVM��SHE�л�			
			{
				
					if(interrupt_index == 11)  // �������ز������½�Ϊ0ʱ�ж��ܷ��л� �Ըı��ز�����
					{
						CheckSwich2SHE(PWMdata);//�ж�SVM��SHE�л��Ƿ������С���� SHEReady = 1������  ��ǰSVM�½��ض�������һSHE�����ض����Ƿ�������С����		
						PWMdata->ChangeSHENPJumpTest = 0;//ChangeSHENPJumpTest=0ʹ��NP_JUMP_AVOIDy2(PWMdata)�������ChangeSHENPJumpFlag��־λ
						NP_JUMP_AVOIDy2(PWMdata);//�ж�SVM��SHE�л��Ƿ����NP����  ChangeSHENPJumpFlag = 1����NP����
						PWMdata->ChangeSHENPJumpTest = 1;//ChangeSHENPJumpTest=1��־�Ѿ�������NP_JUMP_AVOIDy2(PWMdata)����

						if((PWMdata->SHEReady)==1 && (PWMdata->ChangeSHENPJumpFlag) != 1) 
						{
							ChangeTsSHE(PWMdata); 
							SHEPWM(PWMdata);
							PWMdata->Change_SL = 2;
							//TransBoardRegSet(PWMdata);  // 20160720
							PWMRoute=3;
						}
					}
					else if(interrupt_index == 0 || interrupt_index == 6)  // SVM
					{
						ChangeTsSVM(PWMdata);
						SVPWM(PWMdata);
					 	//TransBoardRegSet(PWMdata); // 20160720
						PWMRoute=4;
					}
			} 
			else if((((PWMdata->vvvf1) >= f5point) && (changestep_gaozhan_up==0)&&((PWMdata->vvvf1IncreFlag)==1) && (PWMdata->InvModulationFlag) == 0)) //SVM��SHE�л�			
			{
				
				if(interrupt_index == 0 || interrupt_index == 6)  // SVM only when index == 0 or 6
				{
					ChangeTsSVM(PWMdata);
					SVPWM(PWMdata);		
					//TransBoardRegSet(PWMdata);  // 20160720
				}
			} 
			else if((((PWMdata->vvvf1) >= f5point) && (changestep_gaozhan_up==1) && ((PWMdata->vvvf1IncreFlag)==1) && (PWMdata->InvModulationFlag) == 1)) //SHE�Ա���			
			{
				PWMRoute=5;
				ChangeTsSHE(PWMdata);
				SHEPWM(PWMdata);
				PWMdata->Change_SL = 2;
				//TransBoardRegSet(PWMdata);  // 20160720
			}
			// (2)Ƶ���½���
		else if((((PWMdata->vvvf1) < f5pointB)  && (changestep_gaozhan_dn==1)&& ((PWMdata->vvvf1IncreFlag)==0)&& (PWMdata->InvModulationFlag) == 1) ) //SHE��SVM�л�
			{	
				
				if(interrupt_index == 11)  // SVM��SHE�Ļ����л�ֻ�������ز�����Ϊ0ʱ���У��Ըı��ز�����
					{
							SHEMinPulseCheck(PWMdata); //�ж�SHE�����һ���������ʱ���Ƿ�������С����MinPulseOK=1��־����С����	
							PWMdata->Ts = TsSVM;	
							PWMdata->T1PR = T1PRSVM;  
							PWMdata->T1PRHalf = T1PRHalfSVM; 
							VFGamaStore = PWMdata->VFGama;
							TotalNumStore = PWMdata->Gama_TotalNum;  // 20160713
							Change_SL_Store = PWMdata->Change_SL;
							PWMdata->ChangeSHENPJumpTest = 0;//ChangeSHENPJumpTest=0ʹ��NP_JUMP_AVOIDy2(PWMdata)�������ChangeSHENPJumpFlag��־λ
							SVPWM(PWMdata);//��һ�ε���SVPWM������Ŀ���ǻ����һSVM�����ض���ZRO��CAU��change_slֵ����
						    PWMdata->ChangeSHENPJumpTest = 1;
							PWMdata->Change_SL = Change_SL_Store;
							PWMdata->Gama_TotalNum = TotalNumStore;  // 20160713
							PWMdata->VFGama = VFGamaStore;	
					       PWMdata->ChangeSHENPJumpTest = 0;
							NP_JUMP_AVOIDy2(PWMdata);//�ж�SHE��SVPWM�л��Ƿ���NP����
							PWMdata->ChangeSHENPJumpTest = 1;//ChangeSHENPJumpTest=1������SVPWM������change_slֵ�ı�
						
							
							if((PWMdata->MinPulseOK)==1 && (PWMdata->ChangeSHENPJumpFlag) != 1)  /*�ϴ�SHE--������С���� */
							{
								ChangeTsSVM(PWMdata);
								SVPWM(PWMdata);
								//TransBoardRegSet(PWMdata);  // 20160720
								PWMRoute=6;
							}
							else/* �ϴε�SHE--������С���� */
							{
								ChangeTsSHE(PWMdata);
								SHEPWM(PWMdata);
								PWMdata->Change_SL = 2;
								PWMdata->ChangeSHENPJumpFlag = 0;
								//TransBoardRegSet(PWMdata); // 20160720
								PWMRoute=7;
							}
					}
					else
					{
							ChangeTsSHE(PWMdata);
							SHEPWM(PWMdata);
							PWMdata->Change_SL = 2;
							//TransBoardRegSet(PWMdata);  // 20160720
							PWMRoute=8;
					}
			}	
		else if((((PWMdata->vvvf1) < f5pointB)  && (changestep_gaozhan_dn==0)&& ((PWMdata->vvvf1IncreFlag)==0)&& (PWMdata->InvModulationFlag) == 1) ) //SHE��SVM�л�
		{	
			ChangeTsSHE(PWMdata);
			SHEPWM(PWMdata);
			PWMdata->Change_SL = 2;
			PWMdata->ChangeSHENPJumpFlag = 0;
			//TransBoardRegSet(PWMdata); // 20160720
							
		}		
			else if((((PWMdata->vvvf1) < f5pointB)&& (changestep_gaozhan_dn==1) && ((PWMdata->vvvf1IncreFlag)==0)&& (PWMdata->InvModulationFlag) == 0) ) //SVM�Ա���
			{
				PWMRoute=9;
			if(interrupt_index == 0 || interrupt_index == 6)  // SVM only when index == 0 or 6
					{
						ChangeTsSVM(PWMdata);
						SVPWM(PWMdata);		
						//TransBoardRegSet(PWMdata);  // 20160720
					}
			}
			else if((((PWMdata->vvvf1) >= f5pointB) && (changestep_gaozhan_dn==0)&& ((PWMdata->vvvf1IncreFlag)==0) && (PWMdata->InvModulationFlag) == 1)) //SHE�Ա��� 
			{
				PWMRoute=10;
				ChangeTsSHE(PWMdata);
				SHEPWM(PWMdata);
				PWMdata->Change_SL = 2;
				//TransBoardRegSet(PWMdata);  // 20160720	
			} 
			else if((((PWMdata->vvvf1) >= f5pointB)&& (changestep_gaozhan_dn==1) && ((PWMdata->vvvf1IncreFlag)==0) && (PWMdata->InvModulationFlag) == 0)) //SVM�Ա���	 ��ӦSHE���е�SVMƵ������������־λδ�ı�������Ӧ����SVM				
			{
				PWMRoute=11;
				if(interrupt_index == 0 || interrupt_index == 6)  // SVM only when index == 0 or 6
					{
						ChangeTsSVM(PWMdata);
						SVPWM(PWMdata);		
						//TransBoardRegSet(PWMdata);  // 20160720
					}
			}
		}  // Enable = 1
		else  
		{
			if ((PWMdata->Reg_UpDown) == ET_CTR_PRD)
			{
				ZRO[0] = 2;
				CAU[0] = 2;
				ZRO[2] = 2;
				CAU[2] = 2;
				ZRO[4] = 2;
				CAU[4] = 2;
				
				ZRO[1] = 1;
				CAU[1] = 1;
				ZRO[3] = 1;
				CAU[3] = 1;
				ZRO[5] = 1;
				CAU[5] = 1;	
			}
			else
			{
				PRD[0] = 2;
				CAD[0] = 2;
				PRD[2] = 2;
				CAD[2] = 2;
				PRD[4] = 2;
				CAD[4] = 2;
				
				PRD[1] = 1;
				CAD[1] = 1;
				PRD[3] = 1;
				CAD[3] = 1;
				PRD[5] = 1;
				CAD[5] = 1;			
			}
			PWMRoute=15;
		}
		
	  NP_JUMP_AVOIDy2(PWMdata);    /*���ǻᵼ��PWMֹͣ���� 20150603 */
		
		epwm1cmpaPre = epwm1cmpa;
		epwm2cmpaPre = epwm2cmpa;
		epwm3cmpaPre = epwm3cmpa;
		epwm4cmpaPre = epwm4cmpa;
		epwm5cmpaPre = epwm5cmpa;
		epwm6cmpaPre = epwm6cmpa;
		
		EnablePWMPre = EnablePWM; 
}

/*************************************SVPWM���ú�����ѡ��SVPWML����SVPWMH*************************************************************************************************/
/*Name		:SVPWM
*Description:SVPWM���ú�����ѡ��SVPWML����SVPWMH
*Input		:*PWMdata PWM���ݽṹ��ָ��		
*Output		:��
*Return		:��
*History	:2018,by wxx
*/
void SVPWM(struct PWMdataStruct *PWMdata) 
{
	PWMdata->Gama_TotalNum = 0;	
	SHE2SVFLAG = 0;
	testflag2 = 0;
	
	Kv = PWMdata->Kv;
	
	// (1) ���Ʊ��������߼�
		if(Kv<0.4 && PWMdata->KvIncreFlag==1 && PWMdata->Change_SL == 0) //SVML-SVML
		{
			svmtest=1;
		    SVPWML(PWMdata);
			PWMdata->Change_SL = 0;	
		}
		else if(Kv<0.4 && PWMdata->KvIncreFlag==1 && PWMdata->Change_SL == 2) // SHE->SVML
		{
			svmtest=2;
			PWMdata->PWM4Cnt=1;
			SVPWML(PWMdata);
			if(PWMdata->ChangeSHENPJumpTest != 0)  
	//SHE��SVM�л�ʱ����һ�ε���SVPWM������ChangeSHENPJumpTest=0����Change_SL��Ȼ����2��Ŀ����Ϊ�˵õ��л�����һSVM�Ķ���ZRO,CAU��
	//����NP�������С��������ChangeSHENPJumpTest=1���ٵ���SVPWM��������Change_SL=0
			{
			PWMdata->Change_SL = 0;				
			}
			else
			{
			PWMdata->Change_SL = PWMdata->Change_SL;
			}
		}
		else if(Kv<0.4 && PWMdata->KvIncreFlag==1 && PWMdata->Change_SL == 1) // SVMH->SVMH  ��ӦL��H�л��ɹ������Ʊȿ�ʼ�½�����־λδ���ü��ı䣬��ʱΪ��ֹ�����У�Ҫ����SVMH
		{
			svmtest=3;
			SVPWMH(PWMdata);
			PWMdata->Change_SL=1;
		}
		else if(Kv>=0.4 && PWMdata->KvIncreFlag==1 && PWMdata->Change_SL == 0) // SVML->SVMH
		 {
			svmtest=4;		
				if (((PWMdata->PWM4Cnt)==0)||((PWMdata->PWM4Cnt)==2))	      	
				{	
					SVPWML(PWMdata);
					PWMdata->Change_SL=0;
				}
				else
				{	
					PWMdata->ChangeSHENPJumpTest = 0;
					SVPWMH(PWMdata);
					PWMdata->ChangeSHENPJumpTest = 1;
					
					PWMdata->ChangeSHENPJumpTest = 0;
					NP_JUMP_AVOIDy2(PWMdata);
					PWMdata->ChangeSHENPJumpTest = 1;
					
					if((PWMdata->ChangeSHENPJumpFlag)!=1)
					{
						SVPWMH(PWMdata);
						PWMdata->Change_SL=1;
					}
					else
					{
						SVPWML(PWMdata);
						PWMdata->Change_SL = 0;
						PWMdata->ChangeSHENPJumpFlag=0;
					}
				}
		}
		else if(Kv>=0.4 && PWMdata->KvIncreFlag==1 && PWMdata->Change_SL == 2) // SHE->SVMH
		{
			     svmtest=5;		
				 SVPWMH(PWMdata);
				if(PWMdata->ChangeSHENPJumpTest != 0)  
				{
				PWMdata->Change_SL = 1;				
				}
				else
				{
				PWMdata->Change_SL = PWMdata->Change_SL;
				}				
		}
		else if(Kv>=0.4 && PWMdata->KvIncreFlag==1 && PWMdata->Change_SL == 1) // SVMH->SVMH
		{
			     svmtest=6;		
				SVPWMH(PWMdata);
				PWMdata->Change_SL=1;
		}
		
		// (2) ���Ʊ��½����߼�
		else if(Kv<0.2 && PWMdata->KvIncreFlag==0 && PWMdata->Change_SL == 0) //SVML-SVML
		{
			svmtest=7;
		    SVPWML(PWMdata);
			PWMdata->Change_SL = 0;	
		}
		else if(Kv<0.2 && PWMdata->KvIncreFlag==0 && PWMdata->Change_SL == 2) // SHE->SVML
		{
			svmtest=8;
			PWMdata->PWM4Cnt=1;
			SVPWML(PWMdata);
			if(PWMdata->ChangeSHENPJumpTest != 0)  
			{
			PWMdata->Change_SL = 0;				
			}
			else
			{
			PWMdata->Change_SL = PWMdata->Change_SL;
			}
		}
		else if(Kv<0.2 && PWMdata->KvIncreFlag==0 && PWMdata->Change_SL == 1) // SVMH->SVML  
		{
			    svmtest=9;
				if((PWMdata->IntFlag)==0)
				{
					PWMdata->ChangeSHENPJumpTest = 0;
					PWMdata->PWM4Cnt=1;
					SVPWML(PWMdata);
					PWMdata->ChangeSHENPJumpTest = 1;
					
					PWMdata->ChangeSHENPJumpTest = 0;
					NP_JUMP_AVOIDy2(PWMdata);
					PWMdata->ChangeSHENPJumpTest = 1;
					
					if((PWMdata->ChangeSHENPJumpFlag)!=1)
					{
						PWMdata->PWM4Cnt=1;
						SVPWML(PWMdata);
						PWMdata->Change_SL=0;
					}
					else
					{SVPWMH(PWMdata);
					PWMdata->Change_SL = 1;
					PWMdata->ChangeSHENPJumpFlag=0;
					}
				}
				else  
				{
						SVPWMH(PWMdata);
						PWMdata->Change_SL = 1;		
				}
		}
		else if(Kv>=0.2 && PWMdata->KvIncreFlag==0 && PWMdata->Change_SL == 0) // SVML->SVML  ��ӦH��L���й�ȥ�����Ʊ���������־λδ���ü��ı���������ʱӦ�ñ���SVML
		{
			svmtest=10;		
			 SVPWML(PWMdata);
			PWMdata->Change_SL = 0;		
		}
		else if(Kv>=0.2 && PWMdata->KvIncreFlag==0 && PWMdata->Change_SL == 2) // SHE->SVMH
		{
			     svmtest=11;		
				 SVPWMH(PWMdata);
				if(PWMdata->ChangeSHENPJumpTest != 0)  
				{
				PWMdata->Change_SL = 1;				
				}
				else
				{
				PWMdata->Change_SL = PWMdata->Change_SL;
				}				
		}
		else if(Kv>=0.2 && PWMdata->KvIncreFlag==0 && PWMdata->Change_SL == 1) // SVMH->SVMH
		{
			     svmtest=12;		
				SVPWMH(PWMdata);
				PWMdata->Change_SL=1;
		}
}
/**************����4��Ϊ�ɵĺ���������******************************************/

/*********************************PWM�㷨���ýӿں���*****************************************************************************************************/
/*PWM�㷨���ýӿں�����2��������2������
 * 
 */
void PWM_EntranceFunc(struct PWMdataStruct *PWMdata1,struct PWMdataStruct *PWMdata2,\
		struct PWMdataStruct *PWMdata3,struct PWMdataStruct *PWMdata4,int FreqBasedSw1_AutoSW0,\
		unsigned short index)
{
    /*����PWM���ÿ�ʼ*/	
	GamaInterpolationNew(PWMdata1,PWMdata2,index);  //  ����VF�Ƕ���ע��

	/* ���PWM����Ƶ���Զ��л�SVM��SHE */
	if((index == 6) && (PWMdata1->InvModulationFlag == 0)) 
		{PWMdata1->IntFlag = 0;PWMdata1->Reg_UpDown = 1;InvIntFlag = 1;
		}
	else if((index == 0) && (PWMdata1->InvModulationFlag == 0))
		{PWMdata1->IntFlag = 1;PWMdata1->Reg_UpDown = 0;InvIntFlag = 1;
		 }
	else if((index %2 == 1) && (PWMdata1->InvModulationFlag == 1))  
		{PWMdata1->IntFlag = 0;PWMdata1->Reg_UpDown = 1;InvIntFlag = 1;
		 }
	else if((index %2 == 0) && (PWMdata1->InvModulationFlag == 1))
		{PWMdata1->IntFlag = 1;PWMdata1->Reg_UpDown = 0;InvIntFlag = 1;
		 }

	if((index == 6) && (PWMdata2->InvModulationFlag == 0)) 
		{
		 PWMdata2->IntFlag = 0;PWMdata2->Reg_UpDown = 1;InvIntFlag2 = 1;}
	else if((index == 0) && (PWMdata2->InvModulationFlag == 0))
		{
		 PWMdata2->IntFlag = 1;PWMdata2->Reg_UpDown = 0;InvIntFlag2 = 1;}
	else if((index %2 == 1) && (PWMdata2->InvModulationFlag == 1))  
		{
		 PWMdata2->IntFlag = 0;PWMdata2->Reg_UpDown = 1;InvIntFlag2 = 1;}
	else if((index %2 == 0) && (PWMdata2->InvModulationFlag == 1))
		{
		 PWMdata2->IntFlag = 1;PWMdata2->Reg_UpDown = 0;InvIntFlag2 = 1;}	 	 
	
	SwitchOncePrinciple(&PWMdata_INV1,&PWMdata_INV2);  // 20160622		
 
	if(selfCheck.bit.invBoard1OK==1) 
	{
		InvPwmModeModulation(PWMdata1);
		PWMBoardRegSet(PWMdata1);  // 20160718  board Regs updated only when modulation occurs, modification proved
	}
	InvIntFlag = 0; 
	 
	if(selfCheck.bit.invBoard2OK==1) 
	{
		InvPwmModeModulation(PWMdata2);
		PWMBoardRegSet(PWMdata2);
	}
	 
	/* ������Ƶ���Զ��л�SVM��SHE ���� */
	 
	/*����PWM���ÿ�ʼ*/	 
	
	if(index>= 6)/*6~11,��ʱ�����ǲ��½���*/
	{
		PWMdata3->Reg_UpDown = 1;
		PWMdata4->Reg_UpDown = 1;
		PWMdata3->IntFlag = 0;
		PWMdata4->IntFlag = 0;					
	}
	else/*0~5,��ʱ�����ǲ�������*/
	{
		PWMdata3->Reg_UpDown = 0;
		PWMdata4->Reg_UpDown = 0;
		PWMdata3->IntFlag = 1;
		PWMdata4->IntFlag = 1;					
	}

	if(selfCheck.bit.recBoard1OK==1) 
	{
		SVPWM_VSR(PWMdata3);
		PWMBoardRegSet(PWMdata3); // 20160720
	}
	if(selfCheck.bit.recBoard2OK==1) 
	{
		SVPWM_VSR(PWMdata4);
		PWMBoardRegSet(PWMdata4);// 20160720
	}
	/*����PWM���ý���*/	 		
}

/**************************************************************************************************************************************/
void SwitchOncePrinciple(struct PWMdataStruct *PWMdata1,struct PWMdataStruct *PWMdata2)
{
	if(PWMdata1->vvvf1IncreFlag == 1){
		PWMdata1->SwitchSpeedDnOnce = 0;
		if(PWMdata1->SwitchSpeedUpOnce == 0){
			PWMdata1->SwitchSpeedUpOnce = 0;}
	}
	else if(PWMdata1->vvvf1IncreFlag == 0){
		PWMdata1->SwitchSpeedUpOnce = 0;
		if(PWMdata1->SwitchSpeedDnOnce == 0){
			PWMdata1->SwitchSpeedDnOnce = 0;}
	}
	
	if(PWMdata2->vvvf1IncreFlag == 1){
		PWMdata2->SwitchSpeedDnOnce = 0;
		if(PWMdata2->SwitchSpeedUpOnce == 0){
		PWMdata2->SwitchSpeedUpOnce = 0;}
	}
	else if(PWMdata2->vvvf1IncreFlag == 0){
		PWMdata2->SwitchSpeedUpOnce = 0;
		if(PWMdata2->SwitchSpeedDnOnce == 0){
			PWMdata2->SwitchSpeedDnOnce = 0;}
	}	
	
		if(PWMdata1->Kv > PWMdata1->KvPre)
	{
		if(KvIncre1Decre0 == 0)
			KvCnt = 0;
		KvIncre1Decre0 = 1;
		KvCnt++;
		if(KvCnt >= 1401)
			KvCnt = 1401;
		if(KvCnt>=1400)
			PWMdata1->KvIncreFlag = 1;
	}
	else if(PWMdata1->Kv < PWMdata1->KvPre)
	{
		if(KvIncre1Decre0 == 1)
			KvCnt = 0;
		KvIncre1Decre0 = 0;
		KvCnt++;
		if(KvCnt >= 1401)
			KvCnt = 1401;
		if(KvCnt>=1400)
			PWMdata1->KvIncreFlag = 0;
	}
	else
	{
		KvCnt++;
		if(KvCnt >= 1401)
			KvCnt = 1401;
		if(KvIncre1Decre0 == 1 && KvCnt>=1400)
		PWMdata1->KvIncreFlag = 1;
		else if(KvIncre1Decre0 == 0 && KvCnt>=1400)
		PWMdata1->KvIncreFlag = 0;	
	}
	
		if(PWMdata2->Kv > PWMdata2->KvPre)
	{
		if(KvIncre1Decre0_2 == 0)
			KvCnt_2 = 0;
		KvIncre1Decre0_2 = 1;
		KvCnt_2++;
		if(KvCnt_2 >= 1401)
			KvCnt_2 = 1401;
		if(KvCnt_2>=1400)
			PWMdata2->KvIncreFlag = 1;
	}
	else if(PWMdata2->Kv < PWMdata2->KvPre)
	{
		if(KvIncre1Decre0_2 == 1)
			KvCnt_2 = 0;
		KvIncre1Decre0_2 = 0;
		KvCnt_2++;
		if(KvCnt_2 >= 1401)
			KvCnt_2 = 1401;
		if(KvCnt_2 >=1400)
			PWMdata2->KvIncreFlag = 0;
	}
	else
	{
		KvCnt_2++;
		if(KvCnt_2 >= 1401)
			KvCnt_2 = 1401;
		if(KvIncre1Decre0_2 == 1 && KvCnt_2>=1400)
		PWMdata2->KvIncreFlag = 1;
		else if(KvIncre1Decre0_2 == 0 && KvCnt_2>=1400)
		PWMdata2->KvIncreFlag = 0;	
	}
	
	
	if(PWMdata1->KvIncreFlag == 1){
		PWMdata1->SVMModeSwitchDn = 0;
		if(PWMdata1->SVMModeSwitchUp == 0)
			PWMdata1->SVMModeSwitchUp = 0;
	}
	else if(PWMdata1->KvIncreFlag == 0){
		PWMdata1->SVMModeSwitchUp = 0;
		if(PWMdata1->SVMModeSwitchDn == 0)
			PWMdata1->SVMModeSwitchDn = 0;		
	}
	
	if(PWMdata2->KvIncreFlag == 1){
		PWMdata2->SVMModeSwitchDn = 0;
		if(PWMdata2->SVMModeSwitchUp == 0)
			PWMdata2->SVMModeSwitchUp = 0;
	}
	else if(PWMdata2->KvIncreFlag == 0){
		PWMdata2->SVMModeSwitchUp = 0;
		if(PWMdata2->SVMModeSwitchDn == 0)
			PWMdata2->SVMModeSwitchDn = 0;	
	}	
}
/*********************************************************************/
void resetPWMreg(struct PWMdataStruct *PWMdata)
{
 
   *(short *)(PWMdata->PWMTransLocalAddr+0x0a)=0xaaaa;/*ACQ_ZRO */
   *(short *)(PWMdata->PWMTransLocalAddr+0x0c)=0xaaaa;/*ACQ_CAU */
   *(short *)(PWMdata->PWMTransLocalAddr+0x12)=0;/*CMPAU_shadow[1] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x16)=0;/*CMPAU_shadow[2] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x1a)=0;/*CMPAU_shadow[3] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x1e)=0;/*CMPAU_shadow[4] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x22)=0;/*CMPAU_shadow[5] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x26)=0;/*CMPAU_shadow[6] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x0e)=0xaaaa;/*ACQ_PRD */
   *(short *)(PWMdata->PWMTransLocalAddr+0x10)=0xaaaa;/*ACQ_CAD */ 
   *(short *)(PWMdata->PWMTransLocalAddr+0x14)=0;/*CMPAD_shadow[1] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x18)=0;/*CMPAD_shadow[2] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x1c)=0;/*CMPAD_shadow[3] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x20)=0;/*CMPAD_shadow[4] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x24)=0;/*CMPAD_shadow[5] */
   *(short *)(PWMdata->PWMTransLocalAddr+0x28)=0;/*CMPAD_shadow[6] */
   PWMdata->NeuVoltCtl_sum=0;
 
}
/*********************************************************************/
void PWMBoardRegSet(struct PWMdataStruct *PWMdata)
{ 
  short i=0; 
 
  if(PWMdata->Reg_UpDown==1)/*timer counter decrease down */  /* 20160706 */
  {
	 PWMdata->TransWriteFlag = 0;
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x06)=PWMdata->T1PR;
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x0a)=PWMdata->ACQ_ZRO;/*ACQ_ZRO */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x0c)=PWMdata->ACQ_CAU;/*ACQ_CAU */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x12)=PWMdata->CMPA[0];/*CMPAU_shadow[1] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x16)=PWMdata->CMPA[1];/*CMPAU_shadow[2] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x1a)=PWMdata->CMPA[2];/*CMPAU_shadow[1] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x1e)=PWMdata->CMPA[3];/*CMPAU_shadow[1] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x22)=PWMdata->CMPA[4];/*CMPAU_shadow[1] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x26)=PWMdata->CMPA[5];/*CMPAU_shadow[1] */

  }
   else
  {
   PWMdata->TransWriteFlag = 1;
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x06)=PWMdata->T1PR;
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x0e)=PWMdata->ACQ_PRD;/*ACQ_PRD */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x10)=PWMdata->ACQ_CAD;/*ACQ_CAD */ 
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x14)=PWMdata->CMPA[0];/*CMPAD_shadow[1] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x18)=PWMdata->CMPA[1];/*CMPAD_shadow[2] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x1c)=PWMdata->CMPA[2];/*CMPAD_shadow[1] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x20)=PWMdata->CMPA[3];/*CMPAD_shadow[1] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x24)=PWMdata->CMPA[4];/*CMPAD_shadow[1] */
   *(unsigned short *)(PWMdata->PWMTransLocalAddr+0x28)=PWMdata->CMPA[5];/*CMPAD_shadow[1] */

  }
	
}
/*********************************���ǶȲ�ֵ*****************************************************************************************************/
/*�ǶȲ�ֵ����ΪMCU�´���CPS������1200hz����CPS��7200HZ��ʱ���м��������Ҫ��ֵ
 * 
 */
void GamaInterpolationNew(struct PWMdataStruct *PWMdata1,struct PWMdataStruct *PWMdata2,unsigned short index)
{
	if(index!=0 && index!=6)  // �޸ĳ��ж�����
	{
		DeltaAngle_Inv=1*PI*PWMdata1->vvvf1Interpolation*TsSHE;
		PWMdata1->VFGama = PWMdata1->VFGama + DeltaAngle_Inv;
		PWMdata2->VFGama = PWMdata2->VFGama + DeltaAngle_Inv;
		while(PWMdata1->VFGama>2*PI) PWMdata1->VFGama=PWMdata1->VFGama-2*PI;
		while(PWMdata1->VFGama<0) PWMdata1->VFGama=PWMdata1->VFGama+2*PI;
		while(PWMdata2->VFGama>2*PI) PWMdata2->VFGama=PWMdata2->VFGama-2*PI; 
		while(PWMdata2->VFGama<0) PWMdata2->VFGama=PWMdata2->VFGama+2*PI;  
		InterpolationCnt++;
	}	
	GamaInterDelta = PWMdata1->VFGama - VFGamaPre;
	VFGamaPre = PWMdata1->VFGama;
}
void Rec1_anpc1()
{
	txRecSrioData1.ANPC_type_a=1;
	txRecSrioData1.ANPC_type_b=1;
	txRecSrioData1.ANPC_type_c=1;
}
void Rec1_anpc2()
{
	txRecSrioData1.ANPC_type_a=2;
	txRecSrioData1.ANPC_type_b=2;
	txRecSrioData1.ANPC_type_c=2;
}
void Rec1_npc()
{
	txRecSrioData1.ANPC_type_a=0;
	txRecSrioData1.ANPC_type_b=0;
	txRecSrioData1.ANPC_type_c=0;
}
void Rec2_anpc1()
{
	txRecSrioData2.ANPC_type_a=1;
	txRecSrioData2.ANPC_type_b=1;
	txRecSrioData2.ANPC_type_c=1;
}
void Rec2_anpc2()
{
	txRecSrioData2.ANPC_type_a=2;
	txRecSrioData2.ANPC_type_b=2;
	txRecSrioData2.ANPC_type_c=2;
}
void Rec2_npc()
{
	txRecSrioData2.ANPC_type_a=0;
	txRecSrioData2.ANPC_type_b=0;
	txRecSrioData2.ANPC_type_c=0;
}
void Inv1_anpc1()
{
	txInvSrioData1.ANPC_type_a=1;
	txInvSrioData1.ANPC_type_b=1;
	txInvSrioData1.ANPC_type_c=1;
}
void Inv1_anpc2()
{
	txInvSrioData1.ANPC_type_a=2;
	txInvSrioData1.ANPC_type_b=2;
	txInvSrioData1.ANPC_type_c=2;
}
void Inv1_npc()
{
	txInvSrioData1.ANPC_type_a=0;
	txInvSrioData1.ANPC_type_b=0;
	txInvSrioData1.ANPC_type_c=0;
}

void Inv2_anpc1()
{
	txInvSrioData2.ANPC_type_a=1;
	txInvSrioData2.ANPC_type_b=1;
	txInvSrioData2.ANPC_type_c=1;
}
void Inv2_anpc2()
{
	txInvSrioData2.ANPC_type_a=2;
	txInvSrioData2.ANPC_type_b=2;
	txInvSrioData2.ANPC_type_c=2;
}
void Inv2_npc()
{
	txInvSrioData2.ANPC_type_a=0;
	txInvSrioData2.ANPC_type_b=0;
	txInvSrioData2.ANPC_type_c=0;
}
