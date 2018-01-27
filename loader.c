/* gcc -m32 -g -Wall -Wextra loader.c -o loader -pthread */

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
        gettimeofday(&tv, NULL);
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
#define PAGEMASK            (PAGESIZE - 1)

#include "sw/xbe.h"
int ohci_write(uint32_t addr, const void *val, size_t sz);
int ohci_read(uint32_t addr, void *val, size_t sz);
int apu_write(uint32_t addr, const void *val, size_t sz);
int apu_read(uint32_t addr, void *val, size_t sz);
int aci_write(uint32_t addr, const void *val, size_t sz);
int aci_read(uint32_t addr, void *val, size_t sz);
void nv2a_pfifo_puller(register void *p);
int nv2a_init(void);
void nv2a_irq_restore(register int mask);
int nv2a_irq(void);
void nv2a_pcrtc_start(register uint32_t v);
int nv2a_write(uint32_t addr, const void *val, size_t sz);
int nv2a_read(uint32_t addr, void *val, size_t sz);
#include "xboxkrnl.c"
#include "sw/xbe.c"
#include "gloffscreen.c"
#include "hw/usb/ohci.c"
#include "hw/apu/apu.c"
#include "hw/aci/aci.c"
#include "hw/gpu/nv2a.c"

#if 0
#define X86_SEH_PROLOG \
        "\x68\xb0\x6a\x0f\x00\x64\xa1\x00\x00\x00\x00\x50\x8b\x44\x24\x10" \
        "\x89\x6c\x24\x10\x8d\x6c\x24\x10\x2b\xe0\x53\x56\x57\x8b\x45\xf8" \
        "\x89\x65\xe8\x50\x8b\x45\xfc\xc7\x45\xfc\xff\xff\xff\xff\x89\x45" \
        "\xf8\x8d\x45\xf0\x64\xa3\x00\x00\x00\x00\xc3"
#define X86_SEH_PROLOG_PATCH \
        "\x68\xb0\x6a\x0f\x00\x90\x90\x90\x90\x90\x90\x50\x8b\x44\x24\x10" \
        "\x89\x6c\x24\x10\x8d\x6c\x24\x10\x2b\xe0\x53\x56\x57\x8b\x45\xf8" \
        "\x89\x65\xe8\x50\x8b\x45\xfc\xc7\x45\xfc\xff\xff\xff\xff\x89\x45" \
        "\xf8\x90\x90\x90\x90\x90\x90\x90\x90\x90\xc3"
#define X86_SEH_EPILOG \
        "\x8b\x4d\xf0\x64\x89\x0d\x00\x00\x00\x00\x59\x5f\x5e\x5b\xc9\x51" \
        "\xc3"
#define X86_SEH_EPILOG_PATCH \
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x59\x5f\x5e\x5b\xc9\x51" \
        "\xc3"

int
xboxkrnl_patch_seh(void *addr, size_t size) {
    size_t i;
    size_t tmp;
    int ret = 0;
    ENTER;

    PRINT("/* searching & patching SEH prolog & epilog routines... */", 0);

    for (i = 0; !ret && i <= size; ++i) {
        if (i >= size) {
            PRINT("/* ... not found. */", 0);
            break;
        }
        for (tmp = 0; i + tmp < size && tmp <= sizeof(X86_SEH_PROLOG) - 1; ++tmp) {
            if (tmp >= sizeof(X86_SEH_PROLOG) - 1) {
                if (!memcmp(addr + i + tmp,
                        X86_SEH_EPILOG,
                        sizeof(X86_SEH_EPILOG) - 1)) {
                    PRINT("/* ... found! */", 0);
                    PRINT("/* SEH prolog & epilog @ %p */", addr + i);
//                    *((char *)addr + i)             = 0xc3;
                    memcpy(addr + i,
                            X86_SEH_PROLOG_PATCH,
                            sizeof(X86_SEH_PROLOG_PATCH) - 1);
                    PRINT("/* SEH prolog patched @ %p */", addr + i);
//                    *((char *)addr + i + tmp)       = 0xc3;
                    memcpy(addr + i + tmp,
                            X86_SEH_EPILOG_PATCH,
                            sizeof(X86_SEH_EPILOG_PATCH) - 1);
                    PRINT("/* SEH epilog patched @ %p */", addr + i + tmp);
                    ret = 2;
                } else {
                    PRINT("/* ... found! */", 0);
                    PRINT("/* SEH prolog @ %p */", addr + i);
//                    *((char *)addr + i)             = 0xc3;
                    memcpy(addr + i,
                            X86_SEH_PROLOG_PATCH,
                            sizeof(X86_SEH_PROLOG_PATCH) - 1);
                    PRINT("/* SEH prolog patched @ %p */", addr + i);
                    ret = 1;
                }
                break;
            }
            if (((char *)addr)[i + tmp] != X86_SEH_PROLOG[tmp]) break;
        }
    }
    if (ret == 1) {
        PRINT("/* searching & patching SEH epilog routine... */", 0);

        for (i = 0; ret == 1 && i <= size; ++i) {
            if (i >= size) {
                PRINT("/* ... not found. */", 0);
                break;
            }
            for (tmp = 0; i + tmp < size && tmp <= sizeof(X86_SEH_EPILOG) - 1; ++tmp) {
                if (tmp >= sizeof(X86_SEH_EPILOG) - 1) {
                    PRINT("/* ... found! */", 0);
                    PRINT("/* SEH epilog @ %p */", addr + i);
//                    *((char *)addr + i)             = 0xc3;
                    memcpy(addr + i,
                            X86_SEH_EPILOG_PATCH,
                            sizeof(X86_SEH_EPILOG_PATCH) - 1);
                    PRINT("/* SEH epilog patched @ %p */", addr + i);
                    ret = 2;
                    break;
                }
                if (((char *)addr)[i + tmp] != X86_SEH_EPILOG[tmp]) break;
            }
        }
    }

    LEAVE;
    return ret;
}
#endif
static const char *const greg_names[] = {
    NAME(REG_GS),
    NAME(REG_FS),
    NAME(REG_ES),
    NAME(REG_DS),
    NAME(REG_EDI),
    NAME(REG_ESI),
    NAME(REG_EBP),
    NAME(REG_ESP),
    NAME(REG_EBX),
    NAME(REG_EDX),
    NAME(REG_ECX),
    NAME(REG_EAX),
    NAME(REG_TRAPNO),
    NAME(REG_ERR),
    NAME(REG_EIP),
    NAME(REG_CS),
    NAME(REG_EFL),
    NAME(REG_UESP),
    NAME(REG_SS),
};

static const char *const signal_segv_si_codes[] = {
    "",
    "SEGV_MAPERR",  /* Address not mapped to object. */
    "SEGV_ACCERR",  /* Invalid permissions for mapped object. */
    "SEGV_BNDERR",  /* (since Linux 3.19) Failed address bound checks. */
    "SEGV_PKUERR"   /* (since Linux 4.6) Access was denied by memory protection keys. */
};

struct eflags {
    uint32_t cf   : 1;  /*  0 - carry flag */
    uint32_t r1   : 1;  /*  1 - reserved */
    uint32_t pf   : 1;  /*  2 - parity flag */
    uint32_t r2   : 1;  /*  3 - reserved */
    uint32_t af   : 1;  /*  4 - adjust flag */
    uint32_t r3   : 1;  /*  5 - reserved */
    uint32_t zf   : 1;  /*  6 - zero flag */
    uint32_t sf   : 1;  /*  7 - sign flag */
    uint32_t tf   : 1;  /*  8 - trap flag */
    uint32_t ief  : 1;  /*  9 - interrupt enable flag */
    uint32_t df   : 1;  /* 10 - direction flag */
    uint32_t of   : 1;  /* 11 - overflow flag */
    uint32_t iopl : 2;  /* 12 - i/o privilege level */
    uint32_t nt   : 1;  /* 14 - nested task flag */
    uint32_t r4   : 1;  /* 15 - reserved */
    uint32_t rf   : 1;  /* 16 - resume flag */
    uint32_t vm   : 1;  /* 17 - virtual 8086 mode flag */
    uint32_t ac   : 1;  /* 18 - alignment check flag */
    uint32_t vif  : 1;  /* 19 - virtual interrupt flag */
    uint32_t vip  : 1;  /* 20 - virtual interrupt pending flag */
    uint32_t id   : 1;  /* 21 - cpuid instruction permission flag */
    uint32_t r5   : 10; /* 22 - reserved */
};

