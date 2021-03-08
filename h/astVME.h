#ifndef _AST_VME_H_
#define _AST_VME_H_


/* º¯ÊýÉùÃ÷ */
extern INT32 astVmeReadBlk( 
		UINT32 addr, 
		INT8 *buffer, 
		INT32 length, 
		INT32 am,
		INT32 as,
		INT32 ds,
		INT32 swap);

extern INT32 astVmeWriteBlk( 
		UINT32 addr, 
		const INT8 *buffer, 
		INT32 length, 
		INT32 am,
		INT32 as,
		INT32 ds,
		INT32 swap);

extern INT32 astVmeReadSgl( 
		UINT32 addr, 
		INT8 *buffer, 
		INT32 am,
		INT32 as,
		INT32 ds,
		INT32 swap);

extern INT32 astVmeWriteSgl( 
		UINT32 addr, 
		const INT8 *buffer, 
		INT32 am,
		INT32 as,
		INT32 ds,
		INT32 swap);

extern INT32 astVmeMapAlloc( 
		UINT32 *local_addr,
		UINT32 vme_addr, 
		INT32 length, 
		INT32 am,
		INT32 as,
		INT32 ds,
		INT32 swap );

extern INT32 astVmeMapFree( void );

extern INT32 astVmeInit( void );

extern INT32 astVmeExit( void );

extern struct alt_ioctl_vme_irq *astVmeIrqAlloc( UINT32 is );

extern INT32 astVmeIrqFree( struct alt_ioctl_vme_irq *irq );

extern INT32 astVmeIrqArmWait( 
		struct alt_ioctl_vme_irq *irq, 
		UINT32 tmo, 
		UINT32 *irq_no, 
		UINT32 *vector );

extern INT32 astVmeIrqClear( struct alt_ioctl_vme_irq *irq );

extern INT32 astVmeRegister( void );

#endif
