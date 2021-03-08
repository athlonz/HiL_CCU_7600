/*******************************************************************/
/*
//   Module  : variable_main.h
//   Created : 8/10/2015 3:25:30
//
//
//   Language  : C++*/
/*******************************************************************/
#ifndef _VARIABLE_MAIN_
#define _VARIABLE_MAIN_

#ifdef __cplusplus
extern "C" {
#endif


/*********************************************************************************************/
#include "vxWorks.h"
#include <math.h>
/*********************************************************************************************/
//#define DEBUG_RUN
 
#define NORMAL_RUN
#define CPS_PowerLevel "H"
#define CPS_ID  0x11
#define SVPWMMODE 0X0
#define SHEPWMMODE 0X1
#define UPDIR 0X1
#define DOWNDIR 0X0
#define PI	3.1415926535897932
#define pi	3.1415926535897932
#define vmeIntLevel 5  
#define intVector 200 

#define ast7700VmeAdrs  0x80000000/*7700的VME地址也是通过拨码开关设置的，可a24也可a32,此处设a32*/
#define RecVmeBaseAdrs1 0x100000/*0x100000*/
#define RecVmeBaseAdrs2 0x110000/*0x110000*/
#define InvVmeBaseAdrs1 0x120000/*0x120000*/
#define InvVmeBaseAdrs2 0x130000/*0x130000*/
#define ADVmeBaseAdrs1   0xa00000
#define ADVmeBaseAdrs2   0x600000
#define ClkVmeBaseAdrs   0x800000 /*base Adrsess of CLK_FPGA*/
#define DAVmeBaseAdrs 0Xaaaa80
#define IOVmeBaseAdrs 0xb00000
#define PfbVmeBaseAdrs 0xe00000

#define RecLocalFPGABaseAdrs1 (AVME7600_LOCALBUS_FPGA_ADDR + 0x1000)
#define RecLocalFPGABaseAdrs2 (AVME7600_LOCALBUS_FPGA_ADDR + 0x1000+0x80) 
#define InvLocalFPGABaseAdrs1 (AVME7600_LOCALBUS_FPGA_ADDR + 0x1000+0x100) 
#define InvLocalFPGABaseAdrs2 (AVME7600_LOCALBUS_FPGA_ADDR + 0x1000+0x180) 
#define ADLocalFPGABaseAdrs1  (AVME7600_LOCALBUS_FPGA_ADDR + 0x1000+0x200)
#define ADLocalFPGABaseAdrs2  (AVME7600_LOCALBUS_FPGA_ADDR + 0x1000+0x280)
#define ClockLocalFPGABaseAdrs  (AVME7600_LOCALBUS_FPGA_ADDR + 0x1000+0x300)

/*******************************************************************/

extern short switchINcmd;
extern short switch_DT_BTcmd;
extern short switchOUTcmd;
extern short preChargecmd;
extern short Ratiocmd;
extern double Id1,Id2,Iq1,Iq2;
extern double Id1_filter,Iq1_filter,Id2_filter,Iq2_filter;
#define TsCPU 0.000138888
/*************************************************************************************/
struct TX_7700_STRUCT
{
	short	boardID;
	short	liveCnt;
	short	short_variable[10];	 
	int		int_variable[10];	 
	float 	float_variable[10];	 	
};
struct RX_7700_STRUCT
{
	short	boardID;
	short	liveCnt;
	short	short_variable[10];	 
	int		int_variable[10];	 
	float 	float_variable[10];	 	
};
extern struct TX_7700_STRUCT tx_7700_data;
extern struct RX_7700_STRUCT *rx_7700_data_pointer;
extern struct RX_7700_STRUCT rx_7700_data; 
extern double ia,ib,ic;
extern short txHMI_livecnt;
extern short rxHMI_livecnt;
/*******************************************************************/
struct rxCCUStruct
{
unsigned int   livecnt;
unsigned int   timesec;
unsigned int   timemicrosec;
unsigned int   runCmd;
unsigned int   GPS_OK;

float recKv1;
float recGama1;
float recKv2; 
float recGama2;

float invfreq;
float invKv1;
float invGama1;
float invKv2; 
float invGama2;

unsigned int vvvf1IncreFlag;
float Kp_uzRec;
float Ki_uzRec;

float INVPower;
short switchINcmd;
short switch_DT_BTcmd;
short switchOUTcmd;
short preChargecmd;
short Ratio;
short Discharge_cmd;

short changestep_flag;
short changeStep_SST_Flag;
 
};

struct txCCUStruct
{
unsigned int   begin;

unsigned int   livecnt;
unsigned int   inv1PWMmode;
unsigned int   inv2PWMmode;
UINT32   selfcheck;
unsigned int   runstate;
unsigned int   faultLevel;


float Udc;  
 
float invIu1; 
float invIv1; 
float invIw1; 

float invUa1;
float invUb1; 
float invUc1;

float invIu2; 
float invIv2; 
float invIw2;

float invUa2; 
float invUb2; 
float invUc2; 
 
float IaMotor; 
float IbMotor; 
float IcMotor;  

short return_switchINcmd;
short return_switch_DT_BTcmd;
short return_switchOUTcmd;

unsigned int end;
};


/*******************************************************************/
/*发送给FPGA SRIO从板的结构体定义，各板卡基本一致*/
struct TX_PWM_SRIO_Struct
{
	/*general registers setting*/
	unsigned short  boardID ;//0
	unsigned short  srio_deviceID ;    
	unsigned short  tx_liveCnt ; 	
	unsigned short  tx_frameLen ;
	unsigned short  tx_frameType ;//unsopported here
	unsigned short  clkDivide;
	unsigned short  timerPeriodShadow ;		
	unsigned short  TimerEnable ;		
	unsigned short  clrFaultCmd ;	
	unsigned short  reservered0; //9
	/*PWM special registers setting*/		
	unsigned short  ACQ_ZRO;  //10
	unsigned short  ACQ_CAU;
	unsigned short  ACQ_PRD;
	unsigned short  ACQ_CAD;
	unsigned short  CMPAU1;
	unsigned short  CMPAD1;
	unsigned short  CMPAU2;
	unsigned short  CMPAD2;
	unsigned short  CMPAU3;
	unsigned short  CMPAD3;
	unsigned short  CMPAU4;//20
	unsigned short  CMPAD4;
	unsigned short  CMPAU5;
	unsigned short  CMPAD5;
	unsigned short  CMPAU6;
	unsigned short  CMPAD6;
	unsigned short  minPulseTime;/*min pulse time 10us,50M clock */
	unsigned short  deadZone;/*dead zone time 50us,50M clock */
	unsigned short  turnOffTime;/*IGCT1&4 turn off time befoe IGCT2&3 */
	unsigned short  PwmEnable;/*pwm disable */	
	unsigned short  protectEn;/*small pulse and deadzone and feedback protect enable*///30
	
	unsigned short  CMPR_shadow13;//31
	unsigned short  CMPR_shadow14;//32
		
	unsigned short  ANPC_type_a;//33
	unsigned short  ANPC_type_b;//34
	unsigned short  ANPC_type_c;//35
	
	
	/*结构体长度必须是4的整数倍*/
};
/*******************************************************************/
/*接收PWM从板通过SRIO发送过来的数据的结构体定义*/
struct RX_PWM_SRIO_Struct
{
	unsigned short	BoardID; //0
	unsigned short	srio_deviceid;
	unsigned short  rx_liveCnt;
	unsigned short  srio_status; 
	unsigned short	h_tc_error;
	unsigned short  rx_frameType;
	unsigned short	rx_size;
	unsigned short  tx_liveCnt_fb;		 
	
	//general register
	unsigned short  TimerEnable;		//8
	unsigned short  timerPeriodShadow;		
	unsigned short  clkDivide;
	unsigned short  timerDirection;
	unsigned short  timer_cnt;
	unsigned short  synchronous_cnt;		
	unsigned short  bus_err_out;			//error info form this board,are outputed to back board bus		
	unsigned short  global_err_state;		//whole error state sum	
	
	unsigned short  PWM_shortCuirtErr;
	unsigned short 	minPulseErr;
	unsigned short  deadZoneErr;
	unsigned short 	pwmfb_err;  //19
	

	unsigned short  reservered1;//20~27
	unsigned short  reservered2;
	unsigned short  reservered3;
	unsigned short  reservered4;
	unsigned short  reservered5;
	unsigned short  reservered6;
	unsigned short  reservered7;
	unsigned short  reservered8;
	
	
	unsigned short DeadZoneErr_A;//28
	unsigned short DeadZoneErr_B;
	unsigned short DeadZoneErr_C;
	
	unsigned short MinPulseErr_A; 
	unsigned short MinPulseErr_B; 
	unsigned short MinPulseErr_C;
	
	unsigned short PWM_shortCuirtErr_ANPC;//34
	
};
/******
 * *************************************************************/
/*发送给AD SRIO从板的结构体定义，各板卡基本一致*/
struct TX_AD_SRIO_Struct
{
	/*general registers setting*/
	unsigned short  boardID ;
	unsigned short  srio_deviceID ;
	unsigned short  tx_liveCnt ; 	
	unsigned short  tx_frameLen ;
	unsigned short  tx_frameType ;//unsopported here
	unsigned short  clkDivide;
	unsigned short  timerPeriodShadow ;		
	unsigned short  TimerEnable ;		
	unsigned short  clrFaultCmd ;	
	unsigned short  reservered0;	
	/*AD special registers setting*/
	unsigned short  softProtect_value[16];//8.0/10.0*8192;//8V		
	unsigned short  softProtect_en;//every bit 1 channel
	unsigned short  hardProtect_en;//every bit 1 channel	
}; 
/*******************************************************************/
/*接收PWM从板通过SRIO发送过来的数据的结构体定义*/
struct RX_AD_SRIO_Struct
{
	unsigned short	BoardID;
	unsigned short	srio_deviceid;
	unsigned short  rx_liveCnt;
	unsigned short  srio_status; 
	unsigned short	h_tc_error;
	unsigned short  rx_frameType;
	unsigned short	rx_size;
	unsigned short  tx_liveCnt_fb;		 
	
	//general register
	unsigned short  TimerEnable;		
	unsigned short  timerPeriodShadow;		
	unsigned short  clkDivide;
	unsigned short  timerDirection;
	unsigned short  timer_cnt;
	unsigned short  synchronous_cnt;		
	unsigned short  bus_err_out;			//error info form this board,are outputed to back board bus		
	unsigned short  global_err_state;		//whole error state sum	
	/*AD special registers */
	unsigned short  AD_chanel_data[16];		
	unsigned short  softProtect_flag;
	unsigned short  hardProtect_flag; 
	
};
/*******************************************************************/
/*发送给CLK SRIO从板的结构体定义，各板卡基本一致*/
struct TX_CLK_SRIO_Struct
{
	/*general registers setting*/
	unsigned short  boardID ;
	unsigned short  srio_deviceID ;
	unsigned short  tx_liveCnt ; 	
	unsigned short  tx_frameLen ;
	unsigned short  tx_frameType ;//unsopported here
	unsigned short  clkDivide;
	unsigned short  timerPeriodShadow ;		
	unsigned short  TimerEnable ;		
	unsigned short  clrFaultCmd ;	
	unsigned short  reservered0;
	/*clock board special registers setting*/
	unsigned short  GPS_Channel;
	unsigned short  PwmEnable;
};
/*******************************************************************/
struct RX_CLK_SRIO_Struct
{
	unsigned short	BoardID;
	unsigned short	srio_deviceid;
	unsigned short  rx_liveCnt;
	unsigned short  srio_status; 
	unsigned short	h_tc_error;
	unsigned short  rx_frameType;
	unsigned short	rx_size;
	unsigned short  tx_liveCnt_fb;		 
	
	//general register
	unsigned short  TimerEnable;		
	unsigned short  timerPeriodShadow;		
	unsigned short  clkDivide;
	unsigned short  timerDirection;
	unsigned short  timer_cnt;
	unsigned short  synchronous_cnt;		
	unsigned short  bus_err_out;			//error info form this board,are outputed to back board bus		
	unsigned short  global_err_state;		//whole error state sum	
	/*clock board special registers feedback*/
	unsigned short  GPS_OK;		
	unsigned short  MinSecBuf;
	unsigned short  DayHrBuf; 
	unsigned short  YrHDayBuf;
	
};
/*******************************************************************/
 
/*******************************************************************/
 
struct RecStruct
{
	float Uab;
	float Ubc;
	float Ia;
	float Ib;
	float Ic;
	float Ua;
	float Ub;
	float Uc;
	float Ud;
	float Uq;
	float Id;
	float Iq;
	float theta;
 
} ;
/*******************************************************************/
struct RUNSTATE_BITS
{
	UINT16 reservd:7;
	UINT16 clrFault:1;
	UINT16 fault:1;
	UINT16 disCharge:1;
	UINT16 stop:1;
	UINT16 invRun:1;
	UINT16 UdcOk:1;
	UINT16 charge:1;
	UINT16 preCharge:1;
	UINT16 selfCheck:1;
	 
};

union RUNSTATE_REG
{
	struct RUNSTATE_BITS bit;
	UINT16 word;
};
/*******************************************************************/
struct RUNCMD_BITS
{
	UINT16 reservd:12;
	UINT16 clrFault:1;/*bit3*/
	UINT16 disCharge:1;
	UINT16 invRun:1;/*bit2*/
	UINT16 recRun:1;/*bit1*/
	UINT16 preCharge:1;/*bit0*/
};

union  RUNCMD_REG
{
	UINT16 word;
	struct RUNCMD_BITS bit;
};
/*******************************************************************/
struct selfCheck_BITS
{
	UINT16 reservd:2;/*bit30~31*/
	
	UINT16 DABoardOK:1;	 /*bit29*/	 /*not be musted*/
	UINT16 pfbAb2OnLine:1;
	
	UINT16 pfbAb2Run:1;	
	UINT16 pfbAb2OK:1; /*bit26*/
	UINT16 pfbAb1OnLine:1;/*bit25*/
	UINT16 pfbAb1Run:1;	
	
	UINT16 pfbAb1OK:1;
	UINT16 pfbBoardOK:1; /*bit22*/
	UINT16 chopSrioConnectOk:1;
	UINT16 chopBoardOK:1; /*bit20*/
		
	UINT16 ad2SrioConnectOk:1;/*bit19*/
	UINT16 ad1SrioConnectOk:1;/*bit18*/
	UINT16 inv2SrioConnectOk:1;/*bit17*/		
	UINT16 inv1SrioConnectOk:1;	/*bit16*/ 
	
	UINT16 rec2SrioConnectOk:1;/*bit15*/
	UINT16 rec1SrioConnectOk:1;/*bit14*/
	UINT16 clkSrioConnectOk:1;/*bit13*/
	UINT16 ast7700SrioInitOK:1;/*bit12*/	
   
    UINT16 GPSOK:1;			/*bit11*/
    UINT16 ccuConnnetOK:1;/*bit10*/	
    UINT16 pmc5565OK:1;/*bit9*/
    UINT16 IOBoardOK:1;	/*bit8*/
    
	UINT16 adBoard2OK:1;  /*bit7*/  
	UINT16 adBoard1OK:1;/*bit6*/
	UINT16 invBoard2OK:1;/*bit5*/	
	UINT16 invBoard1OK:1;/*bit4*/
	
	UINT16 recBoard2OK:1;/*bit3*/
	UINT16 recBoard1OK:1;/*bit2*/
    UINT16 clkBoardOK:1;/*bit1*/
    UINT16 ast7700VMEOK:1;/*bit0*/
};
/*******************************************************************/
union selfCheck_REG
{
struct selfCheck_BITS bit;
UINT32 word;
};
/*******************************************************************/
struct convfaultcode1_BITS 
{
	UINT16 reservd:16;
	
	UINT16 Udc_Nobalance:1;
	UINT16 Udc_overVoltage:1;
	UINT16 Idc2_overcurrent:1;
	UINT16 Idc1_overcurrent:1;
	
	UINT16 invIw2_overcurrent:1;
	UINT16 invIv2_overcurrent:1;
	UINT16 invIu2_overcurrent:1;
	UINT16 invIw1_overcurrent:1;
	
	UINT16 invIv1_overcurrent:1;
	UINT16 invIu1_overcurrent:1;
	UINT16 recic2_overcurrent:1;
	UINT16 recib2_overcurrent:1;
	
	UINT16 recia2_overcurrent:1;
	UINT16 recic1_overcurrent:1;
	UINT16 recib1_overcurrent:1; 
	UINT16 recia1_overcurrent:1;
 
};
union convfaultcode1_REG
{
	struct convfaultcode1_BITS bit;
	UINT32 word;
};
/*******************************************************************/
struct convfaultcode2_BITS 
{
 
	UINT16 reservd:12;
	UINT16 pfbBoardErr:1;
	
	UINT16 ad2SrioConnectErr:1;/*bit19*/
	UINT16 ad1SrioConnectErr:1;/*bit18*/
	UINT16 inv2SrioConnectErr:1;/*bit17*/	
	UINT16 inv1SrioConnectErr:1;	/*bit16*/ 
	
	UINT16 rec2SrioConnectErr:1;/*bit15*/
	UINT16 rec1SrioConnectErr:1;/*bit14*/
	UINT16 clkSrioConnectErr:1;/*bit13*/
	UINT16 ast7700SrioInitErr:1;/*bit12*/	 
	
	UINT16 GPSErr:1;
	UINT16 ccuConErr:1;
	UINT16 pmc5565Err:1;
	UINT16 ioBoardErr:1;
	
	UINT16 adBoard2Err:1;
	UINT16 adBoard1Err:1;
	UINT16 invBoard2Err:1;
	UINT16 invBoard1Err:1;
	
	UINT16 recBoard2Err:1;
	UINT16 recBoard1Err:1;
	UINT16 clkBoardErr:1;
	UINT16 ast7700BoardErr:1;
        
};
union convfaultcode2_REG
{
	struct convfaultcode2_BITS bit;
	UINT32 word;
};

struct IGBTFault_BITS   //0826 SPK IGBT故障信息
{
	UINT16 reservd:16;
	  
	UINT16 inv2_igctfault;
	UINT16 inv2_MinPulseErr;
	UINT16 inv2_DeadZoneErr;
	UINT16 inv2_errstate;   
	
	UINT16 inv1_igctfault;
	UINT16 inv1_MinPulseErr;
	UINT16 inv1_DeadZoneErr;
	UINT16 inv1_errstate;
	
	UINT16 rec2_igctfault:1;
	UINT16 rec2_MinPulseErr:1;
	UINT16 rec2_DeadZoneErr:1;
	UINT16 rec2_errstate:1;
	
	UINT16 rec1_igctfault:1;
	UINT16 rec1_MinPulseErr:1;
	UINT16 rec1_DeadZoneErr:1;
	UINT16 rec1_errstate:1;
        
};
union IGBTFault_REG
{
	struct IGBTFault_BITS bit;
	UINT32 word;
};

struct faultcode_REG
{
   union convfaultcode1_REG  convfaultcode1;
   union convfaultcode2_REG  convfaultcode2;
   union IGBTFault_REG       IGBTFault;
   unsigned short rec1_igctfault;
   unsigned short rec1_MinPulseErr;
   unsigned short rec1_DeadZoneErr;
   unsigned short rec1_errstate;
   unsigned short rec2_igctfault;
   unsigned short rec2_MinPulseErr;
   unsigned short rec2_DeadZoneErr;
   unsigned short rec2_errstate;
   unsigned short inv1_igctfault;
   unsigned short inv1_MinPulseErr;
   unsigned short inv1_DeadZoneErr;
   unsigned short inv1_errstate;
   unsigned short inv2_igctfault;
   unsigned short inv2_MinPulseErr;
   unsigned short inv2_DeadZoneErr;
   unsigned short inv2_errstate;
   
   unsigned short rec1_DeadZoneErr_A;
   unsigned short rec1_DeadZoneErr_B;
   unsigned short rec1_DeadZoneErr_C;
   unsigned short rec1_PWM_shortCuirtErr_ANPC;
   unsigned short rec1_MinPulseErr_A; 
   unsigned short rec1_MinPulseErr_B; 
   unsigned short rec1_MinPulseErr_C; 
   
   unsigned short inv1_DeadZoneErr_A;
   unsigned short inv1_DeadZoneErr_B;
   unsigned short inv1_DeadZoneErr_C;
   unsigned short inv1_PWM_shortCuirtErr_ANPC;
   unsigned short inv1_MinPulseErr_A; 
   unsigned short inv1_MinPulseErr_B; 
   unsigned short inv1_MinPulseErr_C; 

};
/*******************************************************************/
extern union RUNCMD_REG     runCmd;
//extern union RUNSTATE_REG   runState,runState1;
extern union selfCheck_REG  selfCheck;
extern struct faultcode_REG faultCode; 
extern unsigned int runState; 
/*******************************************************************/
 
extern struct TX_PWM_SRIO_Struct  txRecSrioData1;
extern struct TX_PWM_SRIO_Struct  txRecSrioData2;
extern struct TX_PWM_SRIO_Struct  txInvSrioData1;
extern struct TX_PWM_SRIO_Struct  txInvSrioData2;
extern struct TX_AD_SRIO_Struct   txAdSrioData1;
extern struct TX_AD_SRIO_Struct   txAdSrioData2;
extern struct TX_CLK_SRIO_Struct  txClkSrioData;

extern struct RX_PWM_SRIO_Struct  *rxRecSrioData1;
extern struct RX_PWM_SRIO_Struct  *rxRecSrioData2;
extern struct RX_PWM_SRIO_Struct  *rxInvSrioData1;
extern struct RX_PWM_SRIO_Struct  *rxInvSrioData2;
extern struct RX_AD_SRIO_Struct   *rxAdSrioData1;
extern struct RX_AD_SRIO_Struct   *rxAdSrioData2;
extern struct RX_CLK_SRIO_Struct  *rxClkSrioData; 


extern struct rxCCUStruct  rxccudata;
extern struct txCCUStruct  txccudata;
extern double Ts;
extern int txPWMSrioLen ;
extern int rxPWMSrioLen ;
extern int txADSrioLen ;
extern int rxADSrioLen ;
extern int txClkSrioLen ;
extern int rxClkSrioLen ;
/*******************************************************************/
extern unsigned int   RxCcuRfmBaseAdrs;
extern unsigned int   TxCcuRfmBaseAdrs; 
extern char* RxCcuLocalBaseAdrs; 
extern char* TxCcuLocalBaseAdrs; 
extern char* RecLocalBaseAdrs1; 
extern char* RecLocalBaseAdrs2;  
extern char* InvLocalBaseAdrs1; 
extern char* InvLocalBaseAdrs2; 
extern char* ADLocalBaseAdrs1; 
extern char* ADLocalBaseAdrs2; 
extern char* ClkLocalBaseAdrs;/*CLK_FPGA基地址对应的vme本地地址*/
extern char* DALocalBaseAdrs;
extern char* IOLocalBaseAdrs;
extern char* PfbLocalBaseAdrs;
extern char* ast7700LocalBaseAdrs;
/*******************************************************************/
extern  short interrupt_index;
extern  short 	interrupt_index;
extern  short 	interrupt_index_next ;
extern  short 	shutOffCmd;
extern  short 	runcmd;
extern  double dccmd,syn_control_flag;
extern  short 	runcmd_old;
extern  unsigned short faultFlag;
extern  unsigned int   recigctfaultcode1;
extern  unsigned int   invigctfaultcode1;
extern  unsigned int   recigctfaultcode2;
extern  unsigned int   invigctfaultcode2;

extern  float Udc,VdcRec;
extern  float Udc_ref; 
extern  int firstrec;
extern float GridUa,GridUb,GridUc,GridIa,GridIb,GridIc;
extern float IaMotor,IbMotor,IcMotor;
extern float IaMotor_old,IbMotor_old,IcMotor_old;
extern float invUa1,invUb1,invUc1,invUa2,invUb2,invUc2;
extern  float recIa1,recIb1,recIc1,recUab1,recUbc1,recUca1,recIa2,recIb2,recIc2,recUab2,recUbc2,recUca2,Udc1,Udc2,Idc1,Idc2,invIu1,invIv1,invIw1,invIu2,invIv2,invIw2;
extern  float recUa1,recUb1,recUc1,recUd1,recId1,recIq1,invId1,invIq1,recTheta1;
extern  float recUa2,recUb2,recUc2,recUd2,recId2,recIq2,invId2,invIq2,recTheta2; 
extern float invfreq,invfreqpre,Kp_motor,theta_syn_SHE_flag,invfreq_filter; 
extern int up1dn0flag;
extern double speed_out1,theta,omega,freq_cmd,theta_zero,speed_pre_filter,speed_cmd,speed_cmdend,freq_cmdend,id_cmd,iq_cmd,uref_delta,uref_delta1,uref_delta2,theta_current_dir,phase_2pi;
extern double theta_syn,theta_syn_rl,iout_d,iout_q,uref_syn_d,uref_syn_q,iout_d_filter,iout_q_filter,ud_for,uq_for,uref_syn_d_filter,uref_syn_q_filter;
extern double theta_syn_pre,theta_syn_filter,theta_syn_bu,temp_theta,flag_theta,flag_theta,theta_syn_filter_old,theta_syn_SHE;
extern double define_position_flag,invfreq_flag,testcmd2;
extern double speed_out,inv_a,inv_b,inv_c,kv1_filter,kv2_filter,kv2_filter_pre,kv1_filter_pre,Serial_cmd,Parallel_cmd,inv_a_sec,inv_b_sec,inv_c_sec;
extern float recIa1_filter,recIb1_filter,recIc1_filter,recIa2_filter,recIb2_filter,recIc2_filter;
extern float invIu1_filter,invIv1_filter,invIw1_filter,invIu2_filter,invIv2_filter,invIw2_filter;
extern int Udc1flag,Udc2flag;
extern float Udc1_rec2,Udc2_rec2;
extern int PWMRoute,svmtest,changestep_gaozhan_dn,changestep_gaozhan_up;
extern short changestep_SST_Flag;

extern  short  adData1[16];
extern  short  adData2[16];
extern  double DeltaAngle_Inv,Kv_Inv,uref_delta_inv,Kv_Rec,VFGama_Inv,VFGama_Rec ;
extern double inv_Kv1,inv_Kv2,Kv_Inv_from_LK,invfreq_from_LK,Kv_cnt;
extern double Udc1_filter,Udc2_filter,Udc1_rec2_filter,Udc2_rec2_filter;
extern double recUa2_grid,recUb2_grid,recUc2_grid;
extern  float  invfreq;
extern  double Is_alpha, Is_belta;
extern  double Isd, Isq,Isd_filter,Isq_filter;
extern  double Isd2, Isq2,Isd_filter2,Isq_filter2;
extern  double Iderr,Iqerr;
extern  double vd,vdctrl;
extern  double vq,vqctrl,vq_rec1,vq_rec2,vd_rec1,vd_rec2;
extern  double usdFw_rec1_filter,usdFw_rec2_filter,usqFw_rec1_filter,usqFw_rec2_filter;
extern  double Us_alpha,Us_belta,usdFw_rec1,usqFw_rec1,usdFw_rec2,usqFw_rec2;
extern  double GridFOC_FeedUGama,u_alph_Rect,u_beta_Rect,UsRefRec;
extern double rec_gama1,rec_gama2,reccmd2,reccmd1,gamaYD,gama_filter,gama_filter1;
extern  double vq_filter,vd_filter,u_alph_Rect_filter,u_beta_Rect_filter,rec_gama_filter1,rec_gama_filter2_bu,rec_gama_filter1_bu,rec_gama_filter2,UsRefRec_filter;
extern unsigned short prd;
extern unsigned short prd1;
extern double verr;
unsigned short frecout;
extern double m1,m2,m3;

extern unsigned short dir;

extern unsigned short phasedate_dir_reg;
extern double phase_dir_reg;

extern unsigned short QEPA;
extern unsigned short phasedata;
extern unsigned short phasedata_z;
extern unsigned short phasedata_z_initial;

extern signed short phasedata_dir,z_flag;
extern unsigned short phasedata_z_dir;

extern double phase_z,phase_z_initial;
extern double phase_dir,phase_z_dir,phase_z_initial_dir;
extern unsigned short phasedata_z_initial_dir;
extern unsigned short prddata;
//unsigned short encoderLine=1024;
extern unsigned short encoderLine;
extern double speed1,speed1_last;
extern double speed2,speed3_last;
extern double speed3;
extern double speed,firstspeed;
extern double speed_pre;
extern double vvvf1IncreFlag,fdecreflag,fincreflag;
extern double phase;
extern double initial_phase;
extern short p; 
extern float invfreq_pre;
extern int motorstart,intCounterspeed,resettime;
extern int  Timer_Counter,start_dc_balance_flag;
extern unsigned int PWM_shortCuirterr,bus_err_other,inv1_MinPulseErr,inv1_DeadZoneErr,inv1_igctfault,inv1_errstate;
extern int stop_inv_flag,stop_rec_flag,discharge_flag,reset_flag,err_stop_flag,discharge_cnt,start_watercool,err_stop_flag_cnt;
extern int wait90ms,step,wait1s;
extern short IntFlagTest,PeriodTest,PeriodTest2,ZROTest,CAUTest,PRDTest,CADTest;
extern short CMPAU1Test,CMPAU2Test,CMPAU3Test,CMPAU4Test,CMPAU5Test,CMPAU6Test;
extern short CMPAD1Test,CMPAD2Test,CMPAD3Test,CMPAD4Test,CMPAD5Test,CMPAD6Test,SyncFlagTest;

extern int 	TotalNumStore ;
extern double 	FreqWithDire ;  // 带方向的频率，用于插值限制角度回撤
extern double  VFGamaWithDire ;
extern short   InterpolationCnt ;
extern int 	CntKv ;
extern int 	CntTestConti ;
extern unsigned short CMPASignDetection ;
extern double TempRoute;
extern short 	InvIntFlag ,InvIntFlag2 ;
extern unsigned int KvCnt ,KvCnt_2 ,KvIncre1Decre0 ,KvIncre1Decre0_2;

extern double speed_out,speed_out1,theta,omega,freq_cmd,theta_zero,speed_pre_filter,speed_cmd,speed_cmdend,freq_cmdend,id_cmd,iq_cmd,uref_delta,uref_delta1,uref_delta2,theta_current_dir,phase_2pi;
extern double theta_syn,theta_syn_rl,iout_d,iout_q,uref_syn_d,uref_syn_q,iout_d_filter,iout_q_filter,ud_for,uq_for,uref_syn_d_filter,uref_syn_q_filter;
extern double theta_syn_pre,theta_syn_filter,theta_syn_bu,temp_theta,flag_theta,flag_theta,theta_syn_filter_old,theta_syn_SHE;
extern float Iinv_alpha,Iinv_beta,Iinv_d,Iinv_q,Iinv_d_filter,Iinv_q_filter,Iinv_rms,Urec_rms,Irec_rms;
extern float Irec_alpha,Irec_belta,Irec_d,Irec_q,Irec_d_filter,Irec_q_filter;
extern float Iinv_alpha2,Iinv_beta2,Iinv_d2,Iinv_q2,Iinv_d_filter2,Iinv_q_filter2,Iinv_rms2,Urec_rms2,Irec_rms2,Uinv_rms,Uinv_rms2,power_factor;
extern float Grid_Irms;
extern float Irec_alpha2,Irec_belta2,Irec_d2,Irec_q2,Irec_d_filter2,Irec_q_filter2;
extern double Udc_protect_LK;
extern double UdcNobalane_protect_LK;
extern double Iinv_protect_LK;
extern double Irec_protect_LK;

extern double Idref_udc,Idref_udc_slop,Id_pf,idref_cnt,Idref;
extern double idrec_filter,iqrec_filter;
extern double idrec1_filter,iqrec1_filter;
extern double idrec2_filter,iqrec2_filter;
extern double Theta_Secd;
extern double UdcSet;
extern float Num_temp;
extern double iq_cmd_ref;  //有速度环时用，无速度环时屏蔽其和声明

extern float Ua_delay,Ub_delay,Uc_delay,Uan_in_theta,Uan_in_theta_cos,Ubn_in_theta,Ubn_in_theta_cos,cosphi_delay,sinphi_delay;
extern float Ucn_in_theta,Ucn_in_theta_cos,ism_ref,iLa_ref,iLb_ref,iLc_ref,Uan_PRout,Ubn_PRout,Ucn_PRout;
extern float Uan_ref_Rec,Ubn_ref_Rec,Ucn_ref_Rec,RL_as_compensate,RL_bs_compensate,RL_cs_compensate;
extern float testgama1,testgama2,recIagama;
extern int  	intCounter1;
extern int   	intCounter2;
extern int   	intConnectStatus;

extern short 	datemp; 
extern short 	statecnt;
extern UINT32 	astTestLocalbusFpgaIrqIsr1Do;

//double Idref = 0.0;
//double Iqref_udc=0;
/************************************************************************************/
extern  SEM_ID 	fpgaIoSem;
/*******************************************************************/
extern unsigned short invPwmMode;
extern unsigned short recPWMDIR;
extern unsigned short invPWMDIR;
/*******************************************************************/
extern unsigned short MainSwitch,Start_Rec,Start_Inv,Stop_Inv,Stop_Rec,Position,Discharge,Reset,Precharge_LK;
extern unsigned short Parallel_LK,Serial_AUTO,moderoute,switchflag,closeflag,testgz,Parallel_AUTO,Serial_LK,AUTO_model_LK,Precharge_OK;
extern unsigned short Parallel_Model,Serial_Model,AUTO_Model,time1freq0,time1freq0_LK;
extern double Serial_AUTO_time,Parallel_AUTO_time,auto_time,close_time;
extern double wait_ns,Parallel_time,Parallelroute,Parallel_AUTO_time;
extern int err1,err2,err3,err4,err5,err6,err7,err8,err9,err10,err11,err12,err13,err14,err15,err16,err17,err18,err19,err20,err21,err22,err23,err24,err25,err26,err27,err28,err29,err30,err31,err32,err33,err34,err35,err36,err37,err38,err39,err40,err41,err42,err43,err44,err45,err46;
extern int err50,err51,err52,err53,err54,err55;
extern short err[16];
extern int statenum1,statenum2,statenum3,statenum4,statenum5,statenum6,statenum7;
extern int statenum8,statenum9,statenum10,statenum11,statenum12,statenum13;

extern int Inv_type_a,Inv_type_b,Inv_type_c,Rec_type_a,Rec_type_b,Rec_type_c;//ANPC type
extern float Rec_type_cnt,Inv_type_cnt;
extern void Varible_Init(void);
extern void Varible_Init_first(void);

extern double Ud_Fwd;
extern short changestep;
extern double Iq_h, Iq_belta_01, Iq_belta_02, Iq_belta1, Iq_belta2, Iq_b, Iq_r;
extern double Id_h, Id_belta_01, Id_belta_02, Id_belta1, Id_belta2, Id_b, Id_r;
extern double Udc_h, Udc_belta_01, Udc_belta_02, Udc_belta1, Udc_belta2, Udc_b, Udc_r;
 
extern float chopStart_Flag;
/*HMI Variables*/
extern unsigned int FaultFlagHMI;
extern unsigned int ANPC_MODE;

#ifdef __cplusplus
}
#endif

#endif
