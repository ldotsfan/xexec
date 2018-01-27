
#ifndef ACI_H
#define ACI_H

#if 1
//#define DEV_PREFIX aci
//#include "../common.h"
//#undef DEV_PREFIX

typedef struct {
    const uint32_t          index;
    const char *            name;
    const uint32_t          offset;
    const uint32_t          size;
} aci_block;
#endif

enum {
    ACI_NAM     =   0,      /* native audio mixer */
    ACI_NABM,   /*  1 */    /* native audio bus master */
};

#define ACI_BLOCK_OFFSET(x) aci_blocks[x].offset
#define ACI_BLOCK_SIZE(x)   aci_blocks[x].size

static const aci_block const aci_blocks[] = {
#define ACI_BLOCK(x,y,z) \
    [ACI_ ## x] = { \
        .index = ACI_ ## x, \
        .name = #x, \
        .offset = y, \
        .size = z \
    }
    ACI_BLOCK(NAM,  0x0000, 0x0100),    /* 0 */
    ACI_BLOCK(NABM, 0x0100, 0x0080),    /* 1 */
#undef ACI_BLOCK
};

#include "reg/nam.h"
#include "reg/nabm.h"

#endif

