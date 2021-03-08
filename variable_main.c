/******************************************************************************/
/*   
* Copyright 2018,4,16 IEE, Inc. 
* description:  全局变量定义 *
* history: wxx 20180606
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
#include "subMod_ControlFunction.h"
#include "ioLib.h"
#include "wdLib.h"
#include "variable_main.h"
#include "main.h"
/******************************************************************************/
struct TX_7700_STRUCT tx_7700_data;
struct RX_7700_STRUCT *rx_7700_data_pointer;
struct RX_7700_STRUCT rx_7700_data; 
/******************************************************************************/
union RUNCMD_REG     runCmd;
union selfCheck_REG  selfCheck;
/******************************************************************************/
struct TX_PWM_SRIO_Struct  txRecSrioData1;
struct TX_PWM_SRIO_Struct  txRecSrioData2;
struct TX_PWM_SRIO_Struct  txInvSrioData1;
struct TX_PWM_SRIO_Struct  txInvSrioData2;
struct TX_AD_SRIO_Struct   txAdSrioData1;
struct TX_AD_SRIO_Struct   txAdSrioData2;
struct TX_CLK_SRIO_Struct  txClkSrioData;

struct RX_PWM_SRIO_Struct  *rxRecSrioData1;
struct RX_PWM_SRIO_Struct  *rxRecSrioData2;
struct RX_PWM_SRIO_Struct  *rxInvSrioData1;
struct RX_PWM_SRIO_Struct  *rxInvSrioData2;
struct RX_AD_SRIO_Struct   *rxAdSrioData1;
struct RX_AD_SRIO_Struct   *rxAdSrioData2;
struct RX_CLK_SRIO_Struct  *rxClkSrioData; 



struct rxCCUStruct  rxccudata;
struct txCCUStruct  txccudata;

int txPWMSrioLen= 0;
int rxPWMSrioLen= 0;
int txADSrioLen= 0;
int rxADSrioLen= 0;
int txClkSrioLen= 0;
int rxClkSrioLen= 0;
/******************************************************************************/
unsigned int   RxCcuRfmBaseAdrs;
char*  		   RxCcuLocalBaseAdrs;
unsigned int   TxCcuRfmBaseAdrs;
char*	TxCcuLocalBaseAdrs; 
char*  	RecLocalBaseAdrs1; 
char*  	RecLocalBaseAdrs2;  
char* 	InvLocalBaseAdrs1; 
char*  	InvLocalBaseAdrs2; 
char*  	AD3123RegLocalBaseAdrs; 
char* 	AD3123MemLocalBaseAdrs;
char*	ADLocalBaseAdrs1; 
char*	ADLocalBaseAdrs2;
char* 	ClkLocalBaseAdrs;/*CLK_FPGA基地址对应的vme本地地址*/
char* 	DALocalBaseAdrs;
char* 	IOLocalBaseAdrs;
char* 	PfbLocalBaseAdrs;
char* 	ast7700LocalBaseAdrs;

/******************************************************************************/
short 	interrupt_index=0;
double syn_control_flag=0;
double speed_out=0,speed_out1,theta,omega=0,freq_cmd,theta_zero,speed_pre_filter,speed_cmd,speed_cmdend,freq_cmdend,id_cmd,inv_a,inv_a_sec,inv_b_sec,inv_c_sec,inv_b,inv_c,Parallel_cmd=1,Serial_cmd,iq_cmd,uref_delta,uref_delta1,uref_delta2,theta_current_dir,phase_2pi;
double theta_syn=0,theta_syn_rl=0,iout_d=0,iout_q=0,uref_syn_d=0,uref_syn_q=0,iout_d_filter=0,iout_q_filter=0,ud_for=0,uq_for=0,uref_syn_d_filter=0,uref_syn_q_filter=0;
double define_position_flag=0,invfreq_flag=0,testcmd2=0;
double theta_syn_pre=0,theta_syn_filter=0,theta_syn_bu=0,temp_theta=0.0,flag_theta=0,theta_syn_filter_old=0,theta_syn_SHE=0;
double inv_Kv1=0,inv_Kv2=0,Kv_Inv_from_LK,invfreq_from_LK,Kv_cnt=0.1;
double Udc1_filter,Udc2_filter=0,Udc1_rec2_filter=0,Udc2_rec2_filter=0;
double recUa2_grid=0,recUb2_grid=0,recUc2_grid=0;
unsigned int runState; 
unsigned short faultFlag=0; 
struct faultcode_REG faultCode; 
double Udc_protect_LK=0;
double UdcNobalane_protect_LK=0;
double Iinv_protect_LK=0;
double Irec_protect_LK=0;
double Ts=1/7200.;
float Udc,VdcRec;
float Udc_ref =0;
int firstrec=0;

