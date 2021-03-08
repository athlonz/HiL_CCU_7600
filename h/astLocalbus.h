#ifndef _AST_LOCALBUS_H_
#define _AST_LOCALBUS_H_

/* GPIO序号定义 */
#define AST_GPIO_P2A1	( 0xa1 )	/*P2连接器信号A1*/
#define AST_GPIO_P2A2	( 0xa2 )	/*P2连接器信号A2*/
#define AST_GPIO_P2A3	( 0xa3 )	/*P2连接器信号A3*/
#define AST_GPIO_P2A4	( 0xa4 )	/*P2连接器信号A4*/
#define AST_GPIO_P2A5	( 0xa5 )	/*P2连接器信号A5*/
#define AST_GPIO_P2A6	( 0xa6 )	/*P2连接器信号A6*/
#define AST_GPIO_P2A7	( 0xa7 )	/*P2连接器信号A7*/
#define AST_GPIO_P2C1	( 0xc1 )	/*P2连接器信号C1*/
#define AST_GPIO_P2C2	( 0xc2 )	/*P2连接器信号C2*/
#define AST_GPIO_P2C3	( 0xc3 )	/*P2连接器信号C3*/
#define AST_GPIO_P2C4	( 0xc4 )	/*P2连接器信号C4*/
#define AST_GPIO_P2C5	( 0xc5 )	/*P2连接器信号C5*/
#define AST_GPIO_P2C6	( 0xc6 )	/*P2连接器信号C6*/
#define AST_GPIO_P2C7	( 0xc7 )	/*P2连接器信号C7*/


/* 函数声明 */
extern INT32 astLocalbusFpgaWRTest( void );

extern INT32 astLocalbusFpgaSynIrqEnable( INT32 sel );
extern INT32 astLocalbusFpgaSynIrqDisable( INT32 sel );
extern INT32 astLocalbusFpgaSynIrqClear( INT32 sel );
extern INT32 astLocalbusFpgaSynShadowSet( INT32 sel, UINT32 value );
extern INT32 astLocalbusFpgaSynShadowGet( INT32 sel, UINT32 *value );
extern INT32 astLocalbusFpgaSynComparerSet( INT32 sel, UINT32 value );
extern INT32 astLocalbusFpgaSynComparerGet( INT32 sel, UINT32 *value );
extern INT32 astLocalbusFpgaSynExternGet( INT32 sel, UINT32 *value );

extern INT32 astLocalbusFpgaIODirSet( INT32 io_x, UINT32 direction );
extern INT32 astLocalbusFpgaIODirGet( INT32 io_x, UINT32 *direction );
extern INT32 astLocalbusFpgaDOSet( INT32 io_x, UINT32 value );
extern INT32 astLocalbusFpgaDIGet( INT32 io_x, UINT32 *value );

extern INT32 astLocalbusFpgaSrioWrite( INT32 target_id, UINT32 target_addr, void *buffer, INT32 nbytes );
extern INT32 astLocalbusFpgaSrioRead( void *buffer, INT32 maxbytes );
extern INT32 astLocalbusFpgaSrioIdGet( INT32 *local_id );

/* Enhanced local bus controller CS3 device address */
#define AVME7600_LOCALBUS_FPGA_ADDR		( 0xf9000000 )

/* Enhanced local bus controller register address define */
#define AVME7600_ELBC_REG_BASE_ADDR		( CCSBAR + 0x124000 )


#endif	/* #define _AST_LOCALBUS_H_ */
