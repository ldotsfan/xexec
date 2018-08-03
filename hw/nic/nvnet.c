/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2017 Michael Saga. All rights reserved.
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

#include "nvnet.h"

#define DEBUG_NVNET 2

#ifdef DEBUG_NVNET
# if DEBUG_NVNET == 1
#  define PRINT_NVNET       PRINT
# elif DEBUG_NVNET == 2
#  define PRINT_NVNET       PRINT
#  define VARDUMP_NVNET     VARDUMP
#  define VARDUMP2_NVNET    VARDUMP2
#  define VARDUMP3_NVNET    VARDUMP3
#  define VARDUMP4_NVNET    VARDUMP4
# else
#  define ENTER_NVNET       ENTER
#  define LEAVE_NVNET       LEAVE
#  define PRINT_NVNET       PRINT
#  define VARDUMP_NVNET     VARDUMP
#  define VARDUMP2_NVNET    VARDUMP2
#  define VARDUMP3_NVNET    VARDUMP3
#  define VARDUMP4_NVNET    VARDUMP4
# endif
#endif
#ifndef ENTER_NVNET
# define ENTER_NVNET
#endif
#ifndef LEAVE_NVNET
# define LEAVE_NVNET
#endif
#ifndef PRINT_NVNET
# define PRINT_NVNET(...)
#endif
#ifndef VARDUMP_NVNET
# define VARDUMP_NVNET(...)
#endif
#ifndef VARDUMP2_NVNET
# define VARDUMP2_NVNET(...)
#endif
#ifndef VARDUMP3_NVNET
# define VARDUMP3_NVNET(...)
#endif
#ifndef VARDUMP4_NVNET
# define VARDUMP4_NVNET(...)
#endif

static int
nvnet_offset(register uint32_t *addr) {
    register int ret;

    if ((ret = RANGE(nvnet->memreg_base, nvnet->memreg_size, *addr))) *addr -= nvnet->memreg_base;

    return !ret;
}

static int
nvnet_write(uint32_t addr, const void *val, size_t sz) {
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    register uint32_t o;
    register int ret = 0;
    ENTER_NVNET;

    if (nvnet_offset(&addr)) {
        LEAVE_NVNET;
        return ret;
    }

    n = nvnet->name;
    p = nvnet->memreg;
    r = addr;

    switch (sz) {
    case 1:
        v = REG08(val);
        o = REG08(p + addr);
        PRINT_NVNET("%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.02hhx)       <- 0x%.02hhx       | "
            "reg: 0x%x",
            n,
            p,
            addr,
            o,
            v,
            r);
        REG08(p + addr) = v;
        ret = 1;
        break;
    case 2:
        v = REG16(val);
        o = REG16(p + addr);
        PRINT_NVNET("%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.04hx)     <- 0x%.04hx     | "
            "reg: 0x%x",
            n,
            p,
            addr,
            o,
            v,
            r);
        REG16(p + addr) = v;
        ret = 1;
        break;
    case 4:
        v = REG32(val);
        o = REG32(p + addr);
        PRINT_NVNET("%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.08x) <- 0x%.08x | "
            "reg: 0x%x",
            n,
            p,
            addr,
            o,
            v,
            r);
        REG32(p + addr) = v;
        ret = 1;
        break;
    default:
        LEAVE_NVNET;
        return ret;
    }

    //TODO do stuff

    LEAVE_NVNET;
    return ret;
}

static int
nvnet_read(uint32_t addr, void *val, size_t sz) {
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    register int ret = 0;
    ENTER_NVNET;

    if (nvnet_offset(&addr)) {
        LEAVE_NVNET;
        return ret;
    }

    n = nvnet->name;
    p = nvnet->memreg;
    r = addr;
    v = REG32(p + addr);

    //TODO do stuff

    switch (sz) {
    case 1:
        v &= 0xff;
        PRINT_NVNET("%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.02hhx       | "
            "reg: 0x%x",
            n,
            p,
            addr,
            v,
            r);
        REG08(val) = v;
        ret = 1;
        break;
    case 2:
        v &= 0xffff;
        PRINT_NVNET("%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.04hx     | "
            "reg: 0x%x",
            n,
            p,
            addr,
            v,
            r);
        REG16(val) = v;
        ret = 1;
        break;
    case 4:
        PRINT_NVNET("%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.08x | "
            "reg: 0x%x",
            n,
            p,
            addr,
            v,
            r);
        REG32(val) = v;
        ret = 1;
        break;
    default:
        break;
    }

    LEAVE_NVNET;
    return ret;
}

static void
nvnet_reset(void) {
    //TODO
}

static int
nvnet_irq(void) {
    return 1;//TODO
}

static void
nvnet_irq_raise(int mask) {
    (void)mask;//TODO
}

static void
nvnet_irq_restore(int mask) {
    (void)mask;//TODO
}

static int
nvnet_init(void) {
    return 1;//TODO
}

static int
nvnet_destroy(void) {
    return 1;//TODO
}

const hw_ops_t nvnet_op = {
    .init           = nvnet_init,
    .destroy        = nvnet_destroy,
    .reset          = nvnet_reset,
    .irq            = nvnet_irq,
    .irq_raise      = nvnet_irq_raise,
    .irq_restore    = nvnet_irq_restore,
    .write          = nvnet_write,
    .read           = nvnet_read,
};

