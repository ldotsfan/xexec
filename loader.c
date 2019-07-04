/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2012-2019 Michael Saga
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

#define XEXEC_LIBNAME "libxexec"

#define XEXEC_DBG_NONE    (0 << 0)
#define XEXEC_DBG_ERROR   (1 << 0)
#define XEXEC_DBG_STACK   (1 << 1)
#define XEXEC_DBG_INFO    (1 << 2)
#define XEXEC_DBG_EVENT   (1 << 3)
#define XEXEC_DBG_MESSAGE (1 << 4)
#define XEXEC_DBG_VARDUMP (1 << 5)
#define XEXEC_DBG_HEXDUMP (1 << 6)
#define XEXEC_DBG_FD      (1 << 7)
#define XEXEC_DBG_MUTEX   (1 << 8)
#define XEXEC_DBG_MEMORY  (1 << 9)
#define XEXEC_DBG_THREAD  (1 << 10)
#define XEXEC_DBG_IRQ     (1 << 11)
#define XEXEC_DBG_REG     (1 << 12)
#define XEXEC_DBG_DMA     (1 << 13)
#define XEXEC_DBG_ALL     (0xffff)

static int xexec_debug = XEXEC_DBG_ALL & ~(XEXEC_DBG_DMA);

int xboxkrnl_tsc_on(void);
int xboxkrnl_tsc_off(void);
void xboxkrnl_clock_local(struct timeval *tv);
void xboxkrnl_clock_wall(struct timespec *tp);

void
debug(int level, int8_t stack, const char *format, ...) {
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    register int *debug;
    va_list args;
    char buf[4096];
    char spc[0x3f * 4 + 1];
    char time[32];
    struct timeval tv;
    register struct tm *tm;
    register ssize_t n = 0;

    if (!(*(debug = &xexec_debug) & level) && level != XEXEC_DBG_ALL) return;

    va_start(args, format);
    n = sizeof(buf);
    if ((n = vsnprintf(buf, n, format, args)) < 0) {
        va_end(args);
        return;
    }
    buf[n - (n == sizeof(buf))] = 0;
    va_end(args);
    spc[0] = 0;
    if ((*debug & XEXEC_DBG_STACK) && (n = stack & 0x3f)) {
        n *= 4;
        memset(spc, ' ', n);
        spc[n] = 0;
    }

    pthread_mutex_lock(&mutex);

    xboxkrnl_clock_local(&tv);
    if ((tm = localtime(&tv.tv_sec)) && strftime(time, sizeof(time), "%T", tm) > 0) {
#ifndef ANDROID
        fprintf(stderr, "%s.%06lu: %s%s\n", time, tv.tv_usec, spc, buf);
#else
        __android_log_print(ANDROID_LOG_VERBOSE, XEXEC_LIBNAME, "%s.%06lu: %s%s", time, tv.tv_usec, spc, buf);
#endif
    } else {
#ifndef ANDROID
        fprintf(stderr, "%lu.%06lu: %s%s\n", tv.tv_sec, tv.tv_usec, spc, buf);
#else
        __android_log_print(ANDROID_LOG_VERBOSE, XEXEC_LIBNAME, "%lu.%06lu: %s%s", tv.tv_sec, tv.tv_usec, spc, buf);
#endif
    }

    pthread_mutex_unlock(&mutex);
}

#define hexdump(x,y) fhexdump(stderr, (x), (y))