short switchINcmd=0;
short switch_DT_BTcmd=0;
short switchOUTcmd=0;
short preChargecmd=0;
short Ratiocmd=0;

float GridUa,GridUb,GridUc,GridIa,GridIb,GridIc;
float IaMotor,IbMotor,IcMotor;
float IaMotor_old,IbMotor_old,IcMotor_old;
float invUa1,invUb1,invUc1,invUa2,invUb2,invUc2;
float recIa1,recIb1,recIc1,recUab1,recUbc1,recUca1,recIa2,recIb2,recIc2,recUab2,recUbc2,recUca2,Udc1,Udc2,Idc1,Idc2,invIu1,invIv1,invIw1,invIu2,invIv2,invIw2;
float recUa1,recUb1,recUc1,recUd1,recId1,recIq1,invId1,invIq1,recTheta1;
float recUa2,recUb2,recUc2,recUd2,recId2,recIq2,invId2,invIq2,recTheta2; 
short adData1[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
short adData2[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double DeltaAngle_Inv,Kv_Inv = 0.7,uref_delta_inv=0,Kv_Rec,VFGama_Inv,VFGama_Rec ;
float invfreq=0,invfreqpre=0,Kp_motor=1.25,theta_syn_SHE_flag=0,invfreq_filter=0;
int up1dn0flag=1;
float Iinv_alpha,Iinv_beta,Iinv_d,Iinv_q,Iinv_d_filter,Iinv_q_filter,Iinv_rms,Urec_rms,Irec_rms;
float Irec_alpha,Irec_belta,Irec_d,Irec_q,Irec_d_filter,Irec_q_filter;
int err1=0,err2=0,err3=0,err4=0,err5=0,err6=0,err7=0,err8=0,err9=0,err10=0,err11=0,err12=0,err13=0,err14=0,err15=0,err16=0,err17=0,err18=0,err19=0,err20=0,err21=0,err22=0,err23=0,err24=0,err25=0,err26=0,err27=0,err28=0,err29=0,err30=0,err31=0,err32=0,err33=0,err34=0,err35=0,err36=0,err37=0,err38=0,err39=0,err40=0,err41=0,err42=0,err43=0,err44=0,err45=0,err46=0;
int err50=0,err51=0,err52=0,err53=0,err54=0,err55=0;

short err[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int statenum1=0,statenum2=0,statenum3=0,statenum4=0,statenum5=0,statenum6=0,statenum7=0;
int statenum8=0,statenum9=0,statenum10=0,statenum11=0,statenum12=0,statenum13=0;
float Iinv_alpha2,Iinv_beta2,Iinv_d2,Iinv_q2,Iinv_d_filter2,Iinv_q_filter2,Iinv_rms2,Urec_rms2,Irec_rms2,Uinv_rms,Uinv_rms2;
float Grid_Irms;
float power_factor;
float Irec_alpha2,Irec_belta2,Irec_d2,Irec_q2,Irec_d_filter2,Irec_q_filter2;
int Udc1flag,Udc2flag;
float Udc1_rec2,Udc2_rec2;
 float invIu1_filter=0,invIv1_filter=0,invIw1_filter=0,invIu2_filter=0,invIv2_filter=0,invIw2_filter=0;
float recIa1_filter=0,recIb1_filter=0,recIc1_filter=0,recIa2_filter=0,recIb2_filter=0,recIc2_filter=0;
/*************************************************************************/
double Is_alpha = 0, Is_belta = 0;
double Isd = 0, Isq = 0,Isd_filter=0,Isq_filter=0;
double Isd2 = 0, Isq2 = 0,Isd_filter2=0,Isq_filter2=0;
double vd = 0,vdctrl = 0;
double GridFOC_FeedUGama=0,u_alph_Rect=0,u_beta_Rect=0,UsRefRec=0;
double rec_gama1=0,rec_gama2=0,reccmd2=0,reccmd1=0,gamaYD=0,gama_filter=0,gama_filter1=0;
double vq_filter=0,vd_filter=0,u_alph_Rect_filter=0,u_beta_Rect_filter=0,rec_gama_filter1=0,rec_gama_filter2_bu=0,rec_gama_filter1_bu=0,rec_gama_filter2=0,UsRefRec_filter=0;
double vq = 0,vqctrl = 0,vq_rec1=0,vq_rec2=0,vd_rec1=0,vd_rec2=0;
double usdFw_rec1_filter=0,usdFw_rec2_filter=0,usqFw_rec1_filter=0,usqFw_rec2_filter=0;
double Us_alpha=0,Us_belta=0,usdFw_rec1=0,usqFw_rec1=0,usdFw_rec2=0,usqFw_rec2=0;
short p=14; 
unsigned short prd=0;
unsigned short prd1=0;
unsigned short frecout=0;
double m1=0,m2=0,m3=0;
unsigned short dir=2;
int PWMRoute=0,svmtest=0,changestep_gaozhan_dn=0,changestep_gaozhan_up=0;
unsigned short phasedate_dir_reg=0;
double phase_dir_reg=0;
unsigned short MainSwitch=0,Start_Rec=0,Start_Inv=0,Stop_Inv=0,Stop_Rec=0,Position=0,Discharge=0,Reset=0,Precharge_LK=0;
unsigned short Parallel_LK=0,Serial_AUTO=0,moderoute=0,switchflag=0,closeflag=0,testgz=0,Parallel_AUTO=1,Serial_LK=0,AUTO_model_LK=0,Precharge_OK=0;
unsigned short Parallel_Model=0,Serial_Model=0,AUTO_Model=0,time1freq0=0,time1freq0_LK=0;
double Serial_AUTO_time=0,Parallel_AUTO_time=0,auto_time=0,close_time=0;
double wait_ns=0,Parallel_time=0,Parallelroute=0;
int   Timer_Counter=0,start_dc_balance_flag=0;
unsigned int PWM_shortCuirterr=0,bus_err_other=0,inv1_MinPulseErr=0,inv1_DeadZoneErr=0,inv1_igctfault=0,inv1_errstate=0;

short pwmcntdirection=0;
short IntFlagTest=0,PeriodTest=0,PeriodTest2=0,ZROTest=0,CAUTest,PRDTest,CADTest;
short CMPAU1Test=0,CMPAU2Test=0,CMPAU3Test=0,CMPAU4Test=0,CMPAU5Test=0,CMPAU6Test=0;
short CMPAD1Test=0,CMPAD2Test=0,CMPAD3Test=0,CMPAD4Test=0,CMPAD5Test=0,CMPAD6Test=0,SyncFlagTest=0;
unsigned int IO_input=0x0000,IO_output=0x0000;
int stop_inv_flag=0,stop_rec_flag=0,discharge_flag=0,reset_flag=0,err_stop_flag=0,discharge_cnt=0,start_watercool=0,err_stop_flag_cnt=0;
int wait90ms=0,step,wait1s=0;
double FreqWithDire = 0;  // 带方向的频率，用于插值限制角度回撤
double VFGamaWithDire = 0;
short InterpolationCnt = 0;
int CntKv = 0;
int CntTestConti = 0;
unsigned short CMPASignDetection = 0;
double speed_ref=0,speed_fb=0,speed_for=0;
double AngleTemp = 0,kv1_filter=0,kv1_filter_pre=0,kv2_filter=0,kv2_filter_pre=0;
int tempindex=0; 
short runcmd_old=0;
unsigned int KvCnt = 0,kvtest=0,KvCnt_2 = 0,KvIncre1Decre0 = 1,KvIncre1Decre0_2=1;
double TempRoute = 0;

unsigned short QEPA=0;
unsigned short phasedata=0;
unsigned short phasedata_z=0;
unsigned short phasedata_z_initial=0;
double ia=0,ib=0,ic=0;
int Ts_period=41667;
float theta_test=0;
short shutOffCmd=0;
//short run=0;
short faultinfo=0;
short InvIntFlag = 0,InvIntFlag2 = 0;
short Printcnt = 0;
short interrupt_index_next = 0;
unsigned int TestStateMachineCntL=0,TestStateMachineCntH=0; 
double VrefPhi=0;
unsigned cntL = 0,cntH = 0;
short clkGPSStatus=0;
short MinSecBuf,DayHrBuf,YrHDayBuf;
short runcmd=0;
double dccmd=0;
int qiehuanflag;
double Idref_udc = 0.0,Idref_udc_slop=0.0,Id_pf=0,idref_cnt=0,Idref;
double idrec_filter=0,iqrec_filter=0;
double idrec1_filter=0,iqrec1_filter=0;
double idrec2_filter=0,iqrec2_filter=0;
double UdcSet=5000;//100;
float Num_temp=0;
double Theta_Secd = 0;
double iq_cmd_ref=0;  //有速度环时用，无速度环时屏蔽其和声明

float Ua_delay=0,Ub_delay=0,Uc_delay=0,Uan_in_theta=0,Uan_in_theta_cos=0,Ubn_in_theta=0,Ubn_in_theta_cos=0,cosphi_delay=0,sinphi_delay=0;
float Ucn_in_theta=0,Ucn_in_theta_cos=0,ism_ref=0,iLa_ref=0,iLb_ref=0,iLc_ref=0,Uan_PRout=0,Ubn_PRout=0,Ucn_PRout=0;
float Uan_ref_Rec=0,Ubn_ref_Rec=0,Ucn_ref_Rec=0,RL_as_compensate=0,RL_bs_compensate=0,RL_cs_compensate=0;
float testgama1=0,testgama2=0,recIagama=0;

int  	intCounter1=0;
int   	intCounter2=0;
int   	intConnectStatus=0;

short 	datemp=1000; 
short 	statecnt = 0;
UINT32 	astTestLocalbusFpgaIrqIsr1Do = 0;

signed short phasedata_dir=0,z_flag;
unsigned short phasedata_z_dir=0;

double phase_z=0,phase_z_initial=0;
double phase_dir=0,phase_z_dir=0,phase_z_initial_dir=0;
unsigned short phasedata_z_initial_dir=0;
unsigned short prddata=0;
//unsigned short encoderLine=1024;
unsigned short encoderLine=600;
double speed1=0,speed1_last;
double speed2=0,speed3_last;
double speed3=0;
double speed=0,firstspeed=0;
double speed_pre=0;
double vvvf1IncreFlag=0,fdecreflag=0,fincreflag=0;
double phase=0;
double initial_phase=0;
float invfreq_pre=0;
int motorstart=0,intCounterspeed;
int resettime=0;//


double Id1,Id2,Iq1,Iq2;
double Id1_filter,Iq1_filter,Id2_filter,Iq2_filter;

float chopStart_Flag;
int Inv_type_a=0,Inv_type_b=0,Inv_type_c=0,Rec_type_a=0,Rec_type_b=0,Rec_type_c=0;//ANPC type
float Rec_type_cnt=0,Inv_type_cnt=0;
/******************************************************************************/
unsigned short invPwmMode=SVPWMMODE;

unsigned short recPWMDIR;
unsigned short invPWMDIR;

/************************变量初始化函数******************************************************/
void Varible_Init(void)
{
	runcmd=0; //停止调用整流、逆变调制算法
	firstrec=0;
	interrupt_index_next=0;
	interrupt_index=0;	
	faultFlag=0;	
	Udc_ref = 0.1;
	recId2=0;
	recIq2=0;
	invId2=0;
	invIq2=0;
	//err[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//	
//	//各指示灯灭
	IO_out_ANPC.bit.main_switch_led=0;
	IO_out_ANPC.bit.inv_overcurrent=0;
	IO_out_ANPC.bit.Udc_Nobalance=0;
	IO_out_ANPC.bit.DC_overvoltage=0;
	IO_out_ANPC.bit.rec1_overcurrent=0;
	IO_out_ANPC.bit.start_inv=0;
	IO_out_ANPC.bit.start_rec=0;
	 IO_out_ANPC.bit.precharge=0;
	
	dccmd=0;reccmd1=0;reccmd2=0;
   uref_syn.alpha=0;uref_syn.beta=0;uref_syn.delta=0;uref_syn.delta=0;uref_delta=0;motorstart=0; iq_cmd=0; id_cmd=0;
	Udc_ref = VdcRec;
	idref_cnt=0.1;Idref_udc=0;Num_temp=0;Kv_cnt=0.1;
	inv_Kv1=0;inv_Kv2=0;uref_delta1=0;uref_delta2=0;iq_cmd=0;uref_syn.d=0;
	uref_syn.q=0;theta_syn_rl=0;iout.a=0;iout.b=0;iout.c=0;Parallel_time=0;Serial_cmd=0;Parallel_cmd=1;up1dn0flag=1;

		Parallel_AUTO=1;Serial_AUTO_time=0;Parallel_AUTO_time=0;Serial_AUTO=0;auto_time=0;moderoute=0;Parallelroute=0;
	invfreqpre=0;closeflag=0;close_time=0;switchflag=0;	
		DeltaAngle_Inv=0;
		Kv_Inv = 0.7;
		Kv_Rec=0.85;
		VFGama_Inv=0;
		VFGama_Rec=0 ;
		invfreq=0;
//		
		statenum1=0;statenum2=0;statenum3=0;statenum4=0;statenum5=0;statenum6=0;statenum7=0;
		statenum8=0;statenum9=0;statenum10=0;statenum12=0;statenum13=0;
		
		VdcRec=0;verr=0;Isd=0;Isq=0;idrec2_filter=0;iqrec2_filter=0;Idref_udc=0;Iderr=0;vdctrl=0;
		vd=0;Iqerr=0;vqctrl=0;vq=0;GridFOC_FeedUGama=0;UsRefRec=0;gama_filter =0;
		
		 Ua_delay=0;Ub_delay=0;Uc_delay=0;Uan_in_theta=0;Uan_in_theta_cos=0;Ubn_in_theta=0;
		 Ubn_in_theta_cos=0;
		Ucn_in_theta=0;Ucn_in_theta_cos=0;ism_ref=0;iLa_ref=0;iLb_ref=0;iLc_ref=0;
		Uan_ref_Rec=0;Ubn_ref_Rec=0;Ucn_ref_Rec=0;RL_as_compensate=0;RL_bs_compensate=0;RL_cs_compensate=0;
		
		Inv_type_cnt=0;
		Rec_type_cnt=0;
		Inv_type_a=0;
		Inv_type_b=0;
		Inv_type_c=0;
		Rec_type_a=0;
		Rec_type_b=0;
		Rec_type_c=0;
		
}

/************************变量初始化函数——只在刚开始时调用一次，初始化各项数据******************************************************/
void Varible_Init_first(void)
{
	
	runcmd=0; //停止调用整流、逆变调制算法
	firstrec=0;
	interrupt_index_next=0;
	interrupt_index=0;	
	faultFlag=0;	
	Udc=0;
	Udc_ref = 0.1;
	recIa1=0;
	recIb1=0;
	recIc1=0;
	recUab1=0;
	recUbc1=0;
	recIa2=0;
	recIb2=0;
	recIc2=0;
	recUab2=0;
	recUbc2=0;
	Udc1=0;
	Udc2=0;
	Idc1=0;
	Idc2=0;
	invIu1=0;
	invIv1=0;
	invIw1=0;
	invIu2=0;
	invIv2=0;
	invIw2=0;
	recUa1=0;
	recUb1=0;
	recUc1=0;
	recUd1=0;
	recId1=0;
	recIq1=0;
	invId1=0;
	invIq1=0;
	recTheta1=0;
	recUa2=0;
	recUb2=0;
	recUc2=0;
	recUd2=0;
	recId2=0;
	recIq2=0;
	invId2=0;
	invIq2=0;
	recTheta2=0; 
	
    Id_belta_01 = 60;//7200;
    Id_belta_02 = 900;//190000;
    Id_belta1 = 2;
    Id_b = 1000;
    Id_r = 40000;

    Iq_belta_01 = 60;//7200;
    Iq_belta_02 = 900;//190000;//200000;
    Iq_belta1 = 2;
    Iq_b = 1000;
    Iq_r = 40000;
	
    Udc_belta_01 = 60;//7200;
    Udc_belta_02 = 900;//190000;//200000;
    Udc_belta1 = 2;
    Udc_b = 300000;
    Udc_r = 40000;
	
	//各指示灯灭
	IO_out_ANPC.bit.main_switch_led=0;
	IO_out_ANPC.bit.inv_overcurrent=0;
	IO_out_ANPC.bit.Udc_Nobalance=0;
	IO_out_ANPC.bit.DC_overvoltage=0;
	IO_out_ANPC.bit.rec1_overcurrent=0;
	IO_out_ANPC.bit.start_inv=0;
	IO_out_ANPC.bit.start_rec=0;
	 IO_out_ANPC.bit.precharge=0;
	
	dccmd=0;reccmd1=0;reccmd2=0;
   uref_syn.alpha=0;uref_syn.beta=0;uref_syn.delta=0;uref_syn.delta=0;uref_delta=0;motorstart=0; iq_cmd=0; id_cmd=0;
	Udc_ref = VdcRec;
	idref_cnt=0.1;Idref_udc=0;Num_temp=0;Kv_cnt=0.1;
	inv_Kv1=0;inv_Kv2=0;uref_delta1=0;uref_delta2=0;iq_cmd=0;uref_syn.d=0;
	uref_syn.q=0;theta_syn_rl=0;iout.a=0;iout.b=0;iout.c=0;Parallel_time=0;Serial_cmd=0;Parallel_cmd=1;up1dn0flag=1;
	Parallel_AUTO=1;Serial_AUTO_time=0;Parallel_AUTO_time=0;Serial_AUTO=0;auto_time=0;moderoute=0;Parallelroute=0;
	invfreqpre=0;closeflag=0;close_time=0;switchflag=0;	
		DeltaAngle_Inv=0;
		Kv_Inv = 0.7;
		Kv_Rec=0.85;
		VFGama_Inv=0;
		VFGama_Rec=0 ;
		invfreq=0;	
	
		err1=0;err2=0;err3=0;err4=0;err5=0;err6=0;err7=0;err8=0;err9=0;err10=0;err11=0;err12=0;
		err13=0;err14=0;err15=0;err16=0;err17=0;err18=0;err19=0;err20=0;err21=0;err22=0;err23=0;
		err24=0;err25=0;err26=0;err27=0;err28=0;err29=0;err30=0;err31=0;err32=0;
		err33=0;err34=0;err35=0;err36=0;
		err37=0;err38=0;err39=0;err40=0;err41=0;err42=0;err43=0;err44=0;err45=0;err46=0;
		err50=0,err51=0,err52=0,err53=0,err54=0,err55=0;
		//err[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		
		statenum1=0;statenum2=0;statenum3=0;statenum4=0;statenum5=0;statenum6=0;statenum7=0;
				statenum8=0;statenum9=0;statenum10=0;statenum11=0;statenum12=0;statenum13=0;
		
	memset( adData1, 0, 32); //memset（s,n,z）函数，将地址s指向的内存中后面z个字节用n替换，即数组初始化函数
	memset( adData2, 0, 32); //注意，不能复位，电气量数据要实时监测！！
	
	selfCheck.word=0;   //注意，不能复位，否则会导致各板卡自检错误！！！
	    runCmd.word=0;
		runState=0;
	//将各板卡赋值结构体的字节数赋值给相应变量，方便对各板卡赋值结构体初始化
		txPWMSrioLen= sizeof(struct TX_PWM_SRIO_Struct);
		rxPWMSrioLen= sizeof(struct RX_PWM_SRIO_Struct);
		txADSrioLen= sizeof(struct TX_AD_SRIO_Struct);
		rxADSrioLen= sizeof(struct RX_AD_SRIO_Struct);
		txClkSrioLen= sizeof(struct TX_CLK_SRIO_Struct);
		rxClkSrioLen= sizeof(struct RX_CLK_SRIO_Struct);	
		//faultCode为faultcode_REG类型结构体，memset函数第一项为地址，故用&faultCode，作用是将结构体faultCode内所有变量赋初值为0
		memset( &faultCode, 0, sizeof(struct faultcode_REG));
		memset( &rxccudata, 0, sizeof(struct rxCCUStruct));
		memset( &rxccudata, 0, sizeof(struct rxCCUStruct));
		
		memset( &txClkSrioData, 0, sizeof(struct TX_CLK_SRIO_Struct));
		memset( &txRecSrioData1, 0, sizeof(struct TX_PWM_SRIO_Struct));
		memset( &txRecSrioData2, 0, sizeof(struct TX_PWM_SRIO_Struct));
		memset( &txInvSrioData1, 0, sizeof(struct TX_PWM_SRIO_Struct));
		memset( &txInvSrioData2, 0, sizeof(struct TX_PWM_SRIO_Struct));
		memset( &txAdSrioData1, 0, sizeof(struct TX_AD_SRIO_Struct));
		memset( &txAdSrioData2, 0, sizeof(struct TX_AD_SRIO_Struct));
		//此处存在问题，memset函数第一项为指针，而rxClkSrioData本身为指针，不应该再加取地址符号
		//memset( &rxClkSrioData, 0, sizeof(struct RX_CLK_SRIO_Struct));
		//memset( &rxRecSrioData1, 0, sizeof(struct RX_PWM_SRIO_Struct));
		//memset( &rxRecSrioData2, 0, sizeof(struct RX_PWM_SRIO_Struct));
		//memset( &rxInvSrioData1, 0, sizeof(struct RX_PWM_SRIO_Struct));
		//memset( &rxInvSrioData2, 0, sizeof(struct RX_PWM_SRIO_Struct));
		//memset( &rxAdSrioData1, 0, sizeof(struct RX_AD_SRIO_Struct));
		//memset( &rxAdSrioData2, 0, sizeof(struct RX_AD_SRIO_Struct));
		memset( rxClkSrioData, 0, sizeof(struct RX_CLK_SRIO_Struct));
		memset( rxRecSrioData1, 0, sizeof(struct RX_PWM_SRIO_Struct));
		memset( rxRecSrioData2, 0, sizeof(struct RX_PWM_SRIO_Struct));
		memset( rxInvSrioData1, 0, sizeof(struct RX_PWM_SRIO_Struct));
		memset( rxInvSrioData2, 0, sizeof(struct RX_PWM_SRIO_Struct));
		memset( rxAdSrioData1, 0, sizeof(struct RX_AD_SRIO_Struct));
		memset( rxAdSrioData2, 0, sizeof(struct RX_AD_SRIO_Struct)); 
		
		 Ua_delay=0;Ub_delay=0;Uc_delay=0;Uan_in_theta=0;Uan_in_theta_cos=0;Ubn_in_theta=0;Ubn_in_theta_cos=0;cosphi_delay=0;sinphi_delay=0;
	    Ucn_in_theta=0;Ucn_in_theta_cos=0;ism_ref=0;iLa_ref=0;iLb_ref=0;iLc_ref=0;Uan_PRout=0;Ubn_PRout=0,Ucn_PRout=0;
	    Uan_ref_Rec=0;Ubn_ref_Rec=0;Ucn_ref_Rec=0;RL_as_compensate=0;RL_bs_compensate=0;RL_cs_compensate=0;
	    
		Inv_type_cnt=0;
		Rec_type_cnt=0;
		Inv_type_a=0;
		Inv_type_b=0;
		Inv_type_c=0;
		Rec_type_a=0;
		Rec_type_b=0;
		Rec_type_c=0;
}
/************************对PR锁相环各项数据初始化******************************************************/
void Control_init(void)
{	
	initPR_control(&PR_control_Ias_rec1);
	initPR_control(&PR_control_Ibs_rec1);
	initPR_control(&PR_control_Ics_rec1);
	initPR_control(&PR_control_Ias_rec2);
	initPR_control(&PR_control_Ibs_rec2);
	initPR_control(&PR_control_Ics_rec2);
	
	initPR_view(&PR_view_Uas_rec1);
	initPR_view(&PR_view_Ubs_rec1);
	initPR_view(&PR_view_Ucs_rec1);
	initPR_view(&PR_view_Uas_rec2);
	initPR_view(&PR_view_Ubs_rec2);
	initPR_view(&PR_view_Ucs_rec2);
	
	//控制参数初始化
	PR_control_Ias_rec1.PR_Out = 0;//A相电流PR控制
	PR_control_Ibs_rec1.PR_Out = 0;//B相电流PR控制
	PR_control_Ics_rec1.PR_Out = 0;//C相电流PR控制

	PR_view_Uas_rec1.phai11=cos(2*50*PI*Ts);
	PR_view_Uas_rec1.phai12=-sin(2*50*PI*Ts);
	PR_view_Uas_rec1.phai21=sin(2*50*PI*Ts);
	PR_view_Uas_rec1.phai22=cos(2*50*PI*Ts);
	PR_view_Uas_rec1.c1=PR_view_Uas_rec1.phai21;
	PR_view_Uas_rec1.c2=1-PR_view_Uas_rec1.phai11;
	PR_view_Uas_rec1.lim_pr=600;
	PR_view_Uas_rec1.kf=0.2;

	PR_view_Ubs_rec1.phai11=cos(2*50*PI*Ts);
	PR_view_Ubs_rec1.phai12=-sin(2*50*PI*Ts);
	PR_view_Ubs_rec1.phai21=sin(2*50*PI*Ts);
	PR_view_Ubs_rec1.phai22=cos(2*50*PI*Ts);
	PR_view_Ubs_rec1.c1=PR_view_Ubs_rec1.phai21;
	PR_view_Ubs_rec1.c2=1-PR_view_Ubs_rec1.phai11;
	PR_view_Ubs_rec1.lim_pr=600;
	PR_view_Ubs_rec1.kf=0.2;

	PR_view_Ucs_rec1.phai11=cos(2*50*PI*Ts);
	PR_view_Ucs_rec1.phai12=-sin(2*50*PI*Ts);
	PR_view_Ucs_rec1.phai21=sin(2*50*PI*Ts);
	PR_view_Ucs_rec1.phai22=cos(2*50*PI*Ts);
	PR_view_Ucs_rec1.c1=PR_view_Ucs_rec1.phai21;
	PR_view_Ucs_rec1.c2=1-PR_view_Ucs_rec1.phai11;
	PR_view_Ucs_rec1.lim_pr=600;
	PR_view_Ucs_rec1.kf=0.2;

	//TZOH Delay time
	cosphi_delay=0.999507; //pwm延迟，theta = 2*50*PI*(T/2+Ts/2)
	sinphi_delay=0.0313948;

	PR_control_Ias_rec1.PR_phai11=cos(2*50*PI*Ts);
	PR_control_Ias_rec1.PR_phai12=-sin(2*50*PI*Ts);
	PR_control_Ias_rec1.PR_phai21=sin(2*50*PI*Ts);
	PR_control_Ias_rec1.PR_phai22=cos(2*50*PI*Ts);
	PR_control_Ias_rec1.PR_c1=PR_control_Ias_rec1.PR_phai21;
	PR_control_Ias_rec1.PR_c2=1-PR_control_Ias_rec1.PR_phai11;
	PR_control_Ias_rec1.PR_Kp=3.5;//0.5;//1;  //200V 8和0.2,5和0.2有抖动
	PR_control_Ias_rec1.PR_Kr=0.2;//0.1;//0.2;
	PR_control_Ias_rec1.PR_OutMax = (700);
	PR_control_Ias_rec1.PR_OutMin = (-700); 

	PR_control_Ibs_rec1.PR_phai11=cos(2*50*PI*Ts);
	PR_control_Ibs_rec1.PR_phai12=-sin(2*50*PI*Ts);
	PR_control_Ibs_rec1.PR_phai21=sin(2*50*PI*Ts);
	PR_control_Ibs_rec1.PR_phai22=cos(2*50*PI*Ts);
	PR_control_Ibs_rec1.PR_c1=PR_control_Ibs_rec1.PR_phai21;
	PR_control_Ibs_rec1.PR_c2=1-PR_control_Ibs_rec1.PR_phai11;
	PR_control_Ibs_rec1.PR_Kp=3.5;//0.5;//1;  
	PR_control_Ibs_rec1.PR_Kr=0.2;//0.1;//0.2;
	PR_control_Ibs_rec1.PR_OutMax = (700);
	PR_control_Ibs_rec1.PR_OutMin = (-700); 

	PR_control_Ics_rec1.PR_phai11=cos(2*50*PI*Ts);
	PR_control_Ics_rec1.PR_phai12=-sin(2*50*PI*Ts);
	PR_control_Ics_rec1.PR_phai21=sin(2*50*PI*Ts);
	PR_control_Ics_rec1.PR_phai22=cos(2*50*PI*Ts);
	PR_control_Ics_rec1.PR_c1=PR_control_Ics_rec1.PR_phai21;
	PR_control_Ics_rec1.PR_c2=1-PR_control_Ics_rec1.PR_phai11;
	PR_control_Ics_rec1.PR_Kp=3.5;//0.5;//1;  
	PR_control_Ics_rec1.PR_Kr=0.2;//0.1;//0.2;
	PR_control_Ics_rec1.PR_OutMax = (700);
	PR_control_Ics_rec1.PR_OutMin = (-700); 
}
