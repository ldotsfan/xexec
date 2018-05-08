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

#define DEBUG_OHCI 2

#ifdef DEBUG_OHCI
# if DEBUG_OHCI == 1
#  define PRINT_OHCI        PRINT
# elif DEBUG_OHCI == 2
#  define PRINT_OHCI        PRINT
#  define VARDUMP_OHCI      VARDUMP
#  define VARDUMP2_OHCI     VARDUMP2
#  define VARDUMP3_OHCI     VARDUMP3
#  define VARDUMP4_OHCI     VARDUMP4
# else
#  define ENTER_OHCI        ENTER
#  define LEAVE_OHCI        LEAVE
#  define PRINT_OHCI        PRINT
#  define VARDUMP_OHCI      VARDUMP
#  define VARDUMP2_OHCI     VARDUMP2
#  define VARDUMP3_OHCI     VARDUMP3
#  define VARDUMP4_OHCI     VARDUMP4
# endif
#endif
#ifndef ENTER_OHCI
# define ENTER_OHCI
#endif
#ifndef LEAVE_OHCI
# define LEAVE_OHCI
#endif
#ifndef PRINT_OHCI
# define PRINT_OHCI(...)
#endif
#ifndef VARDUMP_OHCI
# define VARDUMP_OHCI(...)
#endif
#ifndef VARDUMP2_OHCI
# define VARDUMP2_OHCI(...)
#endif
#ifndef VARDUMP3_OHCI
# define VARDUMP3_OHCI(...)
#endif
#ifndef VARDUMP4_OHCI
# define VARDUMP4_OHCI(...)
#endif

static int
ohci_offset(register uint32_t *addr, register int *i) {
    register int ret;

    if ((ret = RANGE(usb0->memreg_base, usb0->memreg_size, *addr))) {
        *addr -= usb0->memreg_base;
        *i     = 0;
    } else if ((ret = RANGE(usb1->memreg_base, usb1->memreg_size, *addr))) {
        *addr -= usb1->memreg_base;
        *i     = 1;
    }

    return !ret;
}

int
ohci_write(uint32_t addr, const void *val, size_t sz) {
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    register uint32_t o;
    int i;
    register int ret = 0;
    ENTER_OHCI;

    if (ohci_offset(&addr, &i)) {
        LEAVE_OHCI;
        return ret;
    }

    if (!i) {
        n = usb0->name;
        p = usb0->memreg;
    } else {
        n = usb1->name;
        p = usb1->memreg;
    }
    r = addr;

    switch (sz) {
    case 1:
        v = REG08(val);
        o = REG08(p + addr);
        PRINT_OHCI("%s: "
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
        PRINT_OHCI("%s: "
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
        PRINT_OHCI("%s: "
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
        LEAVE_OHCI;
        return ret;
    }

    //TODO do stuff

    LEAVE_OHCI;
    return ret;
}

int
ohci_read(uint32_t addr, void *val, size_t sz) {
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    int i;
    register int ret = 0;
    ENTER_OHCI;

    if (ohci_offset(&addr, &i)) {
        LEAVE_OHCI;
        return ret;
    }

    if (!i) {
        n = usb0->name;
        p = usb0->memreg;
    } else {
        n = usb1->name;
        p = usb1->memreg;
    }
    r = addr;
    v = REG32(p + addr);

    //TODO do stuff

    switch (sz) {
    case 1:
        v &= 0xff;
        PRINT_OHCI("%s: "
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
        PRINT_OHCI("%s: "
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
        PRINT_OHCI("%s: "
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

    LEAVE_OHCI;
    return ret;
}

