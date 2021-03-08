#ifndef _INCriosetuph
#define _INCriosetuph

#ifdef _cplusplus
extern "C" {
#endif
	

#define RIO_SEGMENT_1			0x00
#define RIO_SEGMENT_2			0x01
#define RIO_SEGMENT_4			0x02
#define RIO_SUBSEGMENT_1		0x00
#define RIO_SUBSEGMENT_2		0x01
#define RIO_SUBSEGMENT_4		0x02
#define RIO_SUBSEGMENT_8		0x03

	
#define INFO_LVL1   if(rio_Debug_lvl<=1) printf
#define INFO_LVL2   if(rio_Debug_lvl<=2) printf
#define INFO_LVL3   if(rio_Debug_lvl<=3) printf
#define INFO_LVL4   printf

#define LOG_LVL1   if(rio_log_lvl<=1) logMsg
#define LOG_LVL2   if(rio_log_lvl<=2) logMsg
#define LOG_LVL3   if(rio_log_lvl<=3) logMsg
#define LOG_LVL4   logMsg
	
#define MAX_DEVICES_IN_SYSTEM   256



/* struct define */	
typedef struct srio_ob
{
	UINT32 outboundNo;
	UINT32 outboundBase;
	UINT32 outboundAttr;
	UINT32 outboundTar;
	UINT32 port;
	UINT32 targetID;
	UINT8 segment;
	UINT8 subsegment;
	UINT32 wrtyp;
	UINT32 size;
}srio_ob_t;

typedef struct srio_ib
{
	UINT32 inboundNo;
	UINT32 inboundBase;
	UINT32 inboundAttr;
	UINT32 inboundTar;
	UINT32 port;
	UINT32 targetID;
	UINT32 size;
}srio_ib_t;


/* function declared */
int srio_ob_map(srio_ob_t srioOBound);
int srio_ib_map(srio_ib_t srioIBound);
int srioSetup(unsigned int ccsbar, unsigned int flag, unsigned int);
int srio_switch_route_set(UINT8 destid, UINT8 hopcount, UINT8 portnum, UINT16 id);
int srioDevBoot(void);
int srioFindDevNumGet(void);




#ifdef _cplusplus
}
#endif

#endif
