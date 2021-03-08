#define ChopVmeBaseAdrs 0x200000/*0x100000*/
/*发送给FPGA SRIO从板的结构体定义，各板卡基本一致*/
struct TX_CHOP_SRIO_Struct
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
 
	unsigned short  CMPR1;//10
	unsigned short  CMPR2;
	unsigned short  CMPR3;
	unsigned short  CMPR4; 
	unsigned short  minPulseTime;/*min pulse time 10us,50M clock */ 
	unsigned short  PwmEnable;
	unsigned short  protectEn;/*small pulse and deadzone and feedback protect enable*///30
	unsigned short  noused1[4];//38
 
	/*srio struct long must is 4*x*/
	

	
	/*结构体长度必须是4的整数倍*/
};
/*******************************************************************/
/*接收CHOP从板通过SRIO发送过来的数据的结构体定义*/
struct RX_CHOP_SRIO_Struct
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
	

	unsigned short 	minPulseErr;
	unsigned short 	pwmfb_err;  //19
	
	unsigned short  reservered0[8];//20~27


};

/***************************************************************************/
extern unsigned short chopSrioID;
extern unsigned short chopSrio_status	;//
extern unsigned short chopSrio_error;//
extern unsigned short chopSrio_igen_state;
extern unsigned short rxChopData_srio_rx_live_cnt_old;
extern struct TX_CHOP_SRIO_Struct  txChopSrioData;
extern struct RX_CHOP_SRIO_Struct  *rxChopSrioData;
extern int txChopSrioLen ;
extern int rxChopSrioLen ;
extern char* ChopLocalBaseAdrs;

/***************************************************************************/

void ChopInit_withSrio(void);
void checkChopSrioConnectStatus(void);
void enable_chopPWM(void);
void disable_chopPWM(void);
void chop_control(float Udc_input);
void disCharge(float Udc_input);
