/*
 * rio_Dstream.h
 *
 *  Created on: 2015-2-3
 *      Author: wangyegang
 */

#ifndef RIO_DSTREAM_H_
#define RIO_DSTREAM_H_

#ifdef _cplusplus
extern "C" {
#endif

/* function declared */
int srioDStreamSetup(void *);
int srioDStreamSend(UINT16, UINT8 *, UINT32, FUNCPTR, void *);


#ifdef _cplusplus
}
#endif
#endif /* RIO_DSTREAM_H_ */