static void
signal_segv(int signum, siginfo_t *info, void *ptr) {
    ucontext_t *uc = ptr;
    register void **bp;
    register void **ip = (void **)&uc->uc_mcontext.gregs[REG_EIP];
    register struct eflags *fl = (void *)&uc->uc_mcontext.gregs[REG_EFL];
    register const char *n;
    register uint32_t i = info->si_code;
    uint32_t v;

    if (i == SEGV_ACCERR ||
        i == SEGV_MAPERR) {
#if 0
A1 04 18 00 FD                  // mov     eax, ds:0FD001804h
A3 04 18 00 FD                  // mov     ds:0FD001804h, eax
C7 05 40 01 60 FD 00 00 00 00   // mov     dword ptr ds:0FD600140h, 0
C7 05 40 91 00 FD 00 00 00 00   // mov     dword ptr ds:0FD009140h, 0
89 9E 00 02 00 00               // mov     [esi+200h], ebx
89 86 40 01 00 00               // mov     [esi+140h], eax
89 AE 00 94 00 00               // mov     [esi+9400h], ebp
C7 87 0C 18 00 00 00 F8 00 00   // mov     dword ptr [edi+180Ch], 0F800h
#endif
        do {
            i = (typeof(i))info->si_addr;
            if (xboxkrnl_address_validate(i)) {
                if (*(uint8_t *)*ip == 0xf3) {                      // rep
// rep movsd // F3 A5
                    if (*(uint8_t *)(*ip + 1) == 0xa5) {            // rep movsd
//break;//XXX turok dirty disc
//INT3;//XXX
                        register uint32_t *c   = (void *)&uc->uc_mcontext.gregs[REG_ECX];
                        register uint32_t *di  = (void *)&uc->uc_mcontext.gregs[REG_EDI];
                        register uint32_t **si = (void *)&uc->uc_mcontext.gregs[REG_ESI];
                        while (*c && xboxkrnl_write(*di, *si, 4)) {
                            --*c;
                            if (fl->df) *di -= 4, *si -= 4;
                            else        *di += 4, *si += 4;
                        }
                        if (!*c) {
                            *ip += 2;                               //   opcode: 0xf3 0xa5
                            return;
                        } else break;
                    }
// rep stosd // F3 AB
                    if (*(uint8_t *)(*ip + 1) == 0xab) {            // rep stosd
                        register uint32_t *a  = (void *)&uc->uc_mcontext.gregs[REG_EAX];
                        register uint32_t *c  = (void *)&uc->uc_mcontext.gregs[REG_ECX];
                        register uint32_t *di = (void *)&uc->uc_mcontext.gregs[REG_EDI];
                        while (*c && xboxkrnl_write(*di, a, 4)) {
                            --*c;
                            if (fl->df) *di -= 4;
                            else        *di += 4;
                        }
                        if (!*c) {
                            *ip += 2;                               //   opcode: 0xf3 0xab
                            return;
                        } else break;
                    }
                }
// cmp     [esi+2100h], edi // 39 BE 00 21 00 00
                if (*(uint8_t *)*ip == 0x39) {                      // cmp
                    if (*(uint8_t *)(*ip + 1) == 0xbe) {            // cmp     [esi+<1>], edi
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_read(i, &v, 4)) {
                            *ip   += 2;                             //   opcode: 0x39 0xbe <1:dword>
                            *ip   += 4;                             // <1> size: dword
                            bp     = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                            fl->zf = (*(uint32_t *)&v == *(uint32_t *)bp);
                            PRINT("cmp: "
                                "([0x%.08x] (0x%.08x) == "
                                "edi (0x%.08x)) = %u",
                                i, *(uint32_t *)&v,                 // [esi+<1>]
                                *(uint32_t *)bp,                    // edi
                                fl->zf);                            // zero flag
                            return;
                        } else break;
                    }
                }
// cmp     dword ptr [esi+400100h], 0 // 83 BE 00 01 40 00 00
                if (*(uint8_t *)*ip == 0x83) {                      // cmp
                    if (*(uint8_t *)(*ip + 1) == 0xbe) {            // cmp     [esi+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_read(i, &v, 4)) {
                            *ip   += 2;                             //   opcode: 0x83 0xbe <1:dword> <2:byte>
                            *ip   += 4;                             // <1> size: dword
                            fl->zf = (*(uint32_t *)&v == *(uint8_t *)*ip);
                            PRINT("cmp: "
                                "([0x%.08x] (0x%.08x) == "
                                "0x%.02hhx) = %u",
                                i, *(uint32_t *)&v,                 // [esi+<1>]
                                *(uint8_t *)*ip,                    // <2>
                                fl->zf);                            // zero flag
                            *ip += 1;                               // <2>: value, <2> size: byte
                            return;
                        } else break;
                    }
                }
// cmp     eax, [ecx] // 3B 01
                if (*(uint8_t *)*ip == 0x3b) {                      // cmp
                    if (*(uint8_t *)(*ip + 1) == 0x01) {            // cmp     eax, [ecx]
                        i = uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, &v, 4)) {
                            *ip   += 2;                             //   opcode: 0x3b 0x01
                            bp     = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                            fl->zf = (*(uint32_t *)bp == *(uint32_t *)&v);
                            PRINT("cmp: "
                                "(eax (0x%.08x) == "
                                "[0x%.08x] (0x%.08x)) = %u",
                                *(uint32_t *)bp,                    // eax
                                i, *(uint32_t *)&v,                 // [ecx]
                                fl->zf);                            // zero flag
                            return;
                        } else break;
                    }
// cmp     eax, [ecx+400908h] // 3B 81 08 09 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x81) {            // cmp     eax, [ecx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_read(i, &v, 4)) {
                            *ip   += 2;                             //   opcode: 0x3b 0x81 <1:dword>
                            *ip   += 4;                             // <1> size: dword
                            bp     = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                            fl->zf = (*(uint32_t *)bp == *(uint32_t *)&v);
                            PRINT("cmp: "
                                "(eax (0x%.08x) == "
                                "[0x%.08x] (0x%.08x)) = %u",
                                *(uint32_t *)bp,                    // eax
                                i, *(uint32_t *)&v,                 // [ecx+<1>]
                                fl->zf);                            // zero flag
                            return;
                        } else break;
                    }
// cmp     eax, [esi+edi*4+400980h] // 3B 84 BE 80 09 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x84) {
                        if (*(uint8_t *)(*ip + 2) == 0xbe) {        // cmp     eax, [esi+edi*4+<1>]
                            i  = uc->uc_mcontext.gregs[REG_ESI];
                            i += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            if (xboxkrnl_read(i, &v, 4)) {
                                *ip   += 3;                         //   opcode: 0x3b 0x84 0xbe <1:dword>
                                *ip   += 4;                         // <1> size: dword
                                bp     = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                                fl->zf = (*(uint32_t *)bp == *(uint32_t *)&v);
                                PRINT("cmp: "
                                    "(eax (0x%.08x) == "
                                    "[0x%.08x] (0x%.08x)) = %u",
                                    *(uint32_t *)bp,                // eax
                                    i, *(uint32_t *)&v,             // [esi+edi*4+<1>]
                                    fl->zf);                        // zero flag
                                return;
                            } else break;
                        }
                    }
                }
// test    dword ptr [eax+100410h], 10000h // F7 80 10 04 10 00 00 00 01 00
                if (*(uint8_t *)*ip == 0xf7) {                      // test
                    if (*(uint8_t *)(*ip + 1) == 0x80) {            // test    [eax+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_read(i, &v, 4)) {
                            *ip   += 2;                             //   opcode: 0xf7 0x80 <1:dword> <2:dword>
                            *ip   += 4;                             // <1> size: dword
                            fl->zf = !(*(uint32_t *)&v & *(uint32_t *)*ip);
                            PRINT("test: "
                                "!([0x%.08x] (0x%.08x) & "
                                "0x%.08x) = %u",
                                i, *(uint32_t *)&v,                 // [eax+<1>]
                                *(uint32_t *)*ip,                   // <2>
                                fl->zf);                            // zero flag
                            *ip += 4;                               // <2>: value, <2> size: dword
                            return;
                        } else break;
                    }
                }
                if (*(uint8_t *)*ip == 0xf6) {                      // test
// test    byte ptr [esi+3214h], 10h // F6 86 14 32 00 00 10
                    if (*(uint8_t *)(*ip + 1) == 0x86) {            // test    [esi+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_read(i, &v, 1)) {
                            *ip   += 2;                             //   opcode: 0xf6 0x86 <1:dword> <2:byte>
                            *ip   += 4;                             // <1> size: dword
                            fl->zf = !(*(uint8_t *)&v & *(uint8_t *)*ip);
                            PRINT("test: "
                                "!([0x%.08x] (0x%.02hhx) & "
                                "0x%.02hhx) = %u",
                                i, *(uint8_t *)&v,                  // [esi+<1>]
                                *(uint8_t *)*ip,                    // <2>
                                fl->zf);                            // zero flag
                            *ip += 1;                               // <2>: value, <2> size: byte
                            return;
                        } else break;
                    }
// test    byte ptr [eax], 10h // F6 00 10
                    if (*(uint8_t *)(*ip + 1) == 0x00) {            // test    [eax], <1>
                        i = uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, &v, 1)) {
                            *ip   += 2;                             //   opcode: 0xf6 0x00 <1:byte>
                            fl->zf = !(*(uint8_t *)&v & *(uint8_t *)*ip);
                            PRINT("test: "
                                "!([0x%.08x] (0x%.02hhx) & "
                                "0x%.02hhx) = %u",
                                i, *(uint8_t *)&v,                  // [eax]
                                *(uint8_t *)*ip,                    // <1>
                                fl->zf);                            // zero flag
                            *ip += 1;                               // <1>: value, <1> size: byte
                            return;
                        } else break;
                    }
