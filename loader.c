/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2012 Michael Saga. All rights reserved.
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

#ifndef __i386__
# error "Compile with `gcc -m32'"
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ucontext.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <errno.h>

#include <pthread.h>

static int dbg = 1;

int xboxkrnl_tsc_on(void);
int xboxkrnl_tsc_off(void);
void xboxkrnl_clock_local(struct timeval *tv);
void xboxkrnl_clock_wall(struct timespec *tp);

void
debug(const char *format, ...) {
    va_list args;
    char buf[4096];
    char time[32];
    struct timeval tv;
    struct tm *tm;
    int ret;

    if (!dbg) return;

    va_start(args, format);
    ret = sizeof(buf) - 1;
    ret = vsnprintf(buf, ret, format, args);
    va_end(args);

    if (ret > 0) {
        buf[ret] = 0;
        xboxkrnl_clock_local(&tv);
        if (!(tm = localtime(&tv.tv_sec)) || !strftime(time, sizeof(time), "%T", tm)) {
            fprintf(stderr, "%s\n", buf);
        } else {
            fprintf(stderr, "%s.%06lu: %s\n", time, tv.tv_usec, buf);
        }
    }
}

#define hexdump(x,y) fhexdump(stderr, (x), (y))

void
fhexdump(FILE *stream, const void *in, size_t inlen) {
    register char *b;
    register const uint8_t *d;
    register int i;
    char buf[67];

    memset(buf, ' ', 66), buf[66] = 0;
    for (d = in, b = buf; inlen; b = buf) {
        for (i = 0; i < 16; ++i) {
            if (inlen) {
                *b++ = "0123456789abcdef"[*d >> 4];
                *b++ = "0123456789abcdef"[*d & 15];
                ++b;
                buf[50 + i] = (*d >= ' ' && *d < 0x7f) ? *d : '.';
                ++d;
                --inlen;
            } else {
                *b++ = ' ';
                *b++ = ' ';
                ++b;
                buf[50 + i] = 0;
            }
        }
        fprintf(stream, "%.*s\n", 66, buf);
    }
}

#define PACKED              __attribute__((__packed__))
#define PAGESIZE            0x1000

#include "sw/xbe.h"
#include "hw/common.h"
extern const hw_ops_t nv2a_op;
void nv2a_framebuffer_set(uint32_t addr);
void nv2a_pfifo_pusher(register void *p);
extern const hw_ops_t ohci_op;
extern const hw_ops_t nvnet_op;
extern const hw_ops_t apu_op;
extern const hw_ops_t aci_op;
#include "xboxkrnl.c"
#include "sw/xbe.c"
#include "sw/gloffscreen.c"
#include "hw/gpu/nv2a.c"
#include "hw/usb/ohci.c"
#include "hw/nic/nvnet.c"
#include "hw/apu/apu.c"
#include "hw/aci/aci.c"
#include "sw/arch/x86.c"

