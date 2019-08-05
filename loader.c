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
#include <time.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <getopt.h>
#include <errno.h>

#include <pthread.h>

#define XEXEC_LIBNAME "libxexec"
#ifndef XEXEC_VERSION
# define XEXEC_VERSION "unknown git revision"
#endif
#define XEXEC_VERSION_STRING \
        XEXEC_LIBNAME " version: '" XEXEC_VERSION "' " \
        "(compile localtime: '" __DATE__ " @ " __TIME__ "')"

typedef enum {
    XEXEC_DBG_NONE    = 0 << 0,     /* 0x0 */
    XEXEC_DBG_ERROR   = 1 << 0,     /* 0x1 */
    XEXEC_DBG_STACK   = 1 << 1,     /* 0x2 */
    XEXEC_DBG_INFO    = 1 << 2,     /* 0x4 */
    XEXEC_DBG_EVENT   = 1 << 3,     /* 0x8 */
    XEXEC_DBG_MESSAGE = 1 << 4,     /* 0x10 */
    XEXEC_DBG_VARDUMP = 1 << 5,     /* 0x20 */
    XEXEC_DBG_HEXDUMP = 1 << 6,     /* 0x40 */
    XEXEC_DBG_FD      = 1 << 7,     /* 0x80 */
    XEXEC_DBG_MUTEX   = 1 << 8,     /* 0x100 */
    XEXEC_DBG_MEMORY  = 1 << 9,     /* 0x200 */
    XEXEC_DBG_THREAD  = 1 << 10,    /* 0x400 */
    XEXEC_DBG_IRQ     = 1 << 11,    /* 0x800 */
    XEXEC_DBG_REG     = 1 << 12,    /* 0x1000 */
    XEXEC_DBG_DMA     = 1 << 13,    /* 0x2000 */
    XEXEC_DBG_CPU     = 1 << 14,    /* 0x4000 */ //TODO
    XEXEC_DBG_GPU     = 1 << 15,    /* 0x8000 */ //TODO
    XEXEC_DBG_ALL     = 0xffffffff
} xexec_dbg_t;

static xexec_dbg_t xexec_debug = XEXEC_DBG_ERROR;

int xboxkrnl_tsc_on(void);
int xboxkrnl_tsc_off(void);
void xboxkrnl_clock(struct timespec *tp);
void xboxkrnl_clock_wall(struct timespec *tp);

void
debug(xexec_dbg_t level, int8_t stack, const char *format, ...) {
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    register xexec_dbg_t *debug;
    va_list args;
    char buf[4096];
    char spc[0x3f * 4 + 1];
    char time[32];
    struct timespec tp;
    struct tm tm;
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

    xboxkrnl_clock_wall(&tp);
    if (localtime_r(&tp.tv_sec, &tm) && strftime(time, sizeof(time), "%T", &tm) > 0) {
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_VERBOSE, XEXEC_LIBNAME, "%s.%06lu: %s%s", time, tp.tv_nsec / 1000, spc, buf);
#else
        fprintf(stderr, "%s.%06lu: %s%s\n", time, tp.tv_nsec / 1000, spc, buf);
#endif
    } else {
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_VERBOSE, XEXEC_LIBNAME, "%lu.%06lu: %s%s", tp.tv_sec, tp.tv_nsec / 1000, spc, buf);
#else
        fprintf(stderr, "%lu.%06lu: %s%s\n", tp.tv_sec, tp.tv_nsec / 1000, spc, buf);
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

#define PACKED __attribute__((__packed__))

#define PAGESIZE  0x00001000 /* 4 KiB (4096 bytes) */
#define STACKSIZE 0x00100000 /* 1 MiB (1048576 bytes) */

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

static const struct option long_options[] = {
    { "version", no_argument,       0, 'V' },
    { "debug",   required_argument, 0,  0  },
    { "gdb",     no_argument,       0, 'g' },
    { "help",    no_argument,       0, 'h' },
    { "mode",    required_argument, 0, 'm' },
    { }
};

#define USAGE() usage(argc, argv)

