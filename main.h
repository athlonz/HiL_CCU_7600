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
#include "h\astLocalbus.h"/*��BSP�ｫh�ļ��п����˹��̸�Ŀ¼�£�������������*/
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
#define TimerPeriod 75000000 /*7600��FPGA ��ʱ��ʱ75Mhz,�����Ϊ10khz�ж�Ƶ�ʣ���ô��ʱ������Ӧ����Ϊ7500*/
/*****************************���ļ����õľֲ�����**********************************************/
SEM_ID 	clkIntSem;	/*�ж��ź���*/ 
int    	clkIntTaskId;	/*����������Id*/
SEM_ID  fpgaIoSem;	/*�ж��ź���*/ 
/**********************���ļ����Ӻ�������**************************************************/
INT32 ast7600LocalbusFpgaIsr1( void );/*��ʱ���жϴ�����*/
void clkIntTask(void);		/*��ʱ���жϴ�������*/
void clr_fault(void);      /*������Ϻ���*/
void reset_clr_fault(void);  /*��λ������Ϻ���*/
/************************************************************************/


/**************�������*******************************************************************************************************/
SEM_ID clkIntSem;	    /*�ж��ź���*/ 
int    clkIntTaskId;	/*����������Id*/




