#ifndef _AST_TEMPERATURE_H_
#define _AST_TEMPERATURE_H_



/* Interrupt number */
#define TEMPE_EXINT_IRQ		5

/* routine */
extern INT32 astTempeOfBoardGet( UINT8 *temperature );

extern INT32 astTempeOfCpuGet( float *temperature );

extern void astTempeShow(void);

extern STATUS astTempeWarningEnable(
		UINT8 cpu_temperature, 
		UINT8 board_temperature, 
		VOIDFUNCPTR routine, 
		INT32 parameter
		);

extern STATUS astTempeWarningDisable( void );


#endif