void
fhexdump(FILE *stream, const void *in, size_t inlen) {
    register char *b;
    register const uint8_t *d;
    register int i;
    char buf[67];

    for (memset(buf, ' ', 66), buf[66] = 0, d = in, b = buf; inlen; b = buf) {
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

#define PACKED   __attribute__((__packed__))
#define PAGESIZE 0x1000

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
        .sa_sigaction = x86_signal_segv,
        .sa_flags     = SA_SIGINFO,
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
        PRINT(XEXEC_DBG_ERROR, "error: open(): '%s': '%s'", argv[1], strerror(errno));
        return ret;
    }
    if (sigaction(SIGSEGV, &s, NULL) < 0) {
        PRINT(XEXEC_DBG_ERROR, "error: sigaction(): '%s'", strerror(errno));
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
            PRINT(XEXEC_DBG_ERROR, "error: mmap(): failed to map XBE header @ 0x%.08x: '%s'", xbeh, strerror(errno));
            break;
        }
        if ((rd = read(fd, xbeh, PAGESIZE)) != PAGESIZE) {
            PRINT(XEXEC_DBG_ERROR, "error: read(): '%s': '%s'", argv[1], (rd >= 0) ? "short read" : strerror(errno));
            break;
        }
        if (xbeh->dwMagic != REG32(XBE_MAGIC)) {
            PRINT(XEXEC_DBG_ERROR, "error: invalid XBE file: '%s'", argv[1]);
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

        PRINT(XEXEC_DBG_INFO, "XBE entry: 0x%.08x | thunk table: 0x%.08x", xbeh->dwEntryAddr, xbeh->dwKernelImageThunkAddr);

        if (!xbeh->dwSections) {
            PRINT(XEXEC_DBG_ERROR, "error: XBE has no sections to map: '%s'", argv[1]);
            break;
        }

        xbes   = (typeof(xbes))xbeh->dwSectionHeadersAddr;
        vaddr  = ALIGN(PAGESIZE, xbes[0].dwVirtualAddr);
        raddr  = xbes[xbeh->dwSections - 1].dwVirtualAddr;
        raddr += xbes[xbeh->dwSections - 1].dwVirtualSize;
        raddr  = ALIGN(PAGESIZE, raddr);
        vsize  = raddr - vaddr;

        xboxkrnl_mem_contiguous = (void *)raddr;

        PRINT(
            XEXEC_DBG_MEMORY,
            "mmap(): "
            "total section map: "
            "start: 0x%.08x | "
            "end: 0x%.08x | "
            "size: 0x%.08x",
            vaddr, raddr, vsize);

        if (MEM_ALLOC_EXEC((void *)vaddr, vsize) != (void *)vaddr) {
            PRINT(XEXEC_DBG_ERROR, "error: mmap(): failed to map XBE sections @ 0x%.08x: '%s'", vaddr, strerror(errno));
            break;
        }
        for (i = 0; i < xbeh->dwSections; ++i) {
            XbeSectionHeader_dump(&xbes[i]);
            sname = (typeof(sname))xbes[i].dwSectionNameAddr;
            vaddr = xbes[i].dwVirtualAddr;
            vsize = xbes[i].dwVirtualSize;
            raddr = xbes[i].dwRawAddr;
            rsize = xbes[i].dwSizeofRaw;

            PRINT(
                XEXEC_DBG_MEMORY,
                "mmap(): "
                "section %2u | "
                "name %10s | "
                "virtual: address 0x%.08x size 0x%.08x | "
                "raw: address 0x%.08x size 0x%.08x",
                i, sname, vaddr, vsize, raddr, rsize);

            if (lseek(fd, raddr, SEEK_SET) != (ssize_t)raddr) {
                PRINT(XEXEC_DBG_ERROR, "error: lseek(): '%s': '%s'", argv[1], strerror(errno));
                break;
            }
            if ((rd = read(fd, (void *)vaddr, rsize)) != (ssize_t)rsize) {
                PRINT(XEXEC_DBG_ERROR, "error: read(): '%s': '%s'", argv[1], (rd >= 0) ? "short read" : strerror(errno));
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

        PRINT(XEXEC_DBG_THREAD, "/* spawning entry thread with entry point @ 0x%.08x */", xbeh->dwEntryAddr);

        if (argc >= 3) INT3;

        entry = THREAD_PUSH(xboxkrnl_entry_thread, (void *)xbeh->dwEntryAddr, "entry")->id;
        pthread_join(entry, NULL);

        /* TODO: do other stuff */
    }

    return ret;
}

