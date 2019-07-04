/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2017-2019 Michael Saga
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef ACI_H
#define ACI_H

//#define DEV_PREFIX aci
#include "../common.h"
//#undef DEV_PREFIX

enum {
    ACI_NAM     =   0,      /* native audio mixer */
    ACI_NABM,   /*  1 */    /* native audio bus master */
};

#define ACI_BLOCK_OFFSET(x) aci_block[x].offset
#define ACI_BLOCK_SIZE(x)   aci_block[x].size

static const hw_block_t aci_block[] = {
    HW_BLOCK(ACI_NAM,  0x0000, 0x0100),     /* 0 */
    HW_BLOCK(ACI_NABM, 0x0100, 0x0080),     /* 1 */
};

#include "reg/nam.h"
#include "reg/nabm.h"

#endif

