#ifndef _INCriodbellh
#define _INCriodbellh

#ifdef _cplusplus
extern "C" {
#endif

	
/* function declare */	
int srioDbellSend(UINT8, UINT16, FUNCPTR, void *);
int srioDbellSetup(void);

#ifdef _cplusplus
}
#endif

#endif
