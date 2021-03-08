

#ifndef _SUBMOD_DATA_DISPLAYUDP_
#define _SUBMOD_DATA_DISPLAYUDP_
#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <types.h>
#include <socket.h>
#include <in.h>
#include "vxworks.h"
#include "msgQLib.h"
#include "logLib.h"
#include "stdio.h"
#include "taskLib.h"


#define TEST_NETWORK

/*step1: set IP Address and port Number*//*109 108*/

//#define RECEIVE_PC_IP     "172.16.11.200"
#define RECEIVE_PC_IP     "172.16.11.126"
#define RECEIVE_PC_PORT   (1002)  

//#define RECEIVE_PC_VOLTAGE_FAULT_IP "172.16.11.240"
#define RECEIVE_PC_VOLTAGE_FAULT_IP "172.16.11.223"
#define RECEIVE_PC_VOLTAGE_PORT (1000)
#define RECEIVE_PC_FAULT_PORT   (1001)
 



/*step2: init UDP*/
int SubMod_Data_DisplayUDP_Init(void);

/*step3: update data need to display*/
struct HIGH_RATE_DISPLAY_SAVE       /*可存储的数据24路 2K每秒，一共6个窗口，每个窗口4路波形*/
{
	INT16 Screen1[4];
	INT16 Screen2[4];
	INT16 Screen3[4];
	INT16 Screen4[4];
	INT16 Screen5[4];
	INT16 Screen6[4];
};

struct MODU_VOLTAGE_DISPLAY         /* 14*6 = 84+1 = 85   100个数据每秒*/
{
	INT16 VOLTAGE_U_UP[14];
	INT16 VOLTAGE_U_DOWN[14];

	INT16 VOLTAGE_V_UP[14];
	INT16 VOLTAGE_V_DOWN[14];

	INT16 VOLTAGE_W_UP[14];
	INT16 VOLTAGE_W_DOWN[14];

	INT16 VOLTAGE_DC;
};

struct TEMPR_DISPLAY                /*14*6 = 84 + 6 = 90   1个数据每秒*/
{
	INT16 TEMPR_U_UP[14];
	INT16 TEMPR_U_DOWN[14];

	INT16 TEMPR_V_UP[14];
	INT16 TEMPR_V_DOWN[14];

	INT16 TEMPR_W_UP[14];
	INT16 TEMPR_W_DOWN[14];
};

struct FAULT_DISPLAY 
{
	INT16 FAULT_PMC_U_UP[14];
	INT16 FAULT_PMC_U_DOWN[14];
	INT16 FAULT_PMC_V_UP[14];
	INT16 FAULT_PMC_V_DOWN[14];
	INT16 FAULT_PMC_W_UP[14];
	INT16 FAULT_PMC_W_DWON[14];

	INT16 FAULT_GRID;
	INT16 FAULT_CONVERTER[4];
	INT16 FAULT_VCM[2];
	INT16 FAULT_VGC;
};

struct SORT_DISPLAY
{
	INT16 SORT_U_UP[14];
	INT16 SORT_U_DOWN[14];

	INT16 SORT_V_UP[14];
	INT16 SORT_V_DOWN[14];

	INT16 SORT_W_UP[14];
	INT16 SORT_W_DOWN[14];
};

struct TEMPR_FAULT_DISPLAY
{
	struct TEMPR_DISPLAY temprDisplay;
	struct FAULT_DISPLAY faultDisplay;
	struct SORT_DISPLAY sortDisplay;
};

extern struct HIGH_RATE_DISPLAY_SAVE HighRateDisplaySaveBufMsg; 


/*step4: send data to PC by UDP*/
/*send by message*/
void taskSendHighRateData_24_4K(void);
void taskSendVoltageData_84_100(void);
void taskSendFaultTemprData_x_1(void);

/*Final: close UDP*/
void SubMod_Data_DisplayUDP_Close(void);

#ifdef __cplusplus
}
#endif

#endif

