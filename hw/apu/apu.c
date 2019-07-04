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

#include "apu.h"

#define DEBUG_APU 2

#ifdef DEBUG_APU
# if DEBUG_APU == 1
#  define PRINT_APU         PRINT
# elif DEBUG_APU == 2
#  define PRINT_APU         PRINT
#  define VARDUMP_APU       VARDUMP
#  define VARDUMP2_APU      VARDUMP2
#  define VARDUMP3_APU      VARDUMP3
#  define VARDUMP4_APU      VARDUMP4
# else
#  define ENTER_APU         ENTER
#  define LEAVE_APU         LEAVE
#  define PRINT_APU         PRINT
#  define VARDUMP_APU       VARDUMP
#  define VARDUMP2_APU      VARDUMP2
#  define VARDUMP3_APU      VARDUMP3
#  define VARDUMP4_APU      VARDUMP4
# endif
#endif
#ifndef ENTER_APU
# define ENTER_APU
#endif
#ifndef LEAVE_APU
# define LEAVE_APU
#endif
#ifndef PRINT_APU
# define PRINT_APU(...)
#endif
#ifndef VARDUMP_APU
# define VARDUMP_APU(...)
#endif
#ifndef VARDUMP2_APU
# define VARDUMP2_APU(...)
#endif
#ifndef VARDUMP3_APU
# define VARDUMP3_APU(...)
#endif
#ifndef VARDUMP4_APU
# define VARDUMP4_APU(...)
#endif

static int
apu_offset(register uint32_t *addr) {
    register int ret;

    if ((ret = RANGE(apu->memreg_base, apu->memreg_size, *addr))) *addr -= apu->memreg_base;

    return !ret;
}

static const hw_block_t *
apu_block_lookup(register uint32_t addr, register const char **reg) {
    register const hw_block_t *b;
    register size_t i;
    ENTER_APU;

    if (reg) *reg = NULL;

    for (i = 0; i <= ARRAY_SIZE(apu_block); ++i) {
        if (i >= ARRAY_SIZE(apu_block)) INT3;
        b = &apu_block[i];
        if (RANGE(b->offset, b->size, addr)) {
            if (reg) {
                i = (addr - b->offset) / 4;
                switch (b->index) {
                case NV_PAVS:
                    ARRAY(*reg, apu_pavs_name, i);
                    break;
                case NV_PAHRTFC:
                    ARRAY(*reg, apu_pahrtfc_name, i);
                    break;
                case NV_PAHRTFT:
                    ARRAY(*reg, apu_pahrtft_name, i);
                    break;
#if 0
                case NV_PSSL:
                    ARRAY(*reg, apu_pssl_name, i);
                    break;
#endif
                case NV_PAPU_PRIV:
                    ARRAY(*reg, apu_papu_name, i);
                    break;
                case NV_PAPU_NV1BA0:
                    ARRAY(*reg, apu_papu_vp_name, i);
                    break;
                case NV_PAPU_GP:
                    ARRAY(*reg, apu_papu_gp_name, i);
                    break;
                case NV_PAPU_EP:
                    ARRAY(*reg, apu_papu_ep_name, i);
                    break;
                }
            }
            break;
        }
    }

    LEAVE_APU;
    return b;
}

static int
apu_write(uint32_t addr, const void *val, size_t sz) {
    register const hw_block_t *b;
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    register uint32_t o;
    const char *reg;
    register int ret = 0;
    ENTER_APU;

    if (apu_offset(&addr)) {
        LEAVE_APU;
        return ret;
    }

    b = apu_block_lookup(addr, &reg);
    n = apu->name;
    p = apu->memreg;
    r = addr - b->offset;

    switch (sz) {
    case 1:
        v = REG08(val);
        o = REG08(p + addr);
        PRINT_APU(
            XEXEC_DBG_REG,
            "%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.02hhx)       <- 0x%.02hhx       | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG08(p + addr) = v;
        ret = 1;
        break;
    case 2:
        v = REG16(val);
        o = REG16(p + addr);
        PRINT_APU(
            XEXEC_DBG_REG,
            "%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.04hx)     <- 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG16(p + addr) = v;
        ret = 1;
        break;
    case 4:
        v = REG32(val);
        o = REG32(p + addr);
        PRINT_APU(
            XEXEC_DBG_REG,
            "%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.08x) <- 0x%.08x | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG32(p + addr) = v;
        ret = 1;
        break;
    default:
        LEAVE_APU;
        return ret;
    }

    switch (b->index) {
//    case NV_PAPU_PRIV:
//        switch (r) {
//        }
//        break;
//    case NV_PAPU_NV1BA0:
//        switch (r) {
//        }
//        break;
    case NV_PAPU_GP:
        switch (r) {
        case NV_PAPU_GPRST:
//            proc_rst_write(d->gp.dsp, d->gp.regs[NV_PAPU_GPRST], val);
            break;
        }
        break;
    case NV_PAPU_EP:
        switch (r) {
        case NV_PAPU_EPRST:
//            proc_rst_write(d->ep.dsp, d->ep.regs[NV_PAPU_EPRST], val);
            break;
        }
        break;
    }

    LEAVE_APU;
    return ret;
}

static int
apu_read(uint32_t addr, void *val, size_t sz) {
    register const hw_block_t *b;
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    const char *reg;
    register int ret = 0;
    ENTER_APU;

    if (apu_offset(&addr)) {
        LEAVE_APU;
        return ret;
    }

    b = apu_block_lookup(addr, &reg);
    n = apu->name;
    p = apu->memreg;
    r = addr - b->offset;
    v = REG32(p + addr);

    switch (b->index) {
//    case NV_PAPU_PRIV:
//        switch (r) {
//        }
//        break;
    case NV_PAPU_NV1BA0:
        switch (r) {
        case NV1BA0_PIO_FREE:
            v = APU_MAX_VOICES;//TODO
            break;
        }
        break;
//    case NV_PAPU_GP:
//        switch (r) {
//        }
//        break;
//    case NV_PAPU_EP:
//        switch (r) {
//        }
//        break;
    }

    switch (sz) {
    case 1:
        v &= 0xff;
        PRINT_APU(
            XEXEC_DBG_REG,
            "%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.02hhx       | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG08(val) = v;
        ret = 1;
        break;
    case 2:
        v &= 0xffff;
        PRINT_APU(
            XEXEC_DBG_REG,
            "%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG16(val) = v;
        ret = 1;
        break;
    case 4:
        PRINT_APU(
            XEXEC_DBG_REG,
            "%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.08x | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG32(val) = v;
        ret = 1;
        break;
    default:
        break;
    }

    LEAVE_APU;
    return ret;
}

static void
apu_reset(void) {
    //TODO
}

static int
apu_irq(void) {
    return 1;//TODO
}

static void
apu_irq_raise(int mask) {
    (void)mask;//TODO
}

static void
apu_irq_restore(int mask) {
    (void)mask;//TODO
}

static int
apu_init(void) {
    return 1;//TODO
}

static int
apu_destroy(void) {
    return 1;//TODO
}

const hw_ops_t apu_op = {
    .init           = apu_init,
    .destroy        = apu_destroy,
    .reset          = apu_reset,
    .irq            = apu_irq,
    .irq_raise      = apu_irq_raise,
    .irq_restore    = apu_irq_restore,
    .write          = apu_write,
    .read           = apu_read,
};

