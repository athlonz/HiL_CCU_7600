#ifndef _AST7600SRIO_
#define _AST7600SRIO_

#ifdef __cplusplus
extern "C" {
#endif


#define AST_PPC_SRIO_DMA_INBOUND_BASE	(0xf0000000)
#define AST_PPC_SRIO_DMA_INBOUND_TAR	(0x78000000)
#define AST_PPC_SRIO_DMA_INBOUND_SIZE	(16*1024*1024)

#define AST_PPC_SRIO_DMA_OUTBOUND_BASE	(0xa8000000)
#define AST_PPC_SRIO_DMA_OUTBOUND_TAR	(0xf0000000)
#define AST_PPC_SRIO_DMA_OUTBOUND_SIZE	(16*1024*1024)



#define srioMutiSendID 20 
extern int  ast7600SrioID;



extern INT32 ast7600PpcSrioInit(void);
extern INT32 ast7600PpcSrioDmaInit( void );
extern INT32 ast7600PpcSrioSingleSendData( UINT16 destId, INT32 nbytes, UINT8 * pbuffer );/*单独发送各板卡*/
extern INT32 ast7600PpcSrioMutilSendData( INT32 nbytes, UINT8 * pbuffer );/*组播发送各板卡，主要是4个PWM板*/

#ifdef __cplusplus
}
#endif

#endif