int
main(int argc, char **argv) {
    struct sigaction s = {
        .sa_sigaction   = x86_signal_segv,
        .sa_flags       = SA_SIGINFO,
    };
    pthread_t entry;
    XbeHeader *xbeh;
    int fd = -1;
    int ret = 1;

    setbuf(stdout, NULL);

    if (argc < 2) {
        fprintf(stderr, "usage: %s <xbox executable>.xbe [GDB flag]\n", argv[0]);
        return ret;
    }

    if (xboxkrnl_init()) {
        fprintf(stderr, "error: xboxkrnl initialization failed\n");
        return ret;
    }
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        PRINT("error: open(): '%s': '%s'", argv[1], strerror(errno));
        return ret;
    }
    if (sigaction(SIGSEGV, &s, NULL) < 0) {
        PRINT("error: sigaction(): '%s'", strerror(errno));
        return ret;
    }

    do {
        XbeSectionHeader *xbes;
        const char *sname;
        uint32_t vaddr;
        uint32_t vsize;
        uint32_t raddr;
        uint32_t rsize;
        register ssize_t rd;
        register size_t i;

        xbeh = (void *)XBE_BASE;
        if (MEM_ALLOC_EXEC(xbeh, PAGESIZE) != xbeh) {
            PRINT("error: mmap(): failed to map XBE header @ %p: '%s'", xbeh, strerror(errno));
            break;
        }
        if ((rd = read(fd, xbeh, PAGESIZE)) != PAGESIZE) {
            PRINT("error: read(): '%s': '%s'", argv[1], (rd < 0) ? strerror(errno) : "short read");
            break;
        }
        if (xbeh->dwMagic != REG32(XBE_MAGIC)) {
            PRINT("error: invalid XBE: '%s'", argv[1]);
            break;
        }
        XbeHeader_dump(xbeh);

        memset(&xbeh->dwInitFlags, 0, sizeof(xbeh->dwInitFlags));//TODO catch flag cases

        xbeh->dwEntryAddr ^=
            ((xbeh->dwEntryAddr ^ XBE_XOR_EP_RETAIL) > 0x1000000)
                ? XBE_XOR_EP_DEBUG
                : XBE_XOR_EP_RETAIL;
        xbeh->dwKernelImageThunkAddr ^=
            ((xbeh->dwKernelImageThunkAddr ^ XBE_XOR_KT_RETAIL) > 0x1000000)
                ? XBE_XOR_KT_DEBUG
                : XBE_XOR_KT_RETAIL;

        PRINT("XBE entry: %p | thunk table: %p", xbeh->dwEntryAddr, xbeh->dwKernelImageThunkAddr);

        if (!xbeh->dwSections) {
            PRINT("error: XBE has no sections to map: '%s'", argv[1]);
            break;
        }

        xbes   = (typeof(xbes))xbeh->dwSectionHeadersAddr;
        vaddr  = ALIGN(PAGESIZE, xbes[0].dwVirtualAddr);
        raddr  = xbes[xbeh->dwSections - 1].dwVirtualAddr;
        raddr += xbes[xbeh->dwSections - 1].dwVirtualSize;
        raddr  = ALIGN(PAGESIZE, raddr);
        vsize  = raddr - vaddr;

        xboxkrnl_mem_contiguous = (void *)raddr;

        PRINT("mmap(): total section map: "
            "start: 0x%.08x | "
            "end: 0x%.08x | "
            "size: 0x%.08x",
            vaddr, raddr, vsize);

        if (MEM_ALLOC_EXEC((void *)vaddr, vsize) != (void *)vaddr) {
            PRINT("error: mmap(): failed to map XBE sections @ %p: '%s'", vaddr, strerror(errno));
            break;
        }
        for (i = 0; i < xbeh->dwSections; ++i) {
            XbeSectionHeader_dump(&xbes[i]);
            sname = (typeof(sname))xbes[i].dwSectionNameAddr;
            vaddr = xbes[i].dwVirtualAddr;
            vsize = xbes[i].dwVirtualSize;
            raddr = xbes[i].dwRawAddr;
            rsize = xbes[i].dwSizeofRaw;

            PRINT("mmap(): section %2u | "
                "name %10s | "
                "virtual: address 0x%.08x size 0x%.08x | "
                "raw: address 0x%.08x size 0x%.08x",
                i, sname, vaddr, vsize, raddr, rsize);

            if (lseek(fd, raddr, SEEK_SET) != (ssize_t)raddr) {
                PRINT("error: lseek(): '%s': '%s'", argv[1], strerror(errno));
                break;
            }
            if ((rd = read(fd, (void *)vaddr, rsize)) != (ssize_t)rsize) {
                PRINT("error: read(): '%s': '%s'", argv[1], (rd < 0) ? strerror(errno) : "short read");
                break;
            }
            if (!strncmp(sname, ".text", sizeof(".text"))) {
                xbeh->dwEntryAddr = vaddr;//FIXME
            }
        }
    } while ((ret = 0));

    close(fd);

    if (!ret) {
        XbeTLS_dump((void *)xbeh->dwTLSAddr);

        {
            char cat[32];
            snprintf(cat, sizeof(cat), "cat /proc/%hu/maps", getpid());
            system(cat);
        }

        xboxkrnl_thunk_resolve((void *)xbeh->dwKernelImageThunkAddr);

        PRINT("/* spawning entry thread with entry point @ %p */", xbeh->dwEntryAddr);

        if (argc >= 3) INT3;

        entry = THREAD_PUSH(xboxkrnl_entry_thread, (void *)xbeh->dwEntryAddr, "entry")->id;
        pthread_join(entry, NULL);

        /* TODO: do other stuff */
    }

    return ret;
}