// test    byte ptr ds:0FEC00134h, 1 // F6 05 34 01 C0 FE 01
                    if (*(uint8_t *)(*ip + 1) == 0x05) {            // test    ds:<1>, <2>
                        i = uc->uc_mcontext.gregs[REG_EAX];
                        i = *(uint32_t *)(*ip + 2);                 //      <1>: offset
                        if (xboxkrnl_read(i, &v, 1)) {
                            *ip   += 2;                             //   opcode: 0xf6 0x05 <1:dword> <2:byte>
                            *ip   += 4;                             // <1> size: dword
                            fl->zf = !(*(uint8_t *)&v & *(uint8_t *)*ip);
                            PRINT("test: "
                                "!([0x%.08x] (0x%.02hhx) & "
                                "0x%.02hhx) = %u",
                                i, *(uint8_t *)&v,                  // <1>
                                *(uint8_t *)*ip,                    // <2>
                                fl->zf);                            // zero flag
                            *ip += 1;                               // <2>: value, <2> size: byte
                            return;
                        } else break;
                    }
// test    byte ptr [eax-13FFEFAh], 1 // F6 80 06 01 C0 FE 01
                    if (*(uint8_t *)(*ip + 1) == 0x80) {            // test    [eax+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_read(i, &v, 1)) {
                            *ip   += 2;                             //   opcode: 0xf6 0x80 <1:dword> <2:byte>
                            *ip   += 4;                             // <1> size: dword
                            fl->zf = !(*(uint8_t *)&v & *(uint8_t *)*ip);
                            PRINT("test: "
                                "!([0x%.08x] (0x%.02hhx) & "
                                "0x%.02hhx) = %u",
                                i, *(uint8_t *)&v,                  // [eax+<1>]
                                *(uint8_t *)*ip,                    // <2>
                                fl->zf);                            // zero flag
                            *ip += 1;                               // <2>: value, <2> size: byte
                            return;
                        } else break;
                    }
                }
                if (*(uint8_t *)*ip == 0x85) {                      // test
// test    ds:0FEC00130h, esi // 85 35 30 01 C0 FE
                    if (*(uint8_t *)(*ip + 1) == 0x35) {            // test    ds:<1>, esi
                        i  = *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ESI];
                        if (xboxkrnl_read(i, &v, 4)) {
                            *ip   += 2;                             //   opcode: 0x85 0x35 <1:dword>
                            *ip   += 4;                             // <1> size: dword
                            fl->zf = !(*(uint32_t *)&v & *(uint32_t *)bp);
                            PRINT("test: "
                                "!([0x%.08x] (0x%.08x) & "
                                "0x%.08x) = %u",
                                i, *(uint32_t *)&v,                 // <1>
                                *(uint32_t *)bp,                    // esi
                                fl->zf);                            // zero flag
                            return;
                        } else break;
                    }
                }
                if (*(uint8_t *)*ip == 0xa1) {                      // mov     eax, ds:<1>
                    i  = *(uint32_t *)(*ip + 1);                    //      <1>: offset
                    bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                    if (xboxkrnl_read(i, bp, 4)) {
                        *ip += 1;                                   //   opcode: 0xa1 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                        return;
                    } else break;
                }
                if (*(uint8_t *)*ip == 0xa3) {                      // mov     ds:<1>, eax
                    i  = *(uint32_t *)(*ip + 1);                    //      <1>: offset
                    bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                    if (xboxkrnl_write(i, bp, 4)) {
                        *ip += 1;                                   //   opcode: 0xa3 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                        return;
                    } else break;
                }
                if (*(uint8_t *)*ip == 0xc7) {                      // mov
                    if (*(uint8_t *)(*ip + 1) == 0x05) {            // mov     ds:<1>, <2>
                        i = *(uint32_t *)(*ip + 2);                 //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 6, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x05 <1:dword> <2:dword>
                            *ip += 4;                               // <1> size: dword
                            *ip += 4;                               // <2> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [eax+2500h], 0 // C7 80 00 25 00 00 00 00 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x80) {            // mov     [eax+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 6, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x80 <1:dword> <2:dword>
                            *ip += 4;                               // <1> size: dword
                            *ip += 4;                               // <2> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [eax+edi*4], 0 // C7 04 B8 00 00 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x04) {
                        if (*(uint8_t *)(*ip + 2) == 0xb8) {        // mov     [eax+edi*4], <1>
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            if (xboxkrnl_write(i, *ip + 3, 4)) {
                                *ip += 3;                           //   opcode: 0xc7 0x04 0xb8 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
// mov     dword ptr [eax+esi*4], 3F800000h // C7 04 B0 00 00 80 3F
                        if (*(uint8_t *)(*ip + 2) == 0xb0) {        // mov     [eax+esi*4], <1>
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_ESI] * 4;
                            if (xboxkrnl_write(i, *ip + 3, 4)) {
                                *ip += 3;                           //   opcode: 0xc7 0x04 0xb0 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
// mov     dword ptr [eax+ebx*4], 40000000h // C7 04 98 00 00 00 40
                        if (*(uint8_t *)(*ip + 2) == 0x98) {        // mov     [eax+ebx*4], <1>
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_EBX] * 4;
                            if (xboxkrnl_write(i, *ip + 3, 4)) {
                                *ip += 3;                           //   opcode: 0xc7 0x04 0x98 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
// mov     dword ptr [ecx+edi], 0 // C7 04 39 00 00 00 00
                        if (*(uint8_t *)(*ip + 2) == 0x39) {        // mov     [ecx+edi], <1>
                            i  = uc->uc_mcontext.gregs[REG_ECX];
                            i += uc->uc_mcontext.gregs[REG_EDI];
                            if (xboxkrnl_write(i, *ip + 3, 4)) {
                                *ip += 3;                           //   opcode: 0xc7 0x04 0x39 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
                    }
// mov     dword ptr [edi], 0   // C7 07 00 00 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x07) {            // mov     [edi], <1>
                        i = uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_write(i, *ip + 2, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x07 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [eax+ecx*4+33Ch], 0FFFF0000h // C7 84 88 3C 03 00 00 00 00 FF FF
                    if (*(uint8_t *)(*ip + 1) == 0x84) {
                        if (*(uint8_t *)(*ip + 2) == 0x88) {        // mov     [eax+ecx*4+<1>], <2>
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            if (xboxkrnl_write(i, *ip + 7, 4)) {
                                *ip += 3;                           //   opcode: 0xc7 0x84 0x88 <1:dword> <2:dword>
                                *ip += 4;                           // <1> size: dword
                                *ip += 4;                           // <2> size: dword
                                return;
                            } else break;
                        }
// mov     dword ptr [eax+edi*4+700004h], 0BF800000h // C7 84 B8 04 00 70 00 00 00 80 BF
                        if (*(uint8_t *)(*ip + 2) == 0xb8) {        // mov     [eax+edi*4+<1>], <2>
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            if (xboxkrnl_write(i, *ip + 7, 4)) {
                                *ip += 3;                           //   opcode: 0xc7 0x84 0xb8 <1:dword> <2:dword>
                                *ip += 4;                           // <1> size: dword
                                *ip += 4;                           // <2> size: dword
                                return;
                            } else break;
                        }
                    }
// mov     dword ptr [esi], 7FF0000h // C7 06 00 00 FF 07
                    if (*(uint8_t *)(*ip + 1) == 0x06) {            // mov     [esi], <1>
                        i = uc->uc_mcontext.gregs[REG_ESI];
                        if (xboxkrnl_write(i, *ip + 2, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x06 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [edi+1830h], 0     // C7 87 30 18 00 00 00 00 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x87) {            // mov     [edi+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 6, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x87 <1:dword> <2:dword>
                            *ip += 4;                               // <1> size: dword
                            *ip += 4;                               // <2> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [ecx], 0FFFFFFFFh // C7 01 FF FF FF FF
                    if (*(uint8_t *)(*ip + 1) == 0x01) {            // mov     [ecx], <1>
                        i = uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, *ip + 2, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x01 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [eax+18h], 0 // C7 40 18 00 00 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x40) {            // mov     [eax+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 3, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x40 <1:byte> <2:dword>
                            *ip += 1;                               // <1> size: byte
                            *ip += 4;                               // <2> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [ebx+10h], 40h // C7 43 10 40 00 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x43) {            // mov     [ebx+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_EBX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 3, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x43 <1:byte> <2:dword>
                            *ip += 1;                               // <1> size: byte
                            *ip += 4;                               // <2> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [edi+48h], 1200h // C7 47 48 00 12 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x47) {            // mov     [edi+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 3, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x47 <1:byte> <2:dword>
                            *ip += 1;                               // <1> size: byte
                            *ip += 4;                               // <2> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [eax], 0 // C7 00 00 00 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x00) {            // mov     [eax], <1>
                        i = uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, *ip + 2, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x00 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [esi+400750h], 0EA000Ch // C7 86 50 07 40 00 0C 00 EA 00
                    if (*(uint8_t *)(*ip + 1) == 0x86) {            // mov     [esi+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 6, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x86 <1:dword> <2:dword>
                            *ip += 4;                               // <1> size: dword
                            *ip += 4;                               // <2> size: dword
                            return;
                        } else break;
                    }
// mov     dword ptr [ecx+400750h], 0E00050h // C7 81 50 07 40 00 50 00 E0 00
                    if (*(uint8_t *)(*ip + 1) == 0x81) {            // mov     [ecx+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 6, 4)) {
                            *ip += 2;                               //   opcode: 0xc7 0x81 <1:dword> <2:dword>
                            *ip += 4;                               // <1> size: dword
                            *ip += 4;                               // <2> size: dword
                            return;
                        } else break;
                    }
                }
                if (*(uint8_t *)*ip == 0x8b) {                      // mov
                    if (*(uint8_t *)(*ip + 1) == 0x82) {            // mov     eax, [edx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x82 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
                    if (*(uint8_t *)(*ip + 1) == 0x92) {            // mov     edx, [edx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x92 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     esi, [eax+2500h]     // 8B B0 00 25 00 00
                    if (*(uint8_t *)(*ip + 1) == 0xb0) {            // mov     esi, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ESI];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0xb0 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ecx, [esi]           // 8B 0E
                    if (*(uint8_t *)(*ip + 1) == 0x0e) {            // mov     ecx, [esi]
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x0e
                            return;
                        } else break;
                    }
// mov     ecx, [edx]           // 8B 0A
                    if (*(uint8_t *)(*ip + 1) == 0x0a) {            // mov     ecx, [edx]
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x0a
                            return;
                        } else break;
                    }
// mov     eax, [ecx] // 8B 01
                    if (*(uint8_t *)(*ip + 1) == 0x01) {            // mov     eax, [ecx]
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x01
                            return;
                        } else break;
                    }
// mov     eax, [edi+140h] // 8B 87 40 01 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x87) {            // mov     eax, [edi+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x87 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ecx, [eax] // 8B 08
                    if (*(uint8_t *)(*ip + 1) == 0x08) {            // mov     ecx, [eax]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x08
                            return;
                        } else break;
                    }
// mov     eax, [eax] // 8B 00
                    if (*(uint8_t *)(*ip + 1) == 0x00) {            // mov     eax, [eax]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x00
                            return;
                        } else break;
                    }
// mov     ecx, [eax+8088h] // 8B 88 88 80 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x88) {            // mov     ecx, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x88 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     edx, [ecx] // 8B 11
                    if (*(uint8_t *)(*ip + 1) == 0x11) {            // mov     edx, [ecx]
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x11
                            return;
                        } else break;
                    }
// mov     eax, [esi+400700h] // 8B 86 00 07 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x86) {            // mov     eax, [esi+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x86 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ebp, [eax-3006C0h] // 8B A8 40 F9 CF FF
                    if (*(uint8_t *)(*ip + 1) == 0xa8) {            // mov     ebp, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBP];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0xa8 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     edx, [eax-300680h] // 8B 90 80 F9 CF FF
                    if (*(uint8_t *)(*ip + 1) == 0x90) {            // mov     edx, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x90 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     edi, [eax+3240h] // 8B B8 40 32 00 00
                    if (*(uint8_t *)(*ip + 1) == 0xb8) {            // mov     edi, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0xb8 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     edi, [eax] // 8B 38
                    if (*(uint8_t *)(*ip + 1) == 0x38) {            // mov     edi, [eax]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x38
                            return;
                        } else break;
                    }
// mov     edi, [edx] // 8B 3A
                    if (*(uint8_t *)(*ip + 1) == 0x3a) {            // mov     edi, [edx]
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x3a
                            return;
                        } else break;
                    }
// mov     edi, [edx+4] // 8B 7A 04
                    if (*(uint8_t *)(*ip + 1) == 0x7a) {            // mov     edi, [edx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x7a <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     edx, [edx+30h] // 8B 52 30
                    if (*(uint8_t *)(*ip + 1) == 0x52) {            // mov     edx, [edx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x52 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     eax, [edi+8] // 8B 47 08
                    if (*(uint8_t *)(*ip + 1) == 0x47) {            // mov     eax, [edi+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x47 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     ecx, [eax+4] // 8B 48 04
                    if (*(uint8_t *)(*ip + 1) == 0x48) {            // mov     ecx, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x48 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     ecx, [ecx+44h] // 8B 49 44
                    if (*(uint8_t *)(*ip + 1) == 0x49) {            // mov     ecx, [ecx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x49 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     edx, [esi+2100h] // 8B 96 00 21 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x96) {            // mov     edx, [esi+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x96 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ecx, [esi+2100h] // 8B 8E 00 21 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x8e) {            // mov     ecx, [esi+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x8e <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     edi, [esi+400704h] // 8B BE 04 07 40 00
                    if (*(uint8_t *)(*ip + 1) == 0xbe) {            // mov     edi, [esi+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0xbe <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ebx, [esi+400108h] // 8B 9E 08 01 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x9e) {            // mov     ebx, [esi+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x9e <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ebp, [edx+400700h] // 8B AA 00 07 40 00
                    if (*(uint8_t *)(*ip + 1) == 0xaa) {            // mov     ebp, [edx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBP];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0xaa <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     eax, [ecx+100248h] // 8B 81 48 02 10 00
                    if (*(uint8_t *)(*ip + 1) == 0x81) {            // mov     eax, [ecx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x81 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     eax, [eax+400904h] // 8B 80 04 09 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x80) {            // mov     eax, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x80 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     eax, [ebx] // 8B 03
                    if (*(uint8_t *)(*ip + 1) == 0x03) {            // mov     eax, [ebx]
                        i  = uc->uc_mcontext.gregs[REG_EBX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x03
                            return;
                        } else break;
                    }
// mov     eax, [eax+44h] // 8B 40 44
                    if (*(uint8_t *)(*ip + 1) == 0x40) {            // mov     eax, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x40 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     eax, [ebx+50h] // 8B 43 50
                    if (*(uint8_t *)(*ip + 1) == 0x43) {            // mov     eax, [ebx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EBX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x43 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     esi, [edi+100h] // 8B B7 00 01 00 00
                    if (*(uint8_t *)(*ip + 1) == 0xb7) {            // mov     esi, [edi+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ESI];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0xb7 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     edx, ds:0FD003244h // 8B 15 44 32 00 FD
                    if (*(uint8_t *)(*ip + 1) == 0x15) {            // mov     edx, ds:<1>
                        i  = *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x15 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ebx, ds:0FE820010h // 8B 1D 10 00 82 FE
                    if (*(uint8_t *)(*ip + 1) == 0x1d) {            // mov     ebx, ds:<1>
                        i  = *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x1d <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     eax, [ecx+401A88h] // 8B 81 88 1A 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x81) {            // mov     eax, [ecx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x81 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     edx, [ecx+40186Ch] // 8B 91 6C 18 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x91) {            // mov     edx, [ecx+<1>]
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x91 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ecx, ds:0FD0032A4h // 8B 0D A4 32 00 FD
                    if (*(uint8_t *)(*ip + 1) == 0x0d) {            // mov     ecx, ds:<1>
                        i  = *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x8b 0x0d <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
                }
// mov     [ecx+700008h], eax   // 89 81 08 00 70 00
                if (*(uint8_t *)*ip == 0x89) {                      // mov
                    if (*(uint8_t *)(*ip + 1) == 0x81) {            // mov     [ecx+<1>], eax
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x81 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [eax+edi], esi       // 89 34 38
                    if (*(uint8_t *)(*ip + 1) == 0x34) {
                        if (*(uint8_t *)(*ip + 2) == 0x38) {        // mov     [eax+edi], esi
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_EDI];
                            bp = (void **)&uc->uc_mcontext.gregs[REG_ESI];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x34 0x38
                                return;
                            } else break;
                        }
// mov     [eax+edi*4], esi     // 89 34 B8
                        if (*(uint8_t *)(*ip + 2) == 0xb8) {        // mov     [eax+edi*4], esi
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            bp = (void **)&uc->uc_mcontext.gregs[REG_ESI];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x34 0xb8
                                return;
                            } else break;
                        }
                    }
// mov     ds:0FE83FF00h, esi // 89 35 00 FF 83 FE
                    if (*(uint8_t *)(*ip + 1) == 0x35) {            // mov     ds:<1>, esi
                        i  = *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ESI];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x35 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ds:0FE803028h, edi // 89 3D 28 30 80 FE
                    if (*(uint8_t *)(*ip + 1) == 0x3d) {            // mov     ds:<1>, edi
                        i  = *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x3d <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [esi+0Ch], edi       // 89 7E 0C
                    if (*(uint8_t *)(*ip + 1) == 0x7e) {            // mov     [esi+<1>], edi
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x7e <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [esi+14h], ebx       // 89 5E 14
                    if (*(uint8_t *)(*ip + 1) == 0x5e) {            // mov     [esi+<1>], ebx
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x5e <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [eax+2504h], esi     // 89 B0 04 25 00 00
                    if (*(uint8_t *)(*ip + 1) == 0xb0) {            // mov     [eax+<1>], esi
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ESI];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0xb0 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
                    if (*(uint8_t *)(*ip + 1) == 0x0c) {
// mov     ds:0FE820200h[eax*4], ecx // 89 0C 85 00 02 82 FE
                        if (*(uint8_t *)(*ip + 2) == 0x85) {        // mov     ds:<1>[eax*4], ecx
                            i  = uc->uc_mcontext.gregs[REG_EAX] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x0c 0x85 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
                    }
// mov     ds:0FE801004h, ecx // 89 0D 04 10 80 FE
                    if (*(uint8_t *)(*ip + 1) == 0x0d) {            // mov     ds:<1>, ecx
                        i  = *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x0d <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [esi], ecx           // 89 0E
                    if (*(uint8_t *)(*ip + 1) == 0x0e) {            // mov     [esi], ecx
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x0e
                            return;
                        } else break;
                    }
// mov     [edi], ecx // 89 0F
                    if (*(uint8_t *)(*ip + 1) == 0x0f) {            // mov     [edi], ecx
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x0f
                            return;
                        } else break;
                    }
// mov     [eax+3240h], ecx     // 89 88 40 32 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x88) {            // mov     [eax+<1>], ecx
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x88 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [eax+3224h], ebx     // 89 98 24 32 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x98) {            // mov     [eax+<1>], ebx
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x98 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [eax], ecx           // 89 08
                    if (*(uint8_t *)(*ip + 1) == 0x08) {            // mov     [eax], ecx
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x08
                            return;
                        } else break;
                    }
// mov     [ecx+18h], eax // 89 41 18
                    if (*(uint8_t *)(*ip + 1) == 0x41) {            // mov     [ecx+<1>], eax
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x41 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [ebx+50h], eax // 89 43 50
                    if (*(uint8_t *)(*ip + 1) == 0x43) {            // mov     [ebx+<1>], eax
                        i  = uc->uc_mcontext.gregs[REG_EBX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x43 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [edi+8], eax // 89 47 08
                    if (*(uint8_t *)(*ip + 1) == 0x47) {            // mov     [edi+<1>], eax
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x47 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [eax+4], ecx         // 89 48 04
                    if (*(uint8_t *)(*ip + 1) == 0x48) {            // mov     [eax+<1>], ecx
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x48 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [edi+34h], ecx // 89 4F 34
                    if (*(uint8_t *)(*ip + 1) == 0x4f) {            // mov     [edi+<1>], ecx
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x4f <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [edx], ecx           // 89 0A
                    if (*(uint8_t *)(*ip + 1) == 0x0a) {            // mov     [edx], ecx
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x0a
                            return;
                        } else break;
                    }
// mov     [eax+ecx*4+340h], edx    // 89 94 88 40 03 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x94) {
                        if (*(uint8_t *)(*ip + 2) == 0x88) {        // mov     [eax+ecx*4+<1>], edx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x94 0x88 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
// mov     [eax+edi*4+700004h], edx // 89 94 B8 04 00 70 00
                        if (*(uint8_t *)(*ip + 2) == 0xb8) {        // mov     [eax+edi*4+<1>], edx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x94 0xb8 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
                    }
// mov     [eax+ecx*4+3A0h], esi    // 89 B4 88 A0 03 00 00
                    if (*(uint8_t *)(*ip + 1) == 0xb4) {
                        if (*(uint8_t *)(*ip + 2) == 0x88) {        // mov     [eax+ecx*4+<1>], esi
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            bp = (void **)&uc->uc_mcontext.gregs[REG_ESI];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0xb4 0x88 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
                    }
// mov     [eax+ecx*4+5FCh], ebx // 89 9C 88 FC 05 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x9c) {
                        if (*(uint8_t *)(*ip + 2) == 0x88) {        // mov     [eax+ecx*4+<1>], ebx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x9c 0x88 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
                    }
// mov     ds:80000000h, edx // 89 15 00 00 00 80
                    if (*(uint8_t *)(*ip + 1) == 0x15) {            // mov     ds:<1>, edx
                        i  = *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x15
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [esi], edx // 89 16
                    if (*(uint8_t *)(*ip + 1) == 0x16) {            // mov     [esi], edx
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x16
                            return;
                        } else break;
                    }
// mov     [ebx], edx // 89 13
                    if (*(uint8_t *)(*ip + 1) == 0x13) {            // mov     [ebx], edx
                        i  = uc->uc_mcontext.gregs[REG_EBX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x13
                            return;
                        } else break;
                    }
// mov     [edi], edx // 89 17
                    if (*(uint8_t *)(*ip + 1) == 0x17) {            // mov     [edi], edx
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x17
                            return;
                        } else break;
                    }
// mov     [eax+esi*4], edx // 89 14 B0
                    if (*(uint8_t *)(*ip + 1) == 0x14) {
                        if (*(uint8_t *)(*ip + 2) == 0xb0) {        // mov     [eax+esi*4], edx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_ESI] * 4;
                            bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x14 0xb0
                                return;
                            } else break;
                        }
// mov     [eax+ebx*4], edx // 89 14 98
                        if (*(uint8_t *)(*ip + 2) == 0x98) {        // mov     [eax+ebx*4], edx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_EBX] * 4;
                            bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x14 0x98
                                return;
                            } else break;
                        }
// mov     [eax+ecx*4], edx // 89 14 88
                        if (*(uint8_t *)(*ip + 2) == 0x88) {        // mov     [eax+ecx*4], edx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x14 0x88
                                return;
                            } else break;
                        }
// mov     [eax+ecx], edx // 89 14 08
                        if (*(uint8_t *)(*ip + 2) == 0x08) {        // mov     [eax+ecx], edx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_ECX];
                            bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x14 0x08
                                return;
                            } else break;
                        }
                    }
// mov     [eax+edx*4], ecx // 89 0C 90
                    if (*(uint8_t *)(*ip + 1) == 0x0c) {
                        if (*(uint8_t *)(*ip + 2) == 0x90) {        // mov     [eax+edx*4], ecx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_EDX] * 4;
                            bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x0c 0x90
                                return;
                            } else break;
                        }
// mov     [eax+esi*4], ecx // 89 0C B0
                        if (*(uint8_t *)(*ip + 2) == 0xb0) {        // mov     [eax+esi*4], ecx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_ESI] * 4;
                            bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x0c 0xb0
                                return;
                            } else break;
                        }
// mov     [edi+edx], ecx // 89 0C 17
                        if (*(uint8_t *)(*ip + 2) == 0x17) {        // mov     [edi+edx], ecx
                            i  = uc->uc_mcontext.gregs[REG_EDI];
                            i += uc->uc_mcontext.gregs[REG_EDX];
                            bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x0c 0x17
                                return;
                            } else break;
                        }
                    }
// mov     [eax+edx*4+700014h], ecx // 89 8C 90 14 00 70 00
                    if (*(uint8_t *)(*ip + 1) == 0x8c) {
                        if (*(uint8_t *)(*ip + 2) == 0x90) {        // mov     [eax+edx*4+<1>], ecx
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += uc->uc_mcontext.gregs[REG_EDX] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x8c 0x90 <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
                    }
// mov     [ecx], eax // 89 01
                    if (*(uint8_t *)(*ip + 1) == 0x01) {            // mov     [ecx], eax
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x01
                            return;
                        } else break;
                    }
// mov     [edi+2210h], ecx // 89 8F 10 22 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x8f) {            // mov     [edi+<1>], ecx
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x8f <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [edi+2214h], edx // 89 97 14 22 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x97) {            // mov     [edi+<1>], edx
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x97 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [ecx], edx // 89 11
                    if (*(uint8_t *)(*ip + 1) == 0x11) {            // mov     [ecx], edx
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x11
                            return;
                        } else break;
                    }
// mov     [eax-8], edx // 89 50 F8
                    if (*(uint8_t *)(*ip + 1) == 0x50) {            // mov     [eax+<1>], edx
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x50 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [ecx+4], edx // 89 51 04
                    if (*(uint8_t *)(*ip + 1) == 0x51) {            // mov     [ecx+<1>], edx
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x51 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [eax], edx // 89 10
                    if (*(uint8_t *)(*ip + 1) == 0x10) {            // mov     [eax], edx
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x10
                            return;
                        } else break;
                    }
// mov     [edx+400780h], eax // 89 82 80 07 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x82) {            // mov     [edx+<1>], eax
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x82 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [eax+3210h], edx // 89 90 10 32 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x90) {            // mov     [eax+<1>], edx
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x90 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [esi+200h], ebx // 89 9E 00 02 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x9e) {            // mov     [esi+<1>], ebx
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x9e <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [esi+140h], eax // 89 86 40 01 00 00
                    if (*(uint8_t *)(*ip + 1) == 0x86) {            // mov     [esi+<1>], eax
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x86 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [esi+9400h], ebp // 89 AE 00 94 00 00
                    if (*(uint8_t *)(*ip + 1) == 0xae) {            // mov     [esi+<1>], ebp
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBP];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0xae <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [eax], ebp // 89 28
                    if (*(uint8_t *)(*ip + 1) == 0x28) {            // mov     [eax], ebp
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBP];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x28
                            return;
                        } else break;
                    }
// mov     [eax+4], ebp // 89 68 04
                    if (*(uint8_t *)(*ip + 1) == 0x68) {            // mov     [eax+<1>], ebp
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBP];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x68 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [esi+400754h], edx // 89 96 54 07 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x96) {            // mov     [esi+<1>], edx
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x96 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [esi+400750h], ecx // 89 8E 50 07 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x8e) {            // mov     [esi+<1>], ecx
                        i  = uc->uc_mcontext.gregs[REG_ESI];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x8e <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [edi], ebp // 89 2F
                    if (*(uint8_t *)(*ip + 1) == 0x2f) {            // mov     [edi], ebp
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBP];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x2f
                            return;
                        } else break;
                    }
// mov     [eax], edi // 89 38
                    if (*(uint8_t *)(*ip + 1) == 0x38) {            // mov     [eax], edi
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x38
                            return;
                        } else break;
                    }
// mov     [edx], edi // 89 3A
                    if (*(uint8_t *)(*ip + 1) == 0x3a) {            // mov     [edx], edi
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x3a
                            return;
                        } else break;
                    }
// mov     [ecx+4], edi // 89 79 04
                    if (*(uint8_t *)(*ip + 1) == 0x79) {            // mov     [ecx+<1>], edi
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x79 <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [edx+4], edi // 89 7A 04
                    if (*(uint8_t *)(*ip + 1) == 0x7a) {            // mov     [edx+<1>], edi
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        i += *(uint8_t *)(*ip + 2);                 //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x7a <1:byte>
                            *ip += 1;                               // <1> size: byte
                            return;
                        } else break;
                    }
// mov     [eax+3240h], edi // 89 B8 40 32 00 00
                    if (*(uint8_t *)(*ip + 1) == 0xb8) {            // mov     [eax+<1>], edi
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0xb8 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     ds:0FE85FF00h, ebx // 89 1D 00 FF 85 FE
                    if (*(uint8_t *)(*ip + 1) == 0x1d) {            // mov     ds:<1>, ebx
                        i  = *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x1d
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [edi], ebx // 89 1F
                    if (*(uint8_t *)(*ip + 1) == 0x1f) {            // mov     [edi], ebx
                        i  = uc->uc_mcontext.gregs[REG_EDI];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x1f
                            return;
                        } else break;
                    }
// mov     [edx], eax // 89 02
                    if (*(uint8_t *)(*ip + 1) == 0x02) {            // mov     [edx], eax
                        i  = uc->uc_mcontext.gregs[REG_EDX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x02
                            return;
                        } else break;
                    }
// mov     [ebx], eax // 89 03
                    if (*(uint8_t *)(*ip + 1) == 0x03) {            // mov     [ebx], eax
                        i  = uc->uc_mcontext.gregs[REG_EBX];
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        if (xboxkrnl_write(i, bp, 4)) {
                            *ip += 2;                               //   opcode: 0x89 0x03
                            return;
                        } else break;
                    }
// mov     [esi+edi*4+400980h], eax // 89 84 BE 80 09 40 00
                    if (*(uint8_t *)(*ip + 1) == 0x84) {
                        if (*(uint8_t *)(*ip + 2) == 0xbe) {        // mov     [esi+edi*4+<1>], eax
                            i  = uc->uc_mcontext.gregs[REG_ESI];
                            i += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            i += *(uint32_t *)(*ip + 3);            //      <1>: offset
                            bp = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                            if (xboxkrnl_write(i, bp, 4)) {
                                *ip += 3;                           //   opcode: 0x89 0x84 0xbe <1:dword>
                                *ip += 4;                           // <1> size: dword
                                return;
                            } else break;
                        }
                    }
                }
// movzx   ecx, byte ptr [eax+1] // 0F B6 48 01
                if (*(uint8_t *)*ip == 0x0f) {
                    if (*(uint8_t *)(*ip + 1) == 0xb6) {
                        if (*(uint8_t *)(*ip + 2) == 0x48) {        // movzx   ecx, [eax+<1>]
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += *(uint8_t *)(*ip + 3);             //      <1>: offset
                            bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                            *(uint32_t *)bp = 0;
                            if (xboxkrnl_read(i, bp, 1)) {
                                *ip += 3;                           //   opcode: 0x0f 0xb6 0x48 <1:byte>
                                *ip += 1;                           // <1> size: byte
                                return;
                            } else break;
                        }
// movzx   edi, byte ptr [eax+1] // 0F B6 78 01
                        if (*(uint8_t *)(*ip + 2) == 0x78) {        // movzx   edi, [eax+<1>]
                            i  = uc->uc_mcontext.gregs[REG_EAX];
                            i += *(uint8_t *)(*ip + 3);             //      <1>: offset
                            bp = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                            *(uint32_t *)bp = 0;
                            if (xboxkrnl_read(i, bp, 1)) {
                                *ip += 3;                           //   opcode: 0x0f 0xb6 0x78 <1:byte>
                                *ip += 1;                           // <1> size: byte
                                return;
                            } else break;
                        }
                    }
                }
// mov     byte ptr [eax+6013D4h], 1Fh // C6 80 D4 13 60 00 1F
                if (*(uint8_t *)*ip == 0xc6) {
                    if (*(uint8_t *)(*ip + 1) == 0x80) {            // mov     [eax+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 6, 1)) {
                            *ip += 2;                               //   opcode: 0xc6 0x80 <1:dword> <2:byte>
                            *ip += 4;                               // <1> size: dword
                            *ip += 1;                               // <2> size: byte
                            return;
                        } else break;
                    }
// mov     byte ptr [ecx-13FFEF5h], 2 // C6 81 0B 01 C0 FE 02
                    if (*(uint8_t *)(*ip + 1) == 0x81) {            // mov     [ecx+<1>], <2>
                        i  = uc->uc_mcontext.gregs[REG_ECX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        if (xboxkrnl_write(i, *ip + 6, 1)) {
                            *ip += 2;                               //   opcode: 0xc6 0x81 <1:dword> <2:byte>
                            *ip += 4;                               // <1> size: dword
                            *ip += 1;                               // <2> size: byte
                            return;
                        } else break;
                    }
                }
// mov     bl, [eax+6013D5h] // 8A 98 D5 13 60 00
                if (*(uint8_t *)*ip == 0x8a) {
                    if (*(uint8_t *)(*ip + 1) == 0x98) {            // mov     bl, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                        if (xboxkrnl_read(i, bp, 1)) {
                            *ip += 2;                               //   opcode: 0x8a 0x98 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     cl, [eax+6013D5h] // 8A 88 D5 13 60 00
                    if (*(uint8_t *)(*ip + 1) == 0x88) {            // mov     cl, [eax+<1>]
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_read(i, bp, 1)) {
                            *ip += 2;                               //   opcode: 0x8a 0x88 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
                }
// mov     [eax+6013D5h], cl // 88 88 D5 13 60 00
                if (*(uint8_t *)*ip == 0x88) {
                    if (*(uint8_t *)(*ip + 1) == 0x88) {            // mov     [eax+<1>], cl
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                        if (xboxkrnl_write(i, bp, 1)) {
                            *ip += 2;                               //   opcode: 0x88 0x88 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
// mov     [eax-13FFEFCh], bl // 88 98 04 01 C0 FE
                    if (*(uint8_t *)(*ip + 1) == 0x98) {            // mov     [eax+<1>], bl
                        i  = uc->uc_mcontext.gregs[REG_EAX];
                        i += *(uint32_t *)(*ip + 2);                //      <1>: offset
                        bp = (void **)&uc->uc_mcontext.gregs[REG_EBX];
                        if (xboxkrnl_write(i, bp, 1)) {
                            *ip += 2;                               //   opcode: 0x88 0x98 <1:dword>
                            *ip += 4;                               // <1> size: dword
                            return;
                        } else break;
                    }
                }
// mov     ds:0FEC00000h[edx*2], cx // 66 89 0C 55 00 00 C0 FE
                if (*(uint8_t *)*ip == 0x66) {
                    if (*(uint8_t *)(*ip + 1) == 0x89) {
                        if (*(uint8_t *)(*ip + 2) == 0x0c) {
                            if (*(uint8_t *)(*ip + 3) == 0x55) {            // mov     ds:<1>[edx*2], cx
                                i  = uc->uc_mcontext.gregs[REG_EDX] * 2;
                                i += *(uint32_t *)(*ip + 4);                //      <1>: offset
                                bp = (void **)&uc->uc_mcontext.gregs[REG_ECX];
                                if (xboxkrnl_write(i, bp, 2)) {
                                    *ip += 4;                               //   opcode: 0x66 0x89 0x0c 0x55 <1:dword>
                                    *ip += 4;                               // <1> size: dword
                                    return;
                                } else break;
                            }
                        }
                    }
                }
            }
        } while (0);
    } else if (i == 128) {
#if 0 //XXX turok
# if 0
                                    // SetLastError()
*(short *)0x118f2 = 0xc031;         // movzx   eax, large byte ptr fs:24h
*(int *)  0x118f4 = 0x90909090;     // 64 0F B6 05 24 00 00 00
*(short *)0x118f8 = 0x9090;

*(short *)0x118fe = 0xc031;         // mov     eax, large fs:28h
*(int *)  0x11900 = 0x90909090;     // 64 A1 28 00 00 00

*(short *)0x11904 = 0xc931;         // mov     ecx, large fs:4
*(int *)  0x11906 = 0x90909090;     // 64 8B 0D 04 00 00 00
*(char *) 0x1190a = 0x90;
# else
                                    // SetLastError()
*(int *)  0x118f2 = 0xcc0004c2;     // movzx   eax, large byte ptr fs:24h
                                    // 64 0F B6 05 24 00 00 00
# endif
                                    // GetLastError()
*(char *) 0x118ca = 0xc3;           // movzx   eax, large byte ptr fs:24h
                                    // 64 0F B6 05 24 00 00 00

*(short *)0x11005 = 0xc031;         // mov     eax, large fs:20h
*(int *)  0x11007 = 0x90909090;     // 64 A1 20 00 00 00
*(int *)  0x1100b = 0x90909090;     // mov     eax, [eax+250h]
*(short *)0x1100f = 0x9090;         // 8B 80 50 02 00 00

*(short *)0x141b0 = 0xc031;         // mov     eax, large fs:20h
*(int *)  0x141b2 = 0x90909090;     // 64 A1 20 00 00 00
*(int *)  0x141b6 = 0x90909090;     // mov     eax, [eax+250h]
*(short *)0x141ba = 0x9090;         // 8B 80 50 02 00 00

*(int *)  0xf8ba7 = 0x90909090;     // mov     ecx, large fs:4
*(int *)  0xf8bab = 0x90909090;     // 64 8B 0D 04 00 00 00
*(short *)0xf8baf = 0x9090;         // mov     eax, [ecx+eax*4]
                                    // 8B 04 81
*(int *)  0xf8bb1 = 0x90909090;     // mov     [eax+8], esi
*(short *)0xf8bb5 = 0x9090;         // 89 B0 08 00 00 00

*(short *)0x11bec = 0xc031;         // mov     eax, large fs:28h
*(int *)  0x11bee = 0x90909090;     // 64 A1 28 00 00 00
*(int *)  0x11bf2 = 0x90909090;     // mov     eax, [eax+12Ch]
*(short *)0x11bf6 = 0x9090;         // 8B 80 2C 01 00 00

*(short *)0x14227 = 0xc031;         // mov     eax, large fs:20h
*(int *)  0x14229 = 0x90909090;     // 64 A1 20 00 00 00
*(int *)  0x1422d = 0x90909090;     // mov     eax, [eax+250h]
*(short *)0x14231 = 0x9090;         // 8B 80 50 02 00 00

*(short *)0x101797 = 0xc031;        // mov     eax, large fs:0
*(int *)  0x101799 = 0x90909090;    // 64 A1 00 00 00 00

*(int *)  0x10179e = 0x90909090;    // mov     large fs:0, esp
*(short *)0x1017a2 = 0x9090;        // 64 89 25 00 00 00 00
*(char *) 0x1017a4 = 0x90;

*(int *)  0x101814 = 0x90909090;    // mov     large fs:0, ecx
*(short *)0x101818 = 0x9090;        // 64 89 0D 00 00 00 00
*(char *) 0x10181a = 0x90;

*(int *)  0x10182d = 0x90909090;    // mov     large fs:0, ecx
*(short *)0x101831 = 0x9090;        // 64 89 0D 00 00 00 00
*(char *) 0x101833 = 0x90;

*(short *)0x1014f0 = 0xc031;        // mov     eax, large fs:0
*(int *)  0x1014f2 = 0x90909090;    // 64 A1 00 00 00 00

*(int *)  0x101502 = 0x90909090;    // mov     large fs:0, esp
*(short *)0x101506 = 0x9090;        // 64 89 25 00 00 00 00
*(char *) 0x101508 = 0x90;

*(int *)  0x101594 = 0x90909090;    // mov     large fs:0, ecx
*(short *)0x101598 = 0x9090;        // 64 89 0D 00 00 00 00
*(char *) 0x10159a = 0x90;

*(short *)0x98b47 = 0xc031;         // mov     eax, large fs:0
*(int *)  0x98b49 = 0x90909090;     // 64 A1 00 00 00 00

*(int *)  0x98b4e = 0x90909090;     // mov     large fs:0, esp
*(short *)0x98b52 = 0x9090;         // 64 89 25 00 00 00 00
*(char *) 0x98b54 = 0x90;

*(int *)  0x98be9 = 0x90909090;     // mov     large fs:0, ecx
*(short *)0x98bed = 0x9090;         // 64 89 0D 00 00 00 00
*(char *) 0x98bef = 0x90;

*(short *)0x16b50 = 0xc031;         // mov     eax, large fs:0
*(int *)  0x16b52 = 0x90909090;     // 64 A1 00 00 00 00

*(int *)  0x16b5e = 0x90909090;     // mov     large fs:0, esp
*(short *)0x16b62 = 0x9090;         // 64 89 25 00 00 00 00
*(char *) 0x16b64 = 0x90;

*(int *)  0x16d64 = 0x90909090;     // mov     large fs:0, ecx
*(short *)0x16d68 = 0x9090;         // 64 89 0D 00 00 00 00
*(char *) 0x16d6a = 0x90;

*(short *)0x17b32 = 0xc031;         // mov     eax, large fs:0
*(int *)  0x17b34 = 0x90909090;     // 64 A1 00 00 00 00

*(int *)  0x17b3e = 0x90909090;     // mov     large fs:0, esp
*(short *)0x17b42 = 0x9090;         // 64 89 25 00 00 00 00
*(char *) 0x17b44 = 0x90;

*(int *)  0x17d64 = 0x90909090;     // mov     large fs:0, ecx
*(short *)0x17d68 = 0x9090;         // 64 89 0D 00 00 00 00
*(char *) 0x17d6a = 0x90;

*(short *)0x25337 = 0xc031;         // mov     eax, large fs:0
*(int *)  0x25339 = 0x90909090;     // 64 A1 00 00 00 00

*(int *)  0x2533e = 0x90909090;     // mov     large fs:0, esp
*(short *)0x25342 = 0x9090;         // 64 89 25 00 00 00 00
*(char *) 0x25344 = 0x90;

*(int *)  0x2553f = 0x90909090;     // mov     large fs:0, ecx
*(short *)0x25543 = 0x9090;         // 64 89 0D 00 00 00 00
*(char *) 0x25545 = 0x90;
#endif
#define EIP_PATCH(x,y,z) do { \
            if (!memcmp((x), (y), sizeof((y)) - 1)) { \
                memcpy((x), (z), sizeof((z)) - 1); \
                return; \
            } \
        } while (0)

        EIP_PATCH(*ip,                              // SetLastError() / GetLastError()
            "\x64\x0f\xb6\x05\x24\x00\x00\x00",     // 1: movzx   eax, large byte ptr fs:24h
                                                    // --
            "\x31\xc0\x90\x90\x90\x90\x90\x90");    // 1: xor     eax, eax

        EIP_PATCH(*ip,                              // SetLastError()
            "\x64\xa1\x28\x00\x00\x00"              // 1: mov     eax, large fs:28h
            "\x64\x8b\x0d\x04\x00\x00\x00"          // 2: mov     ecx, large fs:4
            "\xa1"/*\xa0\xea\x24\x00"*/             // 3: mov     eax, TlsIndex
          /*"\x8b\x04\x81"*/                        // 4: mov     eax, [ecx+eax*4]
          /*"\x8b\x4c\x24\x04"*/                    // 5: mov     ecx, [esp+dwErrCode]
          /*"\x89\x88\x04\x00\x00\x00"*/,           // 6: mov     [eax+4], ecx
                                                    // --
            "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
            "\x31\xc9\x90\x90\x90\x90\x90"          // 2: xor     ecx, ecx
            "\x90\x90\x90\x90\x90"                  // 3: nop
            "\x90\x90\x90"                          // 4: nop
            "\x90\x90\x90\x90"                      // 5: nop
            "\x90\x90\x90\x90\x90\x90");            // 6: nop

        EIP_PATCH(*ip,                              // GetLastError()
            "\x64\xa1\x28\x00\x00\x00"              // 1: mov     eax, large fs:28h
            "\xa1"/*\xa0\xea\x24\x00"*/             // 2: mov     eax, TlsIndex
          /*"\x64\x8b\x0d\x04\x00\x00\x00"*/        // 3: mov     ecx, large fs:4
          /*"\x8b\x04\x81"*/                        // 4: mov     eax, [ecx+eax*4]
          /*"\x8b\x80\x04\x00\x00\x00"*/,           // 5: mov     eax, [eax+4]
                                                    // --
            "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
            "\x90\x90\x90\x90\x90"                  // 2: nop
            "\x31\xc9\x90\x90\x90\x90\x90"          // 3: xor     ecx, ecx
            "\x90\x90\x90"                          // 4: nop
            "\x90\x90\x90\x90\x90\x90");            // 5: nop

        EIP_PATCH(*ip,
            "\x64\xa1\x20\x00\x00\x00"              // 1: mov     eax, large fs:20h
            "\x8b\x80\x50\x02\x00\x00",             // 2: mov     eax, [eax+250h]
                                                    // --
            "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
            "\x90\x90\x90\x90\x90\x90");            // 2: nop

        EIP_PATCH(*ip,                              // __mtinit()
            "\x64\x8b\x0d\x04\x00\x00\x00"          // 1: mov     ecx, large fs:4
            "\x8b\x04\x81"                          // 2: mov     eax, [ecx+eax*4]
            "\x89\xb0\x08\x00\x00\x00",             // 3: mov     [eax+8], esi
                                                    // --
            "\x90\x90\x90\x90\x90\x90\x90"          // 1: nop
            "\x90\x90\x90"                          // 2: nop
            "\x90\x90\x90\x90\x90\x90");            // 3: nop

        EIP_PATCH(*ip,                              // GetCurrentThreadId()
            "\x64\xa1\x28\x00\x00\x00"              // 1: mov     eax, large fs:28h
            "\x8b\x80\x2c\x01\x00\x00",             // 2: mov     eax, [eax+12Ch]
                                                    // --
            "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
            "\x90\x90\x90\x90\x90\x90");            // 2: nop

        EIP_PATCH(*ip,                              // __getptd()
            "\x64\xa1\x28\x00\x00\x00"              // 1: mov     eax, large fs:28h
            "\x83\x78\x28\x00",                     // 2: cmp     dword ptr [eax+28h], 0
                                                    // --
            "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
            "\x3c\x00\x90\x90");                    // 2: cmp     al, 0

        EIP_PATCH(*ip,
            "\x64\xa1\x00\x00\x00\x00",             // 1: mov     eax, large fs:0
                                                    // --
            "\x31\xc0\x90\x90\x90\x90");            // 1: xor     eax, eax

        EIP_PATCH(*ip,
            "\x64\xa3\x00\x00\x00\x00",             // 1: mov     large fs:0, eax
                                                    // --
            "\x90\x90\x90\x90\x90\x90");            // 1: nop

        EIP_PATCH(*ip,
            "\x64\x89\x0d\x00\x00\x00\x00",         // 1: mov     large fs:0, ecx
                                                    // --
            "\x90\x90\x90\x90\x90\x90\x90");        // 1: nop

        EIP_PATCH(*ip,
            "\x64\x89\x25\x00\x00\x00\x00",         // 1: mov     large fs:0, esp
                                                    // --
            "\x90\x90\x90\x90\x90\x90\x90");        // 1: nop

        EIP_PATCH(*ip,
            "\x64\xa1\x58\x00\x00\x00",             // 1: mov     eax, large fs:58h
                                                    // --
            "\x31\xc0\x90\x90\x90\x90");            // 1: xor     eax, eax

        EIP_PATCH(*ip,
            "\x0f\x09",                             // 1: wbinvd (Write Back and Invalidate Cache)
                                                    // --
            "\x90\x90");                            // 1: nop

#undef EIP_PATCH

        if (*(uint8_t *)*ip == 0xee) {                              // out     dx, al
            i  = uc->uc_mcontext.gregs[REG_EAX] & 0xff;
            bp = (void **)&uc->uc_mcontext.gregs[REG_EDX];
            PRINT("out: "
                "[0x%.04hx], "
                "al (0x%.02hhx)",
                *(uint16_t *)bp,                                    // dx
                i);                                                 // al
            //TODO
            *ip += 1;                                               //   opcode: 0xee
            return;
        }
    }

    PRINT("Segmentation Fault!", 0);

    PRINT("si_signo \t= %i", signum);
    PRINT("si_errno \t= %i", info->si_errno);
    i = info->si_code;
    n = (i < ARRAY_SIZE(signal_segv_si_codes)) ? signal_segv_si_codes[i] : "unknown";
    PRINT("si_code  \t= %i (%s)", i, n);
    PRINT("si_addr  \t= %p", info->si_addr);
    for (i = 0; i < NGREG; ++i) {
        n = (i < ARRAY_SIZE(greg_names)) ? greg_names[i] : NULL;
        if (n) PRINT("%s \t= 0x%.08x", n, uc->uc_mcontext.gregs[i]);
        else PRINT("reg[%.02i]  \t= 0x%.08x", i, uc->uc_mcontext.gregs[i]);
    }

    PRINT("Stack trace:", 0);

    for (ip = (void **)&uc->uc_mcontext.gregs[REG_EIP],
        bp = (void **)uc->uc_mcontext.gregs[REG_EBP],
        i = 1;
        *ip;
        ip = (void **)&bp[1],
        bp = (void **)bp[0],
        ++i) {
        PRINT("%.02i: bp = %p, ip = %p%s", i, bp, *ip, (i == 1) ? " <- SEGV" : "");
        if (!bp) break;
    }

    _exit(-1);
}

int
main(int argc, char **argv) {
    struct sigaction s = {
        .sa_sigaction   = signal_segv,
        .sa_flags       = SA_SIGINFO,
    };
    pthread_t entry;
    XbeHeader *xbeh;
    void *addr;
    int fd;
    size_t i;

    setbuf(stdout, NULL);

    if (argc < 2) {
        fprintf(stderr, "usage: %s <xbox executable>.xbe [GDB flag]\n", argv[0]);
        return 1;
    }

    if (xboxkrnl_init()) {
        fprintf(stderr, "error: xboxkrnl_init() initialization failed\n");
        return 1;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        PRINT("error: open(): '%s': '%s'", argv[1], strerror(errno));
        return 1;
    }
    addr = (void *)XBE_BASE_ADDR;
    if (mmap(addr,
            PAGESIZE,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_FIXED | MAP_POPULATE | MAP_NORESERVE,
            fd,
            0) != addr) {
        PRINT("error: mmap(): failed to map XBE header @ %p", addr);
        close(fd);
        return 1;
    }
    xbeh = addr;
    if (xbeh->dwMagic != *(uint32_t *)XBE_MAGIC) {
        PRINT("error: invalid XBE magic: 0x%.08x", xbeh->dwMagic);
        close(fd);
        return 1;
    }
    XbeHeader_dump(xbeh);

    memset(&xbeh->dwInitFlags, 0, sizeof(xbeh->dwInitFlags));

    xbeh->dwEntryAddr ^=
        ((xbeh->dwEntryAddr ^ XBE_XOR_EP_RETAIL) > 0x1000000)
            ? XBE_XOR_EP_DEBUG
            : XBE_XOR_EP_RETAIL;
    xbeh->dwKernelImageThunkAddr ^=
        ((xbeh->dwKernelImageThunkAddr ^ XBE_XOR_KT_RETAIL) > 0x1000000)
            ? XBE_XOR_KT_DEBUG
            : XBE_XOR_KT_RETAIL;

    PRINT("XBE entry: %p | thunk table: %p", xbeh->dwEntryAddr, xbeh->dwKernelImageThunkAddr);

    {
        XbeSectionHeader *xbes;
        const char *sname;
        uint32_t vaddr;
        uint32_t vmask;
        uint32_t vsize;
        uint32_t raddr;
        uint32_t rsize;
        ssize_t rd;

        if (xbeh->dwSections) {
            xbes   = (typeof(xbes))xbeh->dwSectionHeadersAddr;
            vaddr  = xbes[0].dwVirtualAddr;
            if ((vmask = vaddr & PAGEMASK)) vaddr -= vmask;
            raddr  = xbes[xbeh->dwSections - 1].dwVirtualAddr;
            raddr += xbes[xbeh->dwSections - 1].dwVirtualSize;
            if ((vmask = raddr & PAGEMASK)) raddr += PAGESIZE - vmask;
            vsize  = raddr - vaddr;

            xboxkrnl_mem_contiguous = (void *)raddr;

            PRINT("mmap(): total section map: "
                "start: 0x%.08x | "
                "end: 0x%.08x | "
                "size: 0x%.08x",
                vaddr, raddr, vsize);

            addr = (typeof(addr))vaddr;
            if (mmap(addr,
                    vsize,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS | MAP_NORESERVE,
                    -1,
                    0) != addr) {
                PRINT("error: mmap(): failed to map XBE sections: '%s'", strerror(errno));
                close(fd);
                return 1;
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

                if (lseek(fd, raddr, SEEK_SET) != (int)raddr) {
                    PRINT("error: lseek(): '%s'", strerror(errno));
                    close(fd);
                    return 1;
                }
                if ((rd = read(fd, (void *)vaddr, rsize)) != (typeof(rd))rsize) {
                    PRINT("error: read(): '%s'", (rd < 0) ? strerror(errno) : "short read");
                    close(fd);
                    return 1;
                }
                if (!strncmp(sname, ".text", sizeof(".text"))) {
                    xbeh->dwEntryAddr = vaddr;
//                    xboxkrnl_patch_seh((void *)vaddr, rsize);//XXX
                }
            }
        }
    }

    close(fd);

    XbeTLS_dump((void *)xbeh->dwTLSAddr);

    {
        char buf[32];
        snprintf(buf, sizeof(buf), "cat /proc/%hu/maps", getpid());
        system(buf);
    }

    xboxkrnl_thunk_resolve((void *)xbeh->dwKernelImageThunkAddr);

    if (sigaction(SIGSEGV, &s, NULL) < 0) {
        PRINT("error: sigaction(): '%s'", strerror(errno));
        return 1;
    }

    PRINT("/* spawning entry thread with entry point @ %p */", xbeh->dwEntryAddr);

    if (argc >= 3) INT3;

    pthread_create(&entry, NULL, (void *)xbeh->dwEntryAddr, NULL);
    pthread_join(entry, NULL);

    return 0;
}

