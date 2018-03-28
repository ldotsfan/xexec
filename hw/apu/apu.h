/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (C) 2012-2018  Michael Saga. All rights reserved.
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

#ifndef APU_H
#define APU_H

#if 1
//#define DEV_PREFIX apu
//#include "../common.h"
//#undef DEV_PREFIX

typedef struct {
    const uint32_t          index;
    const char *            name;
    const uint32_t          offset;
    const uint32_t          size;
} apu_block;
#endif

#define APU_MAX_VOICES              256

enum {
    NV_PAVS             =   0,
    NV_PAHRTFC,         /*  1 */
    NV_PAHRTFT,         /*  2 */
    NV_PSSL,            /*  3 */
    NV_PAPU_PRIV,       /*  4 */    /* setup engine */
    NV_PAPU_NV1BA0,     /*  5 */    /* voice processor */
    NV_PAPU_GPXMEM,     /*  6 */    /* global processor X memory */
    NV_PAPU_GPMIXBUF,   /*  7 */    /* global processor mix buffer */
    NV_PAPU_GPYMEM,     /*  8 */    /* global processor Y memory */
    NV_PAPU_GPPMEM,     /*  9 */    /* global processor P memory */
    NV_PAPU_GP,         /* 10 */    /* global processor */
    NV_PAPU_NV1BXX,     /* 11 */
    NV_PAPU_EPXMEM,     /* 12 */    /* encode processor X memory */
    NV_PAPU_EPYMEM,     /* 13 */    /* encode processor Y memory */
    NV_PAPU_EPPMEM,     /* 14 */    /* encode processor P memory */
    NV_PAPU_EP,         /* 15 */    /* encode processor */
};

#define APU_BLOCK_SIZE(x) apu_blocks[x].size

static const apu_block const apu_blocks[] = {
#define APU_BLOCK(x,y,z) \
    [NV_ ## x] = { \
        .index = NV_ ## x, \
        .name = #x, \
        .offset = y, \
        .size = z \
    }
    APU_BLOCK(PAVS,             0x00000, 0x00080),  /*  0 */
//#define NV_PAVS_SIZE_LG2                                   0x7 /* RW--V */
    APU_BLOCK(PAHRTFC,          0x00300, 0x00080),  /*  1 */
//#define NV_PAHRTFC_SIZE_LG2                                0x7 /* RW--V */
    APU_BLOCK(PAHRTFT,          0x00380, 0x00040),  /*  2 */
//#define NV_PAHRTFT_SIZE_LG2                                0x6 /* RW--V */
//    APU_BLOCK(PSSL,             0x01000, 0x01000),      /*  3 */
//#define NV_PSSL_SIZE_LG2                                   0xc /* RW--V */
    APU_BLOCK(PAPU_PRIV,        0x00000, 0x10000),  /*  4 */
    APU_BLOCK(PAPU_NV1BA0,      0x20000, 0x10000),  /*  5 */
    APU_BLOCK(PAPU_GPXMEM,      0x30000, 0x04000),  /*  6 */
    APU_BLOCK(PAPU_GPMIXBUF,    0x35000, 0x01000),  /*  7 */
    APU_BLOCK(PAPU_GPYMEM,      0x36000, 0x02000),  /*  8 */
    APU_BLOCK(PAPU_GPPMEM,      0x3a000, 0x04000),  /*  9 */
    APU_BLOCK(PAPU_GP,          0x30000, 0x10000),  /* 10 */
//    APU_BLOCK(PAPU_NV1BXX,      0x40000, 0x10000),  /* 11 */    /* not implemented */
    APU_BLOCK(PAPU_EPXMEM,      0x50000, 0x03000),  /* 12 */
    APU_BLOCK(PAPU_EPYMEM,      0x56000, 0x00400),  /* 13 */
    APU_BLOCK(PAPU_EPPMEM,      0x5a000, 0x04000),  /* 14 */
    APU_BLOCK(PAPU_EP,          0x50000, 0x10000),  /* 15 */
#undef APU_BLOCK
};

#include "reg/pavs.h"
#include "reg/pahrtfc.h"
#include "reg/pahrtft.h"
//#include "reg/pssl.h"
#include "reg/papu.h"
#include "reg/papu_vp.h"
#include "reg/papu_gp.h"
#include "reg/papu_ep.h"

#endif