static void __attribute__((noreturn))
usage(int argc, char **argv) {
    (void)argc;
    fprintf(stderr,
        XEXEC_VERSION_STRING "\n"
        "usage: %s [options] <xbox executable>.xbe\n"
        "options:\n"
        "  -V, --version              print version string\n"
        "  -d                         increase debug level verbosity, dumped to standard error\n"
        "      --debug=BITMASK        set debug level verbosity, dumped to standard error\n"
        "  -g, --gdb                  trigger breakpoint for GDB before creating entry thread\n"
        "  -h, --help                 display this help text\n"
        "  -m, --mode=EXEC            execution mode: EXEC value may be:\n"
#ifdef __i386__
        "                              \"de\"  - direct execution (i686/x86_64 hosts only) (default)\n"
        "                              \"x86\" - x86 CPU emulation\n"
#else
        "                              \"x86\" - x86 CPU emulation (default)\n"
#endif
        , basename(argv[0]));
    _exit(1);
}

int
main(int argc, char **argv) {
#ifdef __i386__
    struct sigaction s = {
        .sa_sigaction = x86_sigaction,
        .sa_flags     = SA_SIGINFO,
    };
#endif
    struct utsname un;
    xboxkrnl_thread *entry;
    XbeHeader *xbeh;
    char *path = NULL;
    int dbg    = 0;
    int gdb    = 0;
    int de     = 1;
    int fd     = -1;
    int i;
    int ret    = 1;

    setbuf(stdout, NULL);

    if (uname(&un) < 0) {
        fprintf(stderr, "error: uname(): '%s'\n", strerror(errno));
        return ret;
    }

    while ((ret = getopt_long(argc, argv, "Vdghm:", long_options, &i)) >= 0) {
        switch (ret) {
        case 0:
            if (!strcmp(long_options[i].name, "debug")) {
                if (!strncmp(optarg, "0x", sizeof("0x") - 1)) {
                    sscanf(optarg, "0x%x", &xexec_debug);
                } else {
                    xexec_debug = atoi(optarg);
                }
            }
            break;
        case 'V':
            fputs(XEXEC_VERSION_STRING, stdout);
            fputc('\n', stdout);
            return 0;
        case 'd':
            switch (++dbg) {
            case 1:
                xexec_debug = XEXEC_DBG_ALL & ~(XEXEC_DBG_REG | XEXEC_DBG_DMA);
                break;
            case 2:
                xexec_debug = XEXEC_DBG_ALL & ~XEXEC_DBG_DMA;
                break;
            case 3:
                xexec_debug = XEXEC_DBG_ALL;
                break;
            }
            break;
        case 'g':
            gdb = 1;
            break;
        case 'm':
            if (!strcmp(optarg, "de")) {
                de = 1;
            } else if (!strcmp(optarg, "x86")) {
                de = 0;
            } else {
                USAGE();
            }
            break;
        case 'h':
        case '?':
        default:
            USAGE();
            break;
        }
    }
    if (argc - optind < 1 || argc - optind >= 2) USAGE();
//    if (argc - optind > 1) gdb = 1;
    path = argv[optind];

    fprintf(stderr,
        "xexec - XBE loader & emulator - https://github.com/haxar/xexec\n"
        "running on host architecture: '%s'\n"
        XEXEC_VERSION_STRING "\n",
        un.machine);

    ret = 1;
    do {
        if (xboxkrnl_init(de)) {
            fprintf(stderr, "error: xboxkrnl initialization failed\n");
            break;
        }
        if ((fd = open(path, O_RDONLY)) < 0) {
            PRINT(XEXEC_DBG_ERROR, "error: open('%s'): '%s'", path, strerror(errno));
            break;
        }
    } while ((ret = 0));

    if (ret) {
        if (fd >= 0) close(fd);
        return ret;
    }

    ret = 1;
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
            PRINT(XEXEC_DBG_ERROR, "error: failed to map XBE header @ 0x%.08x: mmap(): '%s'", xbeh, strerror(errno));
            break;
        }
        if ((rd = read(fd, xbeh, PAGESIZE)) != PAGESIZE) {
            PRINT(XEXEC_DBG_ERROR, "error: read('%s'): '%s'", path, (rd >= 0) ? "short read" : strerror(errno));
            break;
        }
        if (xbeh->dwMagic != REG32(XBE_MAGIC)) {
            PRINT(XEXEC_DBG_ERROR, "error: invalid XBE file: '%s'", path);
            errno = ENOEXEC;
            break;
        }
        XbeHeader_dump(xbeh);

        memset(&xbeh->dwInitFlags, 0, sizeof(xbeh->dwInitFlags));//TODO catch flag cases

        xbeh->dwEntryAddr ^=
            ((xbeh->dwEntryAddr ^ XBE_XOR_EP_RETAIL) > 0x01000000)
                ? XBE_XOR_EP_DEBUG
                : XBE_XOR_EP_RETAIL;
        xbeh->dwKernelImageThunkAddr ^=
            ((xbeh->dwKernelImageThunkAddr ^ XBE_XOR_KT_RETAIL) > 0x01000000)
                ? XBE_XOR_KT_DEBUG
                : XBE_XOR_KT_RETAIL;

        PRINT(XEXEC_DBG_INFO, "/* XBE entry point: 0x%.08x | xboxkrnl thunk table: 0x%.08x */", xbeh->dwEntryAddr, xbeh->dwKernelImageThunkAddr);

        if (!xbeh->dwSections) {
            PRINT(XEXEC_DBG_ERROR, "error: XBE has no sections to map: '%s'", path);
            break;
        }

        xbes   = (typeof(xbes))xbeh->dwSectionHeadersAddr;
        vaddr  = ALIGN(PAGESIZE, xbes[0].dwVirtualAddr);
        raddr  = xbes[xbeh->dwSections - 1].dwVirtualAddr;
        raddr += xbes[xbeh->dwSections - 1].dwVirtualSize;
        raddr  = ALIGN(PAGESIZE, raddr);
        vsize  = raddr - vaddr;

        if (!strcmp(un.machine, "i686")) {
            //FIXME using sbrk(0) here on i686 linux (xbox-linux) is a hack to avoid overlap
            xboxkrnl_mem_contiguous = sbrk(0);
        } else {
            //TODO avoid process memory overlap
            xboxkrnl_mem_contiguous = (void *)raddr;
        }

        PRINT(
            XEXEC_DBG_MEMORY,
            "/* total XBE section map: [0x%.08x-0x%.08x] (0x%.08x / %u) */",
            vaddr,
            raddr,
            vsize,
            vsize);

        if (MEM_ALLOC_EXEC((void *)vaddr, vsize) != (void *)vaddr) {
            PRINT(XEXEC_DBG_ERROR, "error: failed to map XBE sections @ 0x%.08x: mmap(): '%s'", vaddr, strerror(errno));
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
                "/* "
                "section: %2u | "
                "name: '%10s' | "
                "virtual: [0x%.08x-0x%.08x] (0x%.08x / %u) | "
                "raw: [0x%.08x-0x%.08x] (0x%.08x / %u)"
                " */",
                i,
                sname,
                vaddr,
                vaddr + vsize,
                vsize,
                vsize,
                raddr,
                raddr + rsize,
                rsize,
                rsize);

            if (lseek(fd, raddr, SEEK_SET) != (ssize_t)raddr) {
                PRINT(XEXEC_DBG_ERROR, "error: lseek('%s'): '%s'", path, strerror(errno));
                break;
            }
            if ((rd = read(fd, (void *)vaddr, rsize)) != (ssize_t)rsize) {
                PRINT(XEXEC_DBG_ERROR, "error: read('%s'): '%s'", path, (rd >= 0) ? "short read" : strerror(errno));
                break;
            }
            if (!strncmp(sname, ".text", sizeof(".text"))) xbeh->dwEntryAddr = vaddr;//FIXME using post-initialization entry point
        }
    } while ((ret = 0));

    close(fd);

    if (ret) INT3;

    XbeTLS_dump((void *)xbeh->dwTLSAddr);

    {
        char cat[32];

        snprintf(cat, sizeof(cat), "cat /proc/%hu/maps", getpid());
        system(cat);
    }

    xboxkrnl_thunk_resolve((void *)xbeh->dwKernelImageThunkAddr);

    PRINT(XEXEC_DBG_THREAD, "/* creating entry thread w/ entry point @ 0x%.08x */", xbeh->dwEntryAddr);

    if (gdb) INT3;

#ifdef __i386__ /* i686-only direct execution */
    if (de) {
        if (sigaction(SIGSEGV, &s, NULL) < 0 ||
            sigaction(SIGTRAP, &s, NULL) < 0) {
            PRINT(XEXEC_DBG_ERROR, "error: sigaction(): '%s'", strerror(errno));
            ret = 1;
        } else {
            if (!(entry = THREAD_PUSH(xboxkrnl_entry_pthread, xbeh->dwEntryAddr))) INT3;
            if (THREAD_JOIN(entry)) INT3;
        }
    } else
#else
    de = 0;
#endif
    {
        if (X86_CPU_0_START(xbeh->dwEntryAddr)) INT3;
        if (X86_CPU_0_JOIN()) INT3;
    }

    /* TODO: do other stuff */

    return ret;
}

