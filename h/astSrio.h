#ifndef _AST_SRIO_H_
#define _AST_SRIO_H_


/* serial RapidIO register address define */
#define AST_SRIO_REG_BASE	( CCSBAR + 0xC0000 )				/* Base address of SRIO registers */
#define AST_SRIO_BDIDCSR	( AST_SRIO_REG_BASE + 0x0060 )		/* Base device ID command and status register */
#define AST_SRIO_PGCCSR		( AST_SRIO_REG_BASE + 0x013C )		/* Port General control command and status register */

/* srio dma struct */
#define AST_SRIO_OUTBOUND 	srio_ob_t
#define AST_SRIO_INBOUND	srio_ib_t

/* nnumber of dma channel */
#define AST_SRIO_DMA_CHANNEL_NUMBER	( 4 )

/* srio dma outbound number */
#define AST_SRIO_OUTBOUND_NUMBER	( 1 + 8 )
#define AST_SRIO_INBOUND_NUMBER		( 1 + 4 )

/* srio outbound segment & subsegment */
#define	AST_RIO_SEGMENT_1			( RIO_SEGMENT_1	)
#define AST_RIO_SEGMENT_2			( RIO_SEGMENT_2	)
#define AST_RIO_SEGMENT_4			( RIO_SEGMENT_4	)
#define AST_RIO_SUBSEGMENT_1		( RIO_SUBSEGMENT_1 )
#define AST_RIO_SUBSEGMENT_2		( RIO_SUBSEGMENT_2 )
#define AST_RIO_SUBSEGMENT_4		( RIO_SUBSEGMENT_4 )
#define AST_RIO_SUBSEGMENT_8		( RIO_SUBSEGMENT_8 )

/*º¯ÊýÉùÃ÷*/
extern INT32 astSrioMsgReceive(
				UINT8 *buffer,
				INT32 max_bytes, 
				_Vx_ticks_t timeout
				);
extern INT32 astSrioMsgSend(
				UINT32 dest_id,
				const UINT8 *buffer, 
				UINT32 buffer_size
				);
extern INT32 astSrioMsgInit( void );

extern INT32 astSrioDbRecive(
				UINT16 *sourceID, 
				UINT16 *data, 
				_Vx_ticks_t timeout
				);
extern INT32 astSrioDbSend(
				UINT16 dest_id, 
				UINT16 data
				);
extern INT32 astSrioDoorbellInit( void );

extern INT32 astSrioDmaOutboundMap( AST_SRIO_OUTBOUND outbound );
extern INT32 astSrioDmaInboundMap( AST_SRIO_INBOUND inbound );
extern INT32 astSrioDmaInit( void );
extern STATUS astSrioDmaSend(
				UINT32 channel,
				UINT32 buffer,
				UINT32 dst_addr,
				INT32 nbytes
				);
extern STATUS astSrioDmaReceive(
				UINT32 channel,
				UINT32 src_addr,
				UINT32 buffer,
				INT32 nbytes
				);

extern UINT8 astSrioDeviceIDGet( void );
extern void astSrioDeviceIDShow( void );
extern STATUS astSrioSetup( void );


#endif


