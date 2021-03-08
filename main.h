#include "vxWorks.h"
#include "taskLib.h"
#include "stdio.h"
#include "logLib.h"
#include "intLib.h"
#include "iv.h"  
#include "sysLib.h"
#include "vme.h"
#include "semLib.h"
#include "math.h"  
#include "ioLib.h"
#include "wdLib.h"
#include "sysLib.h"

#include "variable_main.h"
#include "GlobalVar_Rec.h"
#include "GlobalVar_Inv.h"
#include "slaveBoardRW.h"
#include "pwmControl.h" 
#include "subMod_DataDisplayUDP.h"
#include "subMod_ModbusTcp.h"
#include "subMod_Rfm2g.h" 
#include "closeControl.h"
#include "ast7600Srio.h" 
#include "h\astLocalbus.h"/*从BSP里将h文件夹拷到了工程根目录下，否则难以引用*/
#include "anybusPfb.h"
#include "subMod_ControlFunction.h"
/**************define run state machine**********************************************************/

//#define UDP_DISPLAY 1

#define WAIT 0 								  
#define SELFCHECK_OK 11
#define Precharge 1
#define CloseMainSwitch 2
#define REC_RUN 3
#define INV_RUN 5
#define STOP_INV 6
#define STOP_REC 7
#define DISCHARGE 8
#define RESET 9
#define FAULT 10
#define FindPosition 4
#define STOP 12
#define CLRFAULT 13
/*****************************************************************************/	    
#define TimerPeriod 75000000 /*7600上FPGA 的时钟时75Mhz,如果设为10khz中断频率，那么定时器周期应当设为7500*/
/*****************************本文件所用的局部变量**********************************************/
SEM_ID 	clkIntSem;	/*中断信号量*/ 
int    	clkIntTaskId;	/*收数据任务Id*/
SEM_ID  fpgaIoSem;	/*中断信号量*/ 
/**********************本文件下子函数声明**************************************************/
INT32 ast7600LocalbusFpgaIsr1( void );/*定时器中断处理函数*/
void clkIntTask(void);		/*定时器中断处理任务*/
void clr_fault(void);      /*清除故障函数*/
void reset_clr_fault(void);  /*复位清除故障函数*/
/************************************************************************/


/**************定义变量*******************************************************************************************************/
SEM_ID clkIntSem;	    /*中断信号量*/ 
int    clkIntTaskId;	/*收数据任务Id*/




