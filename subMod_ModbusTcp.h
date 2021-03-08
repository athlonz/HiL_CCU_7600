#ifndef _SUBMOD_DATA_MODBUS_TCP_X_
#define _SUBMOD_DATA_MODBUS_TCP_X_

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
#include "sockLib.h"
#include <socket.h>
#include <errnoLib.h>
/*#include <usrLib.h>*/
#include <string.h>
#include <stdio.h>
#include "in.h"
#include "ioLib.h"
#include "netinet/tcp.h"
/*************************************************************************/
#define echoServerPort (502)

#define LOBYTE_NORMAL(w) ((char)((INT16)(w) & 0xFFFF))
#define HIBYTE_NORMAL(w) ((char)(((INT16)(w) >> 8) & 0xFF)) 

#define WORD_NORMAL(HiWord, LowWord) ((((UINT16)HiWord) <<8) | (((UINT16)LowWord) & 0x00FF))


#pragma pack(2)/*In forcotrol modbus tcp, the registers address are arried by word(16bit),so the structruer should be packed by word */
typedef struct RT_DATA_X
{
	
//	UINT16 INT16[100];/*200 byte,0~199*/
//     UINT32 INT32[50];/*200 byte,200~399*/
//        float  FLOAT[50];/*200 byte,400~599*/
        
	UINT16 INT16[100];/*200 byte,0~99*/
	UINT32 INT32[50];/*200 byte,100~199*/
	float  FLOAT[50];/*200 byte,200~299*/
	UINT16 RX_INT16[100];/*200 byte,300~399*/
	float  FLOAT2[50];/*200 byte,400~499*/
	float  RX_FLOAT[50];/*200 byte,500~599*/

}RT_DATA;

typedef union RT_DATA_UNION_X
{
	RT_DATA RTData;
	UINT8 all[600];
}RT_DATA_UNION;
#pragma pack()/*end pack*/

typedef struct TCP_SEND_RECV_DATA_X
{
	UINT8 receBuff[550];
	UINT8 sendBuff[550];
	INT16 receNum;
	INT16 sendNum;
}TCP_SEND_RECV_DATA;

extern RT_DATA_UNION RTDataUnion;    /*外部变量，只需要对此变量赋值即可*/

STATUS subModeModbusTcpInit(void);
STATUS subModeModbusTcpClose(void);

#ifdef __cplusplus
}
#endif

#endif
