#ifndef _INCriomsgh
#define _INCriomsgh

#ifdef _cplusplus
extern "C" {
#endif

#define RIO_MSG_REGS_OFFSET     0x13000
#define OM_DESC_BASE0   0x1e000000
#define OM_DESC_BASE1   (OM_DESC_BASE0+0x10000)/*0x10000=(2048*32)*/
#define OM_BUF_BASE0 	0x12000000 
#define IM_BUF_BASE0  	(OM_BUF_BASE0 + 0x800000)
#define OM_BUF_BASE1  	(OM_BUF_BASE0 + 0x1000000)
#define IM_BUF_BASE1  	(OM_BUF_BASE0 + 0x1800000)
#define PW_BUF_BASE     0x1f000000

#define RIOQMSG_RX_MBOX0 0x00000001
#define RIOQMSG_RX_MBOX1 0x00000002	
	
	
/* function declare */
int srioMsgSend(UINT16, UINT8, UINT8, UINT8 *, UINT32, FUNCPTR, void *);
int srioMsgSetup(void *);
int srioPWriteSetup(void *);
int srioPWriteSend(UINT16, UINT8 *, UINT32, FUNCPTR, void *);



#ifdef _cplusplus
}
#endif

#endif
