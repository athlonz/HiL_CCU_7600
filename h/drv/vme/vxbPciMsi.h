/* vxbPciMsi.h - PCI lib header file for vxBus */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
30oct14,l_z  Fix wrong routine declaration. (V7PRO-1428)
28apr14,l_z  Fix typo error. (US37630)
12jul13,j_z  Created.
*/

#ifndef __INCvxbPciMsiH
#define __INCvxbPciMsiH

/* dynamic vectors */

int vxbPciMsiAlloc (VXB_DEVICE_ID pDev, UINT32 cnt);
STATUS vxbPciMsiFree(VXB_DEVICE_ID pDev, UINT32 cnt);

#endif /* __INCvxbPciMsiH */

