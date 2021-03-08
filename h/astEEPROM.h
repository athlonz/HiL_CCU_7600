#ifndef _AST_EEPROM_H_
#define _AST_EEPROM_H_


extern INT32 astEepromMWrite(
		UINT16 offset, 
		const UINT8 *buffer, 
		INT32 nbytes
		);


extern INT32 astEepromRead(
		UINT16 offset, 
		UINT8 *buffer, 
		INT32 nbytes
		);


#endif
